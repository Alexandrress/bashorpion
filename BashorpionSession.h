/**
 * \file BashorpionSession.h
 * \brief Fichier "BashorpionSession.h" contenant les prototypes et structures
des fonctions de session. Concerne la couche 5 du modèle OSI (Session).
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
*/

#ifndef _SESSION_H_
#define _SESSION_H_

// ************ BIBLIOTHEQUES ************

#include "BashorpionReqRep.h"
#include "BashorpionData.h"


// ************ FONCTIONS ************

/**
 * \fn int sessionSrv()
 * \brief Permet de créer une nouvelle session pour un serveur.
*/

int sessionSrv();


/**
 * \fn int sessionClt()
 * \brief Permet de créer une nouvelle session pour un client.
*/

int sessionClt();


/**
 * \fn void connectSrv(int sockINET)
 * \brief Permet au client de se connecter au serveur.
*/

void connectSrv(int sockINET);


/**
 * \fn int acceptClt(int sockINET, struct sockaddr_in *clientAdr)
 * \brief Permet au serveur d'accepter la connection client.
*/

int acceptClt(int sockINET, struct sockaddr_in *clientAdr);


/**
 * \fn void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
 * \brief Permet au serveur de dialoguer avec le client
*/

void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient);


/**
 * \fn void dialClientToSrv(int sockINET, const char * MSG)
 * \brief Permet au client de dialoguer avec le serveur
*/

void dialClientToSrv(int sockINET, const char * MSG);


/**
 * \fn int sendRequete(const int sock, const requete_t *req);
 * \brief Permet d'envoyer la requête passé en paramètre à la socket sock.
*/

int sendRequete(const int sock, const requete_t *req);

/**
 * \fn int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg);
 * \brief Permet d'envoyer le message passé en paramètre à la socket sock
*/

int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg);


#endif
