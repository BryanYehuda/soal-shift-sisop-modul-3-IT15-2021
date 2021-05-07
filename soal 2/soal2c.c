#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

int main()
{
    //  Create the two pipes
    int pi[2];
    pipe(pi);
    int pi2[2];

    //  First child
    if (fork() == 0)
    {
        //  Set up the first pipe to be written to
        //  and close the unneeded pipe endpoints
        close(pi[0]);
        dup2(pi[1], 1);
        close(pi[1]);
        execlp("ps", "ps", "aux", NULL);
    }

    //  Don't open the second pipe until it's needed
    //  so extra endpoints don't need to be closed
    pipe(pi2);

    //  Second child
    if (fork() == 0)
    {
        //  Set up first pipe to be read from
        //  and close the unneeded pipe endpoints
        close(pi[1]);
        dup2(pi[0], 0);
        close(pi[0]);
        //  Set up second pipe to be written to
        //  and close the unneeded pipe endpoints
        close(pi2[0]);
        dup2(pi2[1], 1);
        close(pi2[1]);
        execlp("sort", "sort", "-nrk", "3,3", NULL);
    }

    //  Close original pipe endpoints so they aren't
    //  duplicated into the third child (where they would
    //  need to be closed).
    close(pi[1]);
    close(pi[0]);

    //  Third child
    if (fork() == 0)
    {
        //  Set up second pipe to be read from
        //  and close the unneeded pipe endpoints
        close(pi2[1]);
        dup2(pi2[0], 0);
        close(pi2[0]);
        execlp("head", "head", "-5", NULL);
    }

    //  Close parent's copy of second pipes
    close(pi2[1]);
    close(pi2[0]);

    wait(NULL);
}