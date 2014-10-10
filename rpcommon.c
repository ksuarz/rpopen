#include "rpcommon.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Creates a socket for the rpopen server or client.
 *
 * @return A pointer to a newly-created socket, or NULL if an error occurred.
 */
int *
rpopen_get_socket (void) {
    int *sock;

    /* Allocate space for the socket */
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

    return sock;
}

/**
 * Binds a socket for the rpopen server or client.
 *
 * @param sock A pointer to a socket obtained from rpopen_get_socket().
 * @param port A pointer to the port number to bind the socket to or NULL.
 *
 * @return 0 if the socket was bound properly; -1 otherwise.
 */
int
rpopen_bind_socket (int *sock, int *port) {
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port ? *port : 0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(*sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Bind failure");
        return -1;
    }
    else {
        return 0;
    }
}
