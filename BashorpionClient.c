/**
 * \file BashorpionClient.c
 * \brief Programme permettant de lancer un client du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 1.0
 * \date 07 Janvier 2021
 *
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionLib.h"

void client(const message_t MSG);
void dialClientToSrv(int sockINET, const char * MSG);
int sessionClt();
void connectSrv(int sockINET);


// ************ FONCTIONS ************

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
 * \fn void connectSrv(int sockINET)
 * \brief Permet au client de se connecter au serveur.
*/

void connectSrv(int sockINET)
{
	struct sockaddr_in adrSrv;

	//Le client n'a pas besoin d'avoir une adresse donc
	//pas de préparation ni d'association.
	//Par contre le client doit fournir l'adresse du
	//serveur.

	adrSrv.sin_family = PF_INET;
	adrSrv.sin_port= htons(PORT_SRV);
	adrSrv.sin_addr.s_addr = inet_addr(ADDR_SRV); //boucle locale et format réseau
	memset(&(adrSrv.sin_zero), 8, 0);
	
	//Demande connexion
	CHECK(connect(sockINET, (struct sockaddr *)&adrSrv, sizeof(adrSrv)),"Problème connect() ");
}


/**
 * \fn void dialClientToSrv(int sockINET, const char * MSG)
 * \brief Permet au client de dialoguer avec le serveur
*/

void dialClientToSrv(int sockINET, const char * MSG)
{
	int lenSockAdr;
	requete_t *reqClient;
	struct sockaddr_in adrSrv, sockAdr;
	message_t buff;

	//Envoi d'un message à un destinataire
	
	reqClient = createRequete(100, "SHOW", MSG);
	sendRequete(sockINET, &adrSrv, reqClient);
	
	//printf("Envoi d'un mesage sur [%s]\n", SOCK_NAME);
	//CHECK(sendto(sockUNIX, MSG, strlen(MSG) + 1, 0, (struct sockaddr *)&adrSrv, sizeof(adrSrv)), "Problème sendto du client ");

	lenSockAdr = sizeof(sockAdr);
	CHECK(getsockname(sockINET, (struct sockaddr *)&sockAdr, &lenSockAdr),"Problème getsockname()");
	
	//Attente d'une réponse
	memset(&buff, 0, MAX_BUFF);
	CHECK(recv(sockINET, buff, MAX_BUFF, 0),"Problème recv() ");
	
	printf("Réponse serveur : %s\n",buff);
}


/**
 * \fn void client(const message_t MSG)
 * \brief Fonction principale du client permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void client(const message_t MSG)
{
	int sockINET;
	
	sockINET = sessionClt();

	connectSrv(sockINET);
	
	dialClientToSrv(sockINET, MSG);
	
	getchar();
	
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock client ");
}

int main()
{
	client(MSG_CLT1);
	printf("Fin d'application.\n");
	
	return 0;
}
