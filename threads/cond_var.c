#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myCondition = PTHREAD_COND_INITIALIZER;

int var = 0;
int done = 0;

void *producer(void *args) {
  pthread_mutex_lock(&mutex);
  int t;
  for (t = 0; t < 1000000; t++) {
    var++;
  }
  done = 1;
  pthread_cond_signal(&myCondition);
  pthread_mutex_unlock(&mutex);
  printf("Señal enviada\n");
}

void *consumer(void *args) {
  pthread_mutex_lock(&mutex);

  while (!done) {
    pthread_cond_wait(&myCondition, &mutex);
  }

  printf("Consumer: mutex locked\n");

  printf("Var: %d\n", var);
  pthread_mutex_unlock(&mutex);
}

int main(int argc, char **argv) {
  pthread_t thread_h1;
  pthread_t thread_h2;

  pthread_create(&thread_h1, NULL, producer, NULL);
  pthread_create(&thread_h2, NULL, consumer, NULL);

  pthread_join(thread_h1, NULL);
  pthread_join(thread_h2, NULL);

  return 0;
}