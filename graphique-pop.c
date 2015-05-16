#include "graphique-pop.h"
#include "cliquable-pop.h"

#define WIDTH_MAIL_WIN 600
#define HEIGHT_MAIL_WIN 200

#define WIDTH_BUTTON 75
#define HEIGHT_BUTTON 20

#define WIDTH_FOND_SLIDE 20

#define WIDTH_MAIL_CONTENU (WIDTH_MAIL_WIN  - WIDTH_FOND_SLIDE- 2* MARGIN)
#define HEIGHT_MAIL_CONTENU (HEIGHT_MAIL_WIN - HEIGHT_BUTTON - 4* MARGIN)


#define WIDTH_SLIDER 20 
#define HEIGHT_SLIDER 40 

#define BORDER 2
#define MARGIN 10

#define N 10 +1


#define CODE_CURS_XC_draft_large 60

Cursor cursor;




XColor color_fond;
XColor color_focus;
XColor color_fond_de_fen;

XFontStruct* font;

bool besoin_msg_erreur;


typedef struct mail_t{
	int num_du_msg;
	bool init_window;

	Window mail_fen;
	Window mail_contenu_fen;
	Window mail_contenu_inter;
	Window quit_button;
	Window slide_fond;
	Window slider;
	GC gc_glob;

	char* contenu_mail_traiter;
	int height_ligne;
	int height_contenu_inter;
	int posslider;

} mail;

mail tab_mails[N+1];


bool est_present(int id){
	if(tab_mails[id].init_window == true)
		return true;
	return false;
}

// chargement des parametre commun a toutes les fenetres
void init_graphique(void){
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey", &color_fond, &color_fond);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "DimGrey", &color_focus, &color_focus);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "WhiteSmoke", &color_fond_de_fen , &color_fond_de_fen );

	if ((font=XLoadQueryFont(dpy,"-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso8859-1"))==NULL){
	 	fprintf(stderr," Sorry, having font problems.\n");
	    exit(-1);
	}
	cursor= XCreateFontCursor(dpy, CODE_CURS_XC_draft_large);
}

