#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/ipc.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/shm.h>

void sig_handler(int s){}

void prinTree(){
  char cmd[20];
  sprintf(cmd, "pstree -lp %d", getpid());
  system(cmd);
}


void __kill__ (pid_t process_p, int sig){
  usleep(200);
  kill(process_p, sig);
}

int main(int argc, char**argv){

  if(argc < 2){
    printf("too few arguments...\n");
    return EXIT_FAILURE;
  }

  pid_t root = getpid();

  key_t key = getuid();

  int n_child = atoi(argv[1]);
  
  int shm_id = shmget(key, sizeof(int), 0666 | IPC_CREAT);

  int *ptr = (int*)shmat(shm_id, 0, 0);

  signal(SIGUSR1, sig_handler);

  pid_t child;

  for(int i = 0; i<n_child; i++){
    child = fork();
    if(child != 0)
    {
      break;
    }
  }  

  if(root == getpid()){
    usleep(5000);

    int temp;
    printf("Ingresa un numero...");
    scanf("%d", &temp);

    *ptr = temp;

    prinTree();
     
    printf("Proceso [%d]. MSG: %d\n", getpid(), *ptr);
    __kill__(child, SIGUSR1);
    pause();
    printf("Proceso [%d]. MSG: %d\n", getpid(), *ptr);

    for(int i = 0; i < n_child; i++)
      wait(NULL);

  }else{

    pause();
    if(child !=0){
      printf("Proceso [%d]. MSG: %d\n", getpid(), *ptr);
      __kill__(child, SIGUSR1);
      pause();
      printf("Proceso [%d]. MSG: %d\n", getpid(), *ptr);
      __kill__(getppid(), SIGUSR1);
    }else{
      printf("Proceso [%d]. MSG: %d\n", getpid(), *ptr);
      __kill__(getppid(), SIGUSR1);
    }
  }

  return EXIT_SUCCESS;
}

