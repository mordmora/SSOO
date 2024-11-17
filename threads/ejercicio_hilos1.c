#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define __VEC__SIZE__ 100

int vec[__VEC__SIZE__];
pthread_mutex_t _MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;
int turno = 0; // 0 productores, 1 consumidores
int num_it = 0;

void *producer(void *args) {
  do {

    pthread_mutex_lock(&_MUTEX);

    while (turno != 0)
      pthread_cond_wait(&cond_empty, &_MUTEX);

    for (int i = 0; i < __VEC__SIZE__; i++) {
      vec[i] += i * 2;
    }
    turno = 1;
    printf("Productor listo, esperando que los consumidores terminen\n");
    pthread_cond_broadcast(&cond_full);
    pthread_mutex_unlock(&_MUTEX);
    num_it++;
  } while (num_it < 1000);
}

void *consumer(void *args) {
  do {

    long int suma = 0;
    pthread_mutex_lock(&_MUTEX);
    while (turno != 1)
      pthread_cond_wait(&cond_full, &_MUTEX);

    for (int i = 0; i < __VEC__SIZE__; i++)
      suma += vec[i] + num_it + rand() % 5;

    turno = 0;
    pthread_cond_broadcast(&cond_empty);
    pthread_mutex_unlock(&_MUTEX);
    printf("Consumidores listos, esperando a los productores\n");
    printf("Resultado: %ld\n", suma);
  } while (num_it < 1000);
}

int main(int argc, char **argv) {

  pthread_t thread_pid1;
  pthread_t thread_pid2;
  pthread_t thread_pid3;
  pthread_t thread_pid4;

  pthread_create(&thread_pid1, NULL, producer, NULL);
  pthread_create(&thread_pid2, NULL, consumer, NULL);
  pthread_create(&thread_pid3, NULL, producer, NULL);
  pthread_create(&thread_pid4, NULL, consumer, NULL);

  pthread_join(thread_pid1, NULL);
  pthread_join(thread_pid2, NULL);
  pthread_join(thread_pid3, NULL);
  pthread_join(thread_pid4, NULL);

  return EXIT_SUCCESS;
}