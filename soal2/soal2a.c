#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_THREAD 4

int matrikA[4][3];
int matrikB[3][6];
int matrikC[4][6];
int iter = 0;

void *perkalianMatriks(void *arg)
{

    for (int i = iter; i < (iter + 1); i++)
    {
        for (int j = 0; j < 6; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                matrikC[i][j] += matrikA[i][k] * matrikB[k][j];
            }
        }
    }
    iter++;
}

int main(void)
{
    key_t key = 1234;
    int(*matriksShare)[10];
    int shmid = shmget(key, sizeof *matriksShare[24], IPC_CREAT | 0666);
    matriksShare = shmat(shmid, NULL, 0);
    pthread_t thread[MAX_THREAD];

    printf("Enter elements of first matrix\n");

    for (int c = 0; c < 4; c++)
        for (int d = 0; d < 3; d++)
            scanf("%d", &matrikA[c][d]);

    printf("Enter elements of second matrix\n");

    for (int c = 0; c < 3; c++)
        for (int d = 0; d < 6; d++)
            scanf("%d", &matrikB[c][d]);

    for (int i = 0; i < MAX_THREAD; i++)
    {
        int *p;
        pthread_create(&thread[i], NULL, perkalianMatriks, (void *)(p));
    }

    for (int i = 0; i < MAX_THREAD; i++)
        pthread_join(thread[i], NULL);

    printf("\nHasil :\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            matriksShare[i][j] = matrikC[i][j];
            printf("%d\t", matriksShare[i][j]);
        }
        printf("\n");
    }
    // sleep(30);
    // printf("selesai");

    shmdt(matriksShare);
    // shmctl(shmid, IPC_RMID, NULL);
}