#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define EXIT_OK 0

static char buffer[128];

int num_list[10] = {0};

void findDigits(FILE *f){

	fgets(buffer, sizeof(buffer), f);
	
	size_t lenght = strlen(buffer);

	for(int i = 0; i < lenght; i++){
		
		if(isdigit(buffer[i]))
			num_list[buffer[i] - '0']++;
	}

	for(int i = 0; i < 9; i++){
		printf("%i ", num_list[i]);
	}

}

int main(){

	FILE* file = fopen("digits.txt", "r");

	if(file == NULL){
		printf("File not found.\n");
		exit(0);
	}

	findDigits(file);
	return EXIT_OK;
}
