#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>

void printTree(){
  char cmd[20] = {""};
  sprintf(cmd, "pstree -lp %d", getpid());
  system(cmd);
}

void __print(){
  printf("Padre[%d] - hijo[%d]\n", getppid(), getpid());
}

int main(int argc, char** argv){

  int n_child;
  char msg[1024];

  if(argv[1] != NULL){
    n_child = atoi(argv[1]);
  }else{
    n_child = 4;
  }

  int n_p = n_child * 2;
  pid_t root = getpid();
  int fd[n_p][2];

  for(int i = 0; i < n_p; i++){
    if(pipe(fd[i])){
      fprintf(stderr, "Pipe error\n");
      return EXIT_FAILURE;
    }
  }
  int delta = n_p/2;
  int idx=0;

  for(idx = 0; idx < n_child; idx++){
    if(fork())
      break;
  }
  
  if(root == getpid()){

    usleep(10000);
    printTree();
    memset(msg, 0, sizeof(msg));
    printf("Ingresa un mensaje sin espacios: ");
    scanf("%s", msg);
    
    for(int i = 0; i < delta; i++){
      if(i != 0){
        close(fd[i][0]);
      }
      if(i != delta)
        close(fd[i][1]);
    }
    
    printf("MSG: %s ", msg);
    printf("Padre[%d]\n", root);
    write(fd[delta][1], &msg, sizeof(msg)); 
    read(fd[0][0], &msg, sizeof(msg));
    printf("MSG: %s ", msg);
    printf("Padre[%d]\n", root);

    for(int i = 0; i<n_child; i++){
      wait(NULL);
    }
  }else{
    if(idx == 4){
      for(int i = 0; i < delta; i++){
        if(i != idx-1){
          close(fd[i][1]);
          close(fd[delta+i][0]);
        }else{
          close(fd[i][0]);
          close(fd[delta+i][1]);
        }
      }
      
      read(fd[delta+idx-1][0], &msg, sizeof(msg));
      printf("MSG: %s ", msg);
      __print();
      
      write(fd[idx-1][1], &msg, sizeof(msg));

    }else{
      for(int i = 0; i < delta; i++){
        if(i != idx-1){
          close(fd[i][1]);
          close(fd[delta+i][0]);
        }
        if(i != idx){
          close(fd[i][0]);
          close(fd[delta+i][1]);
        }
      }

      read(fd[delta+idx-1][0], &msg, sizeof(msg));
      printf("MSG: %s ", msg);
      __print();
      write(fd[delta+idx][1], &msg, sizeof(msg));
      read(fd[idx][0], &msg, sizeof(msg));
      printf("MSG: %s ", msg);
      __print();
      write(fd[idx-1][1], &msg, sizeof(msg));
    }
  }



  return 0;
}
