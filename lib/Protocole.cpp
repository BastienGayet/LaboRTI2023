#include "Protocole.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


//**********Etat du protocole : Liste des clients loggés ***************

int clients[NB_MAX_CLIENTS];
int nbClients = 0;


int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER; 


//***************** Parsing de la requete******************
bool PROTOCOLE(char * requete, char * reponse, int socket)
{
	//***********Recuperation nom de la requete********** 
	char * ptr = strtok(requete,"#");


	// ***** LOGIN ******************************************
 	if (strcmp(ptr,"LOGIN") == 0)
 	{
 		char user[50], password[50];
 		strcpy(user,strtok(NULL,"#"));
 		strcpy(password,strtok(NULL,"#"));
 		printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);

 		if(estPresent(socket)>=0) // client déja loggé
 		{
 			sprintf(reponse,"LOGIN#ko#Client déjà loggé !");
 			return false;
 		}
 		else
 		{
 			if(PROTOCOLE_Login(user,password))
 			{
 				sprintf(reponse,"LOGIN#OK");
 				ajoute(socket);

 			}
 			else
 			{
 				sprintf(reponse,"LOGIN#ko#Mauvais identifiants !");
 				return false;
 			}
 		}
 	}

 	//*******CONSULT**************************

 	if(strcmp(ptr,"CONSULT")==0)
 	{
 		
 	}

 	//*******LOGOUT***************************
 	if(strcmp(ptr,"LOGOUT")==0)
 	{
 		printf("\t[THREAD %p] LOGOUT\n",pthread_self());
 		retire(socket);
 		sprintf(reponse,"LOGOUT#ok");
 		return false;
 	}

	
}


//***** Gestion de l'état du protocole ******************************
int estPresent(int socket)
{
	int indice = -1;
	pthread_mutex_lock(&mutexClients);
	for(int i=0 ; i<nbClients ; i++)
	{
		if (clients[i] == socket) 
		{ 
			indice = i; break; 
		}

	}
	
	pthread_mutex_unlock(&mutexClients);
	return indice;
}


void ajoute(int socket)
{
 	pthread_mutex_lock(&mutexClients);
 	clients[nbClients] = socket;
 	nbClients++;
 	pthread_mutex_unlock(&mutexClients);
}


void retire(int socket)
{

 int pos = estPresent(socket);
 if (pos == -1) 
 {
 	return;
 }
 	
 pthread_mutex_lock(&mutexClients);

 for (int i=pos ; i<=nbClients-2 ; i++)
 {
 	clients[i] = clients[i+1];
 	
 }
	nbClients--;
	pthread_mutex_unlock(&mutexClients);
}


//***** Fin prématurée **********************************************
void PROTOCOLE_Close()
{
 pthread_mutex_lock(&mutexClients);
 for (int i=0 ; i<nbClients ; i++)
 {
 	close(clients[i]);
 }
 
 pthread_mutex_unlock(&mutexClients);
}