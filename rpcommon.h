#ifndef RPCOMMON_H
#define RPCOMMON_H

/**
 * Creates a socket for the rpopen server or client.
 *
 * @return A pointer to a newly-created socket, or NULL if an error occurred.
 */
int *
rpopen_get_socket (void);

/**
 * Binds a socket for the rpopen server or client.
 *
 * @param sock A pointer to a socket obtained from rpopen_get_socket().
 * @param port A pointer to the port number to bind the socket to or NULL.
 *
 * @return 0 if the socket was bound properly; -1 otherwise.
 */
int
rpopen_bind_socket (int *, int *);

#endif
