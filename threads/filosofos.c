#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#define N 5

pthread_mutex_t tenedores[5];

void* filosofo(void* arg) {
    int* i = (int*)arg;
    int izq = *i;
    int der = (izq + 1) % N;

    while (1) {
        printf("El filosofo %d tiene hambre\n", *i);
        if (*i % 2 == 0) {
            pthread_mutex_lock(&tenedores[izq]);
            pthread_mutex_lock(&tenedores[der]);
            printf("El filosofo %d esta comiendo\n", *i);
            usleep(rand() % N * 1000000);
            pthread_mutex_unlock(&tenedores[izq]);
            pthread_mutex_unlock(&tenedores[der]);
            printf("El filosofo %d esta pensando\n", *i);
            usleep(rand() % N * 1000000);
        }
        else {
            pthread_mutex_lock(&tenedores[der]);
            pthread_mutex_lock(&tenedores[izq]);
            printf("El filosofo %d esta comiendo\n", *i);
            usleep(rand() % N * 1000000);
            pthread_mutex_unlock(&tenedores[der]);
            pthread_mutex_unlock(&tenedores[izq]);
            printf("El filosofo %d esta pensando\n", *i);
            usleep(rand() % N * 1000000);
        }
    }

}

int main(int argc, char** argv) {

    pthread_t filosofos[N];
    for (int i = 0; i < N; i++) {
        int* idx = (int*)malloc(sizeof(int));
        *idx = i;
        pthread_create(&filosofos[i], NULL, filosofo, (void*)idx);
    }

    for(int i = 0; i < N; i++){
        pthread_join(filosofos[i], NULL);
    }

    return EXIT_SUCCESS;

}