#include "graphique-pop.h"
#include "cliquable-pop.h"

#define WIDTH_MAIL_WIN 600
#define HEIGHT_MAIL_WIN 800

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

// a mettre dans une structure 
Window mail_fen;
Window mail_contenu_fen;
Window quit_button;
Window slide_fond;
Window slider;
GC gc_glob;

void init_mail_win_graphique(int num_msg){

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
					       

	// test pour garder le gc 
	gc_glob=XCreateGC(dpy,mail_fen,0,NULL);
 	XSetForeground(dpy,gc_glob,BlackPixel(dpy,DefaultScreen(dpy)));
 	XSetBackground(dpy,gc_glob,color_fond.pixel); 


	XSelectInput(dpy, quit_button,  ButtonPressMask | ExposureMask);

	XMapWindow(dpy, mail_fen);
	XMapSubwindows(dpy, mail_fen);
	XMapWindow(dpy, slide_fond);
	XMapSubwindows(dpy, slide_fond);

	XFlush(dpy);
}

void destroy_mail_win_graphique(void){
	
}

void traiter_ButtonPress_sur_mail_graphique(XButtonEvent  xbe){
	printf("buttonPress event\n");
	if(xbe.window == quit_button){
		printf("Quit\n");
		quit_cliquable=true;
		return;
	}

	// if pas deja dans la liste
	init_mail_win_graphique(0);
}


void expose_graphique(Window win){
	//chercher la fenetre qui a recu le expose
	XDrawString(dpy, quit_button, gc_glob, MARGIN/2, MARGIN*3/2, "Quit", strlen("Quit"));

}

void traiter_ExposeEvent_mail_graphique(XExposeEvent xee){
	printf("Expose Event: %d\n", xee.count);

	if(xee.count == 0){
		printf("Last Expose Event\n");
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
	printf("PAS RECONNU EVENT \n");
}