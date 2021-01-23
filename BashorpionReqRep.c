/**
 * \file BashorpionReqRep.c
 * \brief Fichier "BashorpionReqRep.c" contenant les fonctions
 de génération de requêtes/réponses et de traitements. Concerne la couche 7 du modèle 
 OSI (Application).
 * \author Alexandre.L & Nicolas.S
 * \version 3.0
 * \date 21 Janvier 2021
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
 * \fn reponse_t * createReponse(short no, const message_t resultat);
 * \brief Permet de créer une nouvelle réponse en spécifiant le code, et le message.
*/
reponse_t * createReponse(short no, const message_t resultat)
{
	reponse_t * myRep = (reponse_t *) malloc(sizeof(reponse_t));
	myRep->typeRep = no;
	strcpy(myRep->result, resultat);
	return(myRep);
}


/**
 * \fn reponse_t * traiterRequest(const requete_t *req)
 * \brief Permet de traiter la requête passé en paramètre.
*/

reponse_t * traiterRequest(const requete_t *req)
{
	reponse_t *rep;
	char cmd[MAX_CHAR];
	char buffer[MAX_CHAR];
	
	switch(req->noReq)
	{
		case 100:
			if(strcmp(req->action, "GET") == 0)
			{
				rep=createReponse(200,"Voici la liste.");
			}
			if(strcmp(req->action, "DELETE") == 0)
			{
				rep=createReponse(200,"Je t'ai supprimé.");
			}
			if(strcmp(req->action, "PUT") == 0)
			{
				rep=createReponse(201,"Oui j'ai ajouté l'user.");
			}
			break;
		case 200:
			if(strcmp(req->action, "BATTLE") == 0)
			{
				printf("%s veut se battre, acceptez-vous? (y/n)\n",req->params);
				fgets(buffer, sizeof(buffer), stdin);
				buffer[strlen(buffer)-1] = '\0';
				sscanf(buffer, "%s", cmd);
				if(strcmp(buffer, "y") == 0)
				{
					rep=createReponse(202,"Oui je veux.");
					strcpy(opponentName, req->params);
					hasAcceptedDuel=1;
				}
				else
				{
					rep=createReponse(460,"Non sale plouc.");
					hasAcceptedDuel=0;
				}
			}
			break;
		default:
			rep=createReponse(404,"Quelque chose de mauvais s'est produit!");
			break;
	}
	
	return rep;
}

/**
 * \fn char * traiterReponse(const reponse_t *rep)
 * \brief Permet de traiter la réponse passé en paramètre.
*/

char * traiterReponse(const reponse_t *rep)
{
	char *reponse = (char *) malloc(sizeof(char) * MAX_CHAR);
	
	switch(rep->typeRep)
	{
		case 200:
			strcpy(reponse,"SUCCESS : ");
			strcat(reponse,rep->result);
			break;
		case 201:
			strcpy(reponse,"USER ADDED : ");
			strcat(reponse,rep->result);
			break;
		case 202:
			strcpy(reponse,"DUEL ACCEPTED : ");
			hasAcceptedDuel=1;
			strcat(reponse,rep->result);
			break;
		case 404:
			strcpy(reponse,"ERROR : ");
			strcat(reponse,rep->result);
			break;
		case 408:
			strcpy(reponse,"TIMEOUT : ");
			strcat(reponse,rep->result);
			break;
		case 460:
			strcpy(reponse,"DUEL REFUSED : ");
			hasAcceptedDuel=0;
			strcat(reponse,rep->result);
			break;
		default:
			strcpy(reponse,"UNKNOWN : ");
			strcat(reponse,rep->result);
			break;
	}
	
	return reponse;
}
