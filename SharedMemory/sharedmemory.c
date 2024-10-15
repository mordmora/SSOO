#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include<sys/shm.h>
#include <string.h>
#include<sys/wait.h>
int main(int argc, char**argv){

  key_t key = getuid();

  int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
  if(shmid == -1){
    fprintf(stderr, "shmget error\n");
    return EXIT_FAILURE;
  }
  
  char* shared_str;

  if(!fork()){
    shared_str = (char*)shmat(shmid, 0, 0);
    strcpy(shared_str, "Mensaje desde el hijo\n");
    printf("El hijo escribio : %s\n", shared_str);
    shmdt(shared_str);
  }else{
    wait(NULL);
    shared_str = (char*)shmat(shmid, 0, 0);
    printf("El padre lee: %s", shared_str);
    shmdt(shared_str);
    shmctl(key, IPC_RMID, 0);
  }

  return EXIT_SUCCESS;
}
