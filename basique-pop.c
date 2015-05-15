#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>	
#include <arpa/inet.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "liste_mime.h"


#define LINELENGTH 1024
#define MAXEXTLENGTH 16

FILE* fsock;

bool init_connexion(const char* server, int port, char* sortie){
	if(sortie != NULL){
		sortie[0]='\0';
	}
	int sock;
	struct sockaddr_in const *sin;
	char ascii_buffer[LINELENGTH];
	char sport[255];

	struct addrinfo addrHints, *addrResult;
	struct addrinfo *addrTmp;               

	int i;
	int error;
	sprintf(sport,"%i",port);       
	memset(&addrHints, 0, sizeof(addrHints));     
	addrHints.ai_family = AF_INET;                
	addrHints.ai_socktype = SOCK_STREAM;       

	error = getaddrinfo(server, sport, &addrHints, &addrResult);				
	if (error) {
		printf("Client Erreur %d getaddrinfo error for host %s %s:\n", error,server, sport);
		printf("\t%s\n",gai_strerror(error));
		exit (EXIT_FAILURE);
	}
	i=0;
	//recherche toute les IP du server
	for (addrTmp = addrResult; addrTmp; addrTmp = addrTmp->ai_next) {
		i++;
		sin = (void *)addrTmp->ai_addr;
		if (inet_ntop(AF_INET, &sin->sin_addr, ascii_buffer, sizeof(ascii_buffer)) == NULL)
			printf(" inet_ntop : Echec \n");
	}
	//connection a la premiere socket
	sock = -1;
	for (addrTmp = addrResult; addrTmp; addrTmp = addrTmp->ai_next) {
		sock = socket(addrTmp->ai_family, addrTmp->ai_socktype,addrTmp->ai_protocol);
		if (sock < 0) {
			continue;
		}
		if (connect(sock, addrTmp->ai_addr, addrTmp->ai_addrlen) < 0) {
			close(sock);
			sock = -1;
			continue;
		}
		break;
	  } 


	fsock= fdopen(sock, "r+");

	char buff_ans[128];
	//recuperation du message serveur 
	if(fgets(buff_ans, 128, fsock)== NULL){
		perror("Error Server answer");
	}

	if(sortie != NULL){
		strcat(sortie, buff_ans);
	}
	return true;
}


bool close_connexion(void){
	fclose(fsock);
	return true;
}


bool user_handler(char* arg, char* sortie){
	if(sortie != NULL)
		sortie[0]='\0';

	//envoi requete
	if(fwrite("USER ", strlen("USER "), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);
	}

	if(fwrite(arg, strlen(arg), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);
	}

	fflush(fsock);

	char buff_ans[128];
	//recuperation reponse
	if(fgets(buff_ans, 128, fsock)== NULL){
		printf("Error Server answer");
		exit(EXIT_FAILURE);
	}

	if(sortie != NULL){
		strcat(sortie, buff_ans);
	}

	if(strncmp(buff_ans, "-ERR", 4) == 0)
		return false;
	return true;
}



