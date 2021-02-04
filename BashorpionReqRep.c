/**
 * \file BashorpionReqRep.c
 * \brief Fichier "BashorpionReqRep.c" contenant les fonctions
 de génération de requêtes/réponses et de traitements. Concerne la couche 7 du modèle 
 OSI (Application).
 * \author Alexandre.L & Nicolas.S
 * \version 4.0
 * \date 25 Janvier 2021
*/


// ************ BIBLIOTHEQUES ************

#include "BashorpionReqRep.h"

// ************ FONCTIONS ************

/**
 * \fn requete_t * createRequete(short no, action_t act, const message_t myParams);
 * \brief Permet de créer une nouvelle requête en spécifiant le code, l'action
 et le message.
*/

requete_t * createRequete(short no, action_t act, const message_t myParams)
{
	requete_t * myReq = (requete_t *) malloc(sizeof(requete_t));
	myReq->noReq = no;
	strcpy(myReq->action, act);
	strcpy(myReq->params, myParams);
	return(myReq);
}


/**
 * \fn reponse_t * createReponse(short no, const message_t resultat);
 * \brief Permet de créer une nouvelle réponse en spécifiant le code, et le message.
*/

reponse_t * createReponse(short no, const message_t resultat)
{
	reponse_t * myRep = (reponse_t *) malloc(sizeof(reponse_t));
	myRep->typeRep = no;
	strcpy(myRep->result, resultat);
	return(myRep);
}


/**
 * \fn reponse_t * traiterRequest(const requete_t *req)
 * \brief Permet de traiter la requête passé en paramètre et crée une réponse.
*/

