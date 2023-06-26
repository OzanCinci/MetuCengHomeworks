CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lpthread
SOURCES=hw2.c hw2_output.c
OBJECTS=$(SOURCES:.c=.o)
	EXECUTABLE=hw2

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -f $(EXECUTABLE) $(OBJECTS)
