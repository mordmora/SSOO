#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <time.h>

int* vec = NULL;
int t;
int size;

pthread_barrier_t my_barrier;

double compute(int i, int size) {
    double result = 0;
    if (i - 1 < 0) {
        result += 2 * vec[i] + vec[i + 1];
    }
    if (i + 1 >= size) {
        result += vec[i - 1] + 2 * vec[i];
    }
    if (i > 0 && i < size) {
        result += vec[i - 1] + 2 * vec[i] + vec[i + 1];
    }
    return result / 4;
}

void* diferencia_finita(void* arg) {
    int* index = (int*)arg;
    int iter = 0;
    while(iter < t){

        vec[*index] = compute(*index, size);

        pthread_barrier_wait(&my_barrier);
        iter++;
    }
    return NULL;
}

int* read_file(char* fname) {

    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        fprintf(stderr, "File error");
        exit(1);
    }

    int s = 0;

    fscanf(f, "%d", &s);
    fscanf(f, "%d", &t);

    int* __vec = (int*)malloc(sizeof(int) * s);

    for (int i = 0; i < s; i++) {
        fscanf(f, "%d", &__vec[i]);
    }

    size = s;
    return __vec;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Arg error\n");
        exit(1);
    }

    vec = read_file(argv[1]);

    int n_hilos = atoi(argv[2]);

    pthread_t tids[n_hilos];

    pthread_barrier_init(&my_barrier, NULL, n_hilos);

    for(int i = 0; i < n_hilos; i++){
        int* idx = (int*)malloc(sizeof(int));
        *idx = i;
        pthread_create(&tids[i], NULL, diferencia_finita, (void*)idx);
    }

    for(int i = 0; i < n_hilos; i++){
        pthread_join(tids[i], NULL);
    }

    printf("Vector en el paso %d:\n", t);
    for(int i = 0; i < size; i++){
        printf("%d ", vec[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}
