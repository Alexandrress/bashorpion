/**
 * \file BashorpionReqRep.c
 * \brief Fichier "BashorpionReqRep.c" contenant les fonctions
 de génération de requêtes/réponses et de traitements. Concerne la couche 7 du modèle 
 OSI (Application).
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionReqRep.h"


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
