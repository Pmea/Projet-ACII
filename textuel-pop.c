#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>		/* pour socket */
#include <netinet/in.h>		/* pour sockaddr_in et INET_ADDRSTRLEN */
#include <netdb.h>	
#include <arpa/inet.h>

#define LINELENGTH 1024

FILE* fsock;

bool init_connexion(const char* server, int port){
	int sock;
	struct sockaddr_in const *sin;
	char ascii_buffer[LINELENGTH];
	char sport[255];

	struct addrinfo addrHints, *addrResult; // adresses desirees / recuperees
	struct addrinfo *addrTmp;                //adresse temporaire

	int i;
	int error;
	sprintf(sport,"%i",port);        //conversion du no de port en chaine de char
	memset(&addrHints, 0, sizeof(addrHints));     // raz
	addrHints.ai_family = AF_INET;                //famille  TCP/IP ipv4
	addrHints.ai_socktype = SOCK_STREAM;        // avoir une socket de type stream

	error = getaddrinfo(server, sport, &addrHints, &addrResult);				// nom du server, port server, addrHint type de service utiliser, les differents resultats
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
	//recuperation reponse
	if(fgets(buff_ans, 128, fsock)== NULL){
		perror("Error Server answer");
	}

	printf("%s",buff_ans);
	return true;
}


bool close_connexion(void){
	fclose(fsock);
	return true;
}


void user_handler(char* arg){
	//envoier requet
	if(fwrite("USER ", strlen("USER "), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}
	fflush(fsock);

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

	printf("%s",buff_ans);
}



void pass_handler(char* arg){
	//envoier requet
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

	printf("%s",buff_ans);	
}

void list_handler(void){
	//envoi requet
	if(fwrite("LIST\r\n", strlen("LIST\r\n"), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	char buff_ans[128];
	//recuperation reponse
	while(strcmp(fgets(buff_ans, 128, fsock), ".\r\n") !=0){
		printf("%s",buff_ans);
	}
	printf("%s", buff_ans);

}


void retr_handler(int id_msg){
	char buff_req[64];
	sprintf(buff_req, "RETR %d\r\n", id_msg);

	if(fwrite(buff_req, strlen(buff_req), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	char buff_ans[128];
	//recuperation reponse
	while(strcmp(fgets(buff_ans, 128, fsock), ".\r\n")!=0){
		if(strncmp(buff_ans, "-ERR", 4) == 0)
			break;
		printf("%s",buff_ans);
	}
	printf("%s", buff_ans);

}


void top_handler(int id_msg, int nb_ligne){
	char buff_req[64];
	sprintf(buff_req, "TOP %d %d\r\n", id_msg, nb_ligne);

	printf("%s\n", buff_req);
	if(fwrite(buff_req, strlen(buff_req), sizeof(char), fsock) == -1){
		perror("Error write in socket");
		exit(EXIT_FAILURE);	
	}

	char buff_ans[128];
	//recuperation reponse
	bool err= false;
	while(strcmp(fgets(buff_ans, 128, fsock), "\r\n") != 0){
		printf("%s", buff_ans);
		if(strncmp(buff_ans, "-ERR", 4) == 0){
			err=true;
			break;
		}
	}

	if(err== true)
		return;

	int i;
	for(i=0; i<nb_ligne; i++){
		printf("%s", buff_ans);
		if(strcmp(fgets(buff_ans, 128, fsock), ".\r\n") != 0){			//si on est a la fin du message
			printf("%s", buff_ans);
			return;
		}
	}
	printf("%s", buff_ans);

}