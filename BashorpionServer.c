/*! \mainpage LEVEQUE Alexandre & SHAHPAZIAN Nicolas - Projet MCS
 * \section intro_sec Introduction
 * Bienvenue sur cette page du manuel Doxygen du "Projet CMS : Bashorpion" fait par Alexandre.L & Nicolas.S
*/

/**
 * \file BashorpionServer.c
 * \brief Programme permettant de lancer le serveur du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 1.0
 * \date 07 Janvier 2021
 *
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionLib.h"

void serveur();
void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient);
int sessionSrv();
int acceptClt(int sockINET, struct sockaddr_in *clientAdr);


// ************ FONCTIONS ************

/**
 * \fn void serveur()
 * \brief Fonction principale du serveur permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void serveur()
{
	int sockINET, sd, pid;
	struct sockaddr_in sockAdr, clientAdr;
	
	sockINET = sessionSrv();
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		CHECK(pid = fork(), "Problème fork() ");
		if (pid == 0)
		{
			CHECK(close(sockINET), "Problème close sock serveur "); //Le fils a pas besoin
			//Dialogue avec le client
			dialSrvToClient(sd, &clientAdr);
			CHECK(close(sd), "Problème close sock dialogue serveur ");		
			exit(0);
		}
		CHECK(close(sd), "Problème close sock dialogue serveur ");	
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur ");
}


/**
 * \fn int acceptClt(int sockINET, struct sockaddr_in *clientAdr)
 * \brief Permet au serveur d'accepter la connection client.
*/

int acceptClt(int sockINET, struct sockaddr_in *clientAdr)
{
	int sd;
	socklen_t lenClt;
		
	lenClt = sizeof(clientAdr);
	//Attente de connexion client
	CHECK(sd=accept(sockINET, (struct sockaddr *)&clientAdr, &lenClt), "Problème bind serveur ");
	return sd;
}


/**
 * \fn int sessionSrv()
 * \brief Permet de créer une nouvelle session pour un serveur.
*/

int sessionSrv()
{
	int sockINET;
	struct sockaddr_in sockAdr;
	
	//Création socket INET STREAM (nécessite une co)
	CHECK(sockINET = socket(PF_INET, SOCK_STREAM, 0), "Problème sock serveur ");
	printf("Numéro de canal créé pour la socket serveur : [%d]\n\n", sockINET);

	//Préparation d'un adressage pour une socket INET
	sockAdr.sin_family = PF_INET;
	sockAdr.sin_port = htons(PORT_SRV); //Allocation dynamique d'un numéro de port
	//pas de htons() pour test
	sockAdr.sin_addr.s_addr = inet_addr(ADDR_SRV); //OU INADDRY_ANY POUR TOUTE INTERFACE DE LA MACHINE
	memset(&(sockAdr.sin_zero), 8, 0);
	
	//strcpy(sockAdr.sun_path, SOCK_NAME);

	//Association de la socket avec l'adresse
	//Utilisation à base d'un cast
	CHECK(bind(sockINET, (struct sockaddr *)&sockAdr, sizeof(sockAdr)), "Problème bind serveur ");
	
	//Mise de la socket à l'écoute
	CHECK(listen(sockINET, 5),"Problème listen() "); //5 est le nb de clients mis en attente
	
	return(sockINET);
}


/**
 * \fn void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
 * \brief Permet au serveur de dialoguer avec le client
*/

void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
{
	int req;
	message_t buff;
	requete_t * reqClient;
	socklen_t lenClt;

	lenClt = sizeof(adresseClient);
	//Dialogue avec le client
	memset(&buff, 0, MAX_BUFF);
	//lenClt = sizeof(clientAdr);
	printf("Attente de réception d'un message\n");
	CHECK(recvfrom(socketDialogue, buff, MAX_BUFF, 0, (struct sockaddr *)&adresseClient, &lenClt), "Problème recv serveur ");
	reqClient = stringToReq(buff);
	traiterRequest(reqClient); 
	//printf("\tMessage reçu : \"%s\"\n", buff);
	//printf("\tpar le canal %s\n\n", clientAdr.sun_path);
	sscanf(reqClient->params,"%d",&req);
	if (req==1)
	{
		CHECK(send(socketDialogue, MSG_SRV1, strlen(MSG_SRV1) + 1, 0), "Problème send du serveur ");
	}
	else
	{
		CHECK(send(socketDialogue, MSG_SRV2, strlen(MSG_SRV2) + 1, 0), "Problème send du serveur ");
	}
	CHECK(shutdown(socketDialogue, SHUT_WR),"-- PB : shutdown()");
	sleep(1);
}

int main()
{
	serveur();
	printf("Fin d'application.\n");
	
	return 0;
}
