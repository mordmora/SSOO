#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int* vec = NULL;
int vec_size = 0;
int total_size = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int s;
    int* arr;
}data;

int* readfile(char* fname) {
    FILE* f = fopen(fname, "r");
    if (!f) {
        printf("File error\n");
        exit(1);
    }

    fscanf(f, "%d", &vec_size);
    int* ivec = (int*)malloc(sizeof(int) * vec_size);
    for (int i = 0; i < vec_size; i++) {
        fscanf(f, "%d", &ivec[i]);
    }
    fclose(f);
    return ivec;
}

int max(int* v, int s) {
    int max = v[0];
    for (int i = 0; i < s; i++) {
        if (max < v[i])
            max = v[i];
    }
    return max;
}

int min(int* v, int s) {
    int min = v[0];
    for (int i = 0; i < s; i++) {
        if (min > v[i])
            min = v[i];
    }
    return min;
}

int compare(const void* a, const void* b){
    return (*(int*)a - *(int*)b);
}

void* bucket_sort(void* args) {
    int* lim = (int*)args;
    int* bucket = (int*)malloc(sizeof(int)*vec_size);
    int s = 0;

    for(int i = 0; i < vec_size; i++){
        if(vec[i] >= lim[0] && vec[i] < lim[1]){
            bucket[s] = vec[i];
            s++;
        }
    }

    qsort(bucket, s, sizeof(int), compare);
    data* d = (data*)malloc(sizeof(data));
    d->s = s;

    d->arr = bucket;
    return d;
}

int main(int argc, char** argv) {

    if (argc < 1) {
        printf("Arg error\n");
        exit(1);
    }
    int interval = atoi(argv[2]);
    vec = readfile(argv[1]);

    int _max = max(vec, vec_size);
    int _min = min(vec, vec_size);
    int n_hilos = (_max - _min) / interval;

    pthread_t tids[n_hilos];
    for (int i = 0; i < n_hilos; i++) {
        int* limits = (int*)malloc(sizeof(int) * 2);
        limits[0] = i*interval;
        limits[1] = i == (n_hilos - 1) ? _max+1 : limits[0] + interval;
        pthread_create(&tids[i], NULL, bucket_sort, (void*)limits);
    }

    data* result;

    for(int i = 0; i < n_hilos; i++){
        pthread_join(tids[i], (void**)&result);
        for(int j = 0; j < result->s; j++){
            printf("%d\n", result->arr[j]);
        }
    }
    
    return EXIT_SUCCESS;
}