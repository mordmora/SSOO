#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define MSG_SIZE 16

int main(int argc, char**argv){
  
  char inputBuffer[MSG_SIZE];

  char* msg1 = "Hola mundo 1\n";
  char* msg2 = "Hola mundo 2\n";
  char* msg3 = "Hola mundo 3\n";

  int pipefd[2];
  if(pipe(pipefd) < 0){
    printf("Pipe error\n");
    return EXIT_FAILURE;
  }

  write(pipefd[1], msg1, MSG_SIZE);
  write(pipefd[1], msg2, MSG_SIZE);
  write(pipefd[1], msg3, MSG_SIZE);
  for(int i = 0; i < 3; i++){
    read(pipefd[0], inputBuffer, MSG_SIZE);
    printf("pipe: %s", inputBuffer);
  }

  return EXIT_SUCCESS;
}
