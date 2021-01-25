/**
 * \file BashorpionClient.c
 * \brief Programme permettant de lancer un client du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 4.0
 * \date 25 Janvier 2021
 *
*/

// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"
#include "BashorpionSession.h"
#include "BashorpionReqRep.h"

void client();
void threadComServeur();
void * threadPeerToPeer(char * ip);
void affichagePlateauDeJeu(const char plateau[3][3]);
void playBashorpion(int socket, char * buffer, int playerID);
void intro();
void introLobby();

// ******** VARIABLES GLOBALES ********

int flagInstructions=0;
int sockDialogueServeur, sockDialoguePeerToPeer, sockConso, sockBase;
char serverIP[MAX_CHAR];
infoUser_t informationJoueur;
pthread_t tid;
struct sockaddr_in clientAdr;

//Variables de jeu
int myScore=0;
int opponentScore=0;
int isInGame=0;
int coup=0;
int turn=0;
int playerID=0;
int hasAcceptedDuel=0;

// ************ FONCTIONS ************

/**
 * \fn void client()
 * \brief Fonction principale du client permettant de se connecter au lobby puis
 d'ouvrir un serveur pour le peer to peer.
*/

void client()
{
	printf("\n");
	
	//Ici normalement on ne devrait pas demander le port, celui de base
	//ouvert est le PORT_CLT = 60001 pour le peer-to-peer Bashorpion, cependant
	//comme on test en localhost on ne peut pas ouvrir deux clients avec le 
	//port 60001. Celui défié doit être en port 60001.
	printf("Entrez votre port de serveur à ouvrir pour accepter les duels : ");
	scanf("%d", &informationJoueur.portIP);
	
	printf("Entrez l'adresse du serveur du lobby Bashorpion à rejoindre : ");
	scanf("%s", serverIP);
	
	printf("Entrez votre username pour rejoindre le serveur : ");
	scanf("%s", informationJoueur.username);
	
	printf("\n");

	sockDialogueServeur = sessionClt();
	sockDialogueServeur = connectSrv(sockDialogueServeur, serverIP, PORT_SRV);
	pthread_create(&tid, NULL, (void *)threadComServeur, (void *) &sockDialogueServeur);
	
	sockBase = sessionSrv(informationJoueur.portIP, 1);
	sockConso = acceptClt(sockBase, &clientAdr);
	
	//Pour les users inputs
	char buffer[MAX_CHAR]; 
	
	while(sockConso)
	{
		//On cancel le thread de dialogue avec le serveur lobby (mais pas la communication)
		pthread_cancel(tid);
				
		//On reçoit la demande et on décide si on accepte le duel ou non
		dialSrvToClient(sockConso, &clientAdr);
		
		printf("\n");
		
		if (hasAcceptedDuel == 1)
		{
			myScore=0;
			opponentScore=0;
			isInGame = 1;
			printf("Ton score: %d\n", myScore);
			printf("Score adverse: %d\n", opponentScore);
		}
		else
		{
			myScore=0;
			opponentScore=0;
			isInGame = 0; 
			printf("Tu as refusé...\n");
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
	
	//Fermeture des sockets
	CHECK(close(sockBase), "Problème close sock client 1 ");
	CHECK(close(sockConso), "Problème close sock client 2 ");
	CHECK(close(sockDialoguePeerToPeer), "Problème close sock client 3 ");
	CHECK(close(sockDialogueServeur), "Problème close sock client 4 ");
}


/**
 * \fn void threadComServeur()
 * \brief Thread du client permettant de communiquer avec le serveur lobby
*/

void threadComServeur()
{
	int numberOfParams=0;
	int flagForbidden=1;

	//Permet de close le thread à partir d'un autre thread, utile en duel.
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	//Pour stocker les inputs user
	char MSG_CLIENT[MAX_CHAR] = "100 PUT ";
	char buffer[MAX_CHAR]; 
	char code[MAX_CHAR];
	char ipToConnect[MAX_CHAR];
	
	//Récupére les réponses
	char * reponse = (char *) malloc(sizeof(char) * MAX_CHAR);
	
	//On copie le pseudo dans le message de connexion pour le lobby
	strcat(MSG_CLIENT,informationJoueur.username);
	
	//On se connecte en envoyant nos informations
	reponse = dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
	//printf("%s\n",reponse); Message de confirmation d'ajout à la liste du lobby
	
	memset(&reponse, 0, MAX_CHAR);
	memset(&MSG_CLIENT, 0, MAX_CHAR);
	
	//Message bienvenue serveur + instructions
	introLobby();

	while(1)
	{
		flagForbidden = 0;
	
		if (flagInstructions==1)
		{
			printf("\033[1;32m"); //Couleur verte
			printf("%s > ", informationJoueur.username);
			printf("\033[0m"); //Couleur blanche
		}
		
		// Lit l'input du client
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer)-1] = '\0';		
		char cmd[MAX_CHAR], arg[MAX_CHAR];
		numberOfParams=sscanf(buffer, "%s %s", cmd, arg);
		
		// Permet d'avoir la liste des joueurs.
		if(strcmp(buffer, "list") == 0)
		{
			strcpy(MSG_CLIENT,"100 GET LISTE_USER");
			reponse=dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
			
			char * temp = &(reponse[8]);

			printf("%s\n\n",temp);
			memset(&MSG_CLIENT, 0, MAX_CHAR);
			memset(&reponse, 0, MAX_CHAR);
		}
		
		// Permet de quitter le serveur et de fermer la socket de dialogue.
		else if(strcmp(buffer, "leave") == 0)
		{
			strcpy(MSG_CLIENT,"100 DELETE ");
			strcat(MSG_CLIENT,informationJoueur.username);
			reponse=dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
			memset(&MSG_CLIENT, 0, MAX_CHAR);
			memset(&reponse, 0, MAX_CHAR);
			
			printf("\n");
			printf("Aurevoir! Merci d'avoir joué!\n\n");
			
			//Fermeture des sockets
			CHECK(close(sockBase), "Problème close sock client 1 ");
			CHECK(close(sockConso), "Problème close sock client 2 ");
			CHECK(close(sockDialogueServeur), "Problème close sock client 4 ");
			
			exit(1);
		}
		
		//Permet de demander en duel un autre joueur
		else if(strcmp(cmd, "battle") == 0 && numberOfParams > 1)
		{ 
			strcpy(opponentName, arg);
			strcpy(MSG_CLIENT,"100 GET ");
			strcat(MSG_CLIENT,opponentName);

			if (strcmp(informationJoueur.username,opponentName) == 0)
			{
				printf("Tu ne peux pas te défier toi même...\n\n");
				flagForbidden = 1;
			}

			if (flagForbidden == 0)
			{
				reponse=dialClientToSrv(sockDialogueServeur, MSG_CLIENT);

				if (strcmp(reponse,"ERROR NOT FOUND") == 0)
				{
					printf("L'utilisateur n'est pas en ligne!\n\n");
					memset(&MSG_CLIENT, 0, MAX_CHAR);
					memset(&reponse, 0, MAX_CHAR);
				}
				else
				{
					sscanf(reponse, "%s %s", code, ipToConnect);
					pthread_create(&tid, NULL, (void*)threadPeerToPeer, ipToConnect); 
					memset(&MSG_CLIENT, 0, MAX_CHAR);
					memset(&reponse, 0, MAX_CHAR);
					pthread_exit(0);
				}
			}
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
				printf("\n\n");
				printf("Commandes: \n\n - list\n - battle <nomDuJoueur>\n - accept\n - deny\n - leave\n\n");
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
	//Pour envoyer des requêtes
	char MSG_CLIENT[MAX_CHAR];
	char buffer[MAX_CHAR]; 
	
	//Récupére les réponses
	char * reponse = (char *) malloc(sizeof(char) * MAX_CHAR);

	printf("\n");
	printf("Envoie de l'invitation...\n");	
	
	sockDialoguePeerToPeer = sessionClt();
	
	//Port de connexion normalement constante "PORT_CLT 60001" mais comme on test 
	//en localhost on peut pas ouvrir deux fois le port 60001, donc celui qui se fait défié
	//doit avoir mis un port 60001
	sockDialoguePeerToPeer = connectSrv(sockDialoguePeerToPeer, ip, PORT_CLT);
	
	strcpy(MSG_CLIENT,"200 BATTLE ");
	strcat(MSG_CLIENT,informationJoueur.username);
	reponse=dialClientToSrv(sockDialoguePeerToPeer, MSG_CLIENT);
	//printf("%s > %s\n", opponentName, reponse); Message de refus ou d'acceptation du duel
	memset(&MSG_CLIENT, 0, MAX_CHAR);
	memset(&reponse, 0, MAX_CHAR);
	
	printf("\n");
	
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
 * \fn void affichagePlateauDeJeu(const char plateau[3][3])
 * \brief Permet d'afficher l'état du jeu dans la fonction playBashorpion().
*/

void affichagePlateauDeJeu(const char plateau[3][3])
{
	int j,w = 0;
	
	printf("\n\n");
	
	// Affichage plateau
	for (j=0; j<3; j++)
	{
		for (w=0; w<3; w++)
		{
			if (w<2)
			{
				if (plateau[j][w] == 'X')
				{
					printf("\033[0;34m");
					printf(" %c ",plateau[j][w]);
					printf("\033[0m");
					printf("|");	
				}
				else if (plateau[j][w] == 'O')
				{
					printf("\033[0;31m");
					printf(" %c ",plateau[j][w]);
					printf("\033[0m");
					printf("|");
				}
				else
					printf(" %c |",plateau[j][w]);
			}
			else
			{
				if (plateau[j][w] == 'X')
				{
					printf("\033[0;34m");
					printf(" %c\n",plateau[j][w]);
					printf("\033[0m");
				}
				else if (plateau[j][w] == 'O')
				{
					printf("\033[0;31m");
					printf(" %c\n",plateau[j][w]);
					printf("\033[0m");
				}
				else
					printf(" %c\n",plateau[j][w]);
			}
		}
		if (j<2)
				printf("---+---+---\n");
	}
}


/**
 * \fn void playBashorpion(int socket, char * buffer, int playerID)
 * \brief Fonction permettant de jouer au morpion.
*/

void playBashorpion(int socket, char * buffer, int playerID)
{
	int dataSocket = socket;

	//Pour envoyer des requêtes
	char MSG_CLIENT[MAX_CHAR] = "";
	//Pour stocker les coups user
	char coups[MAX_CHAR] = "";
	
	printf("\n\n\n");
	printf("#######################\n");
	printf("- LE MORPION COMMENCE -\n");
	printf("#######################\n");

	int i = 0;
	int player = 0;
	int row = 0;                        
	int column = 0;                              
	int line = 0;                            
	int winner = 0;                             
	char plateau[3][3] = {                     // Le plateau de jeu                     
					   {'1','2','3'},          // Les valeurs permettent de choisir
					   {'4','5','6'},          // le coup à jouer
					   {'7','8','9'}           
					 };
				 

	//Le jeu continu tant qu'il n'y a pas de winner ou tant que les cases ne sont pas tous remplis.
	for( i = (0 + turn); i<(9 + turn) && winner==0; i++)
	{
		affichagePlateauDeJeu(plateau);
	  
		player = i%2 + 1;

		//On donne la main au bon joueur.
		do
		{
			if (player == playerID)
			{
				strcpy(MSG_CLIENT,"");
				strcpy(coups,"");
				strcpy(MSG_CLIENT,"");
				strcpy(MSG_CLIENT,"200 PLAY ");
				
				printf("\n");
				printf("%s, entrez l'emplacement de votre case à jouer %c: ", informationJoueur.username,(player==1)?'X':'O');
				scanf("%d", &coup);
				
				sprintf(coups, "%d", coup);
				strcat(MSG_CLIENT, coups);
				
				sendClientToClient(dataSocket, MSG_CLIENT);
			}
			else 
			{
				printf("\n");
				printf("En attente de %s...\n", opponentName);
				receiveClientToClient(dataSocket);
				printf("\033[1;32m%s >\033[0m Je choisis la case %d.\n", opponentName, coup);
			}

			row = --coup/3;                                
			column = coup%3;
			
		} while(coup<0 || coup>9 || plateau[row][column]>'9');

		plateau[row][column] = (player == 1) ? 'X' : 'O'; 

		//Conditions de victoire 
		if((plateau[0][0] == plateau[1][1] && plateau[0][0] == plateau[2][2]) ||
		 (plateau[0][2] == plateau[1][1] && plateau[0][2] == plateau[2][0]))
			winner = player;
		else
			for(line = 0; line <= 2; line ++)
				if((plateau[line][0] == plateau[line][1] && plateau[line][0] == plateau[line][2])||
				 (plateau[0][line] == plateau[1][line] && plateau[0][line] == plateau[2][line]))
					winner = player;

	}
	
	// Fin du jeu, plateau final
	affichagePlateauDeJeu(plateau);

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
		printf("L'adversaire %s gagne cette manche...\n", opponentName);
		printf("\033[1;32m%s >\033[0m Ahah t'es nul!\n", opponentName);
		opponentScore++;
	}
	
	printf("\n");
	printf("Ton score: %d\n", myScore);
	printf("Score adverse: %d\n",opponentScore);

	if (turn == 0 ) 
		turn++;
	else 
		turn--;

	//Demander si on fait une autre partie ou non
	printf("\n");
	printf("\033[1;32m%s >\033[0m Une autre partie peut-être? (accept/deny)\n", opponentName);
	fgetc(stdin);
	fgets(buffer, sizeof buffer, stdin);
	buffer[strlen(buffer)-1] = '\0';
	printf("\n");
	printf("En attente de %s...\n", opponentName);

	//On met le flag de la boucle isInGame à 1 ou 0 selon leurs choix
	isInGame=0;
	
	if (strcmp(buffer, "accept") == 0) 
		isInGame = 1;

	strcpy(MSG_CLIENT,"200 RE ");
	strcat(MSG_CLIENT,buffer);
	sendClientToClient(dataSocket, MSG_CLIENT);
	
	receiveClientToClient(dataSocket);
	
	if (strcmp(bufferRevanche, "accept") != 0 && isInGame==1)
	{
		printf("\n");
		printf("%s a refusé...\n", opponentName);
		isInGame = 0;
		myScore=0;
		opponentScore=0;
	}
	
	strcpy(MSG_CLIENT,"");
	strcpy(coups,"");
	strcpy(bufferRevanche,"");
	strcpy(buffer,"");
}


/**
 * \fn void intro()
 * \brief Fonction d'affichage qui accueille le client dans le launcher Bashorpion
*/

void intro()
{
	printf("\n");
	printf("###################################################################\n");
	printf("                                                                   \n");
	printf("\033[0;34mX\033[0m Bienvenue sur Bashorpion! Le morpion directement dans ton bash! \033[0;31mO\033[0m\n");
	printf("                                                                   \n");
	printf("###################################################################\n\n");
}


/**
 * \fn void introLobby()
 * \brief Fonction d'affichage qui accueille le client dans un lobby Bashorpion
*/
void introLobby()
{
	printf("\n");
	printf("###################################################################\n");
	printf("Bienvenue dans le lobby Bashorpion!                                \n");
	printf("###################################################################\n");
	printf("\n\n");
	printf("Commandes: \n\n - list\n - battle <nomDuJoueur>\n - accept\n - deny\n - leave\n\n");
}


int main()
{
	intro();
	client();
	printf("Fin du client Bashorpion.\n");
	
	return 0;
}
