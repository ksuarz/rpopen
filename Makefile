CC=gcc
CFLAGS = -Wall -g 

all: rpopen-demo rpserver

rpopen-demo: rpopen-demo.o rpopen.o rpcommon.o
	$(CC) $(CFLAGS) -o rpopen-demo rpcommon.o rpopen-demo.o rpopen.o $(LIBS) 

rpserver: rpserver.o rpcommon.o
	$(CC) $(CFLAGS) -o rpserver rpcommon.o rpserver.o $(LIBS)

rpopen.o: rpopen.c 

rpopen-demo.o: rpopen-demo.c 

rpserver.o: rpserver.c 

rpcommon.o: rpopen.c
	$(CC) $(CFLAGS) -c rpcommon.c rpcommon.h

clean:
	rm -f rpopen-demo rpserver rpopen-demo.o rpserver.o rpopen.o
