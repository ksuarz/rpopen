/*
 * rpserver.c - remote rpopen server
 * Kyle Suarez
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Runs an rpserver for use with rpopen.
 */
int
main (int argc, char **argv) {
    int port;
    int c;

    /* Parse command line arguments */
    opterr = 1;
    while (opt = getopt(argc, argv, "hp:") != -1) {
        switch (opt) {
            case 'p':
                port = (int) (strtol(optarg, NULL, 0));
                break;
            case 'h':
                printf("Usage: rpserver [-p PORT_NUM]\n");
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return EXIT_FAILURE;
        }
    }

    /* TODO */
}
