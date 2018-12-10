CC=gcc
SCAN_BUILD_DIR = scan-build-out
EXE=output
TEST_obj = advanced_testcase4.o
TEST_c = advanced_testcase4.c

all: $(TEST_obj) 537malloc.o rb_tree.o
	$(CC) -o $(EXE) $(TEST_obj) 537malloc.o rb_tree.o

# main.c is your testcase file name
$(TEST_obj): $(TEST_c)
	$(CC) -Wall -Wextra -c $(TEST_c)

# Include all your .o files in the below rule
obj: 537malloc.o rb_tree.o

537malloc.o: 537malloc.c 537malloc.h rb_tree.h
	$(CC) -Wall -Wextra -g -O0 -c 537malloc.c

range_tree.o: range_tree.c rb_tree.h
	$(CC) -Wall -Wextra -g -O0 -c rb_tree.c

clean:
	-rm *.o $(EXE)

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html
