#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <regex.h>


#define SIZE_BUFF 1024


const char* regex_text= "([[:print:]]+)[[:space:]]+([A-Za-z0-9~+#.]+)";

/*
mime_type mime_glob;

typedef struct mime_type_t*  mime_types;

struct mime_type_t{
	int len_type;
	char* type;
	int len_ext;
	char* ext;

	mime_type* next;
}; 


void free_struct(mime_type mt){
	free(mt->type);
	free(mt->ext);
	free(mt);
}

mime_type alloc_struct(char* type, char* ext){
	mime_type mt= (struct mime_type_t*) malloc(sizeof(struct mime_type_t));

	mt->len_type= strlen(type);
	mt->type= (char*) malloc(sizeof(char) * mt->len_type);
	strncmp(mt->type, type , mt->len_type);

	mt->len_ext= strlen(ext);
	mt->ext=(char*) malloc(sizeof(char) * mt->len_ext);
	strncmp(mt->ext, ext , mt->len_ext);
	return mt;
}

void ajouter_mime_type{
	if(mime_glob == NULL ){

	}
}
*/

int main(int argc, char *argv[]){
	
	FILE* mime_types= fopen("/etc/mime.types" ,"r");

	
	const int n_matches= 3;
	regmatch_t m[n_matches];

	regex_t r;
	int status= regcomp(&r, regex_text, REG_EXTENDED | REG_NEWLINE);
	if(status != 0){
		printf("Error: regcomp");
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
			printf("BUFF-> %s", buff);
			memcpy(buff_type, buff + (int) m[1].rm_so, m[1].rm_eo - m[1].rm_so );
			buff_type[m[1].rm_eo - m[1].rm_so ] = '\0'; 
			printf("%s\n", buff_type);

			memcpy(buff_ext, buff + (int) m[2].rm_so, m[2].rm_eo - m[2].rm_so );
			buff_ext[m[2].rm_eo - m[2].rm_so] = '\0';
			printf("%s\n",buff_ext);
	}

	}

	free(buff_ext);
	free(buff_type);
	regfree(&r);
	fclose(mime_types);

	return EXIT_SUCCESS;
}