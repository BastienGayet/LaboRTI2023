#include "ovesp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>


//**********Etat du protocole : Liste des clients loggés ***************

int clients[NB_MAX_CLIENTS];
int nbClients = 0;


int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER; 

char requete[200];


//***************** Parsing de la requete******************
bool ovsp(char * requete, char * reponse, int socket, MYSQL* connexion, CaddieArticle caddie[21])
{
	int idArticle = 0; 

	//***********Recuperation nom de la requete********** 
	char * ptr = strtok(requete,"#");

	 ARTICLE article; 


	// ***** LOGIN ******************************************
 	if (strcmp(ptr,"LOGIN") == 0)
 	{
 		char user[50], password[50];
 		int isNouveauclient = 0;

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
 			if(ovsp_Login(user,password,isNouveauclient))
 			{
 				sprintf(reponse,"LOGIN#OK");
 				ajoute(socket);

 				return true;

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

 	//*******ARTICLE***************************
 	//*******ACHAT***************************
 	//*******CADDIE***************************
 	//*******CANCEL***************************
 	//*******CANCEL_ALL***************************
 	//*******LOGOUT***************************
 	if(strcmp(ptr,"LOGOUT")==0)
 	{
 		printf("\t[THREAD %p] LOGOUT\n",pthread_self());
 		retire(socket);
 		sprintf(reponse,"LOGOUT#ok");
 		return false;
 	}

	
}


ARTICLE ovsp_Consult(int idArticle, MYSQL* connexion)
{
	ARTICLE reponse;
 	MYSQL_RES *resultat;
 	MYSQL_ROW Tuple;

 	//ACCESBD

 	sprintf(requete,"Select *from articles where id= %d",idArticle); // recup les infos de l'articles en fonction de l'id
 	mysql_query(connexion, requete); // excecution de la requete 

 	resultat= mysql_store_result(connexion);

 	if(resultat && idArticle>0 && idArticle<22)
 	{
 		Tuple = mysql_fetch_row(resultat);

 		printf("ACCESBD RESULTAT : %d,%s,%d,%f,%s\n", Tuple[0],Tuple[1],Tuple[2],Tuple[3],Tuple[4]);
 		reponse.idArticle= atoi(Tuple[0]);
 		strcpy(reponse.intitule,Tuple[1]);
 		reponse.prix= atof(Tuple[2]);
 		reponse.stock=atoi(Tuple[3]);
 		strcpy(reponse.image,Tuple[4]);


 		

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
void ovsp_Close()
{
 pthread_mutex_lock(&mutexClients);
 for (int i=0 ; i<nbClients ; i++)
 {
 	close(clients[i]);
 }
 
 pthread_mutex_unlock(&mutexClients);
}