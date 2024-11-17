#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int* data = NULL;
int** buckets;
int* bucket_sizes;

int* read_file(char* fname, int* s) {
    int* vec;

    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        printf("File error\n");
        exit(1);
    }
    int size;
    fscanf(f, "%d", &size);
    vec = malloc(sizeof(int) * size);

    for (int i = 0; i < size; i++) {
        fscanf(f, "%d", &vec[i]);
    }
    fclose(f);
    *s = size;
    return vec;
}

int max(int* v, int size) {
    int val = v[0];
    for (int i = 0; i < size; i++) {
        if (val < v[i]) {
            val = v[i];
        }
    }
    return val;
}

int min(int* v, int size) {
    int val = v[0];
    for (int i = 0; i < size; i++) {
        if (val > v[i]) {
            val = v[i];
        }
    }
    return val;
}

int main(int argc, char** argv) {

    if (argc < 1) {
        printf("Especifica un archivo\n");
        exit(1);
    }
    int size = 0;
    data = read_file(argv[1], &size);

    int interval_size = atoi(argv[2]);

    int maxi = max(data, size);
    int mini = min(data, size);
    int n_h = (maxi - mini + 1) / interval_size;
    if ((maxi - mini + 1) % interval_size != 0) {
        n_h++;
    }

    int bucket_size = 0;
    buckets = (int**)malloc(sizeof(int*) * n_h);
    bucket_sizes = (int*)malloc(sizeof(int) * n_h);

    for (int i = 0; i < n_h; i++) {
        bucket_sizes[i] = 0;
    }

    for (int i = 0; i < n_h; i++) {
        buckets[i] = (int*)malloc(sizeof(int) * 50);
    }

    for (int i = 0; i < size; i++) {
        int index = (data[i] - mini) / interval_size;
        if (index >= 0 && index < n_h) {
            buckets[index][bucket_sizes[index]] = data[i];
            bucket_sizes[index]++;
            printf("INDEX: %d\n", index);
        }
    }

    return EXIT_SUCCESS;
}