#ifndef CLICABLE_POP_H
#define CLICABLE_POP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h> 


#include "basique-pop.h"
#include "graphique-pop.h"

#define MAX_LENGTH 56


bool quit_cliquable;
bool quit_log;



Display * dpy;

char user_text[MAX_LENGTH];
char pass_text[MAX_LENGTH];
char msg_erreur[MAX_LENGTH];


bool init_main_win(void);
bool init_log_win(void);
bool init_pop_win(int nb_mail);

bool detruire_main_win(void);
bool detruire_log_win(void);
bool detruire_pop_win(void);


void traiter_event(XEvent e);
void initialiser_champs(void);
void afficher_msg(char* msg);

#endif /*CLICABLE_POP_H*/