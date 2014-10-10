#ifndef RPOPEN_H
#define RPOPEN_H

#include <stdio.h>
#include <sys/socket.h>

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
rpopen (const char *, int, const char *);

/**
 * Creates a socket for connecting to the rpserver.
 *
 * @return A socket descriptor created by socket(2). If an error occurs, NULL is
 * returned and errno is set.
 */
int *
rpopen_get_socket (void);

/**
 * Binds a socket for connecting to an rpserver.
 *
 * @param sock A pointer to a socket obtained from rpopen_get_socket().
 *
 * @return 0 if the socket was bound properly; -1 otherwise.
 */
int
rpopen_bind_socket (int *);
#endif
