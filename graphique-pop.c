#include "graphique-pop.h"
#include "cliquable-pop.h"



void init_pop_win_graphique(int nb_msg, char ** top_tab){
	// inutilisé pour l'instant
}

void destroy_pop_graphique(void){
	// inutilisé pour l'instant
}

void traiter_ButtonPress_sur_mail_graphique(XButtonEvent  xbe){
	printf("buttonPress event\n");
}

void traiter_event_mails_graphique(XEvent e){
	if(e.type == Expose){
		traiter_ExposeEvent_mail(e.xexpose);
		return;
	}
	if(e.type == ButtonPress){
		traiter_ButtonPress_sur_mail_graphique(e.xbutton);
		return;
	}
	printf("PAS RECONNU EVENT \n");
}