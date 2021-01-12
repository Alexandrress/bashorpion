/**
 * \file BashorpionClient.c
 * \brief Programme permettant de lancer un client du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 2.0
 * \date 12 Janvier 2021
 *
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"
#include "BashorpionSession.h"
#include "BashorpionReqRep.h"

void client(const message_t MSG);


// ******** VARIABLES GLOBALES ********

infoUser_t informationJoueur;


// ************ FONCTIONS ************

/**
 * \fn void client(const message_t MSG)
 * \brief Fonction principale du client permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void client(const message_t MSG)
{
	int sockINET;
	
	sockINET = sessionClt();

	connectSrv(sockINET);
	
	dialClientToSrv(sockINET, MSG);
	
	getchar();
	
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock client ");
}

int main()
{
	client(MSG_CLT1);
	printf("Fin d'application.\n");
	
	return 0;
}
