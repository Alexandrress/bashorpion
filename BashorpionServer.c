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
	int sd;
	
	sockINET = sessionSrv(PORT_SRV, 6);
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		pthread_create(&tid, NULL, threadLobbyServer, (void *) sd);
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur père ");
}

void * threadLobbyServer(void * socketDialogue)
{
	int socket, positionJoueur = nbPlayer;
	socket = (int) socketDialogue;
	char ipDuJoueur[MAX_CHAR];
	
	struct sockaddr_in clientAdrCom = clientAdr;
	
	struct sockaddr_in peeraddr;
	socklen_t peeraddrlen = sizeof(peeraddr);
	getpeername(socket, &peeraddr, &peeraddrlen);
	inet_ntop(AF_INET, &(peeraddr.sin_addr), ipDuJoueur, MAX_CHAR);
	
	//On copie l'adresse IP du joueur ainsi que son nom.
	dialSrvToClient(socket, &clientAdrCom);
	strcpy(informationDesJoueurs[positionJoueur].ipUser,ipDuJoueur);
	strcpy(informationDesJoueurs[positionJoueur].username,userToAdd);
	
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
