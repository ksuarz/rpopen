/*
 * rpserver.c - remote rpopen server
 * Kyle Suarez
 */

#include "rpcommon.h"
#include "rpserver.h"
#include <sys/errno.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Executes a command read from the socket.
 *
 * @param sock A socket descriptor from an accepted connection.
 *
 * @return On success, this performs an exec(2) and does not return. If an error
 * occurs, errno is set.
 */
void
rpserver_exec_command (int sock) {
    ssize_t bytes_read;
    char cmd_buffer[MAXBUFSIZE];
    ssize_t buflen;

    /* Zero-out the buffer */
    buflen = 0;
    memset(&cmd_buffer, 0, MAXBUFSIZE);

    /* Read the command from the client */
    /* TODO refactor */
    for (buflen = 0, bytes_read = 1; buflen < MAXBUFSIZE && bytes_read > 0; buflen += bytes_read) {
        bytes_read = read(sock, &cmd_buffer + buflen, MAXBUFSIZE - buflen);
        printf("rpserver: Read %zd bytes from client\n", bytes_read);
    }
    if (bytes_read == -1) {
        perror("Failure on read(2) on socket");
        return;
    }

    printf("rpserver: Executing command: %s\n", cmd_buffer);

    /* Duplicate our STDOUT onto the socket */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    dup2(sock, STDOUT_FILENO);

    /* Execute the command */
    execlp("/bin/sh", "/bin/sh", "-c", cmd_buffer, (char *) 0);
    perror("execlp(2) failure");
}

/**
 * Runs an rpserver for use with rpopen.
 */
int
main (int argc, char **argv) {
    int *svr_sock;
    int cli_sock;
    int opt;
    int port;
    int sockoptval;
    pid_t pid;
    socklen_t addr_len;
    struct sockaddr_in cli_addr;

    addr_len = sizeof(cli_addr);
    opterr = 1;
    port = DEFAULT_PORT;
    sockoptval = 1;

    /* Parse command line arguments */
    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: rpserver [-p PORT_NUM]\n");
                return EXIT_SUCCESS;
            case 'p':
                port = (int) (strtol(optarg, NULL, 0));
                if (port == 0) {
                    fprintf(stderr, "Invalid port: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return EXIT_FAILURE;
        }
    }

    /* Create a TCP socket */
    svr_sock = rpopen_get_socket();
    if (!svr_sock) {
        return EXIT_FAILURE;
    }
    setsockopt(*svr_sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

    /* Bind socket */
    if (rpopen_bind_socket(svr_sock, &port) == -1) {
        return EXIT_FAILURE;
    }

    /* Listen on the socket */
    if (listen(*svr_sock, BACKLOG) < 0) {
        perror("Failed to listen on socket");
        return EXIT_FAILURE;
    }

    /* Accept connections */
    printf("rpserver: Listening for connections on port %d\n", port);
    while (1) {
        while ((cli_sock = accept(*svr_sock, (struct sockaddr *) &cli_addr, &addr_len)) < 0) {
            if (errno != ECHILD && errno != EINTR && errno != ERESTART) {
                perror("Accept failure");
                return EXIT_FAILURE;
            }
        }

        /* Connection has been accepted */
        pid = fork();
        if (pid == -1) {
            /* Error */
            perror("Fork failure");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {
            /* Child */
            printf("rpserver: Accepting connection from %s on port %d\n",
                    inet_ntoa(cli_addr.sin_addr),
                    ntohs(cli_addr.sin_port));
            rpserver_exec_command(cli_sock);

            /* If this is reached, exec(2) failed */
            fprintf(stderr, "rpserver: Fatal exit\n");
            return EXIT_FAILURE;
        }
        else {
            /* Parent */
            shutdown(cli_sock, SHUT_RD);
        }
    }

    printf("rpserver exiting: goodbye\n");
    return EXIT_SUCCESS;
}
