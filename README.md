Sockets
=======
Socket programming for Distributed Systems.

Objective
---------
Your goal in this assignment is to use sockets at the operating system level to
allow a program to open the output stream of a remote program. You will write a
function called `rpopen`, which is similar to the `popen(3)` standard I/O
library function that is found on Linux, BSD, OS X, and most other POSIX
systems. You will write a server and the client function and test it with a
client program.  clients and a server.

Languages
---------
This assignment must be done in C. You will use socket-related system calls as
well as `fork(3)` and `execl(3)` system calls to create processes and `dup2(3)`
to duplicate file descriptors.

Specifications
--------------
For this assignment, you will write one server and one rpopen client library.

### The client
The client is a function that implements a "remote popen," which allows a
program to read the output of a specified remote program. The syntax of the
function is:

    FILE *rpopen(char *server, int port, char *cmd);

where:

- `server` is the name of the remote server. The value may be 0 or an empty
  string. In that case, the value of the environment variable `PHOST` will be
  used for the host. You can set a shell environment variable in the bash shell
  with the command

    $ export PHOST=localhost

- `port` is the port number of the remote server. You should set this to
  whatever port your server is using to accept connections. The value may be 0.
  In that case, the value of the environment variable `PPORT` will be used for
  the port number. The port number should be in the range 1024...65536, although
  the use of ports above 49151 is discouraged since that range is reserved for
  ephemeral ports, which are port numbers that the operating system would
  allocate. 
- `cmd` is the remote command to execute. This can be an arbitrary
  command with arguments and will be sent to and executed on the server.
  
An example of a program that uses your `rpopen` function is the following:

    #include <stdio.h>
    #define BSIZE 1024

    FILE *rpopen(char *host, int port, char *cmd);

    main(int argc, char **argv)
    {
        FILE *fp;
        char buf[BSIZE];

        if ((fp = rpopen("localhost", 0, "ls -lL /etc")) != 0) {
            /* read the output from the command */

            while (fgets(buf, BSIZE, fp) != 0)
                fputs(buf, stdout);
        }
        fclose(fp);
    }

This program connects to a server that is running on the same machine on the
client and listening on a port number that is defined in the environment
variable PPORT. The server executes the command:

    ls -lL /etc

and the client reads its results one line at a time, printing each line that it
reads.

### Client Protocol and Operation
The client employs a simple protocol for communicating with the server. It
performs the following steps:

1. Get the host name and port number from the parameters or environment
   variables.
2. Connect to the server. This involves looking up the server's address,
   creating a socket, and using the socket to connect to the server. You should
   be able to use most of the code in the TCP client demo (demo-03) to do this.
   You can use the function that connects to the service without modification.
3. Send the command over the socket to the server. You can use the write system
   call to do this.
4. Convert the file descriptor for the socket to a FILE pointer opened for
   reading with the fdopen function.
5. Return the FILE pointer to the caller.

### The server
The server executable will be named `rpserver` (remote pipe server). It accepts
a port number on the command line following a `-p` flag. For example:

    $ rpserver -p 12345

will start the server listening to port 12345. If this parameter is missing, you
should fall back to some hard-coded default port number in the range
1024...49151.

Use getopt.

The server will perform the following sequence of operations:

1. Open a socket and set it up for listening - follow the example in the TCP
   sockets demo code. Don't forget to setsockopt to SO_REUSEADDR as in the
   sample code in the tutorial if you are using C/C++). Your program does not
   have to be multithreaded. As explained later, each connection will result in
   the spawning of a new process.
2. Loop forever, waiting for incoming connections with the accept system call.
   When accept returns, it gives you a new socket that is used for communicating
   with the client that made the connection.
3. Fork a child process. The parent closes the new socket (it does not need to
   talk to the client) and returns back to waiting for a new connection.
   The child reads data from the client. The only data it will get from the client
   is the command that the client wants it to execute. After reading this data,
   the client can close the socket for reading with the shutdown system call:

        shutdown(sock, SHUT_RD);

   This will ensure that the server will be unable to wait and try to read any
   data from the client.
4. Now the child process needs to execute the requested command. Prior to doing
   this, however, we want to make sure that the standard input is closed and
   that the standard output goes to the socket. These settings will remain in
   effect when the requested program is executed.  To close the current standard
   input and output, just close file descriptors 0 and 1:

        close(0);
        close(1);

   Then duplicate the socket onto file descriptor 1 (standard output) using the
   `dup2(3)` system call.

5. The child process can overwrite itself by executing the requested command. The
   easiest way to do this and avoid the hassle of parsing arguments is to simply
   execute the shell and give it that command as a parameter:

        /bin/sh -c requested_command_with_arguments

   The execlp function makes this simple. It's a thin wrapper over the execve
   system call that takes an arbitrary set of parameters that become the command
   and its arguments. I this case, your command is the shell, /bin/sh and the
   parameters are: (1) the shell's name, (2) the -c flag, and (3) the command
   that you read from the client. The list is terminated with a (char *)0
   sentinel. The call will be something like:

        execlp("/bin/sh", "/bin/sh", "-c", command, (char *)0);

   If the program executes, it will overwrite the code in the child process and no
   further statements in your program will be executed. If an error occurs, then
   execlp will return. You may print a message on the standard error and, since no
   feedback is sent to the client, just exit the client.
