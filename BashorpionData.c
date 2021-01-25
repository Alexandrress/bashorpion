/**
 * \file BashorpionData.c
 * \brief Fichier "BashorpionData.c" contenant les fonctions de serialisation de 
 notre projet MCS. Concerne la couche 6 du modèle OSI (Presentation).
 * \author Alexandre.L & Nicolas.S
 * \version 4.0
 * \date 25 Janvier 2021
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
 * \fn char * repToString(const reponse_t *rep, message_t msg);
 * \brief Décompose une réponse en chaîne de caractère.
*/

char * repToString(const reponse_t *rep, message_t msg)
{	
	//Serialization d'une réponse sous forme d'une chaîne de caractères
	//c-à-d une suite d'octets
	memset(msg,0,MAX_CHAR);
	sprintf(msg,"%hd:%s", rep->typeRep, rep->result);
	return(msg);
} 


/**
 * \fn requete_t *stringToReq(const message_t msg);
 * \brief Permet de convertir une chaîne de caractère en structure de requête.
*/

requete_t *stringToReq(const message_t msg)
{
	//Déserialization d'une chaine de caractères en requête (structure)
	requete_t * req = (requete_t *) malloc(sizeof(requete_t));
	sscanf(msg,"%hd:%[^:]:%[^\n]", &req->noReq, req->action, req->params);
	return(req);
}


/**
 * \fn reponse_t *stringToRep(const message_t msg);
 * \brief Permet de convertir une chaîne de caractère en structure de réponse.
*/

reponse_t *stringToRep(const message_t msg)
{
	//Déserialization d'une chaine de caractères en réponse (structure)
	reponse_t * rep = (reponse_t *) malloc(sizeof(reponse_t));
	sscanf(msg,"%hd:%[^\n]", &rep->typeRep, rep->result);
	return(rep);
}
