CC=gcc
CFLAGS=-Wall -g
FILES = alloc.o alloc_test.c .alloc_util.o

all: alloc_test

alloc.o: alloc.c
	@echo "Building alloc.c"
	$(CC) $(CFLAGS) -c $< -o $@

alloc_test: $(FILES)
	@echo "Building alloc_test"
	$(CC) $(CFLAGS) -Wconversion $(FILES) -o $@

clean:
	@echo "Cleaning/resetting build files"
	$(RM) alloc_test alloc.o
	$(RM) tests/*_diff*.txt tests/*_actual*.txt
