#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>
#include "ovesp.h"


//**********Etat du protocole : Liste des clients loggés ***************

int clients[NB_MAX_CLIENTS];
int nbClients = 0;


int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER; 

char requete[200];


//***************** Parsing de la requete******************
bool ovesp(char * requete, char * reponse, int socket)//, MYSQL* connexion)
{//faire connexion base d edonner si co null on fait sinon on la deja 

	// Connection a MySql
  	printf("Connection a la BD...\n");
  	MYSQL* my_connexion = mysql_init(NULL);
  	mysql_real_connect(my_connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0);

	
	//***********Recuperation nom de la requete********** 
	char * ptr = strtok(requete,"#"); 



	// ***** LOGIN ******************************************
 	if (strcmp(ptr,"LOGIN") == 0) // aller vour base d eoner si log 
 	{
 		char user[50], password[50];
 		int newClient;
 		int retour;
 		MYSQL_RES *resultat;
	 	MYSQL_ROW Tuple;

 		strcpy(user,strtok(NULL,"#"));
 		strcpy(password,strtok(NULL,"#"));
 		newClient= atoi(strtok(NULL,"#"));
 		printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);

 		if(estPresent(socket)>=0) // client déja loggé
 		{
 			sprintf(reponse,"LOGIN#ko#Client déjà loggé !");
 			return true;
 		}
 		else
 		{

	 		if(newClient == 0) // 0 = pas cocher
			{
				sprintf(requete,"Select * from clients where nom like '%s'",user);

				mysql_query(my_connexion,requete);

				resultat= mysql_store_result(my_connexion);


				if(resultat)
				{
					if(mysql_num_rows(resultat)==1)// si il a trouvé une ligne
					{
						Tuple = mysql_fetch_row(resultat);
						printf("ACCESBD) RESULTAT : %s, %s\n", Tuple[0],Tuple[1]);
					
						if(strcmp(password,Tuple[2])==0)
						{
							return sprintf(reponse, "LOGIN#ok#Vous etes bien connecté");// connecter
						}
						else
						{
							return sprintf(reponse, "LOGIN#ko#Mauvais mot de passe"); // mauvais mdp
						}
					}
					else
					{
						return sprintf(reponse, "LOGIN#ko#Le login introuvable");; // login introuvable
					}
				}
				else // si probleme dans la requete
				{
					return sprintf(reponse,"LOGIN#KO#Erreur dans la requete SQL1");

				}
			}
			else
			{
				sprintf(requete,"Select * from clients where nom like '%s'",user);

				mysql_query(my_connexion, requete);
				resultat = mysql_store_result(my_connexion);

				if(resultat)
				{
					if(mysql_num_rows(resultat)==1)// si il touve une ligne c'est que l'utilisateur existe deja
					{
						return sprintf(reponse, "LOGIN#ok#L'identifiant est déja prit");
					}
					else
					{
						sprintf(requete,"INSERT INTO clients (nom,mdp) VALUES ( '%s','%s');",user,password);

						mysql_query(my_connexion,requete);

						return sprintf(reponse, "LOGIN#ok#Vous êtes bien enregistrez et connecté"); // enregistrer et connecter
					}

				}
				else
				{
					return sprintf(reponse,"LOGIN#KO#Erreur dans la requete SQL2"); // probleme lors de la requete
				}
			}

			return true;

		}	
 		
 }

 	//*******CONSULT**************************

 	else if(strcmp(ptr,"CONSULT")==0)
 	{
 		MYSQL_RES *resultat;
	 	MYSQL_ROW Tuple;
	 	char requete[200];
	 	
	 	char idArticle[10];

	 	//ACCESBD
	 	strcpy(idArticle,strtok(NULL,"#"));

	 	if(*idArticle>0 & *idArticle<22)
	 	{
		 	sprintf(requete,"Select *from articles where id= %s",idArticle); // recup les infos de l'articles en fonction de l'id
		 	mysql_query(my_connexion, requete); // excecution de la requete 

		 	resultat= mysql_store_result(my_connexion);

		 	if(resultat)
		 	{
		 		Tuple = mysql_fetch_row(resultat);// test si result retourne null
		 		if(Tuple != NULL)
		 		{
		 			sprintf(reponse,"CONSULT# : %d,%s,%d,%f,%s\n", Tuple[0],Tuple[1],Tuple[2],Tuple[3],Tuple[4]);
		 		}
		 		else
		 		{
		 			sprintf(reponse,"CONSULT#ko#Erreur BD");
		 		}

		 	}

	 	}
	 	else
	 	{
	 		sprintf(reponse,"CONSULT#-1");
	 	}
 	
 		
 	}

 	//*******ACHAT***************************
 	else if(strcmp(ptr,"ACHAT")==0)
 	{
 		MYSQL_RES *resultat;
	 	MYSQL_ROW Tuple;
	 	char requete[200];

 	}
 	//*******CADDIE***************************
 	else if(strcmp(ptr,"CADDIE")==0)
 	{
 		MYSQL_RES *resultat;
	 	MYSQL_ROW Tuple;
	 	char requete[200];

 	}
 	//*******CANCEL***************************
 	else if(strcmp(ptr,"CANCEL")==0)
 	{
 		MYSQL_RES *resultat;
	 	MYSQL_ROW Tuple;
	 	char requete[200];

 	}
 	//*******CANCEL_ALL***************************
 	else if(strcmp(ptr,"CANCEL_ALL")==0)
 	{
 		MYSQL_RES *resultat;
	 	MYSQL_ROW Tuple;
	 	char requete[200];

	 	

 	}
 	//*******LOGOUT***************************
 	else if(strcmp(ptr,"LOGOUT")==0)
 	{
 		printf("\t[THREAD %p] LOGOUT\n",pthread_self());
 		retire(socket);
 		sprintf(reponse,"LOGOUT#ok");
 		return true;
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
void ovesp_Close()
{
 pthread_mutex_lock(&mutexClients);
 for (int i=0 ; i<nbClients ; i++)
 {
 	close(clients[i]);
 }
 
 pthread_mutex_unlock(&mutexClients);
}






