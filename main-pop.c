#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <math.h>


#include "liste_mime.h"
#include "basique-pop.h"
#include "cliquable-pop.h"
#include "graphique-pop.h"

int supp_space(const char* str, int start){
	int ind=start;
	
	while(str[ind]==' '){
		ind++;
		if(str[ind] == '\n'){
			return -1;
		}
	}
	return ind;
}

char option(int argc, char* argv[]){
	char choix= 0;
	int i;
	for(i=3; i<argc; i++){
		if(strncmp(argv[i], "-t", 2) == 0)
			choix+= 't';
		if(strncmp(argv[i], "-c", 2) == 0)
			choix+= 'c';
		if(strncmp(argv[i], "-g", 2) == 0)
			choix+= 'g';
	}
	return choix;
}


int main_cliquable(int argc, char* argv[]){
	init_main_win();
	init_log_win();
	XEvent event;

		//init_connexion

	printf("Trying %s...\n", argv[1]);
	if(init_connexion(argv[1], atoi(argv[2])) == false){
		printf("Error initilize connexion\n");
		exit(EXIT_FAILURE);
	}
	printf("Connected to %s\n", argv[1]);

	while(quit_cliquable== false && quit_log == false) {	
		traiter_event(event);

		if( quit_log == true){
			sprintf(user_text, "%s\n", user_text);
			sprintf(pass_text, "%s\n", pass_text);
			printf("USER %s\nPASS %s\n", user_text, pass_text);
			if( !user_handler(user_text) || !pass_handler(pass_text)){
				initialiser_champs();
				afficher_msg("Identifiant non valide");
				quit_log= false;
			}
		}
		XNextEvent(dpy, &event);
	}

	detruire_log_win();
	int nb_msg= list_handler();
	init_pop_win(nb_msg);
	while(quit_cliquable == false){

		XNextEvent(dpy, &event);
	}
	detruire_pop_win();
	detruire_main_win();

	if(close_connexion() == false){
		printf("Error close connexion\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

int main_graphique(int argc, char* argv[]){
	return EXIT_SUCCESS;
}

int main_textuel(int argc, char* argv[]){

	printf("Trying %s...\n", argv[1]);
	if(init_connexion(argv[1], atoi(argv[2])) == false){
		printf("Error initilize connexion\n");
		exit(EXIT_FAILURE);
	}

	printf("Connected to %s\n", argv[1]);

	char cmd [128];
	bool finish= false;
	int indice;

	while(finish != true){
		if(fgets(cmd, 128, stdin) == NULL){
			printf("Error read on stdin is unavailable");
			exit(EXIT_FAILURE);
		}

		switch(*cmd){
			case 'U':
				if(strncasecmp(cmd, "USER", 4) == 0){
					printf("->USER\n");
					if((indice= supp_space(cmd, 4)) == -1){
						printf("Invalide argument, please enter a new command\n");
					}
					else{	
						user_handler(cmd+indice);
					}
				}
				break;
			case 'P':
				if(strncasecmp(cmd, "PASS", 4) == 0){
					printf("->PASS\n");
					if((indice= supp_space(cmd, 4)) == -1){
						printf("Invalide argument, please enter a new command\n");
					}
					else{	
						//traiter commande
						pass_handler(cmd+indice);
					}
				}
				break;
			case 'L':
				if(strncasecmp(cmd, "LIST", 4) == 0){
					printf("->LIST\n");
					list_handler();		
				}				
				break;
			case 'T':
				if(strncasecmp(cmd, "TOP", 3) == 0){
					printf("->TOP\n");
					//top_handler();
					if((indice= supp_space(cmd, 3)) == -1){
						printf("Invalide argument, please enter a new command\n");
					}
					else{	
						//traiter commande
						int msg_id=atoi(cmd+indice);
						indice= supp_space(cmd, 3 + indice + (int) log10(msg_id));
						int nb_ligne=atoi(cmd+indice);
					
						top_handler(msg_id, nb_ligne);
						
					}
				}
				break;
			case 'R':
				if(strncasecmp(cmd, "RETR", 4) == 0){
					printf("->RETR\n");
					if((indice= supp_space(cmd, 4)) == -1){
						printf("Invalide argument, please enter a new command\n");
					}
					else{	
						//traiter commande
						int msg_id;
						msg_id= atoi(cmd+indice); 
						retr_handler(msg_id);
					}
				}
				break;
			case 'Q':
				if(strncasecmp(cmd, "QUIT", 4)== 0)
					finish= true;
				break;
			default:
				printf("Command unknow please enter a new one\n");
				break;
		}

	}

	if(close_connexion() == false){
		printf("Error close connexion\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
} 

int main (int argc, char* argv[]){
	if(argc != 4){
		printf("Usage: il faut une adresse de serveur un port et une option -t -c -g\n");
		return EXIT_FAILURE;
	}

	if(construire_liste_mime() == false){
		printf("Error creation of mime_type liste\n");
		exit(EXIT_FAILURE);
	}

	switch(option(argc, argv)){
		case 't':
			main_textuel(argc, argv);
			break;
		case 'c':
			main_cliquable(argc, argv);
			break;
		case 'g':
			main_graphique(argc, argv);
			break;
		default:
			printf("Erreur dans les options\n");
	}


	if(detruire_liste_mime() == false){
		printf("Error liste_mime destruction \n");
		exit(EXIT_FAILURE);
	}
	
	return EXIT_SUCCESS; 
} 


