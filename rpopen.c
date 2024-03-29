/*
 * rpopen.c - remote popen
 * Kyle Suarez
 */

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
 * Implements process I/O on a remote host in a way similar to popen(2).
 *
 * @param hostname The name of the host to connect to. If NULL or empty, then it
 * defaults to the PHOST environment variable.
 * @param port The port number to connect to on the remote host. If 0 is
 * specified, then this defaults to PPORT.
 * @param command A string containing the command to execute.
 *
 * @return A socket stream that must be closed with pclose(2). Data can be read
 * from it using read(2). If an error occurs, the return value is NULL.
 */
FILE *
rpopen (const char *hostname,
                int port,
        const char *command)
{
    FILE *fd;
    char *svr_hostname;
    int *sock;
    struct sockaddr_in svr_addr;
    struct hostent *svr_host;

    /* Obtain the hostname to connect to */
    if (!hostname || *hostname == '\0') {
        // Defaults to PHOST.
        svr_hostname = getenv("PHOST");
        if (!svr_hostname) {
            fprintf(stderr, "Hostname and PHOST are not specified\n");
            return NULL;
        }
    }
    else {
        svr_hostname = (char *) malloc(strlen(hostname));
        strcpy(svr_hostname, hostname);
    }

    /* Get the port on the server to connect to */
    if (port == 0) {
        // Defaults to PPORT. An ERANGE triggered by strtol is silently ignored
        // (will be either LONG_MIN or LONG_MAX) and the result is undefined.
        port = (int) strtol(getenv("PPORT"), NULL, 0);
        if (port == 0) {
            fprintf(stderr, "Port number and PPORT are not specified\n");
            return NULL;
        }
    }

    /* The command to execute */
    if (command == NULL) {
        fprintf(stderr, "Null command is invalid\n");
        return NULL;
    }

    /* Get the destination address */
    svr_host = gethostbyname(svr_hostname);
    if (!svr_host) {
        fprintf(stderr, "Failed to find address for '%s'\n", svr_hostname);
        return NULL;
    }
    memset(&svr_addr, 0, sizeof(struct sockaddr_in));
    memcpy((char *) &svr_addr.sin_addr,
           svr_host->h_addr_list[0],
           svr_host->h_length);
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(port);

    /* Obtain and bind a socket */
    sock = rpopen_get_socket();
    if (!sock) {
        perror("Failed to obtain a socket");
        return NULL;
    }
    if (rpopen_bind_socket(sock, NULL) == -1) {
        return NULL;
    }

    /* Connect to the client */
    if (connect(*sock, (struct sockaddr *) &svr_addr, sizeof(struct sockaddr)) < 0) {
        perror("connect(2) failure");
        return NULL;
    }

    /* Write the command to the server */
    write(*sock, command, strlen(command));
    fd = fdopen(*sock, "r");
    shutdown(*sock, SHUT_WR);

    /* Return the response */
    return fd;
}
