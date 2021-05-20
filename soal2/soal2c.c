#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

int main()
{
    int pi[2];
    pipe(pi);
    int pi2[2];

    if (fork() == 0)
    {
        close(pi[0]);
        dup2(pi[1], 1);
        close(pi[1]);
        execlp("ps", "ps", "aux", NULL);
    }

    pipe(pi2);

    if (fork() == 0)
    {
        close(pi[1]);
        dup2(pi[0], 0);
        close(pi[0]);

        close(pi2[0]);
        dup2(pi2[1], 1);
        close(pi2[1]);
        execlp("sort", "sort", "-nrk", "3,3", NULL);
    }

    close(pi[1]);
    close(pi[0]);

    if (fork() == 0)
    {
        close(pi2[1]);
        dup2(pi2[0], 0);
        close(pi2[0]);
        execlp("head", "head", "-5", NULL);
    }

    close(pi2[1]);
    close(pi2[0]);

    wait(NULL);
}