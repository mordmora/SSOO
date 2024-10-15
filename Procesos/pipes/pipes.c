#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>


void alarma(){
  printf("Tuberia llena, proceso bloqueado\n");
  exit(0);
}

int main(int argc, char** argv){

  int pipefd[2];
  int count;
  char c = 'c';

  if(pipe(pipefd) < 0){
    printf("Pipe error\n");
    exit(1);
  }
  signal(SIGALRM, alarma);

  for(count = 0 ;;){
    alarm(20);
    write(pipefd[1], &c, 1);
    alarm(0);
    if((++count &1024) == 0) printf("%d caracteres en el pipe\n", count);

  }

  return EXIT_SUCCESS;
}
