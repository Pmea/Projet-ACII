#include "cliquable-pop.h"

#define N 10

#define CODE_CURS_XC_xterm 152
#define CODE_CURS_XC_draft_large 60

#define MARGIN 10
#define BORDER 2

#define WIDTH_LOG 350
#define HEIGHT_LOG 25

#define WIDTH_BUTTON 75
#define HEIGHT_BUTTON 20

#define WIDTH_MAIL WIDTH_LOG*2 + MARGIN + BORDER *2
#define HEIGHT_MAIL 10 * HEIGHT_LOG

#define WIDTH_MAIN WIDTH_LOG*2 + MARGIN * 3 + BORDER * 4 
#define HEIGHT_MAIN 110 + HEIGHT_MAIL


Window main_fen;

Window log_user_fen;
Window log_pass_fen;

Window quit_button;
Window connect_button;

Window pop_fen;
Window mails_fen[N];
char mails_text[N][1024];
int nb_mails;
bool recup_mail[N];

XFontStruct *font;

XColor color_fond;
XColor color_focus;
XColor color_fond_de_fen;


GC gc_glob;

Cursor fleche, clic;

Window focus_fen;
bool focus_init= false;


bool init_main_win(){
	msg_erreur[0]='\0';

	if ((dpy = XOpenDisplay(NULL)) == NULL){
  		printf("Erro open display");
  		exit(EXIT_FAILURE);
  	}

	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey", &color_fond, &color_fond);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "DimGrey", &color_focus, &color_focus);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "WhiteSmoke", &color_fond_de_fen , &color_fond_de_fen );


  	main_fen = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
					       WIDTH_MAIN,
					       HEIGHT_MAIN,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond_de_fen.pixel);

  	quit_button=  XCreateSimpleWindow(dpy, main_fen, MARGIN, HEIGHT_MAIN  -MARGIN - HEIGHT_BUTTON,
					       WIDTH_BUTTON,
					       HEIGHT_BUTTON,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);

	if ((font=XLoadQueryFont(dpy,"fixed"))==NULL){
	 	fprintf(stderr," Sorry, having font problems.\n");
	    exit(-1);
	}

	gc_glob=XCreateGC(dpy,main_fen,0,NULL);
 	XSetFont(dpy,gc_glob,font->fid);
 	XSetForeground(dpy,gc_glob,BlackPixel(dpy,DefaultScreen(dpy)));
 	XSetBackground(dpy,gc_glob,WhitePixel(dpy,DefaultScreen(dpy))); 

 	clic= XCreateFontCursor(dpy, CODE_CURS_XC_draft_large);
	XDefineCursor(dpy, quit_button, clic);

	XSelectInput(dpy, main_fen, KeyPressMask | ExposureMask);
	XSelectInput(dpy, quit_button,  ButtonPressMask | ExposureMask);

	XMapWindow(dpy, main_fen);
  	XMapSubwindows(dpy, main_fen);
	XFlush(dpy);

	return true;
}
bool init_log_win(){
	log_user_fen=  XCreateSimpleWindow(dpy, main_fen, MARGIN, MARGIN,
					       WIDTH_LOG,
					       HEIGHT_LOG,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);

	log_pass_fen=  XCreateSimpleWindow(dpy, main_fen, WIDTH_LOG + MARGIN*2 +BORDER, MARGIN,
					       WIDTH_LOG,
					       HEIGHT_LOG,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);

	connect_button=  XCreateSimpleWindow(dpy, main_fen, WIDTH_MAIN - MARGIN - WIDTH_BUTTON, HEIGHT_MAIN  -MARGIN - HEIGHT_BUTTON,
					       WIDTH_BUTTON,
					       HEIGHT_BUTTON,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);

	fleche= XCreateFontCursor(dpy, CODE_CURS_XC_xterm);
	XDefineCursor(dpy, log_user_fen, fleche);
	XDefineCursor(dpy, log_pass_fen, fleche);
	XDefineCursor(dpy, connect_button, clic);
	
	XSelectInput(dpy, log_user_fen, KeyPressMask | ButtonPressMask |  ExposureMask);
	XSelectInput(dpy, log_pass_fen, KeyPressMask | ButtonPressMask |  ExposureMask);
	XSelectInput(dpy, connect_button, ButtonPressMask | KeyPressMask | ExposureMask);


	initialiser_champs();

	XMapWindow(dpy, log_pass_fen);		
	XMapWindow(dpy, log_user_fen);
	XMapWindow(dpy, connect_button);

	XFlush(dpy);
	return true;
}

