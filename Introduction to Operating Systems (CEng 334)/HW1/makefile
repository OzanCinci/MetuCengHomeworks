CC = gcc

bgame: bgame.c logging.o message.o
	$(CC) -o bgame bgame.c logging.o message.o

logging.o: logging.h logging.c
	$(CC) -c logging.c

message.o: message.h message.c
	$(CC) -c message.c

clean:
	rm -f bgame *.o
