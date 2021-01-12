/**
 * \file BashorpionData.c
 * \brief Fichier "BashorpionData.c" contenant les fonctions de serialisation de 
 notre projet MCS. Concerne la couche 6 du modèle OSI (Presentation).
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"


// ************ FONCTIONS ************

/**
 * \fn char * reqToString(const requete_t *req, message_t msg);
 * \brief Décompose une requête en chaîne de caractère.
*/

char * reqToString(const requete_t *req, message_t msg)
{	
	//Serialization d'une requête sous forme d'une chaîne de caractères
	//c-à-d une suite d'octets
	memset(msg,0,MAX_CHAR);
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
