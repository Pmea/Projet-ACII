#include "cliquable-pop.h"


#define CODE_CURS_XC_xterm 152

#define MARGIN 10
#define BORDER 2

#define WIDTH_LOG 350
#define HEIGHT_LOG 25

#define WIDTH_MAIN WIDTH_LOG*2 + MARGIN * 3 + BORDER * 4 
#define HEIGHT_MAIN 300

bool init_main_win(){
	if ((dpy = XOpenDisplay(NULL)) == NULL){
  		printf("Erro open display");
  		exit(EXIT_FAILURE);
  	}

	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey", &color_fond, &color_fond);

  	main_fen = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
					       WIDTH_MAIN,
					       HEIGHT_MAIN,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));

	XFontStruct *font=NULL;
	if ((font=XLoadQueryFont(dpy,"fixed"))==NULL){
	 	fprintf(stderr," Sorry, having font problems.\n");
	    exit(-1);
	}

	gc_glob=XCreateGC(dpy,main_fen,0,NULL);
 	XSetFont(dpy,gc_glob,font->fid);
 	XSetForeground(dpy,gc_glob,BlackPixel(dpy,DefaultScreen(dpy)));
 	XSetBackground(dpy,gc_glob,WhitePixel(dpy,DefaultScreen(dpy))); 


  	XMapWindow(dpy, main_fen);
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

	fleche= XCreateFontCursor(dpy, CODE_CURS_XC_xterm);
	XDefineCursor(dpy, log_user_fen, fleche);
	XDefineCursor(dpy, log_pass_fen, fleche);
	
	XSelectInput(dpy, log_user_fen, KeyPressMask | ButtonPressMask);
	XSelectInput(dpy, log_pass_fen, KeyPressMask | ButtonPressMask);

	XMapWindow(dpy, log_pass_fen);		
	XMapWindow(dpy, log_user_fen);
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
	XCloseDisplay(dpy);
	return true;
}
bool detruire_log_win(){
	XDestroyWindow(dpy, log_user_fen);
	XDestroyWindow(dpy, log_pass_fen);
	XFreeCursor(dpy, fleche);
	return true;
}
bool detruire_pop_win(){
	return true;
}