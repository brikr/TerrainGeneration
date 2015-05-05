CC = gcc
LDFLAGS = -L$(HOME)/apps/libbmp/lib
CFLAGS = -g -Wall -I$(HOME)/apps/libbmp/include
LIBS = -lbmp
BINS = bitmap_test

all: $(BINS)

bitmap_test: bitmap_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o bitmap_test bitmap_test.c

clean:
	$(RM) $(BINS) *.bmp
