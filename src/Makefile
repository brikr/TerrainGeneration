CC = gcc
LDFLAGS = -L$(HOME)/apps/libbmp/lib
CFLAGS = -g -Wall -I$(HOME)/apps/libbmp/include
LIBS = -lbmp
BINS = bitmaptest terraingen

all: $(BINS)

bitmaptest: bitmaptest.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o bitmaptest bitmaptest.c

terraingen: terraingen.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o terraingen terraingen.c

clean:
	$(RM) $(BINS) *.bmp
