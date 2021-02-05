/*! \mainpage LEVEQUE Alexandre & SHAHPAZIAN Nicolas - Projet MCS
 * \section intro_sec Introduction
 * Bienvenue sur cette page du manuel Doxygen du "Projet CMS : Bashorpion" fait par Alexandre.L & Nicolas.S
 * - \subpage Description "Application Client/Serveur de Morpion codée en C, avec des threads ainsi que des sockets INET. Deux joueurs minimums sont requis ainsi que le lancement du serveur permettant l'enregistrement des informations afin de jouer une partie."
*/

/**
 * \file BashorpionServer.c
 * \brief Programme permettant de lancer le serveur d'authentification du projet Bashorpion.
 * \author Alexandre.L & Nicolas.S
 * \version 6.0
 * \date 05 Février 2021
 *
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionData.h"
#include "BashorpionSession.h"
#include "BashorpionReqRep.h"

void serveur();
void * threadLobbyServer(void * socketDialogue);
void printAffichageUsers();
void initServer(void);

// ******** VARIABLES GLOBALES ********

int sockINET;
struct sockaddr_in clientAdr;

// ************ FONCTIONS ************

/**
 * \fn void serveur()
 * \brief Fonction principale du serveur permettant de créer la session, lancer
 la connection et de dialoguer.
*/

void serveur()
{
	int sd, i;
	pthread_t *tab_thread; //tableau de Threads de dialogue
  
	//Allocation mémoire pour les threads de dialogue
	tab_thread = (pthread_t*)malloc(CAPACITE_SERVER*sizeof(pthread_t));
	
	pthread_mutex_init(&mutexServeur, NULL);
	
	initServer();
	
	sockINET = sessionSrv(PORT_SRV, CAPACITE_SERVER);
	
	
	
	printf("\n");
	
	//Affiche la structure des users du serveur
	printAffichageUsers();
	
	//Boucle permanente (1 serveur est un daemon)
	while (1)
	{
		//Recherche d'un emplacement de thread disponible pour la création d'un thread de service
		while (&tab_thread[i] == NULL) 
			i++;
		
		//Attente de connexion client
		sd = acceptClt(sockINET, &clientAdr);
		pthread_create(&(tab_thread[i]), NULL, threadLobbyServer, (void *) &sd);
	}
	//Fermeture du socket
	CHECK(close(sockINET), "Problème close sock serveur père ");
}

/**
 * \fn void initServer(void)
 * \brief Fonction d'initialisation du serveur, dont la fonction principale, 
 * 		au démarrage du serveur, est : 
 * 		- d'initialiser le tableau d'infos utilisateurs usersDatas[].
 * 		- De charger en mémoire (dans le tableau de structures leaderBoard[]) les 
 * 			informations contenues dans le fichier ./datas/leaderBoard.json.
*/

void initServer(void){
	
	int i;
	FILE * ficScores;
	char buffer[1024];
	
	struct json_object *parsed_json;
	struct json_object *ip;
	struct json_object *pseudo;
	struct json_object *score;
	struct json_object *users;
	struct json_object *user;
	
	size_t nbJoueurs;
	
	//Init struct infoUsers
	for (i=0 ; i < CAPACITE_SERVER ; i++)
	{
		memset(usersDatas[i].ipUser, 0, sizeof(usersDatas[i].ipUser));
		memset(usersDatas[i].username, 0, sizeof(usersDatas[i].username));
		usersDatas[i].portIP = 0; //Valeurs pemrettant de considérer le slot comme vide
	}
	
	//Au démarrage, s'il n'existe pas, le serveur crée le fichier de leaderboard.
	//Sinon il le charge dans le tableau de structures score_t dédiée : leaderBoard
	ficScores = fopen("./datas/leaderBoard.json", "rw");
	if (ficScores != NULL){
		printf("Fichier accessible --> lecture du fichier\n");
		
		fread(buffer, 1024, 1, ficScores);
		fclose(ficScores);
		
		parsed_json = json_tokener_parse(buffer);
		
		
		
		json_object_object_get_ex(parsed_json, "users", &users);
		nbJoueurs = json_object_array_length(users);
		printf("-- Il y a %ld utilisateurs enregistrés\n\n", nbJoueurs);
		
		for (i=0 ; i<(int)nbJoueurs ; i++){
			user = json_object_array_get_idx(users, i);
			printf("Utilisateur N°%d : \n", i+1);
			json_object_object_get_ex(user, "pseudo", &pseudo);
			json_object_object_get_ex(user, "score", &score);
			json_object_object_get_ex(user, "ip", &ip);
			
			//Chargement dans la structure de sonnées prévue à cet effet
			strcpy(leaderBoard[i].username, json_object_get_string(pseudo));
			strcpy(leaderBoard[i].ipUser, json_object_get_string(ip));
			leaderBoard[i].nbVictoires = json_object_get_int(score);
			
			printf("- Pseudo : %s\n- Score : %d\n- Ip : %s\n", leaderBoard[i].username, leaderBoard[i].nbVictoires, leaderBoard[i].ipUser);
			printf("\n");
		}
		
	}else {
		printf("Fichier indisponible --> Création du fichier... ");
		system("touch ./datas/leaderBoard.json");
		printf("Fichier créé avec succès\n");
	}
	
}


