/*
 * rpserver.c - remote rpopen server
 * Kyle Suarez
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * Used for listen(2) - the number of backlogged processes the socket will allow
 * before refusing new connections.
 */
#define BACKLOG 3

/**
 * Runs an rpserver for use with rpopen.
 */
int
main (int argc, char **argv) {
    int opt;
    int port;
    int sock;
    int sockoptval;
    socklen_t addr_len;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;

    opterr = 1;
    port = 0;

    /* Parse command line arguments */
    while (opt = getopt(argc, argv, "hp:") != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: rpserver [-p PORT_NUM]\n");
                return EXIT_SUCCESS;
            case 'p':
                port = (int) (strtol(optarg, NULL, 0));
                if (errno != 0) {
                    perror(NULL);
                    return EXIT_FAILURE;
                }
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return EXIT_FAILURE;
        }
    }

    /* Create a TCP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create a socket");
        return EXIT_FAILURE;
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

    /* Bind socket */
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) < 1) {
        perror("Bind failure");
        return EXIT_FAILURE;
    }

    /* Listen on the socket */
    if (listen(sock, BACKLOG) < 0) {
        perror("Failed to listen on socket");
        return EXIT_FAILURE;
    }
    
    /* TODO */
}
