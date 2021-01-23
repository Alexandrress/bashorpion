/**
 * \file BashorpionClient.c
 * \brief Programme permettant de lancer un client du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 3.0
 * \date 21 Janvier 2021
 *
*/

// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"
#include "BashorpionSession.h"
#include "BashorpionReqRep.h"

void client();
void threadComServeur(void * obj);
void * threadPeerToPeer(char * ip);
void intro();
void playBashorpion(int socket, char * buffer, int playerID);

// ******** VARIABLES GLOBALES ********

int sockDialogueServeur, sockDialoguePeerToPeer, sockConso, sockBase;
int portServ;
int myScore=0;
int opponentScore=0;
int isInGame=0;
int turn=0;
int playerID=0;
char serverIP[MAX_CHAR];
infoUser_t informationJoueur;
pthread_t  tid; /*Thread ID*/  
struct sockaddr_in clientAdr;

// ************ FONCTIONS ************

/**
 * \fn void client()
 * \brief Fonction principale du client permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void client()
{
	printf("Entrez votre username : ");
	scanf("%s", informationJoueur.username);
	
	printf("\n");
	printf("Entrez l'adresse du serveur du lobby Bashorpion : ");
	scanf("%s", serverIP);

	printf("\n");
	printf("Entrez votre PORT de serveur : ");
	scanf("%d", &portServ);

	sockDialogueServeur = sessionClt();
	sockDialogueServeur = connectSrv(sockDialogueServeur, serverIP, PORT_SRV);
	pthread_create(&tid, NULL, (void *)threadComServeur, (void *) &sockDialogueServeur);
	
	sockBase = sessionSrv(portServ, 1);
	sockConso = acceptClt(sockBase, &clientAdr);
	
	/* Pour stocker les inputs user*/
	char buffer[MAX_CHAR]; 
	
	while(sockConso)
	{
		//On cancel le thread de dialogue avec le serveur lobby (mais pas la communication)
		pthread_cancel(tid);
		
		//On reçoit la demande et on décide si on accepte le duel ou non
		dialSrvToClient(sockConso, &clientAdr);
		
		if (hasAcceptedDuel == 1)
		{
			myScore=0;
			opponentScore=0;
			isInGame = 1;
			printf("Ton score: %d\n", myScore);
			printf("Score adverse: %d\n",opponentScore);
		}
		else
		{
			myScore=0;
			opponentScore=0;
			isInGame = 0; 
			printf("\nTu as refusé...\n");
		}
		
		//On joue au jeu tant qu'ils veulent jouer ensemble
		turn = 0;		
		playerID = 2;
		
		while(isInGame)
		{
			playBashorpion(sockConso, buffer, playerID); 
		}
		
		//Après que la partie se soit terminé on reprend le thread de dialogue avec lobby
		myScore=0;
		opponentScore=0;
		memset(&opponentName, 0, MAX_CHAR);
		pthread_create(&tid, NULL, (void *)threadComServeur, (void *) &sockDialogueServeur);
		sockConso = acceptClt(sockBase, &clientAdr); 
	}
	
	//Fermeture du socket
	CHECK(close(sockBase), "Problème close sock client ");
	CHECK(close(sockConso), "Problème close sock client ");
	CHECK(close(sockDialoguePeerToPeer), "Problème close sock client ");
	CHECK(close(sockDialogueServeur), "Problème close sock client ");
}

