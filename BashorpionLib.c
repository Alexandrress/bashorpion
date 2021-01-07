/**
 * \file BashorpionLib.c
 * \brief Fichier "Bashorpion.c" contenant les fonctions
 de la couche applicative de notre projet MCS.
 * \author Alexandre.L & Nicolas.S
 * \version 1.0
 * \date 07 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionLib.h"

// ************ FONCTIONS ************

/**
 * \fn requete_t * createRequete(short no, action_t act, const message_t myParams);
 * \brief Permet de créer une nouvelle requête en spécifiant le code, l'action
 et le message.
*/

requete_t * createRequete(short no, action_t act, const message_t myParams)
{
	requete_t * myReq = (requete_t *) malloc(sizeof(requete_t));
	myReq->noReq = no;
	strcpy(myReq->action, act);
	strcpy(myReq->params, myParams);
	return(myReq);
}


/**
 * \fn char * reqToString(const requete_t *req, message_t msg);
 * \brief Décompose une requête en chaîne de caractère.
*/

char * reqToString(const requete_t *req, message_t msg)
{	
	//Serialization d'une requête sous forme d'une chaîne de caractères
	//c-à-d une suite d'octets
	memset(msg,0,MAX_BUFF);
	sprintf(msg,"%hd:%s:%s", req->noReq, req->action, req->params);
	return(msg);
} 


/**
 * \fn requete_t *stringToReq(const message_t msg);
 * \brief Permet de convertir une chaîne de requête en structure de requête.
*/

requete_t *stringToReq(const message_t msg)
{
	//Déserialization d'une chaine de caractères en requête (structure)
	requete_t * req = (requete_t *) malloc(sizeof(requete_t));
	sscanf(msg,"%hd:%[^:]:%[^\n]", &req->noReq, req->action, req->params);
	return(req);
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


/**
 * \fn int traiterRequest(const requete_t *req);
 * \brief Permet de traiter la requête passé en paramètre.
*/

int traiterRequest(const requete_t *req)
{
	switch(req->noReq)
	{
		case 100:
			printf("\tRequete numéro [%d] reçue - action [%s] - params [%s]\n", req->noReq, req->action, req->params);
			break;
		default:
			printf("Non supporté par le serveur\n");
			break;
	}
	return 0;
	
	//TODO Génération d'une réponse et traitement d'une réponse normalement
}
