#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <unistd.h>

int* vec = NULL; //vector global
long long int res = 0;

typedef struct {

    int start;
    int end;
    int delta;

}segment;

void* sum(void* args){
    int* seg = (int*) args;
    int start = seg[0];
    int end = seg[1];

    printf("ID HILO: %ld\n", pthread_self());
    
    long long int resultado = 0;
    printf("%d\n", start);
    for(int i = start; i < end; i++)
        resultado += vec[i];

    res += resultado;

    pthread_exit(0);
}

int* readVector(int* size, const char* filename){
    FILE* f = fopen(filename, "r");
    if(f == NULL){
        fprintf(stderr, "File error\n");
        exit(1);
    }

    fscanf(f, "%d", size);

    int* vec = (int*)malloc(sizeof(int) * (*size));
    if(!vec){
        fprintf(stderr, "Memory error");
        exit(1);
    }

    for(int i = 0; i < *size; i++){
        fscanf(f, "%d", &vec[i]);
    }

    return vec;
}

int main(int argc, char**argv){

    int size;
    vec = readVector(&size, argv[1]);
    int n_h = atoi(argv[2]);


    int delta = size / n_h;
    int index_matrix[n_h][2];

    pthread_t hilos[n_h];
    for(int i = 0; i < n_h; i++){
        index_matrix[i][0] = i*delta;
        index_matrix[i][1] = index_matrix[i][0] + delta;
        pthread_create(&hilos[i], NULL, sum, (void*)&index_matrix[i]);
    }

    for(int i = 0; i < n_h; i++)
        pthread_join(hilos[i], NULL);

    printf("Resultado de la suma: %lld\n", res);

    return EXIT_SUCCESS;
}