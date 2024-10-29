#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<math.h>


unsigned int sizeof_dm(int rows, int cols, size_t sizeof_e){
  size_t size = rows * sizeof(void*);
  size += (cols*sizeof_e*rows);
  return size;
}

void create_index(void **m, int rows, int cols, size_t sizeof_e){
  int i;
  size_t sizeRows = cols * sizeof_e;
  m[0] = m + rows;
  for(i = 1; i < rows; i++){
    m[i] = (m[i-1] + sizeRows);
  }
}

void print_matriz(int**m, int filas, int columnas){
  for(int i = 0; i < filas; i++){
    for(int j = 0; j < columnas; j++){
      printf("%d\t", m[i][j]);
    }
    printf("\n");
  }
}

float blur_window(int**M, int f, int c, int i, int j, int factor){
  int iniFila = i - factor;
  int finFila = i + factor;
  int iniCol = j - factor;
  int finCol = j + factor;
  float res = 0;

  for(int k = iniFila; k <= finFila; k++){
    for(int h = iniCol; h <= finCol; h++){
      if((k >= 0 && k < f) && (h >= 0 && h < c)){
        res+=M[k][h];
      }
    }
  }

  return res/pow((factor+factor+1), 2);
}

int main(int argc, char**argv){

  if(argc < 2){
    fprintf(stderr, "Indica el numero de hijos\n");
    return EXIT_FAILURE;
  }

  FILE* f = fopen(argv[2], "r");
  if(f == NULL){
    fprintf(stderr, "File error\n");
    exit(1);
  }

  int nprocess = atoi(argv[1]);

  int filas;
  int col;

  fscanf(f, "%d %d", &filas, &col);
  pid_t root = getpid();

  int** M = NULL;
  float** R = NULL;
  size_t m_size = sizeof_dm(filas, col, sizeof(int));
  int shmM_id = shmget(IPC_PRIVATE, m_size, 0666|IPC_CREAT);
  M = shmat(shmM_id, 0, 0);
  create_index((void*)M, filas, col, sizeof(int));

  size_t r_size = sizeof_dm(filas, col, sizeof(float));
  int shmR_id = shmget(IPC_PRIVATE, r_size, 0666|IPC_CREAT);
  R = shmat(shmR_id, 0, 0);
  create_index((void*)R, filas, col, sizeof(float));


  for(int j = 0; j < filas; j++){
    for(int k = 0; k < col; k++){
      printf("%d ", k);
      fscanf(f, "%d", &M[j][k]);
    }
  }

  printf(".\n");

  int i;
  int offset = filas/nprocess;
  int start;
  int end;
  for(i = 0; i < nprocess; i++){
    start = i*offset;
    end = start + offset;
    if(!fork()) break;
  }
    

  if(root == getpid()){

    FILE* o = fopen("output2.txt", "w");
    if(o == NULL){
      perror("File error\n");
      return EXIT_FAILURE;
    }

    fprintf(o, "%d\n%d\n", filas, col);

    for(int j = 0; j < nprocess; j++)
      wait(NULL);

    for(int i = 0; i < filas; i++){
      for(int j = 0; j < col; j++){
        fprintf(o, "%.0f ", R[i][j]);
      }
      fprintf(o, "\n");
    }



  }else{

    for(int j = start; j < end; j++){
      for(int k = 0; k < col; k++){
        float r = blur_window(M, filas, col, j, k, 9);
        R[j][k] = r;
        printf("%f\t", r);
      }
      printf("\n");
    }


  }
  


  return 0;
}
