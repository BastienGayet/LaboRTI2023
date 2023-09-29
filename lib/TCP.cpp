#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "TCP.h"


	

	int ServerSocket(int port)
	{
		int sServeur;


		// création de la socket
		if((sServeur = socket(AF_INET, SOCK_STREAM,0))==-1)
		{
			perror("Serveur - Erreur de creation Socket ");
			exit(1);
		}

		printf("Serveur - socket creee = %d\n",sServeur);

		//Construction de l'adresse 

		 struct addrinfo hints;
		 struct addrinfo *results;
		 memset(&hints,0,sizeof(struct addrinfo));
		 hints.ai_family = AF_INET;
		 hints.ai_socktype = SOCK_STREAM;
		 hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // pour une connexion passive

		 char buffer[5];
		 sprintf(buffer,"%d",port);

		 if (getaddrinfo(NULL,buffer,&hints,&results) != 0)
		 {
			close(sServeur);
			exit(1);
		 } 

		 char hosts[NI_MAXHOST];
		 char ports[NI_MAXSERV];
		 getnameinfo(results->ai_addr,results->ai_addrlen,hosts,NI_MAXHOST,ports,
		 	 NI_MAXSERV,NI_NUMERICSERV | NI_NUMERICHOST);

		 printf("Serveur - Mon adresse IP: %s -- Mon Port: %s\n",hosts,ports);




		//Liasion de la socket à l'adresse 

		 if (bind(sServeur,results->ai_addr,results->ai_addrlen) < 0)
		 {
		 	perror("Serveur - Erreur de bind");
		 	exit(1);
		 }


		 freeaddrinfo(results);

		 

		 // Mise à l'écoute de la socket
	 	if (listen(sServeur,SOMAXCONN) == -1)
	 	{
	 		perror("Erreur de listen() Serveur - ");
	 		exit(1);
	 	}
	 		printf("Serveur - listen() reussi !\n");


		 printf("Serveur - bind() reussi !\n");

		  return sServeur;

	}




int Accept(int sEcoute,char *ipClient)
{
	int sService;
	char hosts[NI_MAXHOST];
 	char ports[NI_MAXSERV];

	// Mise à l'écoute de la socket
/*	 if (listen(sEcoute,SOMAXCONN) == -1)
	 {
	 	perror("Erreur de listen() Serveur - ");
	 	exit(1);
	 }
	 	printf("Serveur - listen() reussi !\n");
*/
	 // Attente d'une connexion
	 
	 if ((sService = accept(sEcoute,NULL,NULL)) == -1)
	 {
	 perror(" Serveur - Erreur de accept()");
	 exit(1);
	 }
	 printf(" Serveur - Accept() reussi !");
	 printf(" Serveur - socket de service = %d\n",sService);

	 // Recuperation d'information sur le client connecte
	 struct sockaddr_in adrClient;
	 socklen_t adrClientLen = sizeof(struct sockaddr_in); // nécessaire
	 getpeername(sService,(struct sockaddr*)&adrClient,&adrClientLen);
	 getnameinfo((struct sockaddr*)&adrClient,adrClientLen,hosts,NI_MAXHOST,ports,NI_MAXSERV,NI_NUMERICSERV | NI_NUMERICHOST);
	 printf("Client connecte --> Adresse IP: %s -- Port: %s\n",hosts,ports);


	 return sService;



}


int ClientSocket(char* ipServeur,int portServeur)
{
	int sClient;

	//Creation de la socket

	if((sClient = socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("Client - Erreur de  creation socket ");
		exit(1);
	}

	printf("Client - socket creee = %d\n", sClient);

	
	//Construction de l'adresse du serveur 

	struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;

	char buffer[5];
	sprintf(buffer,"%d",portServeur);

	if(getaddrinfo(ipServeur,buffer,&hints,&results)!=0)
	{
		exit(1);
	}

	//Demande de connexion

	if(connect(sClient,results->ai_addr,results->ai_addrlen)==-1)
	{
		perror("Client - Erreur de connect CLient");
		exit(1);
	}

	printf("Client - Connect() reussi!\n");

	return sClient;


}

int Send(int sSocket,char* data,int taille)
{
	 if (taille > TAILLE_MAX_DATA)
	 	return -1;
	
	 // Preparation de la charge utile
	 char trame[TAILLE_MAX_DATA+2];
	 memcpy(trame,data,taille);
	 trame[taille] = '#';
	 trame[taille+1] = ')';


	 // Ecriture sur la socket
	 return write(sSocket,trame,taille+2)-2;

}

int Receive(int sSocket,char* data)
{
	bool fini = false;
	int nbLus, i = 0;
	char lu1,lu2;


	while(!fini)
	{
		if((nbLus = read(sSocket,&lu1,1))==-1)
		{
			return -1;
		}

		if(nbLus==0) return i; // connexion fermee par client

		if(lu1 =='#')
		{
			if ((nbLus = read(sSocket,&lu2,1)) == -1)
 				return -1;

 			if (nbLus == 0) return i; // connexion fermee par client


 			if (lu2 == ')') fini = true;
	        else
			{
				data[i] = lu1;
				data[i+1] = lu2;
				i += 2;
			}

		}
		else
		{
			data[i] = lu1;
			i++;
		}
	}

	return i;
}

