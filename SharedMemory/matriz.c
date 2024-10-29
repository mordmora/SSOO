#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>

typedef struct data_handler
{
    int x, y;
    double val;

} data_handler;

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

int product_matrix(int **mA, int **mB, int f, int c, int n)
{
    int res = 0;
    for (int k = 0; k < n; k++)
    {
        res += mA[f][k] * mB[k][c];
    }
    return res;
}

int main(int argc, char **argv)
{
    int fila, col, n;
    printf("Ingresa la dimension de la matriz: ");
    scanf("%d", &n);
    fila = n;
    col = n;

    int **A;
    size_t sizeA = sizeof_dm(fila, col, sizeof(int));
    int shm_idA = shmget(IPC_PRIVATE, sizeA, 0666 | IPC_CREAT);
    A = shmat(shm_idA, 0, 0);
    create_index((void *)A, fila, col, sizeof(int));

    int **B;
    size_t sizeB = sizeof_dm(fila, col, sizeof(int));
    int shm_idB = shmget(IPC_PRIVATE, sizeB, 0666 | IPC_CREAT);
    B = shmat(shm_idB, 0, 0);
    create_index((void *)B, fila, col, sizeof(int));

    int **C;
    size_t sizeC = sizeof_dm(fila, col, sizeof(int));
    int shm_idC = shmget(IPC_PRIVATE, sizeC, 0666 | IPC_CREAT);
    C = shmat(shm_idC, 0, 0);
    create_index((void *)C, fila, col, sizeof(int));

    int a = 0;
    int b = n * n;

    int fd[2];
    pipe(fd);

    for (int i = 0; i < fila; i++)
    {
        for (int j = 0; j < col; j++)
        {
            A[i][j] = a++;
            B[i][j] = b--;
        }
    }

    size_t root = getpid();
    data_handler *dh = (data_handler *)malloc(sizeof(data_handler));

    int i; // child index
    for (i = 0; i < 3; i++)
    {
        if (!fork())
            break;
    }

    if (root == getpid())
    {
        close(fd[1]);

        while (read(fd[0], dh, sizeof(data_handler)))
        {
            C[dh->x][dh->y] = dh->val;
        }
        for (int k = 0; k < 3; k++)
            wait(NULL);

        print_matrix(C, fila, col);
        shmdt(A);
        shmdt(B);
        shmdt(C);
        shmctl(shm_idA, IPC_RMID, 0);
        shmctl(shm_idB, IPC_RMID, 0);
        shmctl(shm_idC, IPC_RMID, 0);
    }
    else
    {
        close(fd[0]);
        switch (i)
        {
        case 0:
        {
            for (int k = 1; k < n; k++)
            {
                for (int h = 0; h < k; h++)
                {
                    dh->val = product_matrix(A, B, k, h, n);
                    dh->x = k;
                    dh->y = h;
                    write(fd[1], dh, sizeof(data_handler));
                }
            }

        }
        break;
        case 1:
        {
            for (int k = 0; k < n; k++)
            {
                dh->val = product_matrix(A, B, k, k, n);
                dh->x = k;
                dh->y = k;
                write(fd[1], dh, sizeof(data_handler));
            }
        }
        break;
        case 2:
        {
            for (int k = 0; k < n - 1; k++)
            {
                for (int h = k + 1; h < n; h++)
                {
                    dh->val = product_matrix(A, B, k, h, n);
                    dh->x = k;
                    dh->y = h;
                    write(fd[1], dh, sizeof(data_handler));
                }
            }

        }
        break;
        default:
            break;
        }
        shmdt(A);
        shmdt(B);
        shmdt(C);
        close(fd[1]);
    }
    return EXIT_SUCCESS;
}