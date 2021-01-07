#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCK_NAME	"./mySockUnix"
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

typedef char message_t[MAX_BUFF];
typedef char action_t[MAX_ACT];
typedef struct {short noReq; action_t action; message_t params;} requete_t;
typedef struct {short typeRep; message_t result;} reponse_t;

void client(const message_t MSG);
void serveur();
requete_t * createRequete(short no, action_t act, const message_t myParams);
char * reqToString(const requete_t *req, message_t msg);
int sendRequete(const int sock, const struct sockaddr_in *adr, const requete_t *req);
requete_t *stringToReq(const message_t msg);
int traiterRequest(const requete_t *req);
int sendMsg(const int sock, const struct sockaddr_in *adr, const char *msg);
void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient);
void dialClientToSrv(int sockINET, const char * MSG);
int sessionSrv();
int sessionClt();
int acceptClt(int sockINET, struct sockaddr_in *clientAdr);
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

int acceptClt(int sockINET, struct sockaddr_in *clientAdr)
{
	int sd;
	socklen_t lenClt;
		
	lenClt = sizeof(clientAdr);
	//Attente de connexion client
	CHECK(sd=accept(sockINET, (struct sockaddr *)&clientAdr, &lenClt), "Problème bind serveur ");
	return sd;
}

int sessionSrv()
{
	int sockINET;
	struct sockaddr_in sockAdr;
	
	//Création socket INET STREAM (nécessite une co)
	CHECK(sockINET = socket(PF_INET, SOCK_STREAM, 0), "Problème sock serveur ");
	printf("Numéro de canal créé pour la socket serveur : [%d]\n\n", sockINET);

	//Préparation d'un adressage pour une socket INET
	sockAdr.sin_family = PF_INET;
	sockAdr.sin_port = htons(PORT_SRV); //Allocation dynamique d'un numéro de port
	//pas de htons() pour test
	sockAdr.sin_addr.s_addr = inet_addr(ADDR_SRV); //OU INADDRY_ANY POUR TOUTE INTERFACE DE LA MACHINE
	memset(&(sockAdr.sin_zero), 8, 0);
	
	//strcpy(sockAdr.sun_path, SOCK_NAME);

	//Association de la socket avec l'adresse
	//Utilisation à base d'un cast
	CHECK(bind(sockINET, (struct sockaddr *)&sockAdr, sizeof(sockAdr)), "Problème bind serveur ");
	
	//Mise de la socket à l'écoute
	CHECK(listen(sockINET, 5),"Problème listen() "); //5 est le nb de clients mis en attente
	
	return(sockINET);
}

void dialSrvToClient(int socketDialogue, struct sockaddr_in *adresseClient)
{
	int req;
	message_t buff;
	requete_t * reqClient;
	socklen_t lenClt;

	lenClt = sizeof(adresseClient);
	//Dialogue avec le client
	memset(&buff, 0, MAX_BUFF);
	//lenClt = sizeof(clientAdr);
	printf("Attente de réception d'un message\n");
	CHECK(recvfrom(socketDialogue, buff, MAX_BUFF, 0, (struct sockaddr *)&adresseClient, &lenClt), "Problème recv serveur ");
	reqClient = stringToReq(buff);
	traiterRequest(reqClient); 
	//printf("\tMessage reçu : \"%s\"\n", buff);
	//printf("\tpar le canal %s\n\n", clientAdr.sun_path);
	sscanf(reqClient->params,"%d",&req);
	if (req==1)
	{
		CHECK(send(socketDialogue, MSG_SRV1, strlen(MSG_SRV1) + 1, 0), "Problème send du serveur ");
	}
	else
	{
		CHECK(send(socketDialogue, MSG_SRV2, strlen(MSG_SRV2) + 1, 0), "Problème send du serveur ");
	}
	CHECK(shutdown(socketDialogue, SHUT_WR),"-- PB : shutdown()");
	sleep(1);
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

void serveur()
{
	int sockINET, sd, pid;
	struct sockaddr_in sockAdr, clientAdr;
	
	sockINET = sessionSrv();
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		CHECK(pid = fork(), "Problème fork() ");
		if (pid == 0)
		{
			CHECK(close(sockINET), "Problème close sock serveur "); //Le fils a pas besoin
			//Dialogue avec le client
			dialSrvToClient(sd, &clientAdr);
			CHECK(close(sd), "Problème close sock dialogue serveur ");		
			exit(0);
		}
		CHECK(close(sd), "Problème close sock dialogue serveur ");	
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur ");
}

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

int main()
{
	#ifdef SERVER
		serveur();
	#endif
	#ifdef CLIENT1
		client(MSG_CLT1);
	#endif
	#ifdef CLIENT2
		client(MSG_CLT2);
	#endif
	printf("Fin d'application.\n");
	
	return 0;
}