bool pass_handler(char* arg, char* sortie){
	if(sortie != NULL){
		sortie[0]='\0';
	}

	//envoi requete
	if(fwrite("PASS ", strlen("PASS "), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	if(fwrite(arg, strlen(arg), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	char buff_ans[128];
	//recuperation reponse
	if(fgets(buff_ans, 128, fsock)== NULL){
		printf("Error Server answer");
		exit(EXIT_FAILURE);
	}

	if(sortie != NULL){
		strcat(sortie, buff_ans);
	}

	if(strncmp(buff_ans, "-ERR", 4) == 0)
		return false;
	return true;
}

int list_handler(char * sortie){
	if(sortie != NULL)
		sortie[0]='\0';
	
	//envoi requete
	if(fwrite("LIST\r\n", strlen("LIST\r\n"), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	int compt_msg=0;
	char buff_ans[128];
	//recuperation reponse
	while(strcmp(fgets(buff_ans, 128, fsock), ".\r\n") !=0){
		if(sortie != NULL){
			strcat(sortie, buff_ans);
		}
		compt_msg++;
	}

	if(strncmp(buff_ans, "-ERR", 4) == 0)
		return -1;
	return compt_msg-1;
}



// true si pas d'erreur dans l'analyse des entete, si multipart alors bondary != NULL
bool annalyser_Entete(regex_t r, char* ext, char* boundary){
	char buff_ans[128];
	bool multipart=false;

	const int n_matches= 5;
	regmatch_t m[n_matches];
	while(strcmp(fgets(buff_ans, 128, fsock), "\r\n")!=0){
			if(strncmp(buff_ans, "-ERR", 4) == 0){
				printf("%s", buff_ans);
				return false;
			}
			int match= regexec(&r, buff_ans, n_matches, m, 0);
			if(match == 0){
				char* type=NULL;
				type=strndup(buff_ans + (int) m[2].rm_so, m[2].rm_eo - m[2].rm_so );
				if(strncasecmp(type, "multipart", strlen("multipart")) == 0){
					multipart=true;
					char * tmpbound= (char*) malloc(sizeof(char) * LINELENGTH - 4);
					tmpbound=strndup(buff_ans + (int) m[4].rm_so, m[4].rm_eo - m[4].rm_so);
					strcpy(boundary, tmpbound);
					free(tmpbound);
				}
				else{
					char* tmp= (char*) malloc(sizeof(char) * MAXEXTLENGTH);
					tmp= rechercher(type);
					strcpy(ext, tmp);
					free(type);
				}
			}
		}
	return multipart;
}

const char* regex_content_type= "^(Content-[tT]ype:)[[:space:]]+([A-Za-z0-9~+.:/-]+)(.+boundary=\"(.*)\")?";

void retr_handler(int id_msg, char * sortie){
	if(sortie != NULL)
		sortie[0]='\0';

	char buff_req[64];
	sprintf(buff_req, "RETR %d\r\n", id_msg);

	if(fwrite(buff_req, strlen(buff_req), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	regex_t r;
	int status= regcomp(&r, regex_content_type, REG_EXTENDED | REG_NEWLINE);
	if(status != 0){
		printf("Error: regcomp regex\n");
		exit(EXIT_FAILURE);
	}

	char* ext= (char*) malloc(sizeof(char) * MAXEXTLENGTH);
	strcpy(ext, "txt");

	char buff_ans[128];

	char* boundary=(char*) malloc(sizeof(char) * LINELENGTH);
	boundary[0]='-';
	boundary[1]='-';

	//recuperation reponse
	bool multipart=annalyser_Entete(r, ext, boundary+2);

	FILE* out=NULL;
	char name_file[LINELENGTH];
	char dir_name[LINELENGTH];
	dir_name[0]='\0';

	if(multipart == true){
		sprintf(dir_name, "%d", id_msg);
		mkdir(dir_name, 0744);					// pas de verification, si le dossier est absent, 
												// on le cree sinon on peut deja ajouter les fichiers 

		sprintf(dir_name,"%s/", dir_name);

		int size_tmp= strlen(boundary);
		boundary[size_tmp]= '-';				// on ajouter les caracteres au bondary pour 
		boundary[size_tmp+1]= '-';				// avoir chaine delimitant la fin du mail
		boundary[size_tmp+2]= '\r';	
		boundary[size_tmp+3]= '\n';
		boundary[size_tmp+4]= '\0';
		
		int premier_bound=0;
		while(strcmp(fgets(buff_ans, 128, fsock), boundary)!=0){		// on parcours toutes parties 

			if(strncmp(buff_ans, boundary, strlen(boundary) - 4) == 0){	
				if(out != NULL)
					fclose(out);
				char* uselessBoundary=NULL;
				multipart= annalyser_Entete(r, ext, uselessBoundary);
				if(multipart){
					printf("Error email format\n");
					exit(EXIT_FAILURE);
				}

				sprintf(name_file, "%s%d.%s", dir_name, id_msg, ext);
				out= fopen(name_file, "w+");
				if(out == NULL){
					perror("Error open file out RETR");
					exit(EXIT_FAILURE);
				}
				if(strcmp(ext,"asc") != 0 && strcmp(ext,"txt") != 0){
					premier_bound++;
				}
				premier_bound++;
			}
			else{
				if( out!= NULL) 
					fprintf(out, "%s", buff_ans);
				if( sortie != NULL && premier_bound == 1){
					strcat(sortie, buff_ans);
				}
			}
		}
		while(strcmp(fgets(buff_ans, 128, fsock), ".\r\n")!=0){}		// vide la fin du mail qui ne contient plus de fichier
	}

	else{			//si c'est un mail simple
		sprintf(name_file, "%s%d.%s", dir_name, id_msg, ext);
		out= fopen(name_file, "w+");
		if(out == NULL){
			perror("Error open file out RETR");
			exit(EXIT_FAILURE);
		}
		while(strcmp(fgets(buff_ans, 128, fsock), ".\r\n")!=0){
			fprintf(out, "%s", buff_ans);
			if(sortie != NULL){
				strcat(sortie, buff_ans);
			}
		}
		fclose(out);
	}

	free(ext);
	regfree(&r);

}


bool top_handler(int id_msg, int nb_ligne, char * sortie){
	if(sortie != NULL)
		sortie[0]='\0';

	char buff_req[64];
	sprintf(buff_req, "TOP %d %d\r\n", id_msg, nb_ligne);

	if(fwrite(buff_req, strlen(buff_req), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	char buff_ans[128];
	//recuperation reponse
	while(strcmp(fgets(buff_ans, 128, fsock), ".\r\n") != 0){
		if(sortie != NULL){
			strcat(sortie, buff_ans);
		}
		if(strncmp(buff_ans, "-ERR", 4) == 0){
			return false;
		}
	}

	return true;
}
