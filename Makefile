CC=gcc
CFLAGS = -Wall -g 

all: rpopen-demo rpserver

rpopen-demo: rpopen-demo.o rpopen.o
	$(CC) -o rpopen-demo rpopen-demo.o rpopen.o $(LIBS) 

rpserver: rpserver.o 
	$(CC) -o rpserver rpserver.o $(LIBS)

rpopen.o: rpopen.c 

rpopen-demo.o: rpopen-demo.c 

rpserver.o: rpserver.c 

clean:
	rm -f rpopen-demo rpserver rpopen-demo.o rpserver.o rpopen.o