// initialisation d'une fenetre mail, avec du message
void init_mail_win_graphique(int num_msg){
	tab_mails[num_msg].mail_fen = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
			    WIDTH_MAIL_WIN,
				HEIGHT_MAIL_WIN,
				BORDER, 
				BlackPixel(dpy,DefaultScreen(dpy)), 
				color_fond_de_fen.pixel);	

	tab_mails[num_msg].mail_contenu_fen= XCreateSimpleWindow(dpy, tab_mails[num_msg].mail_fen, MARGIN, MARGIN,
					       WIDTH_MAIL_CONTENU,
					       HEIGHT_MAIL_CONTENU,
					       BORDER/2, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));

	// faire la requete 
	char * contenu_mail= (char*) malloc(sizeof(char) * 4096);
	retr_handler(num_msg, contenu_mail);
	if(strcmp(contenu_mail, "") == 0){
		XDrawString(dpy, main_fen, DefaultGC(dpy,DefaultScreen(dpy)), 10, 20, "Mail multipart non conforme", strlen("Mail multipart non conforme") );
		besoin_msg_erreur=true;
		return;
	}

	int font_direction, font_ascent, font_descent;
	XCharStruct text_structure;
	char string_etal[]= "lj";								// pour avoir la taille max d'une ligne 
	XTextExtents(font, string_etal, strlen(string_etal),
	             &font_direction, &font_ascent, &font_descent,
	             &text_structure);

	int width_max= WIDTH_MAIL_CONTENU;
	int width_tmp=0;
	int nb_ligne=0;

	tab_mails[num_msg].height_ligne=font_descent + font_ascent;

	// traiter le text
	tab_mails[num_msg].contenu_mail_traiter= (char*) malloc(sizeof(char) * 4096);
	tab_mails[num_msg].contenu_mail_traiter[0]='\0';
	char* line;
	int cmpt_total=0;

	for(line= strtok(contenu_mail, "\n") ; line != NULL; line= strtok(NULL, "\n")){		// on lit le texte ligne par ligne 
		nb_ligne++;
		width_tmp=0;

		char* debutmot=line;
		int cmpt=0;
		int debut=0;

		while(debutmot[cmpt] != '\n' && debutmot[cmpt] != '\0' ){
			if(debutmot[cmpt] == '\r' ){
				debutmot[cmpt] = ' ';
			}
			else{
				if(debutmot[cmpt] == ' ' ){
					XTextExtents(font, debutmot+debut, cmpt-debut+1, &font_direction, &font_ascent,  &font_descent, &text_structure);
					if(width_tmp + text_structure.width> width_max){
						strcat(tab_mails[num_msg].contenu_mail_traiter, "\n");
						width_tmp=0;
						nb_ligne++;
					}
					else{
						width_tmp+= text_structure.width;
					}	
			
					strncat(tab_mails[num_msg].contenu_mail_traiter, debutmot+debut, cmpt-debut+1);

					debut+=(cmpt-debut+1);
				}
			}
			cmpt++;
		}
		if(cmpt-debut > 1){
			XTextExtents(font, debutmot+debut, cmpt-debut+1, &font_direction, &font_ascent,  &font_descent, &text_structure);
			if(width_tmp + text_structure.width> width_max){
				strcat(tab_mails[num_msg].contenu_mail_traiter, "\n");
				width_tmp=0;
				nb_ligne++;
			}
			else{
				width_tmp+= text_structure.width;
			}	
			
			strncat(tab_mails[num_msg].contenu_mail_traiter, debutmot+debut, cmpt-debut);
			debut+=(cmpt-debut+1);
			cmpt++;
		}
		strcat(tab_mails[num_msg].contenu_mail_traiter, "\n");
		cmpt_total+= cmpt;
	}

	free(contenu_mail);
	tab_mails[num_msg].height_contenu_inter= nb_ligne * (tab_mails[num_msg].height_ligne + BORDER) + MARGIN;

	// faire une fenetre adapté 
	tab_mails[num_msg].mail_contenu_inter= XCreateSimpleWindow(dpy, tab_mails[num_msg].mail_contenu_fen, 0, 0,
					       WIDTH_MAIL_CONTENU,
					       tab_mails[num_msg].height_contenu_inter,
					       0, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));


	tab_mails[num_msg].quit_button=  XCreateSimpleWindow(dpy, tab_mails[num_msg].mail_fen, MARGIN, HEIGHT_MAIL_WIN  -MARGIN - HEIGHT_BUTTON,
					       WIDTH_BUTTON,
					       HEIGHT_BUTTON,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);

	tab_mails[num_msg].slide_fond=  XCreateSimpleWindow(dpy, tab_mails[num_msg].mail_fen, WIDTH_MAIL_WIN - MARGIN - WIDTH_FOND_SLIDE, MARGIN,
					       WIDTH_FOND_SLIDE,
					       HEIGHT_MAIL_CONTENU,
					       BORDER/2, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));
					       
	tab_mails[num_msg].slider=  XCreateSimpleWindow(dpy, tab_mails[num_msg].slide_fond,  0, 0,
					       WIDTH_SLIDER,
					       HEIGHT_SLIDER,
					       0, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_focus.pixel);
	tab_mails[num_msg].posslider=0;			       

	tab_mails[num_msg].gc_glob=XCreateGC(dpy, tab_mails[num_msg].mail_fen, 0, NULL);
 	XSetForeground(dpy, tab_mails[num_msg].gc_glob, BlackPixel(dpy,DefaultScreen(dpy)));
 	XSetBackground(dpy, tab_mails[num_msg].gc_glob, color_fond.pixel); 


	XSelectInput(dpy, tab_mails[num_msg].quit_button,  ButtonPressMask | ExposureMask);
	XSelectInput(dpy, tab_mails[num_msg].mail_contenu_inter, ExposureMask);
	XSelectInput(dpy, tab_mails[num_msg].slider, PointerMotionMask |  ButtonReleaseMask | ButtonPressMask | ExposureMask);

	XDefineCursor(dpy, tab_mails[num_msg].quit_button, cursor);


	XMapWindow(dpy, tab_mails[num_msg].mail_fen);
	XMapSubwindows(dpy, tab_mails[num_msg].mail_fen);
	XMapWindow(dpy, tab_mails[num_msg].mail_contenu_fen);
	XMapSubwindows(dpy, tab_mails[num_msg].mail_contenu_fen);
	XMapWindow(dpy, tab_mails[num_msg].slide_fond);
	XMapSubwindows(dpy, tab_mails[num_msg].slide_fond);

	XFlush(dpy);

	tab_mails[num_msg].init_window=true;
}

