#ifndef PROTOCOLE_H
#define PROTOCOLE_H
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


bool ovesp(char* requete, char* reponse , int socket, MYSQL* connexion, CaddieArticle caddie[21]);
bool ovesp_Login(const char* user, const char* password, int IsNouveauClient);
ARTICLE ovesp_Consult(int idArticle, MYSQL* connexion);
int ovesp_Achat(int idArticle, MYSQL* connexion, int quantite , CaddieArticle caddie[21]);
bool ovsp_Cancel(int idArticle,MYSQL* connexion,CaddieArticle caddie[21]);
bool ovsp_CancelAll(MYSQL* connexion,CaddieArticle caddie[21]);
void ovsp_Close();

#endif