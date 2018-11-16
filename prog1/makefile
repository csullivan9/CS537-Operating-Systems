CC=gcc
CFLAGS=-Wall
EXE = 537ps

all: options.o processes.o parser.o
	$(CC) -o $(EXE) options.o processes.o parser.o

options.o: options.c parser.h processes.h
	$(CC) $(WARNING_FLAGS) -c options.c

processes.o: processes.c processes.h options.h parser.h
	$(CC) $(WARNING_FLAGS) -c processes.c

parser.o: parser.c parser.h
	$(CC) $(WARNING_FLAGS) -c parser.c

clean:
	rm $(EXE) *.o

