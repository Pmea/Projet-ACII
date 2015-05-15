#ifndef TEXTUELPOP_H
#define TEXTUELPOP_H

#include <stdbool.h>

bool init_connexion(char* serveur, int port, char* sortie);
bool close_connexion(void);

bool user_handler(char* arg, char* sortie);
bool pass_handler(char* arg, char* sortie);

int list_handler(char* sortie);

void retr_handler(int id_msg, char* sortie );
bool top_handler(int id_msg, int nb_ligne, char* sortie);

#endif /*TEXTUELPOP_H*/