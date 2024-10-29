#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>

unsigned int sizeof_dm(int rows, int cols, size_t sizeof_element)
{
    size_t size = rows * sizeof(void *);
    size += (cols * rows * sizeof_element);
    return size;
}

void create_index(void **m, int rows, int cols, size_t sizeof_element)
{
    int i;
    size_t sizeRows = cols * sizeof_element;
    m[0] = m + rows;
    for (i = 1; i < rows; i++)
    {
        m[i] = (m[i - 1] + sizeRows);
    }
}

void print_matrix(int **m, int f, int c)
{
    for (int i = 0; i < f; i++)
    {
        for (int j = 0; j < c; j++)
        {
            printf("%d\t", m[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    int filas = 10;
    int col = 10;

    int **A;
    size_t shm_size = sizeof_dm(filas, col, sizeof(int));
    int shmID = shmget(IPC_PRIVATE, shm_size, 0666 | IPC_CREAT);
    A = shmat(shmID, 0, 0);
    create_index((void *)A, filas, col, sizeof(int));

    size_t root = getpid();

    if (root == getpid())
    { // parent block
    }
    else
    { // childs block
    }
    return EXIT_SUCCESS;
}