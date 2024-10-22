#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

void dbg(char* msg){
  printf("%s\n", msg);
}

void print_tree(){
  char cmd[20] = {""};
  sprintf(cmd, "pstree -cAlp %d", getpid());
  system(cmd);
}


int main(int argc, char**argv){

  int idx;
  int p_idx;
  int sub_idx;
  int level = 0;
  char msg = 0;

  int write_idx;
  int read_idx;


  pid_t root = getpid();
  int fd[11][2];
  
  pid_t c_v[3];

  for(int i=0;i<11;i++)
    pipe(fd[i]);

  for(p_idx = 0; p_idx < 3; p_idx++){
    if((c_v[p_idx] = fork()) == 0){
      level = 1;
      if(p_idx == 1){
        for(idx = 0; idx < 2; idx++){
          if(fork()){
            break;
          }else{
            level = 2;
          }
        }
        if(idx == 2){
          for(sub_idx = 0; sub_idx<2; sub_idx++)
            if(!fork()){
              level = 3;
              break;
            }
        }
      }
      break;
    } 
  }

  if(level == 0){
    for(int i = 0; i < 11; i++){
      if(i != 0){
        close(fd[i][1]);
      }else{
        write_idx = i;
      }
      if(i != 10){
        close(fd[i][0]);
      }
        read_idx = i;
    }
  }else if(level == 1){
    if(p_idx == 1){
      for(int i = 0; i < 11; i++){
        if(i != 2 && i != 9)
          close(fd[i][1]);
        else
          write_idx = i;
        if(i != 1 && i != 8)
          close(fd[i][0]);
        else
          read_idx = i;
      }
    }else{
      
    }
  } else if(level == 2){
    if(idx == 2){
      for(int i = 0; i < 11; i++){
        if(i != 3 && i != 6)
          close(fd[i][0]);
        else
          read_idx = i;
        if(i != 4 && i != 7)
          close(fd[i][1]);
        else
          write_idx = i;
      }
    }else{
      for(int i = 0; i < 11; i++){
        if(i != 3 && i != 8)
          close(fd[i][1]);
        else
          write_idx = i;
        if(i != 2 && i != 7)
          close(fd[i][0]);
        else
          read_idx = i;
      }
    }
  }else if (level == 3){
    switch(sub_idx){
      case 0: {
        for(int i = 0; i < 11; i++){
          if(i != 6)
            close(fd[i][1]);
          else
            write_idx = i;
          if(i != 5)
            close(fd[i][0]);
          else
            read_idx = i;
        }
      }
      case 1: {
        for(int i = 0; i < 11; i++){
          if(i != 4)
            close(fd[i][0]);
          else
            read_idx = i;
          if(i != 5)
            close(fd[i][1]);
          else
            write_idx = i;
        }
      }
    }
  }



  if(root == getpid()){

    msg = 'A';

    write(fd[0][1], &msg, sizeof(char));
    read(fd[10][0], &msg, sizeof(char));

    usleep(117000);
    print_tree();
    
    for(int i = 0; i < 3; i++)
      wait(NULL);
  }

  return EXIT_SUCCESS;
}
