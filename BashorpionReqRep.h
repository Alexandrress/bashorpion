/**
 * \file BashorpionReqRep.h
 * \brief Fichier "BashorpionReqRep.h" contenant les prototypes et structures
 de génération de requêtes/réponses et de traitements. Concerne la couche 7 du modèle 
 OSI (Application).
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
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


// ************ DEFINES ************

#define MSG_CLT1		"1 : Ceci est un message client!" 
#define MSG_CLT2		"2 : Ceci est un second message client!"
#define MSG_SRV1		"Ceci est une réponse serveur!"
#define MSG_SRV2		"Ceci est une autre réponse serveur!"
#define MAX_CHAR		512 //Constante permettant de spécifier la taille MAX des char*.
#define PORT_SRV		5120 //doit être > à 1023
							 //exclure les ports assigned services dans more /etc/services
#define ADDR_SRV		"127.0.0.1" //Adresse du serveur
	

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
 * \fn int traiterRequest(const requete_t *req);
 * \brief Permet de traiter la requête passé en paramètre.
*/

int traiterRequest(const requete_t *req);


#endif
