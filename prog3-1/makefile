CC = gcc
CFLAGS = -Wall -Wextra -g
EXE = 537make
SCAN_BUILD_DIR = scan-build-out

all: main.o parser.o traverse.o graph.o
	$(CC) $(CFLAGS) -o 537make main.o parser.o traverse.o graph.o

main.o: main.c parser.h traverse.h graph.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c graph.h traverse.h
	$(CC) $(CFLAGS) -c parser.c

traverse.o: traverse.c graph.h
	$(CC) $(CFLAGS) -c traverse.c

graph.o: graph.c
	$(CC) $(CFLAGS) -c graph.c

clean: 
	rm $(EXE) *.o

#
# Run the Clang Static Analyzer
#
scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

#
# View the one scan available using firefox
#
scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html