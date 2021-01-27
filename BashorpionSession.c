/**
 * \file BashorpionSession.c
 * \brief Fichier "BashorpionSession.c" contenant les fonctions de sessions des
 clients et du serveur. Concerne la couche 5 du modèle OSI (Session).
 * \author Alexandre.L & Nicolas.S
 * \version 4.0
 * \date 25 Janvier 2021
 *
*/

// ************ BIBLIOTHEQUES ************

#include "BashorpionSession.h"


// ************ FONCTIONS ************

/**
 * \fn int sessionSrv(int portNumber, int nbDeClients)
 * \brief Permet de créer une nouvelle session pour un serveur en spécifiant le port, et le nb MAX de clients.
*/

int sessionSrv(int portNumber, int nbDeClients)
{
	int sockINET;
	struct sockaddr_in sockAdr;
	
	//Création socket INET STREAM (nécessite une connection)
	CHECK(sockINET = socket(PF_INET, SOCK_STREAM, 0), "Problème sock serveur ");
	printf("Numéro de canal créé pour la socket serveur : [%d]\n\n", sockINET);

	//Préparation d'un adressage pour une socket INET
	sockAdr.sin_family = PF_INET;
	sockAdr.sin_port = htons(portNumber);
	sockAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(sockAdr.sin_zero), 0, 8);

	//Association de la socket avec l'adresse
	//Utilisation à base d'un cast
	CHECK(bind(sockINET, (struct sockaddr *)&sockAdr, sizeof(sockAdr)), "Problème bind serveur ");
	
	//Mise de la socket à l'écoute
	CHECK(listen(sockINET, nbDeClients),"Problème listen() "); //5 est le nb de clients mis en attente
	
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
 * \fn int connectSrv(int sockINET, char* serverIP, int portNumber)
 * \brief Permet au client de se connecter au serveur.
*/

int connectSrv(int sockINET, char* serverIP, int portNumber)
{
	struct sockaddr_in adrSrv;

	adrSrv.sin_family = PF_INET;
	adrSrv.sin_port= htons(portNumber);
	adrSrv.sin_addr.s_addr = inet_addr(serverIP);
	memset(&(adrSrv.sin_zero), 0, 8);
	
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
	CHECK(sd=accept(sockINET, (struct sockaddr *)&clientAdr, &lenClt), "Problème accept serveur ");
	return sd;
}


/**
 * \fn void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
 * \brief Permet au serveur de dialoguer avec le client.
*/

void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
{
	message_t buff;
	requete_t * reqClient;
	reponse_t * repSrv;
	socklen_t lenClt;

	lenClt = sizeof(adresseClient);
	//Dialogue avec le client
	memset(&buff, 0, MAX_CHAR);

	printf("Attente de réception d'un message...\n");
	CHECK(recvfrom(socketDialogue, buff, MAX_CHAR, 0, (struct sockaddr *)&adresseClient, &lenClt), "Problème recv serveur ");
	printf("Requête reçue !\n\n");
	
	reqClient = stringToReq(buff); //On transforme le string en requête pour traiter
	repSrv=traiterRequest(reqClient); //On génére une réponse qu'on renvoit
	
	sendReponse(socketDialogue, repSrv);
}


/**
 * \fn char * dialClientToSrv(int sockINET, const char * MSG)
 * \brief Permet au client de dialoguer avec le serveur.
*/

char * dialClientToSrv(int sockINET, const char * MSG)
{
	socklen_t lenSockAdr;
	requete_t *reqClient;
	reponse_t * repSrv;
	struct sockaddr_in sockAdr;
	
	char *buff = (char *) malloc(sizeof(char) * MAX_CHAR);
	char *reponse = (char *) malloc(sizeof(char) * MAX_CHAR);

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
	CHECK(getsockname(sockINET, (struct sockaddr *)&sockAdr, &lenSockAdr),"Problème getsockname() ");
	
	//Attente d'une réponse
	memset(buff, 0, MAX_CHAR);
	CHECK(recv(sockINET, buff, MAX_CHAR, 0),"Problème recv() ");
	
	repSrv = stringToRep(buff); //On transforme le string en réponse pour traiter
	reponse = traiterReponse(repSrv); //On génére ce qu'on affiche.
	
	return(reponse);
}


/**
 * \fn void sendClientToClient(int sockINET, const char * MSG)
 * \brief Permet au client d'envoyer des messages à son peer.
*/

void sendClientToClient(int sockINET, const char * MSG)
{
	requete_t *reqClient;

	//Envoi d'un message à un destinataire
	char code[MAX_CHAR]="";
	action_t action;
	message_t msg;
	short codeShort=0;
	
	sscanf(MSG, "%s %s %s\n", code, action, msg);
	codeShort = atoi(code);
	
	reqClient = createRequete(codeShort, action, msg);
	sendRequete(sockINET, reqClient);
}


/**
 * \fn void receiveClientToClient(int socketDialogue)
 * \brief Permet au client de recevoir des messages de son peer.
*/

void receiveClientToClient(int socketDialogue)
{
	message_t buff;
	requete_t * reqClient;

	//Dialogue avec le client
	memset(&buff, 0, MAX_CHAR);

	printf("Attente de réception d'un message...\n");
	CHECK(recv(socketDialogue, buff, MAX_CHAR, 0), "Problème recv serveur ");
	
	reqClient = stringToReq(buff); //On transforme le string en requête pour traiter
	traiterRequest(reqClient); //On traite la requête
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
	//printf("Envoi du message INET suivant %s\n",msg);
	CHECK(send(sock, msg, strlen(msg) + 1, 0), "Problème send du client ");
	return(0);
}


/**
 * \fn int sendReponse(const int sock, const reponse_t *rep)
 * \brief Permet d'envoyer la réponse passé en paramètre à la socket sock.
*/

int sendReponse(const int sock, const reponse_t *rep)
{
	message_t msg;
	repToString(rep, msg);
	
	//Envoi d'un message à un destinataire
	//printf("Envoi du message INET suivant %s\n",msg);
	CHECK(send(sock, msg, strlen(msg) + 1, 0), "Problème send du serveur ");
	return(0);
}

