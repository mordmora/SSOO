#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char**argv){
  
  //int *vec = NULL;

  pid_t root = getpid();

  int size;
  FILE *f;
  f = fopen(argv[1], "r");
  if(f == NULL){
    printf("No se pudo abrir el archivo, asegurate que este exista...\n");
    exit(1);
  }

  FILE *o = fopen("output.txt", "w");
  if(o == NULL){
    printf("Error al crear o leer output.txt...\n");
    exit(1);
  }
  fclose(o);

  if(argv[2] == NULL){
    printf("Indica la cantidad de hijos para realizar la suma...\n");
    exit(1);
  }

  int n_childs = atoi(argv[2]);
  
  fscanf(f, "%d", &size);
  
  int *vec = (int*)malloc(size * sizeof(int));
  
  for(int i = 0; i < size; i++){
    fscanf(f, "%d", &vec[i]);
  }
// 1 2 5 3 6 8 2 3 6 2
  int radix = size/n_childs;
  int start;
  int end;
  int child;

  for(child = 0; child < n_childs; child++){
    start = child*radix;
    end = start + radix;
    if(!fork()){
      printf("Hijo %d creado, inicia en %d y termina en %d\n", getpid(), start, end);
      break;
    }
  }

  if(root == getpid()){

    for(int i = 0; i < n_childs; i++){
      wait(NULL);
    }

    FILE* sum = fopen("output.txt", "r");
    if(sum == NULL){
      printf("No se pudo abrir output.txt...\n");
      exit(1);
    }

    int line ;
    int result = 0;
    
    for(int i = 0; i < n_childs; i++){
      fscanf(sum, "%d", &line);
      result+=line;
    }

    printf("Resultado de la suma: %d\n", result);

  }else{ //bloque de hijos 
    FILE *out = fopen("output.txt", "a");
    if(out == NULL){
      printf("No se pudo abrir output.txt\n");
      exit(1);
    }

    int sum = 0;
    for(int i = start; i < end; i++){
      sum+=vec[i];
    }
  
    fprintf(out, "%d\n", sum);
    fclose(out);
  }

  
  
  
  return 0;
}
