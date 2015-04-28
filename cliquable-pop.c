#include "cliquable-pop.h"


#define CODE_CURS_XC_xterm 152
#define CODE_CURS_XC_draft_large 60

#define MARGIN 10
#define BORDER 2

#define WIDTH_LOG 350
#define HEIGHT_LOG 25

#define WIDTH_BUTTON 75
#define HEIGHT_BUTTON 20

#define WIDTH_MAIN WIDTH_LOG*2 + MARGIN * 3 + BORDER * 4 
#define HEIGHT_MAIN 300



Window main_fen;

Window log_user_fen;
Window log_pass_fen;

Window quit_button;
Window connect_button;

XFontStruct *font;

XColor color_fond;

GC gc_glob;

Cursor fleche, clic;

Window focus_fen;
bool focus_init= false;
XColor color_focus;



bool init_main_win(){
	if ((dpy = XOpenDisplay(NULL)) == NULL){
  		printf("Erro open display");
  		exit(EXIT_FAILURE);
  	}

	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey", &color_fond, &color_fond);
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "DimGrey", &color_focus, &color_focus);

  	main_fen = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
					       WIDTH_MAIN,
					       HEIGHT_MAIN,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));

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
	XSelectInput(dpy, quit_button,  ButtonPressMask | KeyPressMask | ExposureMask);

	XMapWindow(dpy, main_fen);
  	XMapSubwindows(dpy, main_fen);
	XFlush(dpy);

	return true;
}
bool init_log_win(){
	initialiser_champs();

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


	XMapWindow(dpy, log_pass_fen);		
	XMapWindow(dpy, log_user_fen);
	XMapWindow(dpy, connect_button);

	XFlush(dpy);
	return true;
}

bool init_pop_win(){
	return true;
}

bool detruire_main_win(){
	XDestroySubwindows(dpy, main_fen);
	//XFreeColors(dpy, DefaultScreen(dpy), color_fond.pixel, 1);
	XFreeGC(dpy, gc_glob);
	XFreeFont(dpy, font);
	XCloseDisplay(dpy);
	return true;
}
bool detruire_log_win(){
	XDestroyWindow(dpy, log_user_fen);
	XDestroyWindow(dpy, log_pass_fen);
	XDestroyWindow(dpy, connect_button);
	XFreeCursor(dpy, fleche);
	return true;
}
bool detruire_pop_win(){
	return true;
}


void expose(void){
	XDrawString(dpy, log_user_fen, gc_glob, MARGIN/2, MARGIN*3/2, user_text, strlen(user_text));
	XDrawString(dpy, log_pass_fen, gc_glob, MARGIN/2, MARGIN*3/2, pass_text, strlen(pass_text));
	XDrawString(dpy, quit_button, gc_glob, MARGIN/2, MARGIN*3/2, "Quit", strlen("Quit"));
	XDrawString(dpy, connect_button, gc_glob, MARGIN/2, MARGIN*3/2, "Connection", strlen("Connection"));

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

	XSetWindowBackground(dpy, focus_fen, color_focus.pixel);
	XUnmapWindow(dpy, log_pass_fen);
	XMapWindow(dpy, log_pass_fen);

	XUnmapWindow(dpy, log_user_fen);
	XMapWindow(dpy, log_user_fen);

	expose();
	XFlush(dpy);
}

void traiter_ButtonPressEvent(XButtonEvent xbp){
	printf("ButtonPress Event\n");
	focus_fen= xbp.window;
	change_focus_attibute(focus_fen);
	focus_init=true;

	if(focus_fen == quit_button){
		printf("Quit\n");
		quit_cliquable=true;
	}
	if(focus_fen == connect_button){
		printf("CONNECTION\n");
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


void initialiser_champs(){
	user_text[0]='\0';
	pass_text[0]='\0';
}