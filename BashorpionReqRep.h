/**
 * \file BashorpionReqRep.h
 * \brief Fichier "BashorpionReqRep.h" contenant les prototypes et structures
 de génération de requêtes/réponses et de traitements. Concerne la couche 7 du modèle 
 OSI (Application).
 * \author Alexandre.L & Nicolas.S
 * \version 4.0
 * \date 25 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#ifndef _REQREP_H_
#define _REQREP_H_

//Si nous sommes sous Windows
#if defined (WIN32)

	#include <ws2tcpip.h>
    #include <winsock2.h>
    typedef int socklen_t;

// Sinon, si nous sommes sous Linux
#elif defined (linux)

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/wait.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(s) close (s)

    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
    
#endif

// On inclut les fichiers standards
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <pthread.h>


// ************ DEFINES ************

#define MAX_CHAR		512 //Constante permettant de spécifier la taille MAX des char*.
#define PORT_CLT		60001 //doit être > à 1023 et exclure les ports assigned services dans more /etc/services
#define PORT_SRV		60002 //doit être > à 1023 et exclure les ports assigned services dans more /etc/services
#define ADDR_SRV		"127.0.0.1"
#define CAPACITE_SERVER 10 	//Capacité maximmale du server
	


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
 ainsi que son IP associé et le port.
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


// ******** VARIABLES GLOBALES ********

int coup;
int hasAcceptedDuel;
char opponentName[MAX_CHAR];
char bufferRevanche[MAX_CHAR]; 
char userToAdd[MAX_CHAR];
infoUser_t usersDatas[CAPACITE_SERVER]; //Tableau de structures d'infos de clients
pthread_mutex_t mutexServeur; //Mutex pour que la ressource de usersDatas ne soit pas manipulé en mm temps.

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
 * \brief Permet de traiter la requête passé en paramètre et crée une réponse.
*/

reponse_t * traiterRequest(const requete_t *req);


/**
 * \fn char * traiterReponse(const reponse_t *rep)
 * \brief Permet de traiter la réponse passé en paramètre, renvoie une chaine de caractère pour 
 * afficher le résultat du traitement.
*/

char * traiterReponse(const reponse_t *rep);


#endif
