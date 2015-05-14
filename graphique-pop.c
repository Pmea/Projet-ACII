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

XColor color_fond;
XColor color_focus;
XColor color_fond_de_fen;

XFontStruct* font;


// a mettre dans une structure 
//typedef struct {
	int ip;
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

//	mail* next;

//} mail;


bool est_present(void){		// va tester si la fenetre est deja presente
	return true;		
}


void init_mail_win_graphique(int num_msg){
	init_window=true;

	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey", &color_fond, &color_fond);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "DimGrey", &color_focus, &color_focus);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "WhiteSmoke", &color_fond_de_fen , &color_fond_de_fen );

	mail_fen = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
			    WIDTH_MAIL_WIN,
				HEIGHT_MAIL_WIN,
				BORDER, 
				BlackPixel(dpy,DefaultScreen(dpy)), 
				color_fond_de_fen.pixel);	

	mail_contenu_fen= XCreateSimpleWindow(dpy, mail_fen, MARGIN, MARGIN,
					       WIDTH_MAIL_CONTENU,
					       HEIGHT_MAIL_CONTENU,
					       BORDER/2, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));


	if ((font=XLoadQueryFont(dpy,"fixed"))==NULL){
	 	fprintf(stderr," Sorry, having font problems.\n");
	    exit(-1);
	}

	// faire la requete 
	char * contenu_mail= (char*) malloc(sizeof(char) * 4096);
	retr_handler(num_msg, contenu_mail);
	//printf("contenu_mail:%s\n", contenu_mail );

	int font_direction, font_ascent, font_descent;
	XCharStruct text_structure;
	char string_etal[]= "lj";
	XTextExtents(font, string_etal, strlen(string_etal),
	             &font_direction, &font_ascent, &font_descent,
	             &text_structure);

	int width_max= WIDTH_MAIL_CONTENU;
	int width_tmp=0;
	int nb_ligne=0;

	height_ligne=font_descent + font_ascent;
	printf("height_ligne: %d\n", height_ligne );

	// traiter le text
	contenu_mail_traiter= (char*) malloc(sizeof(char) * 4096);
	contenu_mail_traiter[0]='\0';
	char* line;
	int cmpt_total=0;

	for(line= strtok(contenu_mail, "\n") ; line != NULL; line= strtok(NULL, "\n")){
		nb_ligne++;
		width_tmp=0;

		//printf("line: %s\n", line);

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
						// inserer un retour a la ligne 
						strcat(contenu_mail_traiter, "\n");
						width_tmp=0;
						//printf("NOUVELLE LIGNE\n");
						nb_ligne++;
					}
					else{
						width_tmp+= text_structure.width;
					}	
					//printf("width:%d\n", width_tmp);

					//char* coincoin= strndup( debutmot+debut, cmpt-debut+1);
					//printf("MOT: |%s|\n", coincoin);
					/*if(debutmot[cmpt+1] == '\r'){
						printf("ICI\n");
					}*/
					strncat(contenu_mail_traiter, debutmot+debut, cmpt-debut+1);

					debut+=(cmpt-debut+1);
				}
			}
			cmpt++;
		}
		if(cmpt-debut > 1){
			XTextExtents(font, debutmot+debut, cmpt-debut+1, &font_direction, &font_ascent,  &font_descent, &text_structure);
			if(width_tmp + text_structure.width> width_max){
				// inserer un retour a la ligne 
				strcat(contenu_mail_traiter, "\n");
				width_tmp=0;
				//printf("NOUVELLE LIGNE\n");
				nb_ligne++;
			}
			else{
				width_tmp+= text_structure.width;
			}	
			//printf("width:%d\n", width_tmp);

			//char* coincoin= strndup( debutmot+debut, cmpt-debut-1);
			//printf("MOT: |%s|\n", coincoin);
			strncat(contenu_mail_traiter, debutmot+debut, cmpt-debut);
			debut+=(cmpt-debut+1);
			cmpt++;
		}
		strcat(contenu_mail_traiter, "\n");
		cmpt_total+= cmpt;
	}

	free(contenu_mail);
	height_contenu_inter= nb_ligne * (height_ligne + BORDER);

	//printf("traité\n|%s|\n", contenu_mail_traiter);
	printf("nb_ligne %d height %d\n", nb_ligne, nb_ligne * (height_ligne + BORDER));
	// faire une fenetre adapté 
	mail_contenu_inter= XCreateSimpleWindow(dpy, mail_contenu_fen, 0, 0,
					       WIDTH_MAIL_CONTENU,
					       height_contenu_inter,
					       0, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));


	quit_button=  XCreateSimpleWindow(dpy, mail_fen, MARGIN, HEIGHT_MAIL_WIN  -MARGIN - HEIGHT_BUTTON,
					       WIDTH_BUTTON,
					       HEIGHT_BUTTON,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);

	slide_fond=  XCreateSimpleWindow(dpy, mail_fen, WIDTH_MAIL_WIN - MARGIN - WIDTH_FOND_SLIDE, MARGIN,
					       WIDTH_FOND_SLIDE,
					       HEIGHT_MAIL_CONTENU,
					       BORDER/2, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));
					       
	slider=  XCreateSimpleWindow(dpy, slide_fond,  0, 0,
					       WIDTH_SLIDER,
					       HEIGHT_SLIDER,
					       0, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_focus.pixel);
	posslider=0;			       

	// test pour garder le gc 
	gc_glob=XCreateGC(dpy,mail_fen,0,NULL);
 	XSetForeground(dpy,gc_glob,BlackPixel(dpy,DefaultScreen(dpy)));
 	XSetBackground(dpy,gc_glob,color_fond.pixel); 


	XSelectInput(dpy, quit_button,  ButtonPressMask | ExposureMask);
	XSelectInput(dpy, mail_contenu_inter,  ExposureMask);
	XSelectInput(dpy, slider, PointerMotionMask |  ButtonReleaseMask | ButtonPressMask | ExposureMask);


	XMapWindow(dpy, mail_fen);
	XMapSubwindows(dpy, mail_fen);
	XMapWindow(dpy, mail_contenu_fen);
	XMapSubwindows(dpy, mail_contenu_fen);
	XMapWindow(dpy, slide_fond);
	XMapSubwindows(dpy, slide_fond);

	XFlush(dpy);
}

