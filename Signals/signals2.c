#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>


void print_debug_tree(){
  char cmd[50];
  sprintf(cmd, "pstree -lp %d", getpid());
  system(cmd);
}

void __kill(pid_t c, int sig){
  usleep(400);
  kill(c, sig);
}

void signal_handler(int s){}

int main(int argc, char**argv){

  pid_t root = getpid();
  
  if(argc < 2){
    printf("Uso: signal n_hijos\n");
    return EXIT_FAILURE;
  }

  int n_hijos = atoi(argv[1]);
  int idx;
  pid_t child;

  signal(SIGUSR1, signal_handler);

  pid_t *pids = (pid_t*)malloc(sizeof(pid_t)*n_hijos);
  
  for(idx = 0; idx < n_hijos; idx++){
    pids[idx] = fork();
    if(pids[idx] == 0){
      if(idx == 0 || idx == (n_hijos - 1)){
        child = fork();
        break;
      }else{
        break;
      }
    }
  }

  if(root == getpid()){
    print_debug_tree();

    printf("Proceso [%d] \n", getpid());
    __kill(pids[n_hijos - 1], SIGUSR1);
    pause();
    printf("Proceso [%d]\n", getpid());

    for(int i = 0; i < n_hijos; i++){
      wait(NULL);
    }
  } else {
    pause();
    printf("Proceso [%d]\n", getpid());
    if(idx == (n_hijos - 1)){
      if(child > 0){
        __kill(child, SIGUSR1);
        pause();
        printf("Proceso [%d]\n", getpid());
        __kill(pids[idx-1], SIGUSR1);
      } else{
        __kill(getppid(), SIGUSR1);
      }
    } else if(idx == 0){
      if(child > 0){
        __kill(child, SIGUSR1);
        pause();
        printf("Proceso [%d]\n", getpid());
        __kill(getppid(), SIGUSR1);
      } else {
        __kill(getppid(), SIGUSR1);
      }
    } else {
      __kill(pids[idx-1], SIGUSR1);
    }
  }

  return EXIT_SUCCESS;
}
