/**
 * \file BashorpionLib.h
 * \brief Fichier "Bashorpion.h" contenant les prototypes et structures
 de la couche applicative de notre projet MCS.
 * \author Alexandre.L & Nicolas.S
 * \version 1.0
 * \date 07 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// ************ DEFINES ************

#define MSG_CLT1		"1 : Ceci est un message client!"
#define MSG_CLT2		"2 : Ceci est un second message client!"
#define MSG_SRV1		"Ceci est une réponse serveur!"
#define MSG_SRV2		"Ceci est une autre réponse serveur!"
#define MAX_BUFF		512 
#define MAX_ACT			10
#define PORT_SRV		5120 //doit être > à 1023
							 //exclure les ports assigned services dans more /etc/services
#define ADDR_SRV		"127.0.0.1"
							 
#define CHECK(status, msg)                                                   \
    if (-1 == (status)) {                                                    \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }


// ************ STRUCTURES ************

typedef char message_t[MAX_BUFF];
typedef char action_t[MAX_ACT];
typedef struct {short noReq; action_t action; message_t params;} requete_t;
typedef struct {short typeRep; message_t result;} reponse_t;


// ************ FONCTIONS ************

/**
 * \fn requete_t * createRequete(short no, action_t act, const message_t myParams);
 * \brief Permet de créer une nouvelle requête en spécifiant le code, l'action
 et le message.
*/

requete_t * createRequete(short no, action_t act, const message_t myParams);


/**
 * \fn char * reqToString(const requete_t *req, message_t msg);
 * \brief Décompose une requête en chaîne de caractère.
*/

char * reqToString(const requete_t *req, message_t msg);


/**
 * \fn int sendRequete(const int sock, const struct sockaddr_in *adr, const requete_t *req);
 * \brief Permet d'envoyer la requête passé en paramètre à la socket sock.
*/

int sendRequete(const int sock, const struct sockaddr_in *adr, const requete_t *req);


/**
 * \fn requete_t *stringToReq(const message_t msg);
 * \brief Permet de convertir une chaîne de requête en structure de requête.
*/

requete_t *stringToReq(const message_t msg);


/**
 * \fn int traiterRequest(const requete_t *req);
 * \brief Permet de traiter la requête passé en paramètre.
*/

int traiterRequest(const requete_t *req);


/**
 * \fn int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg);
 * \brief Permet d'envoyer le message passé en paramètre à la socket sock
*/

int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg);
