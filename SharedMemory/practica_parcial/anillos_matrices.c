#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>

unsigned int sizeof_dm(int rows, int cols, size_t sizeof_element){
  size_t size = rows * sizeof(void*);
  size += (cols * rows * sizeof_element);
  return size;
}

void create_index(void **m, int rows, int cols, size_t sizeof_element){
  int i; 
  size_t sizeRows = cols * sizeof_element;
  m[0] = m + rows;
  for(i = 1; i < rows; i++){
    m[i] = (m[i-1] + sizeRows);
  }
}

void print_matrix(int** m, int f, int c){
  for(int i = 0; i < f; i++){
    for(int j = 0; j < c ;j++){
      printf("%d\t", m[i][j]);
    }
    printf("\n");
  }
}

int matrix_product(int**mA, int**mB,int n ,int f, int c){
  int res = 0;
  for(int i = 0; i < n; i++){
    res += mA[f][i] + mB[i][c];
  }
  return res;
}

int main(){

  int n_process, i, n=6;

  int *turn, **A, **B, **C;

  printf("Ingresa el numero de filas y columnas");

  scanf("%d", &n);

  int t_shm, mA_shm, mB_shm, mC_shm;
  int filas = n, col = n;
  size_t size_m = sizeof_dm(filas, col, sizeof(int));

  t_shm = shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT);
  turn = shmat(t_shm, 0, 0);
  
  mA_shm = shmget(IPC_PRIVATE, size_m, 0666 | IPC_CREAT);
  A = shmat(mA_shm, 0, 0);
  create_index((void*)A, filas, col, sizeof(int));

  mB_shm = shmget(IPC_PRIVATE, size_m, 0666 | IPC_CREAT);
  B = shmat(mB_shm, 0, 0);
  create_index((void*)B, filas ,col, sizeof(int));


  mC_shm = shmget(IPC_PRIVATE, size_m, 0666 | IPC_CREAT);
  C = shmat(mC_shm, 0, 0);
  create_index((void*)C, filas, col, sizeof(int));

  n_process = (n%2==0) ? n/2 : n/2 + 1;

  pid_t root = getpid();

  for(i = 0; i < n_process; i++)
    if(!fork()) break;

  *turn = -1;

  if(root == getpid()){
   
    int st = 0, end = n*n;
    for(int f = 0; f < filas; f++){
      for(int c = 0; c < col; c++){
        A[f][c] = st++;
        B[f][c] = end--;
      }
    }
    printf("Matriz A\n");
    print_matrix(A, filas, col);
    printf("\nMatriz B\n");
    print_matrix(B, filas, col);

    *turn = 0;

    for(int h = 0; h < n_process; h++)
      wait(NULL);

    printf("Matriz resultante::\n");
    print_matrix(C, filas, col);

    shmdt(A);
    shmdt(B);
    shmdt(C);
    shmdt(turn);
    shmctl(mA_shm, IPC_RMID, 0);
    shmctl(mB_shm, IPC_RMID, 0);
    shmctl(mB_shm, IPC_RMID, 0);
    shmctl(t_shm, IPC_RMID, 0);
  }else{
    
    while(*turn != i) sleep(1);
    for(int c = i; c < n-i; c++){
      C[i][c] = matrix_product(A, B, n, i, c);
      C[n-i-1][c] = matrix_product(A, B, n, n-i-1, c);
    }
    
    for(int f = i+1; f < n-i-1; f++){
      C[f][i] = matrix_product(A, B, n, f, i);
      if(f != i)
        C[f][n-i-1] = matrix_product(A, B, n, f, n-i-1);
    }
    printf("Matriz resultante: \n");
    print_matrix(C, filas, col);

    *turn += 1;

    shmdt(A);
    shmdt(B);
    shmdt(C);
    shmdt(turn);
  }
  return EXIT_SUCCESS;
}
