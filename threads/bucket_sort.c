#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int** data;
int* block_size;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void read_file(char* fname, int* s, int n_t){
    FILE* f = fopen(fname, "r");
    if(f == NULL){
        printf("File error\n");
        exit(1);
    }
    int n_block = 0;
    fscanf(f, "%d", &n_block);
    printf("n_b: %d\n", n_block);
    data = (int**)malloc(sizeof(int*)*n_block);
    if(data == NULL){
        printf("Memory error\n");
        exit(1);
    }
    block_size = (int*)malloc(sizeof(int)*n_t);
    if(block_size == NULL){
        printf("Mem error\n");
        exit(1);
    }
    for(int i = 0; i < n_t; i++){
        fscanf(f, "%d", &block_size[i]);
        data[i] = (int*)malloc(sizeof(int)*block_size[i]);
        if(data[i] == NULL){
            printf("Memory error\n");
            exit(1);
        }
        for(int j = 0; j < block_size[i]; j++){
            fscanf(f, "%d", &data[i][j]);
        }
    }
    fclose(f);
}

void* bucket_sort(void* args){
    int* idx = (int*)args;
    int* fragment = data[*idx];
    int size = block_size[*idx];

    for(int i = 1; i < size; i++){
        int tmp = fragment[i];
        int j = i - 1;
        while(j >= 0 && fragment[j] > tmp){
            fragment[j+1] = fragment[j];
            j--;
        }
        fragment[j+1] = tmp;
    }

    pthread_mutex_lock(&mtx);
    printf("Hilo %d\n", *idx);
    for(int i = 0; i < size; i++){
        printf("%d ", fragment[i]);
    }
    printf("\n");
    pthread_mutex_unlock(&mtx);
}

int main(int argc, char** argv){

    if(argc < 1){
        printf("Error file not especified\n");
        return EXIT_FAILURE;
    }

    int size = 0;
    int t_size = atoi(argv[2]);
    read_file(argv[1], &size, t_size);
    pthread_t* tids = (pthread_t*)malloc(sizeof(pthread_t) * t_size);

    for(int i = 0; i < t_size; i++){
        int* index = (int*)malloc(sizeof(int));
        *index = i;
        pthread_create(&tids[i], NULL, bucket_sort, (void*)index);
    }

    for(int i = 0; i < t_size; i++){
        pthread_join(tids[i], NULL);
    }

    return EXIT_SUCCESS;
}