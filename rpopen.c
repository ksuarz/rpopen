/*
 * rpopen.c - remote popen
 * Kyle Suarez
 */

#include <stdio.h>
#include <stdlib.h>

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
rpopen (char *host, int port, char *cmd)
{
    // Do argument checking
    if (!host || *host == '\0') {
        // Defaults to PHOST.
        host = getenv("PHOST");
        if (!host) {
            fprintf(stderr, "Hostname and PHOST are not specified.");
            return NULL;
        }
    }
    if (port == 0) {
        // Defaults to PPORT. An ERANGE triggered by strtol is silently ignored
        // (will be either LONG_MIN or LONG_MAX) and the result is undefined.
        port = (int) strtol(getenv("PPORT"), NULL, 0);
        if (port == 0) {
            fprintf(stderr, "Port number and PPORT are not specified.");
            return NULL;
        }
    }
    if (cmd == NULL) {
        fprintf(stderr, "Null command is invalid.");
        return NULL;
    }
    /* TODO */
}