bool init_pop_win(int nb_mail, char ** top_mails){
	nb_mails=nb_mail;

	pop_fen= XCreateSimpleWindow(dpy, main_fen, MARGIN, MARGIN*6,
					       WIDTH_MAIL,
					       HEIGHT_MAIL,
					       BORDER,
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);
	XSelectInput(dpy, pop_fen,  ButtonPressMask | ExposureMask);
	XMapWindow(dpy, pop_fen);
	int i;
	for(i=0; i<nb_mail; i++){
		mails_fen[i]=  XCreateSimpleWindow(dpy, pop_fen, -BORDER, -BORDER + HEIGHT_LOG * i,
					       WIDTH_MAIL,
					       HEIGHT_MAIL,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       color_fond.pixel);
		XSelectInput(dpy, mails_fen[i],  ButtonPressMask | ExposureMask);
		
		strncpy(mails_text[i], top_mails[i], 1024);
		XMapWindow(dpy, mails_fen[i]);

		XDrawString(dpy, mails_fen[i], gc_glob, MARGIN/2, MARGIN*3/2, mails_text[i], strlen(mails_text[i]));
	}
	
	return true;
}

bool detruire_main_win(){
	XFreeCursor(dpy, fleche);
	XFreeCursor(dpy, clic);
	XDestroySubwindows(dpy, main_fen);
	//XFreeColors(dpy, DefaultScreen(dpy), color_fond.pixel, 1);
	//XFreeColors(dpy, DefaultScreen(dpy), color_focus.pixel, 1);
	//XFreeColors(dpy, DefaultScreen(dpy), color_fond_de_fen.pixel, 1);
	XFreeGC(dpy, gc_glob);
	XFreeFont(dpy, font);
	XCloseDisplay(dpy);
	return true;
}
bool detruire_log_win(){
	XDestroyWindow(dpy, log_user_fen);
	XDestroyWindow(dpy, log_pass_fen);
	XDestroyWindow(dpy, connect_button);
	return true;
}
bool detruire_pop_win(){
	XDestroySubwindows(dpy, pop_fen);
	return true;
}


void expose(void){
	XDrawString(dpy, log_user_fen, gc_glob, MARGIN/2, MARGIN*3/2, user_text, strlen(user_text));
	XDrawString(dpy, log_pass_fen, gc_glob, MARGIN/2, MARGIN*3/2, pass_text, strlen(pass_text));
	XDrawString(dpy, quit_button, gc_glob, MARGIN/2, MARGIN*3/2, "Quit", strlen("Quit"));
	XDrawString(dpy, connect_button, gc_glob, MARGIN/2, MARGIN*3/2, "Connection", strlen("Connection"));
	XDrawString(dpy, main_fen, gc_glob, MARGIN*2, MARGIN*5, msg_erreur, strlen(msg_erreur));
}

void traiter_ExposeEvent(XExposeEvent xee){
	printf("Expose Event: %d\n", xee.count);

	if(xee.count == 0){
		printf("Last Expose Event\n");
		expose();
	}
}

void change_focus_attibute(Window focus_fen){
	XSetWindowBackground(dpy, log_user_fen, color_fond.pixel);
	XSetWindowBackground(dpy, log_pass_fen, color_fond.pixel);

	if(focus_fen != main_fen){
		XSetWindowBackground(dpy, focus_fen, color_focus.pixel);
	}
	XUnmapWindow(dpy, log_pass_fen);
	XMapWindow(dpy, log_pass_fen);

	XUnmapWindow(dpy, log_user_fen);
	XMapWindow(dpy, log_user_fen);

	expose();
}

void traiter_ButtonPressEvent(XButtonEvent xbp){
	printf("ButtonPress Event\n");
	focus_fen= xbp.window;
	change_focus_attibute(focus_fen);
	focus_init=true;

	if(strlen(msg_erreur) != 0){
		printf("MSG erreur %s\n", msg_erreur);
		XClearWindow(dpy, main_fen);
		msg_erreur[0]='\0';
	}

	if(focus_fen == quit_button){
		printf("Quit\n");
		quit_cliquable=true;
		return;
	}
	if(focus_fen == connect_button){
		printf("CONNECTION\n");
		if( strcmp(user_text, "")!=0 && strcmp(pass_text, "") != 0)
			quit_log=true;
	}
	printf("CHANGEMENT DE FOCUS\n");
}



bool est_caractere_valide(char* key, KeySym sym){
	if(strlen(key) != 1)
		return false;
	return (key[0] >= '!' && key[0] <= '~')  || sym == XK_space;
}