/**
 * \fn void * threadComServeur(void * obj)
 * \brief Thread du client permettant de communiquer avec le serveur lobby
*/
void threadComServeur(void * obj)
{
	int numberOfParams=0;
	int flagInstructions=0;

	/* Permet de close le thread d'un autre thread. */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	/* Pour stocker les inputs user*/
	char MSG_CLIENT[MAX_CHAR] = "100 PUT ";
	char buffer[MAX_CHAR]; 
	
	//Récupére les réponses
	char * reponse = (char *) malloc(sizeof(char) * MAX_CHAR);
	
	//On copie le pseudo dans dans le message de connexion
	strcat(MSG_CLIENT,informationJoueur.username);
	
	//On se connecte en envoyant nos informations
	reponse = dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
	printf("%s\n",reponse);
	
	memset(&reponse, 0, MAX_CHAR);
	memset(&MSG_CLIENT, 0, MAX_CHAR);
	
	/* Instructions lobby */
	printf("\n");
	printf("Commandes: \n\n - list\n - battle <nomDuJoueur>\n - accept\n - deny\n - exit\n\n");

	while(1)
	{
		// Lit l'input du client
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer)-1] = '\0';		
		char cmd[MAX_CHAR], arg[MAX_CHAR];
		numberOfParams=sscanf(buffer, "%s %s", cmd, arg);
		
		// Permet d'avoir la liste des joueurs.
		if(strcmp(buffer, "list") == 0)
		{
			strcpy(MSG_CLIENT,"100 GET liste");
			reponse=dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
			printf("%s\n",reponse);
			memset(&MSG_CLIENT, 0, MAX_CHAR);
			memset(&reponse, 0, MAX_CHAR);
		}
		
		// Permet de quitter le serveur et de fermer la socket de dialogue.
		else if(strcmp(buffer, "exit") == 0)
		{
			strcpy(MSG_CLIENT,"100 DELETE exit");
			reponse=dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
			memset(&MSG_CLIENT, 0, MAX_CHAR);
			memset(&reponse, 0, MAX_CHAR);
			
			printf("Aurevoir! Merci d'avoir joué!\n");
			close(sockDialogueServeur);
			exit(1);
		}
		
		//Permet de demander en duel un autre joueur
		else if(strcmp(cmd, "battle") == 0 && numberOfParams > 1)
		{ 
			strcpy(opponentName, arg);
			strcpy(MSG_CLIENT,"100 GETIP ");
			strcat(MSG_CLIENT,opponentName);
			//reponse=dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
			reponse="127.0.0.1";
			pthread_create(&tid, NULL, (void*)threadPeerToPeer, reponse); 
			memset(&MSG_CLIENT, 0, MAX_CHAR);
			memset(&reponse, 0, MAX_CHAR);
			pthread_exit(0);
		}
		
		// Si le client oublie les commandes...
		else
		{
			if(flagInstructions==0)
			{
				flagInstructions=1;
			}
			else
			{
				printf("\n");
				printf("Commandes: \n\n - list\n - battle <nomDuJoueur>\n - accept\n - deny\n - exit\n\n");
			}
		}
	}
}

/**
 * \fn void * threadPeerToPeer(char * ip)
 * \brief Thread du client permettant de communiquer avec un autre client en peer to peer
*/
void * threadPeerToPeer(char * ip)
{
	/* Pour stocker les inputs user*/
	char MSG_CLIENT[MAX_CHAR];
	char buffer[MAX_CHAR]; 
	
	//Récupére les réponses
	char * reponse = (char *) malloc(sizeof(char) * MAX_CHAR);

	printf("\n");
	printf("Envoie de l'invitation...\n");	
	
	sockDialoguePeerToPeer = sessionClt();
	sockDialoguePeerToPeer = connectSrv(sockDialoguePeerToPeer, ip, 60006);
	
	strcpy(MSG_CLIENT,"200 BATTLE ");
	strcat(MSG_CLIENT,informationJoueur.username);
	reponse=dialClientToSrv(sockDialoguePeerToPeer, MSG_CLIENT);
	printf("%s > %s\n", opponentName, reponse);
	memset(&MSG_CLIENT, 0, MAX_CHAR);
	memset(&reponse, 0, MAX_CHAR);
	
	if (hasAcceptedDuel == 1)
	{
		myScore=0;
		opponentScore=0;
		isInGame = 1;
		printf("Ton score: %d\n", myScore);
		printf("Score adverse: %d\n",opponentScore);
	}
	else
	{
		myScore=0;
		opponentScore=0;
		isInGame = 0; 
		printf("%s a refusé...\n", opponentName);
	}
		
	//On joue au jeu tant qu'ils veulent jouer ensemble
	turn = 0;		
	playerID = 1;
		
	while(isInGame)
	{
		playBashorpion(sockDialoguePeerToPeer, buffer, playerID); 
	}
		
	//Après que la partie se soit terminé on reprend le thread de dialogue avec lobby
	//et on ferme la socket peer to peer.
	myScore=0;
	opponentScore=0;
	memset(&opponentName, 0, MAX_CHAR);
	close(sockDialoguePeerToPeer);
	pthread_create(&tid, NULL, (void *)threadComServeur, (void *) &sockDialogueServeur);
	pthread_exit(0);
}

