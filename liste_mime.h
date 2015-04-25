#ifndef LISTE_MIME_H
#define LISTE_MIME_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <regex.h>


void ajouter_mime_type(char* type, char* ext);
void supp_entete_liste_mime_type(void);

char* rechercher(char* type);

bool construire_liste_mime(void);
bool detruire_liste_mime(void);

#endif	/* LISTE_MIME_H */