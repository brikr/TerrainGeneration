CC = gcc
CFLAGS = -g -Wall -Iinclude
LDFLAGS = -Llib
LIBS = -lbmp
BINS = bitmap_noktec bitmap_test

all: $(BINS)

bitmap_noktec: bitmap_noktec.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o bitmap_noktec bitmap_noktec.c

bitmap_test: bitmap_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o bitmap_test bitmap_test.c

clean:
	$(RM) $(BINS) *.bmp
