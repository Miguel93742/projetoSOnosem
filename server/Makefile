# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2019-20

CC   = gcc 
LD   = gcc
CFLAGS =-Wall -std=gnu99 -I../
LDFLAGS=-lm


# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean 

all: tecnicofs-nosync tecnicofs-mutex tecnicofs-rwlock

tecnicofs-rwlock: lib/bst.o lib/hash.o sync-rwlock.o fs-rwlock.o main-rwlock.o
	$(LD) $(CFLAGS) $(LDFLAGS)  -pthread -o tecnicofs-rwlock lib/bst.o lib/hash.o sync-rwlock.o fs-rwlock.o main-rwlock.o


lib/bst.o: lib/bst.c lib/bst.h
	$(CC) $(CFLAGS) -c lib/bst.c -o lib/bst.o 
lib/hash.o: lib/hash.c lib/hash.h
	$(CC) $(CFLAGS) -c lib/hash.c -o lib/hash.o

sync-rwlock.o: CFLAGS+=-DRWLOCK
sync-rwlock.o: sync.c sync.h
	$(CC) $(CFLAGS) -c sync.c -o sync-rwlock.o

fs-rwlock.o: CFLAGS+=-DRWLOCK
fs-rwlock.o: fs.c fs.h lib/bst.h lib/hash.h sync.h
	$(CC) $(CFLAGS)  -c fs.c -o fs-rwlock.o

main-rwlock.o: CFLAGS+=-DRWLOCK
main-rwlock.o: main.c fs.h lib/bst.h lib/hash.h sync.h
	$(CC) $(CFLAGS) -c main.c -o main-rwlock.o

clean:
	@echo Cleaning...
	rm -f lib/*.o *.o *.out tecnicofs-rwlock

run: tecnicofs-rwlock
	./tecnicofs-rwlock socketName test_rwlock.out 3 

