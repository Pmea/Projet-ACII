#ifndef TEXTUELPOP_H
#define TEXTUELPOP_H

#include <stdbool.h>

bool init_connexion(char* serveur, int port);
bool close_connexion(void);

void user_handler(char* arg);
void pass_handler(char* arg);

void list_handler(void);

void retr_handler(int id_msg);
void top_handler(int id_msg, int nb_ligne);


#endif /*TEXTUELPOP_H*/