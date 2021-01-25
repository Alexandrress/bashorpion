/*! \mainpage LEVEQUE Alexandre & SHAHPAZIAN Nicolas - Projet MCS
 * \section intro_sec Introduction
 * Bienvenue sur cette page du manuel Doxygen du "Projet CMS : Bashorpion" fait par Alexandre.L & Nicolas.S
 * - \subpage Description "Application Client/Serveur de Morpion codée en C, avec des threads ainsi que des sockets INET. Deux joueurs minimums sont requis ainsi que le lancement du serveur permettant l'enregistrement des informations afin de jouer une partie."
*/

/**
 * \file BashorpionServer.c
 * \brief Programme permettant de lancer le serveur d'authentification du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 4.0
 * \date 25 Janvier 2021
 *
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"
#include "BashorpionSession.h"
#include "BashorpionReqRep.h"

void serveur();
void * threadLobbyServer(void * socketDialogue);
void printAffichageUsers();

// ******** VARIABLES GLOBALES ********

int sockINET;
struct sockaddr_in clientAdr;

// ************ FONCTIONS ************

/**
 * \fn void serveur()
 * \brief Fonction principale du serveur permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void serveur()
{
	int sd, i;
	pthread_t *tab_thread; //tableau de Threads de dialogue
	
	//Allocation mémoire pour les threads de dialogue
	tab_thread = (pthread_t*)malloc(CAPACITE_SERVER*sizeof(pthread_t));
	
	sockINET = sessionSrv(PORT_SRV, CAPACITE_SERVER);
	
	//Init struct infoUsers
	for (i=0 ; i < CAPACITE_SERVER ; i++)
	{
		memset(usersDatas[i].ipUser, 0, sizeof(usersDatas[i].ipUser));
		memset(usersDatas[i].username, 0, sizeof(usersDatas[i].username));
		usersDatas[i].portIP = 0; //Valeurs pemrettant de considérer le slot comme vide
	}
	
	printf("\n");
	
	//Affiche la structure des users du serveur
	printAffichageUsers();
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Recherche d'un emplacement de thread disponible pour la création d'un thread de service
		while (&tab_thread[i] == NULL) 
			i++;
		
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		pthread_create(&(tab_thread[i]), NULL, threadLobbyServer, (void *) &sd);
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur père ");
}


/**
 * \fn void * threadLobbyServer(void * socketDialogue)
 * \brief Fonction de thread permettant de d'enregistrer le client et de dialoguer avec
*/

void * threadLobbyServer(void * socketDialogue)
{
	int i, socket;
	int * sockTemp;
	sockTemp = (int*) socketDialogue;
	socket = *sockTemp;
	char ipDuJoueur[MAX_CHAR];

	struct sockaddr_in clientAdrCom = clientAdr;
	socklen_t peeraddrlen = sizeof(clientAdrCom);
	
	getpeername(socket, (struct sockaddr *)&clientAdrCom, &peeraddrlen);
	inet_ntop(AF_INET, &(clientAdrCom.sin_addr), ipDuJoueur, MAX_CHAR);
	
	//On copie l'adresse IP du joueur ainsi que son nom.
	dialSrvToClient(socket, &clientAdrCom);
	
	//On détermine le 1er emplacement libre auquel le joueur peut être rangé
	i=0;
	
	while (strcmp(usersDatas[i].username,"")) 
		i++; //True tant que usersDatas[i].username n'est pas vide

	//Remplissage de la structure d'infos utilisateurs
	strcpy(usersDatas[i].ipUser, ipDuJoueur);
	strcpy(usersDatas[i].username, userToAdd);
	
	printf("\n");
	printf("\033[1;32m"); //Couleur verte
	printf("Utilisateur ajouté sur le slot %d : \n", i);
	printf("\033[0m"); //Couleur blanche
	printf("- username = %s\n- ipUser = %s\n\n", usersDatas[i].username, usersDatas[i].ipUser);
	
	while(strcmp(usersDatas[i].username, "") != 0) //Tant que l'user est enregistré
	{
		//Dialogue avec le client
		dialSrvToClient(socket, &clientAdrCom);
	}
	
	close(socket);
	pthread_exit(0);
}


/**
 * \fn void printAffichageUsers()
 * \brief Permet d'afficher toute la structure usersDatas du serveur.
*/

void printAffichageUsers()
{
	int i;
	
	printf("\033[1;32m"); //Couleur verte
	printf("------ Affichage des slots : \n\n");
	printf("\033[0m"); //Couleur blanche
	
	for (i=0 ; i < CAPACITE_SERVER ; i++)
	{
		printf("\033[1;32m Slot %d : \033[0m \n- username = >%s<\n- ipUser = >%s<\n- portIP = >%d<\n\n", i, usersDatas[i].username, usersDatas[i].ipUser, usersDatas[i].portIP);
	}
}

int main()
{
	serveur();
	printf("Fin d'application.\n");
	
	return 0;
}