/**
 * \fn void * threadLobbyServer(void * socketDialogue)
 * \brief Fonction de thread permettant de d'enregistrer le client et de dialoguer avec
*/

void * threadLobbyServer(void * socketDialogue)
{
	int i, socket;
	int * sockTemp;
	sockTemp = (int*) socketDialogue;
	socket = *sockTemp;
	char ipDuJoueur[MAX_CHAR];

	struct sockaddr_in clientAdrCom = clientAdr;
	socklen_t peeraddrlen = sizeof(clientAdrCom);
	
	getpeername(socket, (struct sockaddr *)&clientAdrCom, &peeraddrlen);
	inet_ntop(AF_INET, &(clientAdrCom.sin_addr), ipDuJoueur, MAX_CHAR);
	
	//On copie l'adresse IP du joueur ainsi que son nom.
	dialSrvToClient(socket, &clientAdrCom);
	
	//On détermine le 1er emplacement libre auquel le joueur peut être rangé
	i=0;
	
	while (strcmp(usersDatas[i].username,"")) 
		i++; //True tant que usersDatas[i].username n'est pas vide

	//Remplissage de la structure d'infos utilisateurs
	strcpy(usersDatas[i].ipUser, ipDuJoueur);
  
	printf("STOCKE!!\n");
	strcpy(usersDatas[i].username, userToAdd);
	
	printf("\n");
	printf("\033[1;32m"); //Couleur verte
	printf("Utilisateur ajouté sur le slot %d : \n", i);
	printf("\033[0m"); //Couleur blanche
	printf("- username = %s\n- ipUser = %s\n\n", usersDatas[i].username, usersDatas[i].ipUser);
	
	while(strcmp(usersDatas[i].username, "") != 0) //Tant que l'user est enregistré
	{
		//Dialogue avec le client
		dialSrvToClient(socket, &clientAdrCom);
	}
	
	close(socket);
	pthread_exit(0);
}


/**
 * \fn void printAffichageUsers()
 * \brief Permet d'afficher toute la structure usersDatas du serveur.
*/

void printAffichageUsers()
{
	int i;
	
	printf("\033[1;32m"); //Couleur verte
	printf("------ Affichage des slots : \n\n");
	printf("\033[0m"); //Couleur blanche
	
	for (i=0 ; i < CAPACITE_SERVER ; i++)
	{
		printf("\033[1;32m Slot %d : \033[0m \n- username = >%s<\n- ipUser = >%s<\n- portIP = >%d<\n\n", i, usersDatas[i].username, usersDatas[i].ipUser, usersDatas[i].portIP);
	}
}

int main()
{    
	// Si la plateforme est Windows
    #if defined (WIN32)
        WSADATA WSAData;
        WSAStartup(MAKEWORD(2,2), &WSAData);
    #endif
    
	serveur();
	printf("Fin d'application.\n");
	
	#if defined (WIN32)
        WSACleanup();
    #endif
	
	return 0;
}

