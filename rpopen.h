#ifndef RPOPEN_H
#define RPOPEN_H
#include <stdio.h>

FILE *
rpopen (char *, int, char *);

int
rpopen_get_socket (void);

int rpopen_bind_socket (void);
#endif
