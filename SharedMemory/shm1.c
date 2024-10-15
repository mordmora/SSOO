//Contador concurrente

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include <sys/ipc.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/stat.h>

void signal_h(int s){}

int main(int argc, char**argv){
  
  int* ptr = NULL;
  int shmsize = sizeof(int)*10;

  int shmid = shmget(IPC_PRIVATE, shmsize, IPC_CREAT | S_IRUSR | S_IWUSR);

  signal(SIGUSR1, signal_h);

  pid_t child = fork();
  if(!child){
  
    printf("[H]Proceso hijo iniciado...\n");

    ptr = (int*)shmat(shmid,0,0);
    pause();

    printf("[H]Leyendo valores del segmento de memoria compartida...\n");

    for(int i = 0; i < 10; i++)
      printf("[H] -> %d\n", ptr[i]);

    printf("[H]Valores leidos...\n");

    shmdt(ptr);

  }else{
    printf("[P]Proceso padre iniciado...\n");
    int vec[10] = {1,2,3,4,5,6,7,8,9,10};
    ptr = (int*)shmat(shmid, 0, 0);

    usleep(3000);
    printf("[P]Escribiendo valores en el segmento de memoria compartida...\n");
    for(int i = 0; i < 10; i++){
      printf("[P] -> %d\n", vec[i]);
      ptr[i] = vec[i];
    }

    printf("[P]Valores escritos...\n");

    kill(child, SIGUSR1);
    wait(NULL);
    shmdt(ptr);
    shmctl(shmid, IPC_RMID, 0);
  }


  return EXIT_SUCCESS;
}
