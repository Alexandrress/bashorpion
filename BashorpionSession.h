/**
 * \file BashorpionSession.h
 * \brief Fichier "BashorpionSession.h" contenant les prototypes et structures
des fonctions de session. Concerne la couche 5 du modèle OSI (Session).
 * \author Alexandre.L & Nicolas.S
 
 * \version 5.0
 * \date 05 Février 2021

*/

#ifndef _SESSION_H_
#define _SESSION_H_

// ************ BIBLIOTHEQUES ************

#include "BashorpionReqRep.h"
#include "BashorpionData.h"


// ************ FONCTIONS ************

/**
 * \fn int sessionSrv(int portNumber, int nbDeClients)
 * \brief Permet de créer une nouvelle session pour un serveur en spécifiant le port, et le nb MAX de clients.
*/

int sessionSrv(int portNumber, int nbDeClients);


/**
 * \fn int sessionClt()
 * \brief Permet de créer une nouvelle session pour un client.
*/

int sessionClt();


/**
 * \fn int connectSrv(int sockINET, char* serverIP, int portNumber)
 * \brief Permet au client de se connecter au serveur.
*/

int connectSrv(int sockINET, char* serverIP, int portNumber);


/**
 * \fn int acceptClt(int sockINET, struct sockaddr_in *clientAdr)
 * \brief Permet au serveur d'accepter la connection client.
*/

int acceptClt(int sockINET, struct sockaddr_in *clientAdr);


/**
 * \fn void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
 * \brief Permet au serveur de dialoguer avec le client.
*/

void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient);


/**
 * \fn char * dialClientToSrv(int sockINET, const char * MSG)
 * \brief Permet au client de dialoguer avec le serveur.
*/

char * dialClientToSrv(int sockINET, const char * MSG);


/**
 * \fn void sendClientToClient(int sockINET, const char * MSG)
 * \brief Permet au client d'envoyer des messages à son peer.
*/

void sendClientToClient(int sockINET, const char * MSG);


/**
 * \fn void receiveClientToClient(int socketDialogue)
 * \brief Permet au client de recevoir des messages de son peer.
*/

void receiveClientToClient(int socketDialogue);


/**
 * \fn int sendRequete(const int sock, const requete_t *req);
 * \brief Permet d'envoyer la requête passé en paramètre à la socket sock.
*/

int sendRequete(const int sock, const requete_t *req);


/**
 * \fn int sendReponse(const int sock, const reponse_t *rep)
 * \brief Permet d'envoyer la réponse passé en paramètre à la socket sock.
*/

int sendReponse(const int sock, const reponse_t *rep);


#endif
