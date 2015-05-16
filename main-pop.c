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

const char * reg_From= "[fF]rom:[[:space:]]+([a-zA-Z0-9~+@._-]+)";
const char * reg_Date= "[dD]ate:[[:space:]]+([a-zA-Z0-9,:+[:space:]]+)";

void preparer_pour_affichage(int nb_du_msg, char* top){

	regex_t r_from;
	int status= regcomp(&r_from, reg_From, REG_EXTENDED );
	if(status != 0){
		printf("Error: regcomp regex\n");
		exit(EXIT_FAILURE);
	}
	regex_t r_date;
	status= regcomp(&r_date, reg_Date, REG_EXTENDED);
	if(status != 0){
		printf("Error: regcomp regex\n");
		exit(EXIT_FAILURE);
	}

	char* buff_form= (char*) malloc(sizeof(char) * 1024);
	char* buff_date= (char*) malloc(sizeof(char) * 1024);;

	const int n_matches= 2;
	regmatch_t m[n_matches];
	int match= regexec(&r_from, top, n_matches, m, 0);
	if(match != REG_NOMATCH){
		buff_form=strndup(top + (int) m[1].rm_so, m[1].rm_eo - m[1].rm_so );
	}
	else{
		sprintf(buff_form, "xxxxxxxxxxx@xxxxxx.xxx");			// si le champs et manquant, on le remplace par un autre champs par defaut
	}

	match= regexec(&r_date, top, n_matches, m, 0);
	if(match != REG_NOMATCH){
		buff_date=strndup(top + (int) m[1].rm_so, m[1].rm_eo - m[1].rm_so );
	}
	else{
		sprintf(buff_date, "jour, xx mois annee hh:mm:ss");
	}

	sprintf(top, "%d From: %s Date: %s", nb_du_msg, buff_form, buff_date);
	free(buff_form);
	free(buff_date);
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
	if(init_connexion(argv[1], atoi(argv[2]), NULL) == false){
		printf("Error initilize connexion\n");
		exit(EXIT_FAILURE);
	}

	while(quit_cliquable== false && quit_log == false) {	// fenetre pour se connecter
		traiter_event(event);
		if( quit_log == true){
			sprintf(user_text, "%s\n", user_text);
			sprintf(pass_text, "%s\n", pass_text);
			if( !user_handler(user_text, NULL) || !pass_handler(pass_text, NULL)){
				initialiser_champs();							// si l'identiiant non valide ou le mot de passe
				afficher_msg("Identifiant non valide");			// on reinitialise les champs
				quit_log= false;
			}
		}
		XNextEvent(dpy, &event);
	}

	detruire_log_win();

	if(quit_log == true){					// si s'est connecté, si on a quitté la fenetre de log, si on n'a pas cliqué sur le bouton quitter general
		int nb_msg= list_handler(NULL);

		char ** top_tab= (char**) malloc(sizeof(char*) * nb_msg);
		int i;
		for(i= 1; i<=nb_msg; i++){
			top_tab[i-1]= (char*) malloc(sizeof(char) * 1024);		// on remplit les champs pour la partie graphique
			top_handler(i, 0, top_tab[i-1]);
			preparer_pour_affichage(i, top_tab[i-1]);
		}

		init_pop_win(nb_msg, top_tab);

		for(i=0; i<nb_msg; i++){
			free(top_tab[i]);
		}
		free(top_tab);

		XEvent event_mails;
		while(quit_cliquable == false){			// tant que l'on a pas cliqué sur quitter
			XNextEvent(dpy, &event_mails);
			traiter_event_mails(event_mails);
		}
		detruire_pop_win();
	}
	detruire_main_win();

	if(close_connexion() == false){
		printf("Error close connexion\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

int main_graphique(int argc, char* argv[]){
	init_main_win();
	init_log_win();
	XEvent event;

	//connexion au serveur
	if(init_connexion(argv[1], atoi(argv[2]), NULL) == false){
		printf("Error initilize connexion\n");
		exit(EXIT_FAILURE);
	}

	// la fenetre de connexion 
	while(quit_cliquable== false && quit_log == false) {	
		traiter_event(event);
		if( quit_log == true){
			sprintf(user_text, "%s\n", user_text);
			sprintf(pass_text, "%s\n", pass_text);
			if( !user_handler(user_text, NULL) || !pass_handler(pass_text, NULL)){
				initialiser_champs();
				afficher_msg("Identifiant non valide");
				quit_log= false;
			}
		}
		XNextEvent(dpy, &event);
	}

	detruire_log_win();
	
	// preparer la liste des mails
	if(quit_log == true){
		int nb_msg= list_handler(NULL);

		char ** top_tab= (char**) malloc(sizeof(char*) * nb_msg);
		int i;
		for(i= 1; i<=nb_msg; i++){
			top_tab[i-1]= (char*) malloc(sizeof(char) * 1024);
			top_handler(i, 0, top_tab[i-1]);					// recuperation des entetes des mails
			preparer_pour_affichage(i, top_tab[i-1]);
		}

		init_pop_win(nb_msg, top_tab);

		for(i=0; i<nb_msg; i++){
			free(top_tab[i]);
		}
		free(top_tab);

		init_graphique();
		XEvent event_mails;

		while(quit_cliquable == false){					// boucle principal du programme
			XNextEvent(dpy, &event_mails);
			traiter_event_mails_graphique(event_mails);
		}
		detruire_pop_win();
	}
	detruire_main_win();
	destroy_graphique();

	if(close_connexion() == false){
		printf("Error close connexion\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

int main_textuel(int argc, char* argv[]){
	char* reponse=malloc(sizeof(char) * 4096);

	printf("Trying %s...\n", argv[1]);
	// initialisation de la connexion
	if(init_connexion(argv[1], atoi(argv[2]), reponse) == false){
		printf("Error initilize connexion\n");
		exit(EXIT_FAILURE);
	}
	printf("%s\n", reponse);
	printf("Connected to %s\n", argv[1]);

	char cmd [128];
	bool finish= false;
	int indice;

	// boucle principale de l'application
	while(finish != true){
		if(fgets(cmd, 128, stdin) == NULL){
			printf("Error read on stdin is unavailable");
			exit(EXIT_FAILURE);
		}

		switch(*cmd){
			case 'U':
				if(strncasecmp(cmd, "USER", 4) == 0){
					printf("->USER\n");
					if((indice= supp_space(cmd, 4)) == -1){							// si les arguments ne sont pas valide
						printf("Invalide argument, please enter a new command\n");	// on n'envoi pas de requete au serveur
					}
					else{	
						user_handler(cmd+indice, reponse);
						printf("%s\n",reponse);
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
						pass_handler(cmd+indice, reponse);
						printf("%s\n", reponse);
					}
				}
				break;
			case 'L':
				if(strncasecmp(cmd, "LIST", 4) == 0){
					printf("->LIST\n");

					list_handler(reponse);
					printf("%s\n", reponse);	
				}				
				break;
			case 'T':
				if(strncasecmp(cmd, "TOP", 3) == 0){
					printf("->TOP\n");
					if((indice= supp_space(cmd, 3)) == -1){
						printf("Invalide argument, please enter a new command\n");
					}
					else{	
						int msg_id=atoi(cmd+indice);
						indice= supp_space(cmd, 3 + indice + (int) log10(msg_id));
						int nb_ligne=atoi(cmd+indice);
						
						top_handler(msg_id, nb_ligne, reponse);
						printf("%s\n", reponse);						
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
						int msg_id;
						msg_id= atoi(cmd+indice); 
						retr_handler(msg_id, reponse);
						printf("mail recupere\n");
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
	free(reponse);

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

	//fabrication de la liste mime pour faire la correspondance type et extension
	if(construire_liste_mime() == false){
		printf("Error creation of mime_type liste\n");
		exit(EXIT_FAILURE);
	}

	// on regarde l'argument pour savoir quoi lancer
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


