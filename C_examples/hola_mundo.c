#include<stdio.h>
#include<stdlib.h>
#include<string.h>


static char buffer[1024];

char* readline(char* prompt){
	
	fputs(prompt, stdout);
	fgets(buffer, sizeof(buffer), stdin );
	
	int buff_lenght = strlen(buffer);

	if(buff_lenght > 0 && buffer[buff_lenght - 1] == '\n'){
		buffer[--buff_lenght] = '\0';
	}
	
	char* aux = (char*)malloc(buff_lenght);

	if(aux == NULL){
		fprintf(stdout, "Memory error");
		return NULL;
	}

	strcpy(aux, buffer);
	return aux;

}


int main(){
	char* out = readline(">>>");
	printf("String de salida: %s\n", out);
	free(out);
	out = NULL;

	return 0;
}
