CC = gcc

bgame: SOLUTION_BGAME.c logging.o message.o
	$(CC) -o SOLUTION_BGAME SOLUTION_BGAME.c logging.o message.o

logging.o: logging.h logging.c
	$(CC) -c logging.c

message.o: message.h message.c
	$(CC) -c message.c

clean:
	rm -f SOLUTION_BGAME *.o
