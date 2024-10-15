#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>

void showtree(){
  char cmd[20] = {""};
  sprintf(cmd, "pstree -lp %d", getpid());
  system(cmd);
}

int main(void){

  pid_t root = getpid();
  int n_branch = 3;
  pid_t child;
  int num_pipes = n_branch*4;
  int pipe_idx = 0;
  int delta = num_pipes/2;

  int fd[num_pipes][2];

  char msg[1024];
  char buff[1024] = "";

  for(int i = 0; i < num_pipes; i++){
    pipe(fd[i]);
  }

  int idx;
  for(idx = 0; idx < n_branch; idx++){
    if(!fork()){
      child = fork();
      break;
    }
  }

  if(root == getpid()){ //padre
    for(int i = 0; i < delta; i++){
      if(i%2 == 0){
        close(fd[i][1]);
      }
    }
    for(int i = delta; i<num_pipes; i++){
      close(fd[i][0]);
      close(fd[i][1]);
    }

    usleep(5000);
    showtree();
    do{
      memset(buff, 0, sizeof(buff));
      printf(">>> ");
      scanf("%s", msg);
      for(int i = (delta - 1); i >= 0; i--){
        if(i%2 != 0){
          printf("[%d] -> %s\n", root, msg);
          write(fd[i][1], msg, strlen(msg));
        }else{
          read(fd[i][0], buff, sizeof(buff));
        }
      }
      printf("[%d] -> %s\n", getpid(), buff);

    }while(strcmp(msg, "fin") != 0);

    for(int i = 0; i < n_branch; i++){
      wait(NULL);
    }

  }else{ //hijos
    int k = idx*2;
    if(!child){

      for(int i = 0; i < delta;i++){
        close(fd[i][0]);
        close(fd[i][1]);
      }
      
      close(fd[k+delta+1][1]);
      close(fd[k+delta][0]);
      do{
        
        memset(buff, 0, sizeof(buff));

        read(fd[k+delta+1][0], buff, sizeof(buff));
        printf("[%d] -> %s\n", getpid(), buff);
        write(fd[k+delta][1], buff, sizeof(buff));

      }while(strcmp(buff, "fin") != 0);

    }else{
      close(fd[k][0]);
      close(fd[k+1][1]);
      close(fd[k+delta][1]);
      close(fd[k+delta+1][0]);
      
      do{

        memset(buff, 0, sizeof(buff));

        read(fd[k+1][0], buff, sizeof(buff));
        printf("[%d] -> %s\n", getpid(), buff);
        write(fd[k+delta+1][1], buff, sizeof(buff));
        read(fd[k+delta][0], buff, sizeof(buff));
        printf("[%d] -> %s\n", getpid(), buff);
        write(fd[k][1], buff, sizeof(buff));
      }while(strcmp(buff, "fin") != 0);
    }
  }

  return EXIT_SUCCESS;
}
