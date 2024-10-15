#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<unistd.h>

int main(int argc, char** argv){
  pid_t root;
  int estado;
  root = getpid();
  printf("Padre %d\n", root);
  for(int i = 0; i < 3; i++){
    if(!fork()){
      printf("Hijo creado %d padre %d\n", getpid(), getppid());
      break;
    }
  }
      
  if(root == getpid()){
    for(int i = 0; i < 3; i++){
      printf("Finalizo hijo %d con estado %d\n", wait(&estado), estado);
    }
    printf("Finalizando padre\n");
  }else{
    printf("Finalizando hijo %d\n", getpid());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
