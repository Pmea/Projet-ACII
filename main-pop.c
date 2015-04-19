#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "textuel-pop.h"


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

int main (int argc, char* argv[]){
	if(argc != 4){
		printf("Usage: il faut une adresse de serveur un port et une option -t -c -g\n");
		return EXIT_FAILURE;
	}


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
						printf("Invalide argument, please enter a new commande\n");
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
						printf("Invalide argument, please enter a new commande\n");
					}
					else{	
						//traiter commande
						printf("%s\n", cmd+indice);
						//pass_handler(cmd+indice);
					}
				}
				break;
			case 'L':
				if(strncasecmp(cmd, "LIST", 4) == 0){
					printf("->LIST\n");
					//list_handler();
				}
				break;
			case 'T':
				if(strncasecmp(cmd, "TOP", 3) == 0){
					printf("->TOP\n");
					//top_handler();
				}
				break;
			case 'R':
				if(strncasecmp(cmd, "RETR", 4) == 0){
					printf("->RETR\n");
					//retr_handler();
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