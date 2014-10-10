/*
 * rpserver.c - remote rpopen server
 * Kyle Suarez
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Create a socket for listening for the rpopen server.
 *
 * @return A pointer to a descriptor for the socket. If an error occurs, NULL is
 * returned.
 */
int
rpserver_get_socket (void) {
    int *sock;
    int sockoptval;

    /* Allocate space */
    sock = (int *) malloc(sizeof(int));
    if (!sock) {
        perror("Failed to malloc(3) space for a socket");
        return NULL;
    }

    /* Initialize socket */
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0) {
        perror("Failed to create a socket");
        return NULL;
    }

    /* Set socket options */
    sockoptval = 1;
    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));
    return sock;
}

/*
 * Binds a socket ready for listening by the rpopen server.
 *
 * @param sock A pointer to the socket to bind.
 * @param port The port for the socket to be bound to.
 * @return 0 if the socket was bound properly; -1 otherwise.
 */
int
rpserver_bind_socket (int *sock, int port) {
    struct sockaddr_in svr_addr;

    memset(&svr_addr, 0, sizeof(struct sockaddr_in));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(port);
    svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(*sock, (struct sockaddr *) &svr_addr, sizeof(struct sockaddr_in)) < 1) {
        perror("Bind failure");
        return -1;
    }
    else {
        return 0;
    }
}

/**
 * Executes a command read from the socket.
 *
 * @param socket sock socket descriptor from an accepted connection.
 * @return On success, this performs an exec(2) and does not return. If an error
 * occurs, errno is set.
 */
void
rpserver_exec_command (int socket) {
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
    }
    if (bytes_read == -1) {
        perror("Failure on read(2) on socket");
        return;
    }

    /* Duplicate our STDOUT onto the socket */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    dup2(request_sock, STDOUT_FILENO);

    /* Execute the command */
    execlp("/bin/sh", "/bin/sh -c", &buffer, (char *) 0);
    perror("execlp(2) failure");
}

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
    pid_t pid;
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
    server_sock = rpserver_get_socket();
    if (server_sock == -1) {
        return EXIT_FAILURE;
    }

    /* Bind socket */
    if (rpserver_bind_socket(server_sock) == -1) {
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
                    fprintf("rpserver: Warning: Ignoring error %d\n", errno);
                    break;
                default:
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
            rpserver_exec_command(request_sock);

            /* If this is reached, exec(2) failed */
            return EXIT_FAILURE;
        }
        else {
            /* Parent */
            shutdown(request_sock, SHUT_RD);
        }
    }

    printf("rpserver exiting: goodbye\n");
    return EXIT_SUCCESS;
}
