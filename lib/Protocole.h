#ifndef PROTOCOLE_H
#define PROTOCOLE_H
#define NB_MAX_CLIENTS 20
#include <mysql.h>


typedef struct
{
  int   id;
  char  intitule[20];
  float prix;
  int   stock;  
  char  image[20];
} ARTICLE;

bool PROTOCOLE(char* requete, char* reponse , int socket);
bool PROTOCOLE_Login(const char* user, const char* password);
void PROTOCOLE_Close();

#endif