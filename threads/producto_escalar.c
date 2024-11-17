#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *vec_a;
int *vec_b;

int result;
int delta;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int inicio;
    int fin;
} segment;

void *compute_vec(void *arg) {
    segment* limits = (segment*)arg;
    printf("Ini: %d, fin: %d\n", limits->inicio, limits->fin);

    pthread_mutex_lock(&mutex);
    for(int i = limits->inicio; i < limits->fin; i++){
        result += vec_a[i]*vec_b[i];
    }
    pthread_mutex_unlock(&mutex);

    free(arg);
}

int main(int argc, char **argv) {

  FILE *f = fopen(argv[1], "r");
  int size;
  fscanf(f, "%d", &size);

  vec_a = (int *)malloc(sizeof(int) * size);
  vec_b = (int *)malloc(sizeof(int) * size);

  for (int i = 0; i < size; i++){
    fscanf(f, "%d", &vec_a[i]);
  }

  for (int i = 0; i < size; i++){
    fscanf(f, "%d", &vec_b[i]);
  }
  fclose(f);

  int n_thr = atoi(argv[2]);
  delta = size/(n_thr+1);
  pthread_t *tids = malloc(sizeof(pthread_t)*n_thr);
  
  int ini, fin;
  ini = 0;
  fin = delta;

  for(int i = 0; i < n_thr; i++){
    segment* seg = malloc(sizeof(segment));
    seg->inicio = fin + i*delta;
    seg->fin = i == n_thr - 1 ? size : seg->inicio + delta;
    pthread_create(&tids[i], NULL, compute_vec, (void*)seg);
  }

  pthread_mutex_lock(&mutex);
    for(int i = 0; i < size; i++){
        result+=vec_a[i]*vec_b[i];
    }
  pthread_mutex_unlock(&mutex);

  for(int i = 0; i < n_thr; i++){
    pthread_join(tids[i], NULL);
  }


  printf("Producto escalar de los vectores: %d\n", result);

  return EXIT_SUCCESS;
}