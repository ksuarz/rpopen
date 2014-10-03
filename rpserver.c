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
 * Default port number.
 */
#define DEFAULT_PORT 0

/**
 * Max size for input command buffer. (That is, commands cannot exceed this
 * length in bytes.)
 */
#define MAXBUFSIZE 2048

/**
 * Runs an rpserver for use with rpopen.
 */
int
main (int argc, char **argv) {
    char buffer[MAXBUFSIZE];
    int opt;
    int port;
    int request_sock;
    int server_sock;
    int sockoptval;
    pid_t pid;
    size_t socklen;
    socklen_t addr_len;
    ssize_t bytes_read;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;

    opterr = 1;
    port = DEFAULT_PORT;

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
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create a socket");
        return EXIT_FAILURE;
    }
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

    /* Bind socket */
    socklen = sizeof(struct sockaddr_in);
    memset(&server_addr, 0, socklen);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server_sock, (struct sockaddr *) &server_addr, socklen) < 1) {
        perror("Bind failure");
        return EXIT_FAILURE;
    }

    /* Listen on the socket */
    if (listen(server_sock, BACKLOG) < 0) {
        perror("Failed to listen on socket");
        return EXIT_FAILURE;
    }

    /* Accept connections */
    while (1) {
        while ((request_sock = accept(server_sock, (struct sockaddr *) &client_addr, socklen)) < 0) {
            /* Error handling */
            switch(errno) {
                case ECHILD:
                case ERESTART:
                case EINTR:
                    /* Ignore and restart */
                    break;
                default:
                    perror("Accept faulure");
                    return EXIT_FAILURE;
            }
        }

        /* Connection has been accepted */
        pid = fork();
        if (f == -1) {
            /* Error */
            perror("Fork failure");
            return EXIT_FAILURE;
        }
        else if (f == 0) {
            /* Child */
            bytes_read = read(request_sock, buffer, MAXBUFSIZE);
            if (bytes_read == -1) {
                perror("Failure on socket read");
                return EXIT_FAILURE;
            }
            /* TODO What if we need multiple reads? */

            /* Duplicate our STDOUT onto the socket */
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            dup2(request_sock, STDOUT_FILENO);

            /* Execute the command */
            execlp("/bin/sh", "/bin/sh -c", &buffer, (char *) 0);
            perror("execlp(2) failure");
            return EXIT_FAILURE;
        }
        else {
            /* Parent */
            shutdown(request_sock, SHUT_RD);
        }
    }

    return EXIT_SUCCESS;
}