/**
 * \fn void playBashorpion(int socket, char * buffer, int playerID)
 * \brief Fonction permettant de jouer au morpion
*/
void playBashorpion(int socket, char * buffer, int playerID)
{
	int len, dataSocket = socket;

	printf("\n\n\n");
	printf("- LE MORPION COMMENCE -\n");

	int i = 0;
	int player = 0;
	int go = 0;                                  /* Square selection number for turn     */
	int row = 0;                                 /* Row index for a square               */  
	int column = 0;                              /* Column index for a square            */
	int line = 0;                                /* Row or column index in checking loop */
	int winner = 0;                              /* The winning player                   */
	char board[3][3] = {                         /* The board                            */
					   {'1','2','3'},          /* Initial values are reference numbers */
					   {'4','5','6'},          /* used to select a vacant square for   */
					   {'7','8','9'}           /* a turn.                              */
					 };
				 

	//Le jeu continu tant qu'il n'y a pas de winner ou tant que les cases ne sont pas tous remplis.
	for( i = (0 + turn); i<(9 + turn) && winner==0; i++)
	{
		// Affichage plateau
		printf("\n\n");
		printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
		printf("---+---+---\n");
		printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
		printf("---+---+---\n");
		printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
	  
		player = i%2 + 1;

		//On donne la main au bon joueur.
		do
		{
			if (player == playerID)
			{
				printf("\n");
				printf("%s, Entrez l'emplacement de votre case à jouer %c: ", informationJoueur.username,(player==1)?'X':'O');
				scanf("%d", &go);
				send(dataSocket, &go, sizeof(go), 0);
			}
			else 
			{
				printf("\n");
				printf("En attente de %s...\n", opponentName);
				len = recv(dataSocket, &go, MAX_CHAR, 0);
				printf("%s a choisi %d\n", opponentName, go);
			}

			row = --go/3;                                
			column = go%3;
		} while(go<0 || go>9 || board[row][column]>'9');

		board[row][column] = (player == 1) ? 'X' : 'O'; 

		//Conditions de victoire 
		if((board[0][0] == board[1][1] && board[0][0] == board[2][2]) ||
		 (board[0][2] == board[1][1] && board[0][2] == board[2][0]))
			winner = player;
		else
			for(line = 0; line <= 2; line ++)
				if((board[line][0] == board[line][1] && board[line][0] == board[line][2])||
				 (board[0][line] == board[1][line] && board[0][line] == board[2][line]))
					winner = player;

	}
	
	// Fin du jeu, plateau final
	printf("\n\n");
	printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);

	// Message de fin
	if(winner == 0)
	{
		printf("\n");
		printf("Prévisible! C'est une égalité!\n");
	}
	else if (winner == playerID)
	{
		printf("\n");
		printf("BRAVO CHAMPION! %s TU AS GAGNE!\n", informationJoueur.username);
		myScore++;
	}
	else
	{
		printf("\n");
		printf("%s gagne cette manche... Ahah t'es nul!\n", opponentName);
		opponentScore++;
	}
	
	printf("\n");
	printf("Ton score: %d\n", myScore);
	printf("\n");
	printf("Score adverse: %d\n",opponentScore);


	if (turn == 0 ) 
		turn++;
	else 
		turn--;

	//Demander si on fait une autre partie ou non
	printf("\n");
	printf("Une autre partie peut-être? (y/n)\n");
	fgetc(stdin);
	fgets(buffer, sizeof buffer, stdin);
	buffer[strlen(buffer)-1] = '\0';
	printf("\n");
	printf("En attente de %s...\n", opponentName);

	//On met le flag de la boucle isInGame à 1 ou 0 selon leurs choix
	isInGame=0;
	
	if (strcmp(buffer, "y") == 0) 
		isInGame = 1;

	send(dataSocket, buffer, strlen(buffer), 0);
	len = recv(dataSocket, buffer, MAX_CHAR, 0);
	buffer[len] = '\0';
	
	if (strcmp(buffer, "y") != 0 && isInGame==1)
	{
		printf("\n");
		printf("\n%s a refusé...\n", opponentName);
		isInGame = 0;
		myScore=0;
		opponentScore=0;
	}
}

/**
 * \fn void intro()
 * \brief Fonction d'affichage qui accueil le client
*/
void intro()
{
	printf("\n");
	printf("Bienvenue sur Bashorpion! Le morpion directement dans ton bash!\n\n");
}

int main()
{
	hasAcceptedDuel = 0;
	intro();
	client();
	printf("Fin du client Bashorpion.\n");
	
	return 0;
}
