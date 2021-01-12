/**
 * \file BashorpionData.h
 * \brief Fichier "BashorpionData.h" contenant les prototypes des fonctions de 
 serialisation. Concerne la couche 6 du modèle OSI (Presentation).
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
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
 * \fn requete_t *stringToReq(const message_t msg);
 * \brief Permet de convertir une chaîne de requête en structure de requête.
*/

requete_t *stringToReq(const message_t msg);

#endif
