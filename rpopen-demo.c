/*
    demo program that uses rpopen
*/

#include "rpopen.h"
#include "rpserver.h"
#include <stdio.h>

#define BSIZE 1024

int
main(int argc, char **argv)
{
    FILE *fp;
    char buf[BSIZE];

    if ((fp = rpopen(0, 0, "ls -lL /etc")) != 0) {
        printf("rpopen-demo: Reading from file descriptor\n");
        /* read the output from the command */

        while (fgets(buf, BSIZE, fp) != 0) {
            fputs(buf, stdout);
        }
    }
    printf("rpopen-demo: goodbye\n");
    fclose(fp);
}
