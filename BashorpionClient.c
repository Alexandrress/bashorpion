#include "BashorpionLib.h"

void client(const message_t MSG);
void dialClientToSrv(int sockINET, const char * MSG);
int sessionClt();
void connectSrv(int sockINET);

int sessionClt()
{
	int sockINET;
	
	//Création socket INET STREAM
	CHECK(sockINET = socket(PF_INET, SOCK_STREAM, 0), "Problème sock client ");
	
	return sockINET;
}

void connectSrv(int sockINET)
{
	struct sockaddr_in adrSrv;

	//Le client n'a pas besoin d'avoir une adresse donc
	//pas de préparation ni d'association.
	//Par contre le client doit fournir l'adresse du
	//serveur.

	adrSrv.sin_family = PF_INET;
	adrSrv.sin_port= htons(PORT_SRV);
	adrSrv.sin_addr.s_addr = inet_addr(ADDR_SRV); //boucle locale et format réseau
	memset(&(adrSrv.sin_zero), 8, 0);
	
	//Demande connexion
	CHECK(connect(sockINET, (struct sockaddr *)&adrSrv, sizeof(adrSrv)),"Problème connect() ");
}

void dialClientToSrv(int sockINET, const char * MSG)
{
	int lenSockAdr;
	requete_t *reqClient;
	struct sockaddr_in adrSrv, sockAdr;
	message_t buff;

	//Envoi d'un message à un destinataire
	
	reqClient = createRequete(100, "SHOW", MSG);
	sendRequete(sockINET, &adrSrv, reqClient);
	
	//printf("Envoi d'un mesage sur [%s]\n", SOCK_NAME);
	//CHECK(sendto(sockUNIX, MSG, strlen(MSG) + 1, 0, (struct sockaddr *)&adrSrv, sizeof(adrSrv)), "Problème sendto du client ");

	lenSockAdr = sizeof(sockAdr);
	CHECK(getsockname(sockINET, (struct sockaddr *)&sockAdr, &lenSockAdr),"Problème getsockname()");
	
	//Attente d'une réponse
	memset(&buff, 0, MAX_BUFF);
	CHECK(recv(sockINET, buff, MAX_BUFF, 0),"Problème recv() ");
	
	printf("Réponse serveur : %s\n",buff);
}

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
