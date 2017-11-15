CC = g++
CFLAGS = -O2 $(INCLUDEDIR)

OBJ = irmain.o irmodel1.o filesys1.o utility.o
OBJ1 = irindex.o irmodel1.o filesys1.o utility.o

all : irmain
index : irindex
clean:
	rm -f *.o *% core irmain irindex
irmain : irmain.o irmodel1.o filesys1.o utility.o
	$(CC) $(CFLAGS) -o irmain $(OBJ) -lm
irindex : irindex.o irmodel1.o filesys1.o utility.o
	$(CC) $(CFLAGS) -o irindex $(OBJ1) -lm

irmain.o : irmain.c
	$(CC) -c $(CFLAGS) irmain.c
irindex.o : irindex.c
	$(CC) -c $(CFLAGS) irindex.c
filesys1.o : filesys1.c
	$(CC) -c $(CFLAGS) filesys1.c
utility.o : utility.c
	$(CC) -c $(CFLAGS) utility.c
irmodel1.o : irmodel1.c
	$(CC) -c $(CFLAGS) irmodel1.c

