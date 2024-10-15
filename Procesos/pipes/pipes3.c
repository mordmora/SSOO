#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/wait.h>
#include<sys/types.h>

//Programa para enviar un mensaje desde el proceso padre a un proceso hijo
#define BUFF_SIZE 16

int main(int argc, char** argv){

  pid_t root = getpid();

  char buffer[BUFF_SIZE];

  char* msg = "Hola mundo\n";

  int pipefd[2];
  int i;

  if(pipe(pipefd) < 0){
    printf("Pipe error\n");
    exit(1);
  }

  pid_t child = fork();

  if(child < 0){
    printf("Fork error\n");
    exit(1);
  }

  if(child > 0){
    close(pipefd[0]);
    write(pipefd[1], msg, BUFF_SIZE);
    wait(NULL);
  } else {
   
    close(pipefd[1]);
    read(pipefd[0], buffer, BUFF_SIZE);
    printf("Child pipe msg recieved: %s", buffer);

  }
  


  return EXIT_SUCCESS;
}
