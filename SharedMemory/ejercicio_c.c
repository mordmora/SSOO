#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<sys/shm.h>

int main(int argc, char**argv){

  if(argv[1] == NULL){
    fprintf(stderr, "File not specified.\n");
    return EXIT_FAILURE;
  }

  pid_t root = getpid();

  key_t pk = getuid();
  int shm_id = shmget(pk, sizeof(int)*3, 0666 | IPC_CREAT);

  int* shared_vec = (int*)shmat(shm_id, 0, 0);

  int idx;
  for(idx = 0; idx < 3; idx++){
    if(!fork())
      break;
  }

  if(getpid() == root){
  
    
    for(int i = 0; i < 3; i++)
      wait(NULL);

  }else{
    switch (idx) {
      case 0:{

      }
      case 1:{

      }
      case 2:{

      }
      default:{
        printf("Error: what\n");
        return EXIT_FAILURE;
      }
    }

  }
  

  return EXIT_SUCCESS;
}
