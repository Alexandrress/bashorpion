/*! \mainpage LEVEQUE Alexandre & SHAHPAZIAN Nicolas - Projet MCS
 * \section intro_sec Introduction
 * Bienvenue sur cette page du manuel Doxygen du "Projet CMS : Bashorpion" fait par Alexandre.L & Nicolas.S
 * - \subpage Description "Application Client/Serveur de Morpion codée en C, avec des threads ainsi que des sockets INET. Deux joueurs minimums sont requis ainsi que le lancement du serveur permettant l'enregistrement des informations afin de jouer une partie."
*/

/**
 * \file BashorpionServer.c
 * \brief Programme permettant de lancer le serveur du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
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
void * serverFils(void *);

#define CAPACITE_SERVER 10


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
	int sd, /*pid,*/ i;
	struct sockaddr_in clientAdr;
	int overcharged=0; /*Surcharge du server : 0 <=> server disponible
												1 <=> serveur en capacité max, ne peut plus prendre de clients*/
	//struct sockaddr_in clientAdr;
	pthread_t *tab_thread; //tableau de Threads de dialogue
	//pthread_t threadErr; //Thread de secours utilisé pour informer le client de la surcharge du serveur (capacité max de clients connectés atteinte)
	
	printf("Démarrage de l'application serveur\n");
	
	//Allocation mémoire pour les threads de dialogue
	tab_thread = (pthread_t*)malloc(CAPACITE_SERVER*sizeof(pthread_t));
	
	acquitterFinClient();
	sockINET = sessionSrv();
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		
		i=0;
		while (&tab_thread[i] == NULL){
			i++;
			if (i == CAPACITE_SERVER){
				printf("Serveur surchargé, ne peut plus prendre de demandes de connexion\n");
				overcharged = 1;
				break;
			}
		}
		
		
		//Création d'un thread de dialogue pour communiquer avec le client
		if (overcharged == 1) //Envoi de message de surcharge au client
			/*CHECK(pthread_create());*/ printf("Overcharged\n");
		else
			CHECK(pthread_create(&tab_thread[i], NULL, serverFils, (void *) &sd), "PB - pthreadCreate"); //Création d'un thread de server fils
		/*
		 * A FINIR !!!!!
		 * */
		
		CHECK(close(sd), "Problème close sock dialogue serveur fils ");
		
		/*
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
		CHECK(close(sd), "Problème close sock dialogue serveur fils ");	*/
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur père ");
}


void * serverFils(void *arg){
	int* sd = (int*)arg;
	printf("Ouais je suis le serveur fils : %d\n", *sd);
	
	//On commence par traiter la requête
	
	//On envoie la réponse appropriée
}



int main()
{
	//Thread principal (main)
	struct sigaction newact;

    newact.sa_handler = deroute;
	CHECK(sigemptyset(&newact.sa_mask),"Problème sigemptyset() pour SIGINT ");
	newact.sa_flags = SA_RESTART;
	CHECK(sigaction(SIGINT, &newact, NULL), "Appel de sigaction() pour SIGINT ");

	serveur();
	printf("Fin d'application.\n");
	
	return 0;
}