void traiter_KeyPressEvent(XKeyEvent xke){
	if(focus_init == false)
		return; 

	printf("KeyPress event\n");
	char key[16];
	KeySym sym;
	XLookupString(&xke, key, 16, &sym, NULL);

	if( sym == XK_BackSpace){
		XClearWindow(dpy, focus_fen);
			if(strlen(user_text) > 0){
				user_text[strlen(user_text) -1]= '\0';
				XDrawString(dpy, log_user_fen, gc_glob, MARGIN/2, MARGIN*3/2, user_text, strlen(user_text));
			}
		
			if(focus_fen == log_pass_fen){
			if(strlen(pass_text) > 0){
				pass_text[strlen(pass_text) -1]= '\0';
				XDrawString(dpy, log_pass_fen, gc_glob, MARGIN/2, MARGIN*3/2, pass_text, strlen(pass_text));
			}
		}
	}

	if( sym == XK_Tab){
		if(focus_fen == log_user_fen){
			focus_fen= log_pass_fen;
			change_focus_attibute(focus_fen);
		}
		else{
			if(focus_fen == log_pass_fen){
				focus_fen= log_user_fen;
				change_focus_attibute(focus_fen);		
			}
		}
	}

	if(est_caractere_valide(key, sym)){
		if(focus_fen == log_user_fen){
			if( strlen(user_text) < MAX_LENGTH){
				sprintf(user_text, "%s%s", user_text, key);
				XDrawString(dpy, log_user_fen, gc_glob, MARGIN/2, MARGIN*3/2, user_text, strlen(user_text));
			}
		}
		if(focus_fen == log_pass_fen){
			if( strlen(pass_text) < MAX_LENGTH){
				sprintf(pass_text, "%s%s", pass_text, key);
				XDrawString(dpy, log_pass_fen, gc_glob, MARGIN/2, MARGIN*3/2, pass_text, strlen(pass_text));		
			}
		}
	}
	else{
		printf("caractere non valide\n");
	}
}


void traiter_event(XEvent e){

	if(e.type == Expose){
		traiter_ExposeEvent(e.xexpose);
		return;
	}
	if(e.type == ButtonPress){
		traiter_ButtonPressEvent(e.xbutton);
		return;
	}
	if(e.type == KeyPress){
		traiter_KeyPressEvent(e.xkey);
		return;
	}
	printf("PAS RECONNU EVENT \n");
}


void expose_mail(void){
	int i;
	for(i=0; i<nb_mails; i++){
		if(recup_mail[i]== true)
			XSetWindowBackground(dpy, mails_fen[i], color_focus.pixel);
		XUnmapWindow(dpy, mails_fen[i]);
		XMapWindow(dpy, mails_fen[i]);
		XDrawString(dpy, mails_fen[i], gc_glob, MARGIN/2, MARGIN*3/2, mails_text[i], strlen(mails_text[i]));
	}
}

void traiter_ExposeEvent_mail(XExposeEvent xee){
	printf("Expose Event mail: %d\n", xee.count);

	if(xee.count == 0){
		printf("Last Expose Event\n");
		int i;
		for(i=0; i<nb_mails; i++){
			XDrawString(dpy, mails_fen[i], gc_glob, MARGIN/2, MARGIN*3/2, mails_text[i], strlen(mails_text[i]));
		}
	}
}

void traiter_ButtonPress_sur_mail(XButtonEvent xbe){
	if(xbe.window == quit_button){
		printf("Quit\n");
		quit_cliquable=true;
		return;
	}

	int i;
	for(i=0; i<nb_mails; i++){
		if(mails_fen[i] ==  xbe.window && recup_mail[i] == false){
			//recuperer email
			recup_mail[i]=true;
			retr_handler(i+1);
		}
	}
	expose_mail();
}

void traiter_event_mails(XEvent e){
	if(e.type == Expose){
		traiter_ExposeEvent_mail(e.xexpose);
		return;
	}
	if(e.type == ButtonPress){
		traiter_ButtonPress_sur_mail(e.xbutton);
		return;
	}
	printf("PAS RECONNU EVENT \n");
}

void initialiser_champs(){
	user_text[0]='\0';
	pass_text[0]='\0';
	change_focus_attibute(main_fen);
}


void afficher_msg(char* msg){
	strncpy(msg_erreur, msg, MAX_LENGTH);
	printf("msg %s\n", msg_erreur);
	change_focus_attibute(main_fen);
}
