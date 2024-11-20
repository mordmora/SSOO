#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

int** matriz = NULL;
int** matriz_resultante = NULL;
int f, c;

typedef struct
{
    int start;
    int end;
}limits;


int sliding_blur_window(int f, int c, int i, int j, int factor){
    int row_start = i - factor;
    int row_end = i + factor;
    int col_start = j - factor;
    int col_end = j + factor;

    int result = 0;

    for(int h = row_start; h < row_end; h++){
        for(int k = col_start; k < col_end; k++){
            if((h >= 0 && h < f) && (k >= 0 && k < c)){
                result+=matriz[h][k];
            }
        }
    }
    return result/9;
}

void* blur(void* arg){
    int* limit = (int*)arg;

    for(int i = limit[0]; i < limit[1]; i++){
        for(int j = 0; j < c; j++){
            matriz_resultante[i][j] = sliding_blur_window(f, c, i, j, 1);
        }
    }
    return NULL;
}


int** read_matrix(char* fname){
    FILE* file = fopen(fname, "r");
    if(file == NULL){
        fprintf(stderr, "File error\n");
        exit(1);
    }

    fscanf(file, "%d", &f);
    fscanf(file, "%d", &c);

    int** lmatriz = (int**)malloc(sizeof(int*)*f);
    for(int i = 0; i < f; i++){
        lmatriz[i] = (int*)malloc(sizeof(int)*c);
    }

    for(int i = 0; i < f; i++){
        for(int j = 0; j < c; j++){
            fscanf(file, "%d", &lmatriz[i][j]);
        }
    }
    return lmatriz;
}

int** inicializar_matriz_resultante(int fila, int col){
    int** tmp =(int**)malloc(sizeof(int*)*fila);
    for(int i = 0; i < fila; i++){
        tmp[i] = (int*)malloc(sizeof(int)*col);
    }

    return tmp;
}

void imprimir_matriz(int fila, int col, int**matriz){
    for(int i = 0; i < fila; i++){
        for(int j = 0; j < col; j++){
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}


int main(int argc, char**argv){

    if(argc < 3){
        fprintf(stderr, "Too few arguments\n");
        return EXIT_FAILURE;
    }

    matriz = read_matrix(argv[1]);
    matriz_resultante = inicializar_matriz_resultante(f, c);
    int n_hilos = atoi(argv[2]);
    int size = f*c;
    pthread_t tids[n_hilos];
    int delta = f/n_hilos;
    int lim[n_hilos][2]; //0 = inicio, 1 = fin
    for(int i = 0; i < n_hilos; i++){
        lim[i][0] = i*delta;
        lim[i][1] = (i == n_hilos - 1) ? f : lim[i][0] + delta;
        pthread_create(&tids[i], NULL, blur, (void*)&lim[i]);
    }

    for(int i = 0; i < n_hilos; i++){
        pthread_join(tids[i], NULL);
    }

    imprimir_matriz(f, c, matriz_resultante);

    return EXIT_SUCCESS;
}