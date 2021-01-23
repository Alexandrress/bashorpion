/**
 * \file BashorpionReqRep.h
 * \brief Fichier "BashorpionReqRep.h" contenant les prototypes et structures
 de génération de requêtes/réponses et de traitements. Concerne la couche 7 du modèle 
 OSI (Application).
 * \author Alexandre.L & Nicolas.S
 * \version 3.0
 * \date 21 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#ifndef _REQREP_H_
#define _REQREP_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>


// ************ DEFINES ************

#define MSG_CLT1		"1 : Ceci est un message client!" 
#define MSG_CLT2		"2 : Ceci est un second message client!"
#define MSG_SRV1		"Ceci est une réponse serveur!"
#define MSG_SRV2		"Ceci est une autre réponse serveur!"
#define MAX_CHAR		512 //Constante permettant de spécifier la taille MAX des char*.
#define PORT_CLT		60001 //doit être > à 1023
#define PORT_SRV		60002 //doit être > à 1023
							 //exclure les ports assigned services dans more /etc/services
#define ADDR_SRV		"127.0.0.1"
	
int hasAcceptedDuel;
char opponentName[MAX_CHAR];

// ************ MACRO - FONCTIONS ************					
	
/**
* \fn 		CHECK
* \brief 	Fonction qui permet de vérifier qu'une commande système status s'est 
bien exécuter. Renvoie le message passé en paramètre en cas d'erreur.
*/
						 
#define CHECK(status, msg)                                                   \
    if (-1 == (status)) {                                                    \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }


// ************ STRUCTURES ************

typedef char message_t[MAX_CHAR];

typedef char action_t[MAX_CHAR];


/**
 * \struct infoUser_t
 * \brief Objet Information Users.
 *
 * infoUser_t est une structure permettant de spécifier l'username d'un joueur 
 ainsi que son IP associé.
*/

typedef struct
{
	char username[MAX_CHAR];
	char ipUser[MAX_CHAR];
	int portIP;
} infoUser_t;


/**
 * \struct requete_t
 * \brief Objet Requete.
 *
 * requete_t est une structure permettant d'envoyer un code de requête, une action
 * correspondant à ce code ainsi qu'un message en paramètre.
*/

typedef struct 
{
	short noReq; 
	action_t action; 
	message_t params;
} requete_t;


/**
 * \struct reponse_t
 * \brief Objet Reponse.
 *
 * reponse_t est une structure permettant d'envoyer un code de retour sur la requete
 * émise précédemment, ainsi qu'un message contenant le résultat de la requete.
*/

typedef struct {
	short typeRep; 
	message_t result;
} reponse_t;


// ************ FONCTIONS ************

/**
 * \fn requete_t * createRequete(short no, action_t act, const message_t myParams);
 * \brief Permet de créer une nouvelle requête en spécifiant le code, l'action
 et le message.
*/

requete_t * createRequete(short no, action_t act, const message_t myParams);

/**
 * \fn reponse_t * createReponse(short no, const message_t resultat);
 * \brief Permet de créer une nouvelle réponse en spécifiant le code, et le message.
*/

reponse_t * createReponse(short no, const message_t resultat);

/**
 * \fn reponse_t * traiterRequest(const requete_t *req)
 * \brief Permet de traiter la requête passé en paramètre.
*/

reponse_t * traiterRequest(const requete_t *req);

/**
 * \fn char * traiterReponse(const reponse_t *rep)
 * \brief Permet de traiter la réponse passé en paramètre.
*/
char * traiterReponse(const reponse_t *rep);

#endif
