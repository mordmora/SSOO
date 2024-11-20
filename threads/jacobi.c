#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int row, col;
int** mx = NULL;
int t = 0;

pthread_barrier_t my_barrier;

typedef struct {

    int ini;
    int fin;

}limits;

int sliding_window(int i, int j) {

    int up = (i - 1 <= 0) ? 0 : mx[i - 1][j];
    int down = (i + 1 >= row) ? 0 : mx[i + 1][j];
    int left = (j - 1 <= 0) ? 0 : mx[i][j - 1];
    int right = (j + 1 >= col) ? 0 : mx[i][j + 1];

    return (up + down + left + right) / 4;
}

void* jacobi(void* args) {

    limits* l = (limits*)args;
    int it = 0;
    while (it < t) {

        for (int i = l->ini; i < l->fin; i++) {
            for (int j = 1; j < col - 1; j++) {
                mx[i][j] = sliding_window(i, j);
                //printf("%d ", mx[i][j]);
            }
        }
        pthread_barrier_wait(&my_barrier);
        it++;
    }

    return NULL;
}

int** read_file(char* fname) {
    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        fprintf(stderr, "File error\n");
        exit(1);
    }

    fscanf(f, "%d", &row);
    fscanf(f, "%d", &col);
    int** tmp = (int**)malloc(sizeof(int*) * row);
    for (int i = 0; i < row; i++) {
        tmp[i] = (int*)malloc(sizeof(int) * col);
    }

    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            fscanf(f, "%d", &tmp[i][j]);
        }
    }

    return tmp;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Arg error\n");
        return EXIT_FAILURE;
    }

    mx = read_file(argv[1]);
    int n_hilos = atoi(argv[2]);
    t = atoi(argv[3]);
    pthread_t tids[n_hilos];

    int new_row_size = row - 2;
    int delta = new_row_size / n_hilos;

    pthread_barrier_init(&my_barrier, NULL, n_hilos);

    limits* l;
    for (int i = 0; i < n_hilos; i++) {
        l = (limits*)malloc(sizeof(limits));
        l->ini = i * delta + 1;
        l->fin = l->ini + delta ;
        pthread_create(&tids[i], NULL, jacobi, (void*)l);
    }

    for(int i = 0; i < n_hilos; i++){
        pthread_join(tids[i], NULL);
    }

    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            printf("%d ", mx[i][j]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}