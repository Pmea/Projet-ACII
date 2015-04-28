#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h> 

#define XC_xterm 152 


#define WIDTH 500
#define HEIGHT 150
#define BORDER 2


Display* dpy;
Window main_fen;

Window focus_fen;

Cursor fleche;

GC gc;


void log_func(void);

int main (int argc, char* argv[]){
  	if ((dpy = XOpenDisplay(NULL)) == NULL){
  		printf("Erro open display");
  		exit(EXIT_FAILURE);
  	}

    main_fen = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
					       WIDTH,
					       HEIGHT,
					       BORDER, 
					       BlackPixel(dpy,DefaultScreen(dpy)),
					       WhitePixel(dpy,DefaultScreen(dpy)));


   XMapWindow(dpy, main_fen);
	

   log_func();

	XEvent event;
	while(1) {	



		XNextEvent(dpy, &event);
	}
		//	unsigned long mask=0;
	/*if (p->onmouseover) mask |= EnterWindowMask;
	if (p->onmouseout)  mask |= LeaveWindowMask;
	if (mask)	XSelectInput(dpy,p->fenetre,mask|ButtonPressMask)
	  j+= (width*p->colspan);
	}
    }*/
XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

void get_event(XEvent e){
	focus_fen= e.xany.window;
	

	char text[16];
	text[0]='\0';
	printf("EVENT\n");
	if(e.type == ButtonPress){

		XNextEvent(dpy, &e);
		//changer le masque de la focus_fen;
		XSelectInput(dpy, focus_fen, KeyPressMask);					//pourquoi j'ai encore l'evenement 
		while( XLookupKeysym(&e.xkey, 0) !=  XK_Return){
  			if (XLookupKeysym(&e.xkey, 0) == XK_space)  {
  				continue;
  			}
  			else{
  				if (XLookupKeysym(&e.xkey, 0) == XK_BackSpace)  {
	  				XColor color_return;
					XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey",&color_return, &color_return );
	  				XSetForeground(dpy,gc, color_return.pixel);
	  				XDrawString(dpy,focus_fen,gc, 20,20, text, strlen(text));
	  				text[strlen(text)-1]= '\0';
	  				XSetForeground(dpy,gc, BlackPixel(dpy,DefaultScreen(dpy)));

	  			}
	  			else{
		  			printf("key:%s\n", XKeysymToString(XLookupKeysym(&e.xkey,0)));
		  			sprintf(text, "%s%s", text, XKeysymToString(XLookupKeysym(&e.xkey,0)));
		  		}
	  			printf("%s\n", text);
	  			XDrawString(dpy,focus_fen,gc, 
	  				20,20, 
	  				text, strlen(text));
	  		}
			XNextEvent(dpy, &e);
		}
	}		
}

void log_func(void){
	Window user_fen;
	Window pass_fen;

	XColor color_return;
	XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), "grey",&color_return, &color_return );

	user_fen=  XCreateSimpleWindow(dpy, main_fen, 18, 18,
					       220,
					       100,
					       BORDER, 
					       WhitePixel(dpy,DefaultScreen(dpy)),
					       color_return.pixel);
	pass_fen=  XCreateSimpleWindow(dpy, main_fen, 220 +36 +BORDER, 18,
					       220,
					       100,
					       BORDER, 
					       WhitePixel(dpy,DefaultScreen(dpy)),
					       color_return.pixel);

	XFontStruct *font=NULL;
	if ((font=XLoadQueryFont(dpy,"fixed"))==NULL){
	 	fprintf(stderr," Sorry, having font problems.\n");
	    exit(-1);
	}

 	gc=XCreateGC(dpy,user_fen,0,NULL);
 	XSetFont(dpy,gc,font->fid);
 	XSetForeground(dpy,gc,BlackPixel(dpy,DefaultScreen(dpy)));
 	XSetBackground(dpy,gc,WhitePixel(dpy,DefaultScreen(dpy))); 

	XMapWindow(dpy, pass_fen);		
	XMapWindow(dpy, user_fen);
	
	  fleche= XCreateFontCursor(dpy, 152);
	  XDefineCursor(dpy, user_fen, fleche);
	  XDefineCursor(dpy, pass_fen, fleche);

	  XSelectInput(dpy, main_fen, KeyPressMask | ButtonPressMask);

	  XSelectInput(dpy, user_fen, KeyPressMask | ButtonPressMask);
	  XSelectInput(dpy, pass_fen, KeyPressMask | ButtonPressMask);
	  

	XEvent event;
	while(1) {	

		get_event(event);
		XNextEvent(dpy, &event);
	}


	// pas de free color
	XFreeCursor(dpy, fleche);
	XFreeGC(dpy, gc);
	XFreeFont(dpy, font);
	XDestroySubwindows(dpy, main_fen);
}