void destroy_mail_win_graphique(int num_msg){
	tab_mails[num_msg].init_window=false;

	XDestroyWindow(dpy,  tab_mails[num_msg].mail_contenu_inter);
	XDestroyWindow(dpy, tab_mails[num_msg].mail_contenu_fen);
	XDestroyWindow(dpy,  tab_mails[num_msg].quit_button);
	XDestroyWindow(dpy,  tab_mails[num_msg].slider);
	XDestroyWindow(dpy,  tab_mails[num_msg].slide_fond);
	XDestroyWindow(dpy, tab_mails[num_msg].mail_fen);
	XFreeGC(dpy,tab_mails[num_msg].gc_glob);

}

// detruire les variables globales et  les fenetres graphiques encore ouverte
void destroy_graphique(void){
	if(font != NULL)
		XFreeFont(dpy, font);
	XFreeCursor(dpy, cursor );
	int i;
	for(i=0; i< N; i++)
		if(tab_mails[i].init_window == true)
			destroy_mail_win_graphique(i);
}



void expose_graphique(Window win){
	if(besoin_msg_erreur==true){
		XClearWindow(dpy, main_fen);
	}

	int k;
	for(k=0; k<N; k++){
		if(tab_mails[k].init_window == true){
			XDrawString(dpy, tab_mails[k].quit_button, tab_mails[k].gc_glob, MARGIN/2, MARGIN*3/2, "Quit", strlen("Quit"));
			
			if(tab_mails[k].contenu_mail_traiter != NULL){
				int i=0;
				int line=0;
				int j;

				while(tab_mails[k].contenu_mail_traiter[i] != '\0'){			// on relit le texte et on affiche ligne par ligne
					j=0;
					line++;
					while(tab_mails[k].contenu_mail_traiter[i+j] != '\n'){
						j++;
					}

					XDrawString(dpy, tab_mails[k].mail_contenu_inter, tab_mails[k].gc_glob, 5, line*(tab_mails[k].height_ligne+ BORDER), tab_mails[k].contenu_mail_traiter+i, j);
					i+=j;
					i++;
				}
			}
		}
	}
}


void traiter_ButtonPress_sur_mail_graphique(XButtonEvent  xbe){ 
	if(xbe.window == quit_general){
		quit_cliquable=true;
		return;
	}

	int i;
	for(i=0; i< N; i++){
		if(xbe.window == tab_mails[i].quit_button){
			destroy_mail_win_graphique(i);
			return;
		}
	}
	
	for(i=0; i<N; i++){
		if(xbe.window == tab_mails[i].slider){
			XEvent tmp;
			XNextEvent(dpy, &tmp);

			int pos= xbe.y_root;
			while(tmp.type != ButtonRelease){
				if(tmp.type == Expose){
					expose_graphique(tab_mails[i].slider);
				}

				if(tmp.type == MotionNotify){
					int diff= pos - tmp.xmotion.y_root;
					pos= tmp.xmotion.y_root;

					if(tab_mails[i].posslider+diff <= 0 && tab_mails[i].posslider+diff >= -1*(HEIGHT_MAIL_CONTENU- HEIGHT_SLIDER)){
						XMoveWindow(dpy, tab_mails[i].slider, 0, -1 * (tab_mails[i].posslider + diff) );
						tab_mails[i].posslider+= diff;
					}
					if(tab_mails[i].posslider+diff <= 0 && tab_mails[i].posslider+diff >= -1*(HEIGHT_MAIL_CONTENU- HEIGHT_SLIDER))
					XMoveWindow(dpy, tab_mails[i].mail_contenu_inter,0, ((tab_mails[i].posslider + diff)*tab_mails[i].height_contenu_inter) / HEIGHT_MAIL_CONTENU );
				}
				XNextEvent(dpy, &tmp);
			}
			return;
		}
	}

	int tmp= numero_msg(xbe.window);
	if( tmp == -1){
		printf("Erreur lors de la recherche du numero de message\n");
		exit(EXIT_FAILURE);
	}
	if(est_present(tmp)== false){
		init_mail_win_graphique(tmp);
	}
	
}


void traiter_ExposeEvent_mail_graphique(XExposeEvent xee){

	if(xee.count == 0){
		expose_mail_graphique();
		expose_graphique(xee.window);
	}
}

void traiter_event_mails_graphique(XEvent e){
	if(e.type == Expose){
		traiter_ExposeEvent_mail_graphique(e.xexpose);
		return;
	}
	if(e.type == ButtonPress){
		traiter_ButtonPress_sur_mail_graphique(e.xbutton);
		return;
	}

}

