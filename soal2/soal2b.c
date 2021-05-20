#include <stdio.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <unistd.h>

unsigned long long angka;
int matrikD[4][6];
int matrikE[4][6];

struct node
{
    int f, h, o;
};
typedef struct node node;

void *factorial(void *a)
{
    node *args = (node *)a;
    int a1 = args->f;
    int a2 = args->h;
    int a3 = args->o;
    long long fact = 1, fact2 = 1;
    int ma = matrikD[a1][a2];

    // printf("%d %d %d\n", a1, a2, a3);

    if (a3 == 0 || ma == 0)
    {
        matrikE[a1][a2] = 0;
        // printf("pertama\t%d\n",matrikE[a1][a2]);
    }
    else if (ma > a3)
    {
        for (int i = 1; i <= a3; ++i)
        {
            fact *= i;
        }
        matrikE[a1][a2] = fact;
        // printf("kedua\t%d %d %d\n", ma, a3, matrikE[a1][a2]);
    }
    else
    {
        int m = (a3 - ma);
        for (int i = 1; i <= a3; ++i)
        {
            fact *= i;
        }
        for (int i = 1; i <= m; ++i)
        {
            fact2 *= i;
        }
        matrikE[a1][a2] = fact / fact2;
        // printf("ketiga\t%d %d %d\n", ma, a3, matrikE[a1][a2]);
    }
}

void main()
{
    pthread_t thread[24];
    key_t key = 1234;
    int(*matriksShare)[10];
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    matriksShare = shmat(shmid, 0, 0);

    node mynode;

    printf("Hasil Perkalian antara matriks A dan B adalah: \n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            printf("%d\t", matriksShare[i][j]);
        }
        printf("\n");
    }

    printf("Enter elements of third matrix\n");

    for (int c = 0; c < 4; c++)
    {
        for (int d = 0; d < 6; d++)
        {
            scanf("%d", &matrikD[c][d]);
        }
    }
    for (int c = 0; c < 4; c++)
    {
        for (int d = 0; d < 6; d++)
        {
            mynode.f = c;
            mynode.h = d;
            mynode.o = matriksShare[c][d];
            // printf("%d %d\n", matrikD[c][d], mynode.o);
            pthread_create(&(thread[c * 6 + d]), NULL, &factorial, (void *)&mynode);
            pthread_join(thread[c * 6 + d], NULL);
        }
    }
    printf("\nHasil perhitungan dengan matrix baru :\n");
    for (int c = 0; c < 4; c++)
    {
        for (int d = 0; d < 6; d++)
        {
            printf("%d\t", matrikE[c][d]);
        }
        printf("\n");
    }
    shmdt(matriksShare);
    shmctl(shmid, IPC_RMID, NULL);
}