.PHONY: clean

CC=gcc
CFLAGS=-march=native -Wall -O -g

# Uncomment the next line to disable assertions and buffer-zeroing
#CFLAGS+=-DNDEBUG

rdrand-stream: main.o rdrand-fill.o cpuid.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -fv *.o rdrand-stream
