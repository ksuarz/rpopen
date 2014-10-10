#ifndef RPOPEN_H
#define RPOPEN_H

#include <stdio.h>
#include <sys/socket.h>

/**
 * Implements process I/O on a remote host in a way similar to popen(2).
 *
 * @param host The name of the host to connect to. If NULL or empty, then it
 * defaults to the PHOST environment variable.
 * @param port The port number to connect to on the remote host. If 0 is
 * specified, then this defaults to PPORT.
 * @param cmd A string containing the command to execute.
 *
 * @return An I/O stream that must be closed with pclose(2). If an error occurs,
 * the return value is NULL.
 */
FILE *
rpopen (const char *, int, const char *);

/**
 * Creates an IPv4 socket.
 *
 * @return A socket descriptor created by socket(2). If an error occurs, NULL is
 * returned and errno is set.
 */
int *
rpopen_get_socket (void);

/**
 * Binds an IPv4 socket for connecting to an rpserver.
 *
 * @param sock A pointer to the socket to bind.
 */
int
rpopen_bind_socket (int *);
#endif
