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
void intro();

// ******** VARIABLES GLOBALES ********

int sockDialogueServeur;
char serverIP[MAX_CHAR];
infoUser_t informationJoueur;
pthread_t  tid; /*Thread ID*/  
char MSG[MAX_CHAR] = "1 : Ceci est un message client!";

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
	
	//Port client utilisé pour Bashorpion
	informationJoueur.portIP = PORT_CLT;
	
	printf("\n");
	printf("Entrez l'adresse du serveur du lobby Bashorpion : ");
	scanf("%s", serverIP);

	sockDialogueServeur = sessionClt();
	sockDialogueServeur = connectSrv(sockDialogueServeur, serverIP);
	pthread_create(&tid, NULL, (void *)threadComServeur, (void *) &sockDialogueServeur);  
	
	//Incoming connections autres joueurs
	
	while(1){
	
	}
	
	//Fermeture du socket
	CHECK(close(sockDialogueServeur), "Problème close sock client ");
}

/**
 * \fn void * threadComServeur(void * obj)
 * \brief Thread du client permettant de communiquer avec le serveur lobby
*/
void threadComServeur(void * obj)
{
	int flagInstructions=0;

	/* Permet de close le thread d'un autre thread. */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	/* Pour stocker les inputs user*/
	char MSG_CLIENT[MAX_CHAR] = "100 PUT ";
	char buffer[MAX_CHAR]; 
	
	//On copie le pseudo dans dans le message de connexion
	strcat(MSG_CLIENT,informationJoueur.username);
	
	//On se connecte en envoyant nos informations
	dialClientToSrv(sockDialogueServeur, MSG_CLIENT);
	
	/* Instructions lobby */
	printf("\n");
	printf("Commandes: \n\n - list\n - battle <nomDuJoueur>\n - accept\n - deny\n - exit\n\n");

	memset(&MSG_CLIENT, 0, MAX_CHAR);
	while(1)
	{
		// Lit l'input du client
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer)-1] = '\0';		
		char cmd[MAX_CHAR];
		sscanf(buffer, "%s", cmd);
		
		// Permet d'avoir la liste des joueurs.
		if(strcmp(buffer, "list") == 0)
		{
			strcpy(MSG_CLIENT,"100 GET liste");
			dialClientToSrv(sockDialogueServeur, MSG);
			memset(&MSG_CLIENT, 0, MAX_CHAR);
		}
		
		// Permet de quitter le serveur et de fermer la socket de dialogue.
		else if(strcmp(buffer, "exit") == 0)
		{
			strcpy(MSG_CLIENT,"100 DELETE exit");
			dialClientToSrv(sockDialogueServeur, MSG);
			memset(&MSG_CLIENT, 0, MAX_CHAR);
			
			printf("Aurevoir! Merci d'avoir joué!\n");
			close(sockDialogueServeur);
			exit(1);
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
	intro();
	client();
	printf("Fin du client Bashorpion.\n");
	
	return 0;
}
