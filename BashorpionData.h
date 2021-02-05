/**
 * \file BashorpionData.h
 * \brief Fichier "BashorpionData.h" contenant les prototypes des fonctions de 
 serialisation. Concerne la couche 6 du modèle OSI (Presentation).
 * \author Alexandre.L & Nicolas.S
 * \version 6.0
 * \date 05 Février 2021
*/

#ifndef _DATA_H_
#define _DATA_H_

// ************ BIBLIOTHEQUES ************

#include "BashorpionReqRep.h"


// ************ FONCTIONS ************

/**
 * \fn char * reqToString(const requete_t *req, message_t msg);
 * \brief Décompose une requête en chaîne de caractère.
*/

char * reqToString(const requete_t *req, message_t msg);


/**
 * \fn char * repToString(const reponse_t *rep, message_t msg);
 * \brief Décompose une réponse en chaîne de caractère.
*/

char * repToString(const reponse_t *rep, message_t msg);


/**
 * \fn requete_t *stringToReq(const message_t msg);
 * \brief Permet de convertir une chaîne de caractère en structure de requête.
*/

requete_t *stringToReq(const message_t msg);


/**
 * \fn reponse_t *stringToRep(const message_t msg);
 * \brief Permet de convertir une chaîne de caractère en structure de réponse.
*/

reponse_t *stringToRep(const message_t msg);


#endif