reponse_t * traiterRequest(const requete_t *req)
{
	reponse_t *rep;
	char cmd[MAX_CHAR];
	char buffer[MAX_CHAR];
	char userName[50];
	char ipUser[20];
	int i/*, j*/, trouve;
	char customMsg[10000];
	char saveLeaderboard[1500];
	char temp[1500];
	FILE * fp; //Pointeur sur le fichier json de leaderboard
	
	switch(req->noReq)
	{
		case 100: //Lobby
		
			pthread_mutex_lock(&mutexServeur); //On lock la ressource serveur
			
			if(strcmp(req->action, "GET") == 0)
			{
				if (strcmp(req->params, "LISTE_USER") == 0) //Juste la liste des joueurs
				{
					sprintf(customMsg, "Voici la liste des joueurs : ");
					for (i=0 ; i<CAPACITE_SERVER ; i++)
					{
						if (strcmp(usersDatas[i].username, ""))
						{ 	
							sprintf(customMsg, "%s - %s", customMsg, usersDatas[i].username);
						}
					}
					rep=createReponse(200, customMsg);
				}
				else //Une IP particulière
				{
					for (i=0 ; i<CAPACITE_SERVER ; i++)
					{
						if (!strcmp(usersDatas[i].username, req->params))
						{	//false lorsque les 2 chaines sont identiques
							sprintf(customMsg, "%s", usersDatas[i].ipUser);
							
							rep=createReponse(200, customMsg);
							break;
						}
						else
							rep=createReponse(404,"NOT FOUND");
					}
				}
				if (strcmp(req->params, "LEADERBOARD") == 0) //Le leaderboard
				{
					
					sprintf(customMsg, "Voici le leaderBoard des joueurs : ");
					for (i=0 ; strcmp(leaderBoard[i].username, ""); i++){
						sprintf(customMsg, "%s | %s - %d victoires", customMsg, leaderBoard[i].username, leaderBoard[i].nbVictoires);
						printf("%s - %d victoires\n", leaderBoard[i].username, leaderBoard[i].nbVictoires);
					}
					rep=createReponse(200, customMsg);
				}
			}
			if(strcmp(req->action, "DELETE") == 0)
			{
				for(int joueur=0; joueur<CAPACITE_SERVER; joueur++)
				{
					if(strcmp(req->params, usersDatas[joueur].username)==0)
					{
						memset(usersDatas[joueur].ipUser, 0, sizeof(usersDatas[joueur].ipUser));
						memset(usersDatas[joueur].username, 0, sizeof(usersDatas[joueur].username));
					}
				}
				
				//On met à jour le fichier de sauvegarde des scores
				sprintf(saveLeaderboard, "{\"users\": [\n\t");
				for (i=0 ; strcmp(leaderBoard[i].username, ""); i++){
					sprintf(temp, "{ \"ip\": \"%s\", \"pseudo\": \"%s\", \"score\": \"%d\" },\n\t"
									, leaderBoard[i].ipUser, leaderBoard[i].username, leaderBoard[i].nbVictoires);
					strcat(saveLeaderboard, temp);
				}
				sprintf(saveLeaderboard, "%s]}", saveLeaderboard);
				system("rm ./datas/leaderBoard.json ; touch ./leaderBoard.json");
				fp = fopen("./datas/leaderBoard.json", "w");
				if (fp != NULL){
					fprintf(fp, "%s", saveLeaderboard);
					fclose(fp);
				}else {
					printf("Noooo\n");
				}
				
				printf("Mise à jour confirmée\n");
				
				rep=createReponse(200,"Je t'ai supprimé du lobby.");
				
			}
			if(strcmp(req->action, "PUT") == 0)
			{
				strcpy(userToAdd,req->params);
				rep=createReponse(201,"Oui j'ai ajouté l'user à la liste.");
			}
			
			pthread_mutex_unlock(&mutexServeur); //On libère la ressource serveur
			
			break;
			
		case 101:
			pthread_mutex_lock(&mutexServeur); //On lock la ressource serveur
			if(strcmp(req->action, "PUT") == 0){
				//Demande de mise à jour du tableau des scores
				
				//On commence par récupérer les informations dans la chaine de paramètres
				printf("params : %s", req->params);
				strcpy(userName, strtok((char * restrict)req->params, ":"));
				strcpy(ipUser, strtok(NULL, ":"));
				//sscanf(req->params, "%s;%s", , );
				printf("Info récupérées : username=>%s< ; ipUser=>%s<", userName, ipUser);
				
				//On parcourt la liste des utilisateurs classés dans le leaderboard
				//et on incrémentele score du joueur cible
				//Si le joueur n'est pas trouvé, on lui crée un emplacement.
				trouve=0;
				for (i=0 ; strcmp(leaderBoard[i].username, "") && trouve == 0; i++){
					if (!(strcmp(leaderBoard[i].username, req->params))){
						trouve=1;
						leaderBoard[i].nbVictoires++;
						printf("Je t'ai trouvé, j'incrémente ton score à %d\n", leaderBoard[i].nbVictoires);
					}
				}
				if (trouve == 0){
					printf("Je ne t'ai pas trouvé, tu es nouveau : Je te crée un emplacement!\n");
					strcpy(leaderBoard[i].username, req->params);
					leaderBoard[i].nbVictoires=1;
					strcpy(leaderBoard[i].ipUser,"127.0.0.1");
				}
				rep=createReponse(208,"Oui je t'ai ajouté au leaderboard");
			}
			pthread_mutex_unlock(&mutexServeur); //On libère la ressource serveur
		break;
			
		case 200: //Peer-to-peer
			if(strcmp(req->action, "BATTLE") == 0)
			{
				printf("\n");
				printf("%s veut se battre, acceptez-vous? (accept/deny)\n",req->params);
				fgets(buffer, sizeof(buffer), stdin);
				buffer[strlen(buffer)-1] = '\0';
				sscanf(buffer, "%s", cmd);
				if(strcmp(buffer, "accept") == 0)
				{
					rep=createReponse(202,"Oui, je veux.");
					strcpy(opponentName, req->params);
					hasAcceptedDuel=1;
				}
				else
				{
					rep=createReponse(460,"Non, je ne veux pas.");
					hasAcceptedDuel=0;
				}
			}
			else if (strcmp(req->action, "PLAY") == 0)
			{
				rep=createReponse(200,"OK j'ai joué ton coup. C'est mon tour!");
				int tempCoup = atoi(req->params);
				coup = tempCoup;
			}
			else if (strcmp(req->action, "RE") == 0)
			{
				rep=createReponse(200,"OK j'ai reçu ta réponse pour la revanche!");
				strcpy(bufferRevanche,req->params);
			}
			break;
		default:
			rep=createReponse(404,"Quelque chose de mauvais s'est produit!");
			break;
	}
	
	return rep;
}


/**
 * \fn char * traiterReponse(const reponse_t *rep)
 * \brief Permet de traiter la réponse passé en paramètre, renvoie une chaine de caractère pour 
 * afficher le résultat du traitement.
*/

char * traiterReponse(const reponse_t *rep)
{
	char *reponse = (char *) malloc(sizeof(char) * MAX_CHAR);
	
	switch(rep->typeRep)
	{
		case 200:
			strcpy(reponse,"SUCCESS ");
			strcat(reponse,rep->result);
			break;
		case 201:
			strcpy(reponse,"USER ADDED ");
			strcat(reponse,rep->result);
			break;
		case 202:
			strcpy(reponse,"DUEL ACCEPTED ");
			strcat(reponse,rep->result);
			hasAcceptedDuel=1;
			break;
		case 404:
			strcpy(reponse,"ERROR ");
			strcat(reponse,rep->result);
			break;
		case 408:
			strcpy(reponse,"TIMEOUT ");
			strcat(reponse,rep->result);
			break;
		case 460:
			strcpy(reponse,"DUEL REFUSED ");
			strcat(reponse,rep->result);
			hasAcceptedDuel=0;
			break;
		default:
			strcpy(reponse,"UNKNOWN ");
			strcat(reponse,rep->result);
			break;
	}
	return reponse;
}

