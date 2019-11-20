# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2019-20

CC   = gcc 
LD   = gcc
CFLAGS =-Wall -std=gnu99 -I../
LDFLAGS=-lm


# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean 

all: tecnicofs-rwlock

tecnicofs-rwlock: server/lib/bst.o server/lib/hash.o sync-rwlock.o fs-rwlock.o main-rwlock.o
	$(LD) $(CFLAGS) $(LDFLAGS)  -pthread -o tecnicofs-rwlock server/lib/bst.o server/lib/hash.o sync-rwlock.o fs-rwlock.o main-rwlock.o


server/lib/bst.o: server/lib/bst.c server/lib/bst.h
	$(CC) $(CFLAGS) -c server/lib/bst.c -o server/lib/bst.o 
server/lib/hash.o: server/lib/hash.c server/lib/hash.h
	$(CC) $(CFLAGS) -c server/lib/hash.c -o server/lib/hash.o

sync-rwlock.o: CFLAGS+=-DRWLOCK
sync-rwlock.o: server/sync.c server/sync.h
	$(CC) $(CFLAGS) -c server/sync.c -o server/sync-rwlock.o

fs-rwlock.o: CFLAGS+=-DRWLOCK
fs-rwlock.o: server/fs.c server/fs.h server/lib/bst.h server/lib/hash.h server/sync.h
	$(CC) $(CFLAGS)  -c server/fs.c -o server/fs-rwlock.o

main-rwlock.o: CFLAGS+=-DRWLOCK
main-rwlock.o: server/main.c server/fs.h server/lib/bst.h server/lib/hash.h server/sync.h
	$(CC) $(CFLAGS) -c server/main.c -o server/main-rwlock.o

clean:
	@echo Cleaning...
	rm -f server/lib/*.o *.o *.out server/tecnicofs-rwlock

run: tecnicofs-rwlock
	./server/tecnicofs-rwlock socketName test_rwlock.out 3 

