#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <mysql.h>
#include "TCP.h"
#include "ovesp.h"

void HandlerSIGINT(int s);
void TraitementConnexion(int sService);
void* FctThreadClient(void* p);

int sEcoute;

//Gestion du pool de threads
#define NB_THREADS_POOL 2
#define TAILLE_FILE_ATTENTE 20
int socketsAcceptees[TAILLE_FILE_ATTENTE];
int indiceEcriture=0, indiceLecture=0;
pthread_mutex_t mutexSocketsAcceptees;
pthread_cond_t condSocketsAcceptees;

MYSQL* connexion;

int main(int argc, char * argv[])
{
	if(argc !=2)
	{
		printf("Erreur...\n");
		printf("USAGE : Serveur portServeur\n");
		exit(1);
	}



	// Initialisation socketsAcceptees
	pthread_mutex_init(&mutexSocketsAcceptees,NULL);
 	pthread_cond_init(&condSocketsAcceptees,NULL);
 	
 	for (int i=0 ; i<TAILLE_FILE_ATTENTE ; i++)
 	{
 		socketsAcceptees[i]= -1;
 	}
 	



	// Armement des signaux
	struct sigaction A;
	A.sa_flags = 0;
	sigemptyset(&A.sa_mask);
	A.sa_handler = HandlerSIGINT;
	if (sigaction(SIGINT,&A,NULL) == -1)
	{
		perror("Erreur de sigaction");
		exit(1);
	}

	//Creation de la socket d'écoute

 	if ((sEcoute = ServerSocket(atoi(argv[1]))) == -1)
 	{
 		perror("Erreur de ServeurSocket");
 		exit(1);
 	}

 	/*connexion = mysql_init(NULL);
  if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
  {
    fprintf(stderr,"(ACCESBD) Erreur de connexion à la base de données...\n");
    exit(1);  
  }
  else
  {
  	fprintf(stderr,"Connexion reussie à la base de données...\n");
  }*/

 	// Mise en boucle du serveur
	int sService;
	pthread_t th;
	char ipClient[50];
	printf("Demarrage du serveur.\n");
	while(1)
	{
	  printf("Attente d'une connexion...\n");
	  if ((sService = Accept(sEcoute,ipClient)) == -1)
	 {
	   perror("Erreur de Accept");
	   close(sEcoute);
	   ovesp_Close();
	   exit(1);
	 }

	 printf("Connexion acceptée : IP=%s socket=%d\n",ipClient,sService);
	 // Insertion en liste d'attente et réveil d'un thread du pool
	 // (Production d'une tâche)
	 pthread_mutex_lock(&mutexSocketsAcceptees);

	 socketsAcceptees[indiceEcriture] = sService; // !!!
	 indiceEcriture++;

	 if (indiceEcriture == TAILLE_FILE_ATTENTE) 
	 	indiceEcriture = 0;

	 pthread_mutex_unlock(&mutexSocketsAcceptees);
	 pthread_cond_signal(&condSocketsAcceptees);

	 // Traitement de la connexion (consommation de la tâche)
 	 printf("\t[THREAD %p] Je m'occupe de la socket %d\n",pthread_self(),sService);
 	 TraitementConnexion(sService);

	}

}


void HandlerSIGINT(int s)
{
 printf("\nArret du serveur.\n");
 close(sEcoute);

 pthread_mutex_lock(&mutexSocketsAcceptees);

 for (int i=0 ; i<TAILLE_FILE_ATTENTE ; i++)
 {	
 	if (socketsAcceptees[i] != -1) 
 		close(socketsAcceptees[i]);
 }
 pthread_mutex_unlock(&mutexSocketsAcceptees);
 ovesp_Close();
 mysql_close(connexion);// ferme la connexion sql
 exit(0);
}

void* FctThreadClient(void* p)
{
	int sService;

	while(1)
	{
		printf("\t[THREAD %p] Attente socket...\n",pthread_self());
 		// Attente d'une tâche
 		pthread_mutex_lock(&mutexSocketsAcceptees);
 		while (indiceEcriture == indiceLecture)
 			pthread_cond_wait(&condSocketsAcceptees,&mutexSocketsAcceptees);

		sService = socketsAcceptees[indiceLecture];
		socketsAcceptees[indiceLecture] = -1;
		indiceLecture++;

		if (indiceLecture == TAILLE_FILE_ATTENTE) 
			indiceLecture = 0;

		pthread_mutex_unlock(&mutexSocketsAcceptees);


		 // Traitement de la connexion (consommation de la tâche)
 		printf("\t[THREAD %p] Je m'occupe de la socket %d\n",
 		pthread_self(),sService);


 		TraitementConnexion(sService);
	}
}

void TraitementConnexion(int sService)
{
 char requete[200], reponse[200];
 int nbLus, nbEcrits;
 bool onContinue = true;
 while (onContinue)
 {
   printf("\t[THREAD %p] Attente requete...\n",pthread_self());
   // ***** Reception Requete ******************
   if ((nbLus = Receive(sService,requete)) < 0)
  {
    perror("Erreur de Receive");
    close(sService);
    HandlerSIGINT(0);
  }
   // ***** Fin de connexion ? *****************
   if (nbLus == 0)
  {
    printf("\t[THREAD %p] Fin de connexion du client.\n",pthread_self());
    close(sService);
    return;
  }
   requete[nbLus] = 0;


   printf("\t[THREAD %p] Requete recue = %s\n",pthread_self(),requete);
   // ***** Traitement de la requete ***********
  onContinue = ovesp(requete,reponse,sService);


   // ***** Envoi de la reponse ****************
   if ((nbEcrits = Send(sService,reponse,strlen(reponse))) < 0)
  {
    perror("Erreur de Send");
    close(sService);
    HandlerSIGINT(0);
  }


   printf("\t[THREAD %p] Reponse envoyee = %s\n",pthread_self(),reponse);
 

   if (!onContinue)
   {
   	 printf("\t[THREAD %p] Fin de connexion de la socket %d\n",pthread_self(),sService);
   }
   	
 }
}
