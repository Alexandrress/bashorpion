/**
 * \file BashorpionSession.c
 * \brief Fichier "BashorpionSession.c" contenant les fonctions de sessions des
 clients et du serveur. Concerne la couche 5 du modèle OSI (Session).
 * \author Alexandre.L & Nicolas.S
 * \version 3.0
 * \date 21 Janvier 2021
 *
*/

// ************ BIBLIOTHEQUES ************

#include "BashorpionSession.h"


// ************ FONCTIONS ************

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
	memset(&(sockAdr.sin_zero), 0, 8);
	
	//strcpy(sockAdr.sun_path, SOCK_NAME);

	//Association de la socket avec l'adresse
	//Utilisation à base d'un cast
	CHECK(bind(sockINET, (struct sockaddr *)&sockAdr, sizeof(sockAdr)), "Problème bind serveur ");
	
	//Mise de la socket à l'écoute
	CHECK(listen(sockINET, 5),"Problème listen() "); //5 est le nb de clients mis en attente
	
	return(sockINET);
}


/**
 * \fn int sessionClt()
 * \brief Permet de créer une nouvelle session pour un client.
*/

int sessionClt()
{
	int sockINET;
	
	//Création socket INET STREAM
	CHECK(sockINET = socket(PF_INET, SOCK_STREAM, 0), "Problème sock client ");
	
	return sockINET;
}


/**
 * \fn int connectSrv(int sockINET, char* serverIP)
 * \brief Permet au client de se connecter au serveur.
*/

int connectSrv(int sockINET, char* serverIP)
{
	struct sockaddr_in adrSrv;

	adrSrv.sin_family = PF_INET;
	adrSrv.sin_port= htons(PORT_SRV);
	adrSrv.sin_addr.s_addr = inet_addr(serverIP);
	memset(&(adrSrv.sin_zero), 0, 8); // ---
	
	//Demande connexion
	CHECK(connect(sockINET, (struct sockaddr *)&adrSrv, sizeof(adrSrv)),"Problème connect() ");
	
	return(sockINET);
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
	memset(&buff, 0, MAX_CHAR);
	//lenClt = sizeof(clientAdr);
	printf("Attente de réception d'un message\n");
	CHECK(recvfrom(socketDialogue, buff, MAX_CHAR, 0, (struct sockaddr *)&adresseClient, &lenClt), "Problème recv serveur ");
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


/**
 * \fn void dialClientToSrv(int sockINET, const char * MSG)
 * \brief Permet au client de dialoguer avec le serveur
*/

void dialClientToSrv(int sockINET, const char * MSG)
{
	socklen_t lenSockAdr;
	requete_t *reqClient;
	struct sockaddr_in sockAdr;
	message_t buff;

	//Envoi d'un message à un destinataire
	char code[MAX_CHAR]="";
	action_t action;
	message_t msg;
	short codeShort=0;
	
	sscanf(MSG, "%s %s %s\n", code, action, msg);
	codeShort = atoi(code);
	
	reqClient = createRequete(codeShort, action, msg);
	sendRequete(sockINET, reqClient);

	lenSockAdr = sizeof(sockAdr);
	CHECK(getsockname(sockINET, (struct sockaddr *)&sockAdr, &lenSockAdr),"Problème getsockname()");
	
	//Attente d'une réponse
	memset(&buff, 0, MAX_CHAR);
	CHECK(recv(sockINET, buff, MAX_CHAR, 0),"Problème recv() ");
	
	printf("Réponse serveur : %s\n",buff);
}

/**
 * \fn int sendRequete(const int sock, const requete_t *req);
 * \brief Permet d'envoyer la requête passé en paramètre à la socket sock.
*/


int sendRequete(const int sock, const requete_t *req)
{
	message_t msg;
	reqToString(req, msg);
	
	//Envoi d'un message à un destinataire
	printf("Envoi d'un mesage INET.\n");
	CHECK(send(sock, msg, strlen(msg) + 1, 0), "Problème send du client ");
	return(0);
}


/**
 * \fn int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg);
 * \brief Permet d'envoyer le message passé en paramètre à la socket sock
*/


int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg)
{
	//Envoi d'un message à un destinataire
	printf("Envoi d'un mesage INET.\n");
	CHECK(sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr *)adr, sizeof(*adr)), "Problème sendto du client ");
	return(0);
}
