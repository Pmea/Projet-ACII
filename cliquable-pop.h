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


Display * dpy;
Window main_fen;

Window log_user_fen;
Window log_pass_fen;
XColor color_fond;

GC gc_glob;

Cursor fleche;


bool init_main_win();
bool init_log_win();
bool init_pop_win();

bool detruire_main_win();
bool detruire_log_win();
bool detruire_pop_win();


void traiter_event(XEvent e);

#endif /*CLICABLE_POP_H*/