#include "liste_mime.h"

#include <sys/stat.h>
#include <sys/types.h>


#define SIZE_BUFF 1024


const char* regex_text= "^([A-Za-z0-9~+./\\-]+)[[:space:]]+([A-Za-z0-9~+#.]+)";


typedef struct mime_type_t*  mime_type;

struct mime_type_t{
	int len_type;
	char* type;
	int len_ext;
	char* ext;

	mime_type next;
}; 


mime_type mime_glob;

void free_struct(mime_type mt){
	free(mt->type);
	free(mt->ext);
	free(mt);
}

mime_type alloc_struct(char* type, char* ext, mime_type next_mime){
	mime_type mt= (struct mime_type_t*) malloc(sizeof(struct mime_type_t));

	mt->len_type= strlen(type);
	mt->type=strndup(type , mt->len_type);

	mt->len_ext= strlen(ext);
	mt->ext=strndup( ext , mt->len_ext);

	mt->next=next_mime;
	return mt;
}

void ajouter_mime_type(char* type, char* ext){
	if(mime_glob == NULL ){
		mime_glob=alloc_struct(type, ext, NULL);
	}
	else{
		mime_type mt= alloc_struct(type, ext, mime_glob);
		mime_glob= mt;
	}
}

void supp_entete_liste_mime_type(void){			//supp l'entete
	while(mime_glob != NULL){
		mime_type tmp= mime_glob;
		mime_glob= mime_glob->next;
		free_struct(tmp);
	}
}


char* rechercher(char* type){
	mime_type curs= mime_glob;
	while(curs != NULL){
		if(strcmp(type, curs->type) == 0)
			return curs->ext; 
		curs= curs->next;
	}
	return "";
}


bool construire_liste_mime(void){
	
	FILE* mime_types= fopen("/etc/mime.types" ,"r");

	const int n_matches= 3;
	regmatch_t m[n_matches];

	regex_t r;
	int status= regcomp(&r, regex_text, REG_EXTENDED | REG_NEWLINE);
	if(status != 0){
		printf("Error: regcomp\n");
		return EXIT_FAILURE;
	}

	if(mime_types == NULL){
		perror("ERROR: open file");
		exit(EXIT_FAILURE);
	}
	char buff[SIZE_BUFF];

	char* buff_type= (char*) malloc(sizeof(char) * SIZE_BUFF);
	char* buff_ext= (char*) malloc(sizeof(char) * SIZE_BUFF);

	while(fgets(buff, SIZE_BUFF, mime_types) > 0){
		int match= regexec(&r, buff, n_matches, m, 0);
		if(match == 0){
			memcpy(buff_type, buff + (int) m[1].rm_so, m[1].rm_eo - m[1].rm_so );
			buff_type[m[1].rm_eo - m[1].rm_so ] = '\0'; 

			memcpy(buff_ext, buff + (int) m[2].rm_so, m[2].rm_eo - m[2].rm_so );
			buff_ext[m[2].rm_eo - m[2].rm_so] = '\0';

			ajouter_mime_type(buff_type, buff_ext);
		}
	}
	
	free(buff_ext);
	free(buff_type);
	regfree(&r);
	fclose(mime_types);

	return true;
}



bool detruire_liste_mime(void){
	supp_entete_liste_mime_type();
	return true;
}