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

void deroute(int signal_number);
void acquitterFinClient(void);
void nothing(void);
void fin(void);
void serveur();


// ******** VARIABLES GLOBALES ********

int sockINET;


// ************ FONCTIONS ************

/**
 * \fn void deroute(int signal_number)
 * \brief Fonction permettant de dérouter les signaux
*/

void deroute(int signal_number)
{
	int status;
	int pidClient;

	switch (signal_number)
	{	
		case SIGCHLD :
			CHECK(pidClient = wait(&status),"Problème acquittement fils ");
			printf("Le processus %d s'est terminé.\n",pidClient);
			break;
		case SIGINT :
			fin();
			exit(0);
			break;
		default :
			break;
	}
}


/**
 * \fn void acquitterFinClient(void)
 * \brief Fonction permettant d'acquitter les serveurs fils
*/

void acquitterFinClient(void)
{
	struct sigaction newact;
	newact.sa_handler = deroute;
	CHECK(sigemptyset(&newact.sa_mask),"Problème sigemptyset() pour SIGCHLD ");
	newact.sa_flags = SA_RESTART;
	CHECK(sigaction(SIGCHLD, &newact, NULL), "Appel de sigaction() pour SIGCHLD ");
}


/**
 * \fn void fin(void)
 * \brief Fonction permettant de fermer la socket 
*/

void fin(void)
{
	//Fermeture de socket
	CHECK(close(sockINET),"Problème de close() socket pour le serveur père ");
}


void nothing(void)
{
	return;
}


/**
 * \fn void serveur()
 * \brief Fonction principale du serveur permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void serveur()
{
	int sd, pid;
	struct sockaddr_in clientAdr;
	//struct sockaddr_in sockAdr;
	
	acquitterFinClient();
	sockINET = sessionSrv(PORT_SRV, 6);
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		CHECK(pid = fork(), "Problème fork() ");
		if (pid == 0)
		{
			atexit(nothing);
			CHECK(close(sockINET), "Problème close sock serveur fils "); //Le fils a pas besoin
			//Dialogue avec le client
			dialSrvToClient(sd, &clientAdr);
			CHECK(close(sd), "Problème close sock dialogue serveur fils ");		
			exit(0);
		}
		CHECK(close(sd), "Problème close sock dialogue serveur fils ");	
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur père ");
}


int main()
{
	struct sigaction newact;

    newact.sa_handler = deroute;
	CHECK(sigemptyset(&newact.sa_mask),"Problème sigemptyset() pour SIGINT ");
	newact.sa_flags = SA_RESTART;
	CHECK(sigaction(SIGINT, &newact, NULL), "Appel de sigaction() pour SIGINT ");

	serveur();
	printf("Fin d'application.\n");
	
	return 0;
}
