/*! \mainpage LEVEQUE Alexandre & SHAHPAZIAN Nicolas - Projet MCS
 * \section intro_sec Introduction
 * Bienvenue sur cette page du manuel Doxygen du "Projet CMS : Bashorpion" fait par Alexandre.L & Nicolas.S
 * - \subpage Description "Application Client/Serveur de Morpion codée en C, avec des threads ainsi que des sockets INET. Deux joueurs minimums sont requis ainsi que le lancement du serveur permettant l'enregistrement des informations afin de jouer une partie."
*/

/**
 * \file BashorpionServer.c
 * \brief Programme permettant de lancer le serveur du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 3.0
 * \date 21 Janvier 2021
 *
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"
#include "BashorpionSession.h"
#include "BashorpionReqRep.h"

void serveur();
void * threadLobbyServer(void * socketDialogue);

// ******** VARIABLES GLOBALES ********

int sockINET;
struct sockaddr_in clientAdr;
pthread_t tid;

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
	for (i=0 ; i < CAPACITE_SERVER ; i++){
		memset(usersDatas[i].ipUser, 0, sizeof(usersDatas[i].ipUser));
		memset(usersDatas[i].username, 0, sizeof(usersDatas[i].username));
		usersDatas[i].portIP = 0; //Valeurs pemrettant de considérer le slot comme vide
	}
	
	printf("\n");
	printf("Toutes les structures d'info users ont été réinitialisées !\n");
	
	printf("------ Affichage des slots : \n");
	for (i=0 ; i < CAPACITE_SERVER ; i++){
		printf("Slot %d : \n- username = >%s<\n- ipUser = >%s<\n- portIP = >%d<\n\n", i, usersDatas[i].username, usersDatas[i].ipUser, usersDatas[i].portIP);
	}
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Recherche d'un emplacement de thread disponible pour la création d'un thread de service
		while (&tab_thread[i] == NULL) i++;
		
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		pthread_create(&(tab_thread[i]), NULL, threadLobbyServer, (void *) sd);
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur père ");
}



void * threadLobbyServer(void * socketDialogue)
{
	
	int i, socket, positionJoueur = nbPlayer;
	socket = (int) socketDialogue;
	char ipDuJoueur[MAX_CHAR];
	struct sockaddr_in clientAdrCom = clientAdr;
	struct sockaddr_in peeraddr;
	
	socklen_t peeraddrlen = sizeof(peeraddr);
	
	getpeername(socket, &peeraddr, &peeraddrlen);
	inet_ntop(AF_INET, &(peeraddr.sin_addr), ipDuJoueur, MAX_CHAR);
	
	//On copie l'adresse IP du joueur ainsi que son nom.
	dialSrvToClient(socket, &clientAdrCom);
	
	//On détermine le 1er emplacement libre auquel le joueur peut être rangé
	i=0;
	while (strcmp(usersDatas[i].username,"")) i++; //True tant que usersDatas[i].username n'est pas vide
	//Remplissage de la structure d'infos utilisateurs
	strcpy(usersDatas[i].ipUser, ipDuJoueur);
	strcpy(usersDatas[i].username, userToAdd);
	printf("Utilisateur ajouté sur le slot %d : \n", i);
	printf("- username = %s\n- ipUser = %s\n", usersDatas[i].username, usersDatas[i].ipUser);
	
	//printf("\n\n");
	//printf("------ Affichage des slots : \n");
	//~ for (i=0 ; i < CAPACITE_SERVER ; i++){
		//~ printf("Slot %d : \n- username = >%s<\n- ipUser = >%s<\n- portIP = >%d<\n\n", i, usersDatas[i].username, usersDatas[i].ipUser, usersDatas[i].portIP);
	//~ }
	
	while(1)
	{
		//Dialogue avec le client
		dialSrvToClient(socket, &clientAdrCom);
	}
	
	pthread_exit(0);
}

int main()
{
	serveur();
	printf("Fin d'application.\n");
	
	return 0;
}
