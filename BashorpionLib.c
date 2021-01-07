#include "BashorpionLib.h"

requete_t * createRequete(short no, action_t act, const message_t myParams)
{
	requete_t * myReq = (requete_t *) malloc(sizeof(requete_t));
	myReq->noReq = no;
	strcpy(myReq->action, act);
	strcpy(myReq->params, myParams);
	return(myReq);
}

char * reqToString(const requete_t *req, message_t msg)
{	
	//Serialization d'une requête sous forme d'une chaîne de caractères
	//c-à-d une suite d'octets
	memset(msg,0,MAX_BUFF);
	sprintf(msg,"%hd:%s:%s", req->noReq, req->action, req->params);
	return(msg);
} 

requete_t *stringToReq(const message_t msg)
{
	//Déserialization d'une chaine de caractères en requête (structure)
	requete_t * req = (requete_t *) malloc(sizeof(requete_t));
	sscanf(msg,"%hd:%[^:]:%[^\n]", &req->noReq, req->action, req->params);
	return(req);
}

int sendRequete(const int sock, const struct sockaddr_in *adr, const requete_t *req)
{
	message_t msg;
	reqToString(req, msg);
	
	//Envoi d'un message à un destinataire
	printf("Envoi d'un mesage INET.\n");
	CHECK(send(sock, msg, strlen(msg) + 1, 0), "Problème send du client ");
	return(0);
}

int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg)
{
	//Envoi d'un message à un destinataire
	printf("Envoi d'un mesage INET.\n");
	CHECK(sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr *)adr, sizeof(*adr)), "Problème sendto du client ");
	return(0);
}

int traiterRequest(const requete_t *req)
{
	switch(req->noReq)
	{
		case 100:
			printf("\tRequete numéro [%d] reçue - action [%s] - params [%s]\n", req->noReq, req->action, req->params);
			break;
		default:
			printf("Non supporté par le serveur\n");
			break;
	}
	return 0;
	
	//TODO Génération d'une réponse et traitement d'une réponse normalement
}
