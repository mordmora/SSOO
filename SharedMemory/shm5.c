#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>

void sig_h(int s){}

typedef struct data{
  int result;
  int* vec;
}data;

int main(int argc, char**argv){
  if(argc < 3){
    printf("Usage %s filename n_childs\n", argv[0]);
    return EXIT_FAILURE;
  }

  data* datos;

  FILE *f = fopen(argv[1], "r");
  if(f == NULL){
    printf("File error\n");
    return EXIT_FAILURE;
  }

  int size;

  fscanf(f, "%d", &size);
  
  int shm_id = shmget(IPC_PRIVATE, sizeof(data) + size*sizeof(int) , IPC_CREAT | S_IRUSR | S_IWUSR );
  datos = (data*)shmat(shm_id, 0, 0);
  datos->vec = (int*)(datos + 1);
  datos->result = 0;

  for(int i = 0; i < size; i++){
    fscanf(f, "%d", &datos->vec[i]);
  }

  pid_t root = getpid();

  int idx;
  int n_childs = atoi(argv[2]);

  signal(SIGUSR1, sig_h);

  int ini = 0;
  int fin;
  int delta = size/n_childs;
  int resto = size%n_childs;

  for(idx = 0; idx < n_childs; idx++){
    ini = idx*delta;
    fin = ini + delta;
    if(!fork()){
      break;
    }
  }

  if(root == getpid()){

    for(int i = 0; i < n_childs; i++){
      wait(NULL);
    }

    printf("Result: %d\n", datos->result);

    shmdt(datos);
    shmctl(shm_id, IPC_RMID, 0);
  }else{
    int res = 0;

    if(idx == n_childs - 1)
      fin+=resto;

    for(int i = ini; i < fin; i++ ){
      res += datos->vec[i];
    }
    datos->result += res;
    shmdt(datos);
  }

  return EXIT_SUCCESS;
}
