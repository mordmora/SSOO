#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_SIZE sizeof(d_handler)

typedef struct data_handler {
  int COM;
  int OP;
  int Time;
}d_handler;

int main(){
  
  pid_t root = getpid();


  int shm_id = shmget(IPC_PRIVATE, SHM_SIZE, 0666 | IPC_CREAT);
  
  if(shm_id < 0){
    printf("shmid error\n");
    exit(1);
  }

  d_handler* data = (d_handler*)shmat(shm_id, 0, 0);
  if(data == (d_handler*)-1){
    printf("Shared memory error\n");
    exit(1);
  }

  int idx;
  for(idx = 0; idx < 5; idx++){
    if(!fork())
      break;
  }

  if(root == getpid()){

    usleep(200);

    int turno;
    int controller;
      printf("Compuerta a seleccionar (entre 1 y 5): ");
      scanf("%d", &turno);
      fflush(stdout);
      printf("Operacion a realizar (0 cerrar, 1 abrir): ");
      scanf("%d", &data->OP);
      fflush(stdout);
      printf("Temporizador (numero de segundos): ");
      scanf("%d", &data->Time);
      fflush(stdout);
  
      data->COM = turno;

    for(int i = 0; i < 5; i++)
      wait(NULL);

    shmdt(data);
    shmctl(shm_id, IPC_RMID, 0);
  }else{
    int state = 0;
    while(data->COM != idx+1);
    if(state == 1 && data->OP == 1){
      printf("La compuerta %d ya esta abierta...\n", data->COM);
    }else if(state == 0 && data->OP == 0){
      printf("La compuerta %d ya esta cerrada...\n", data->COM);
    }else if(state == 1 && data->OP == 0){
      printf("La compuerta %d se cerrara en %d segundos\n", data->COM, data->Time);
      usleep(data->Time * 1000000);
      printf("Compuerta %d cerrada.\n", data->COM);
    }else if(state == 0 && data->OP == 1){
      printf("La compuerta %d se abrira en %d segundos\n", data->COM, data->Time);
      usleep(data->Time*1000000);
      printf("Compuerta %d abierta\n", data->COM);
    }
    shmdt(data);
  }









  return EXIT_SUCCESS;
}
