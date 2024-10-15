#include <complex.h>
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void sig_handler(int s){}

void print_debug_tree(){
  char cmd[50];
   sprintf(cmd, "pstree -lp %d", getpid());
   system(cmd);
}

void _kill (pid_t c, int sig){
  usleep(100);
  kill(c, sig);
}

int main (int argc, char**argv) {
    
  if(argc < 2){
    printf("No hay argumentos\n");
  }

  signal(SIGUSR1, sig_handler);

  pid_t root = getpid();
  pid_t *c_vec;
  unsigned int n_child = atoi(argv[1]);
  int idx;
  pid_t child;

  c_vec = (pid_t*)malloc(sizeof(pid_t)*n_child);

  for(idx = 0; idx < n_child; idx++){
    c_vec[idx] = fork();

    if(c_vec[idx] == 0 && idx == 0){
      break;
    }

    if(c_vec[idx] == 0 && idx == n_child-1){
      break;
    }

    if(c_vec[idx] == 0){
      child = fork();
      break;
    }
  }
  

  if(root == getpid()){
    print_debug_tree();
    printf("Proceso [%d]\n", getpid());
    _kill(c_vec[n_child - 1], SIGUSR1);
    pause();
    printf("Proceso [%d]\n", getpid());

    for(int i = 0; i < n_child; i++)
      wait(NULL);
  } else {
    pause();
    printf("Proceso [%d]\n", getpid());
    if(idx == n_child-1 ){
      _kill(c_vec[idx-1], SIGUSR1);
    } else if(idx == 0 ){
      _kill(getppid(), SIGUSR1);
    } else {
      if(child > 0){
        _kill(child, SIGUSR1);
        pause();
        printf("Proceso [%d]\n", getpid());
        _kill(c_vec[idx-1], SIGUSR1);
      }else{
        _kill(getppid(), SIGUSR1);
      }
    }
  }

  return EXIT_SUCCESS;
}
