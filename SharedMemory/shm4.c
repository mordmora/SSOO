#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<unistd.h>

int main(int argc, char**argv){

  pid_t root = getpid();

  int n_childs = 4;

  if(argc < 2){
    printf("File not especified\n");
    exit(1);
  }

  FILE * f = fopen(argv[1], "r");
  if(f == NULL){
    printf("File error\n");
    exit(1);
  }
  int size;
  fscanf(f, "%d", &size);
 

  int shm_size = sizeof(int);


  int* result;

  int idx = 0;
  int shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | S_IREAD | S_IWRITE);

  int delta = size/n_childs;
  int res = size%n_childs;
  int start = 0;
  int end;

  int *vec = (int*)malloc(sizeof(int)*size);

  for(int i = 0; i < size; i++)
    fscanf(f, "%d", &vec[i]);


  for(idx = 0; idx < n_childs; idx++){
    start = idx*delta;
    end = start + delta;
    if(!fork())
      break;
  }

  if(root == getpid()){
    result = (int*)shmat(shm_id, 0, 0);
    for(int i = 0; i < n_childs; i++)
      wait(NULL);

    printf("Result: %d\n", *result);

    shmdt(result);
    shmctl(shm_id,IPC_RMID,0);
  }else{

    result = (int*)shmat(shm_id, 0, 0);

    if(idx == n_childs - 1)
      end = end+res;

    for(int i = start; i < end; i++){
      *result += vec[i];
      printf("[%d] result -> %d | child -> %d\n", getpid(), *result, idx+1);
    }
    shmdt(result);
  }
  free(vec);
  return EXIT_SUCCESS;
}

