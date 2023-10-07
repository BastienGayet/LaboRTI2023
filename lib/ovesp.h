#ifndef OVESP_H
#define OVESP_H
#define NB_MAX_CLIENTS 20
#include <mysql.h>


typedef struct
{
  int   idArticle;
  char  intitule[20];
  float prix;
  int   stock;  
  char  image[20];
} ARTICLE;

typedef struct 
{
    int idArticle;
    char intitule[100];
    int stock;
    float prix;
    char image[100];
}CaddieArticle;


bool ovesp(char* requete, char* reponse , int socket);// ouvre la dedans la connexion base de donnee et mettre les autre fonctions dedans
int ovesp_Login(const char* user, const char* password, int IsNouveauClient);
void ovesp_Consult(int idArticle);
int ovesp_Achat(int idArticle, int quantite);
bool ovesp_Cancel(int idArticle);
bool ovesp_CancelAll();
void ovesp_Close();

#endif