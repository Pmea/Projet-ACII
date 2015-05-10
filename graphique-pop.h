#ifndef GRAPHIQUEPOP_H
#define GRAPHIQUEPOP_H
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


void init_pop_win_graphique(int nb_msg, char ** top_tab);
void destroy_pop_graphique(void);

void traiter_event_mails_graphique(XEvent event_mails);

#endif /*GRAPHIQUEPOP_H*/