#include<stdio.h>
#include<stdlib.h>
#include <sys/ipc.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>

int main(int argc, char**argv){

  int shm_size = sizeof(int);

  int shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | S_IRUSR | S_IWUSR);
  int max_it = 1000;

  int* ptr = (int*)shmat(shm_id, 0, 0);
  *ptr = 0;

  if(!fork()){

    printf("[+] Child process started...\n");

    for(int i = 0; i < max_it; i++)
      *ptr = *ptr+1;

    shmdt(ptr);
  } else {
    printf("[+] Parent process started...\n");

    for(int i = 0; i < max_it; i++)
      *ptr = *ptr + 1;

    wait(NULL);

    printf("[!] max_it -> %d\n", max_it);
    printf("[!] shm_val -> %d\n", *ptr);

    shmdt(ptr);
    shmctl(shm_id, IPC_RMID, 0);
  }

  return EXIT_SUCCESS;
}

