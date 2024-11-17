#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditional = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

typedef struct {
  int COMP;
  int OP;
  int time;
} data;

data *d = NULL;

void *compuerta(void *arg) {

  int *idx = (int *)arg;

  while (1) {
    pthread_mutex_lock(&mutex);
    while (d->COMP != *idx)
      pthread_cond_wait(&conditional, &mutex);

    if (d->OP == 0) {

      printf("Cerrando compuerta %d\n", *idx);
      usleep(d->time * 1000000);
      printf("Compuerta %d cerrada\n", *idx);

    } else if (d->OP == 1) {

      printf("Abriendo compuerta %d\n", *idx);
      usleep(d->time * 1000000);
      printf("Compuerta %d abierta\n", *idx);
    }

    pthread_cond_broadcast(&cond1);
    d->COMP = -1;
    pthread_mutex_unlock(&mutex);
  }
  free(arg);
}

int main(int argc, char **argv) {

  pthread_t tids[5];

  d = (data *)malloc(sizeof(data));
  d->COMP = -1;
  for (int i = 0; i < 5; i++) {
    int *id = (int *)malloc(sizeof(int));
    *id = i;
    pthread_create(&tids[i], NULL, compuerta, (void *)id);
  }

  while (1) {

    pthread_mutex_lock(&mutex);

    printf("Ingresa los datos COMP - OP - TIME: ");
    scanf("%d %d %d", &d->COMP, &d->OP, &d->time);

    pthread_cond_broadcast(&conditional);
    pthread_cond_wait(&cond1, &mutex);
    pthread_mutex_unlock(&mutex);
  
  }

  for (int i = 0; i < 5; i++) {
    pthread_join(tids[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&conditional);

  return EXIT_SUCCESS;
}