CC=gcc
CFLAGS=-Wall -Wextra
EXE = prodcomm
SCAN_BUILD_DIR = scan-build-out

all: main.o reader.o munch1.o  munch2.o writer.o queue.o
	$(CC) -o $(EXE) main.o reader.o munch1.o munch2.o writer.o queue.o -lpthread

main.o: main.c reader.h munch1.h munch2.h writer.h queue.h
	$(CC) $(WARNING_FLAGS) -c  main.c

reader.o: reader.c queue.h main.h
	$(CC) $(WARNING_FLAGS) -c reader.c

munch1.o: munch1.c  queue.h main.h
	$(CC) $(WARNING_FLAGS) -c munch1.c

munch2.o: munch2.c queue.h main.h
	$(CC) $(WARNING_FLAGS) -c munch2.c

writer.o: writer.c queue.h main.h
	$(CC) $(WARNING_FLAGS) -c writer.c

queue.o: queue.c 
	$(CC) $(WARNING_FLAGS) -c queue.c

clean:
	rm $(EXE) *.o
	rm -rf $(SCAN_BUILD_DIR)

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
