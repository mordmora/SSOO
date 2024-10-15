#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

void sig_handler(int s){}

int main(int argc, char**argv){

  int shm_size = sizeof(char)*1024;

  int shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | S_IRUSR | S_IWUSR);

  char* msg_segment = NULL;

  signal(SIGUSR1, sig_handler);

  pid_t child = fork();
  if(!child ){
    msg_segment = (char*)shmat(shm_id, 0, 0);
    pause();
    while(strcmp(msg_segment, "stop") != 0){
      strcat(msg_segment, " :p");
      kill(getppid(), SIGUSR1);
      pause();
    }
    kill(getppid(), SIGUSR1);
    shmdt(msg_segment);
  }else{
    usleep(3000);
    msg_segment = (char*)shmat(shm_id,0,0);
    do{
      printf("[!] input: ");
      scanf("%s", msg_segment);
      kill(child, SIGUSR1);
      pause();
      printf("[!] Recieved: %s\n", msg_segment);
    } while(strcmp(msg_segment, "stop") != 0);
    shmdt(msg_segment);
    shmctl(shm_id, IPC_RMID, 0);
  }

  return EXIT_SUCCESS;
}