void destroy_mail_win_graphique(void){
	init_window=false;

	XDestroyWindow(dpy,  mail_contenu_inter);
	XDestroyWindow(dpy, mail_contenu_fen);
	XDestroyWindow(dpy,  quit_button);
	XDestroyWindow(dpy,  slider);
	XDestroyWindow(dpy,  slide_fond);
	XDestroyWindow(dpy, mail_fen);


	XFreeGC(dpy, gc_glob);
	XFreeFont(dpy, font);
}

void traiter_ButtonRelease_sur_mail_graphique(XButtonEvent  xbe){
	if(xbe.window == slider){
		printf("SLIDER unclic\n");
		return;
	}
}

void expose_graphique(Window win){
	//chercher la fenetre qui a recu le expose
	if(init_window== true)
		XDrawString(dpy, quit_button, gc_glob, MARGIN/2, MARGIN*3/2, "Quit", strlen("Quit"));
	//rechercher dans la liste des fenetres
	
	if(contenu_mail_traiter != NULL){
		int i=0;
		int line=0;
		int j;
		while(contenu_mail_traiter[i] != '\0'){
			j=0;
			line++;
			while(contenu_mail_traiter[i+j] != '\n'){
				j++;
			}
			XDrawString(dpy, mail_contenu_inter, gc_glob, 5, line*(height_ligne+ BORDER), contenu_mail_traiter+i, j);
			i+=j;
			i++;
		}
	}
}


void traiter_ButtonPress_sur_mail_graphique(XButtonEvent  xbe){	// besoin de regarder la fenetre !! main ou l'autre 
	printf("buttonPress event\n");
	if(xbe.window == quit_general){
		quit_cliquable=true;
		return;
	}

	if(xbe.window == quit_button){
		printf("Quit fenetre\n");
		destroy_mail_win_graphique();
		return;
	}

	if(xbe.window == slider){
		printf("SLIDER clic\n");
		XEvent tmp;
		XNextEvent(dpy, &tmp);

		int pos= xbe.y_root;
		while(tmp.type != ButtonRelease){
			printf("BOUCLE\n");
			if(tmp.type == Expose){
				expose_graphique(slider);
			}

			if(tmp.type == MotionNotify){
				int diff= pos - tmp.xmotion.y_root;
				pos= tmp.xmotion.y_root;

				if(posslider+diff <= 0 && posslider+diff >= -1*(HEIGHT_MAIL_CONTENU- HEIGHT_SLIDER)){
					XMoveWindow(dpy, slider, 0, -1 * (posslider + diff) );
					posslider+= diff;
				}
				if(posslider+diff <= 0 && posslider+diff >= -1*(HEIGHT_MAIL_CONTENU- HEIGHT_SLIDER))
				XMoveWindow(dpy, mail_contenu_inter,0, ((posslider + diff)*height_contenu_inter) / HEIGHT_MAIL_CONTENU );
			}
			XNextEvent(dpy, &tmp);
		}
		return;
	}
	// if pas deja dans la liste
	int tmp= numero_msg(xbe.window);
	if( tmp == -1){
		printf("Erreur lors de la recherche du numero de message\n");
		exit(EXIT_FAILURE);
	}
	init_mail_win_graphique(tmp);
}



void traiter_ExposeEvent_mail_graphique(XExposeEvent xee){
	printf("Expose Event: %d\n", xee.count);

	if(xee.count == 0){
		printf("Last Expose Event\n");
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
		printf("FIN EVENT\n");
		return;
	}

	if(e.type == ButtonRelease){
		traiter_ButtonRelease_sur_mail_graphique(e.xbutton);
		return;
	}

	if(e.type == MotionNotify){
		//il faudrait faire un traitant
		return;
	}
	printf("PAS RECONNU EVENT \n");
}