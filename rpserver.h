#ifndef RPSERVER_H
#define RPSERVER_H
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
 * Create a socket for listening for the rpopen server.
 * @return -1 if an error occurs; otherwise, a descriptor for the socket.
 */
int
rpserver_get_socket (void);

/*
 * Binds a socket ready for listening by the rpopen server.
 * @param socket A socket created by rpserver_get_socket.
 */
int
rpserver_bind_socket (int);

/**
 * Executes a command read from the socket.
 *
 * @param socket A socket descriptor from an accepted connection.
 * @return On success, this performs an exec(2) and does not return. If an error
 * occurs, errno is set and this function returns -1.
 */
void
rpserver_exec_command (int);

#endif
