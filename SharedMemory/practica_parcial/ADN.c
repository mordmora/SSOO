#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<string.h>

void s_h(int s){}

typedef struct final_data {
  int* vec_res;
  int child_id;
}final_data;

char* read_file(const char* filename){
  FILE* f = fopen(filename, "r");
  if(f == NULL){
    fprintf(stderr, "File error\n");
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  long f_size = ftell(f);
  rewind(f);

  char* content = (char*)malloc((f_size+1)*sizeof(char));
  if(content == NULL){
    fprintf(stderr, "Memory error\n");
    exit(1);
  }

  fread(content, sizeof(char), f_size, f);
  content[f_size] = '\0';

  fclose(f);
  return content;
}

char* substr(const char* str, int start, int lenght){
  if(start < 0 || lenght <= 0 || start + lenght > strlen(str))
    return NULL;

  char* tmp = (char*)malloc((lenght + 1) * sizeof(char));
  if(tmp == NULL)
    return NULL;

  strncpy(tmp, str + start, lenght);
  tmp[lenght] = '\0';
  return tmp;
}

int* get_occurrences(char* src,char* pattern ,int offset, int *__vsize){

  int ini_size = 10;
  int* vec_pos = (int*)calloc(ini_size, sizeof(int));

  int size = ini_size;
  char* tmp = src;
  int i = 0;
  while((tmp = strstr(tmp, pattern)) != NULL){
    int pos = tmp - src;

    if(i >= size){
      size*=2;
      vec_pos = (int*)realloc(vec_pos, size * sizeof(int));
      if(vec_pos == NULL){
        perror("Memory error\n");
        exit(1);
      }
    }
    vec_pos[i++] = pos + offset;

    tmp+=1;
  }
  *__vsize = i;
  printf("I val: %d\n", i);
  vec_pos = (int*)realloc(vec_pos, i*sizeof(int));

  return vec_pos;
}

int main(int argc, char**argv){

  if(argc < 2){
    fprintf(stderr, "Ingresa el numero de hijos\n");
    exit(1);
  }

  int n_hijos = atoi(argv[1]);
  pid_t root = getpid();
  int i;
  char* buff = NULL;
  FILE* f;

  if((f = fopen(argv[2], "r")) == NULL){
    fprintf(stderr, "File error\n");
    exit(1);
  }

  pid_t vec[n_hijos];

  int size;
  fscanf(f, "%d", &size);
  printf("%d\n", size);
  fclose(f);
  int shm_id = shmget(IPC_PRIVATE, size*sizeof(char), 0666 | IPC_CREAT );
  char *data = (char*)shmat(shm_id, 0, 0);

  signal(SIGUSR1, s_h);

  int start = 0;
  int delta = size/n_hijos;
  int rest = size%n_hijos;
  int end;

  int fd[n_hijos][2];

  for(int j; j < n_hijos; j++)
    pipe(fd[j]);

  for(i = 0; i < n_hijos; i++){
    start = delta*i;
    end = (i == n_hijos - 1) ? size : start + delta;
    if(!(vec[i] = fork()))
      break;
  }

  if(root == getpid()){

    final_data *final_data;

    for(int j = 0; j < n_hijos; j++)
      close(fd[i][1]);

    strncpy(data, read_file(argv[2]), size); 

    for(int j = 0; j < n_hijos; j++)
      kill(vec[j], SIGUSR1);

    printf("Childs unlocked\n");

    for(int j = 0; j < n_hijos; j++)
      wait(NULL);

    shmdt(data);
    shmctl(shm_id, IPC_RMID, 0);

  }else{
    pause();
    if(i > 0){
      int __size;
      char* subs = substr(data, start, end-start);
      int* vec_pos = process_input(subs, start, &__size);
      for(int j = 0; j < __size; i++){
        printf("Ocurrency on %d\n", vec_pos[j]);
      }
      shmdt(data);
    }

  }

  return EXIT_SUCCESS;
}

