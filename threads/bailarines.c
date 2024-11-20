
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int capacidad;
int** pasos = NULL;
int fil, col;

pthread_mutex_t *pista;
pthread_barrier_t barrier;

int** readFile(char* fname){
    FILE* f = fopen(fname, "r");
    if(!f){
        printf("File error\n");
        exit(1);
    }
    
    fscanf(f, "%d", &col);
    int** lpasos = (int**)malloc(sizeof(int*)*fil);
    for(int i = 0; i < fil; i++){
        lpasos[i] = (int*)malloc(sizeof(int)*col);
    }
    
    for(int i = 0; i < fil; i++){
        for(int j = 0; j < col; j++){
            fscanf(f, "%d", &lpasos[i][j]);
        }
    }
    fclose(f);
    
    return lpasos;
}

void* baile(void* arg){

    int *i = (int*)arg;
    int pos = *i%capacidad;

    printf("El robot %d esta esperando para realizar el paso 1\n", *i+1);
    pthread_mutex_lock(&pista[pos]);
    for(int j = 0; j < col; j++){
        printf("El robot %d realizo el paso %d\n", *i+1, j);
    }
    pthread_mutex_unlock(&pista[pos]);
    printf("El robot %d ha terminado y esta esperando a los demas\n", *i+1);
    pthread_barrier_wait(&barrier);
    return NULL;
}


int main(int argc, char**argv){

    if(argc < 3){
        printf("Arg error\n");
        return 1;
    }

    int n_hilos;
    fil = n_hilos = atoi(argv[2]);
    capacidad = atoi(argv[3]);
    pasos = readFile(argv[1]);

    pista = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*capacidad);
    pthread_barrier_init(&barrier, NULL, n_hilos);
    
    pthread_t tids[n_hilos];
    for(int i = 0; i < n_hilos; i++){
        int *idx = (int*)malloc(sizeof(int));
        *idx = i;

        pthread_create(&tids[i], NULL, baile, (void*)idx);
    }

    for(int i = 0; i < n_hilos; i++){
        pthread_join(tids[i], NULL);
    }

    printf("Todos los robots han terminado de bailar y estan comenzando a trabajar\n");

    return EXIT_SUCCESS;
}
