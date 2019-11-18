#ifndef FS_H
#define FS_H
#include "lib/bst.h"
#include "sync.h"


//Variaveis para distinguir chamdas de funcoes com o rename e sem rename
#define COMMAND_REN 123
#define NOT_REN 321

typedef struct tecnicofs {
    int nextINumber;
    int numBuckets;
    node ** hashRoot;
} tecnicofs;

LOCK *countLock;

int obtainNewInumber(tecnicofs* fs);
tecnicofs* new_tecnicofs();
void free_tecnicofs(tecnicofs* fs);
void create(tecnicofs* fs, char *name, int inumber, int isRename);
void delete(tecnicofs* fs, char *name, int isRename);
int lookup(tecnicofs* fs, char *name, int isRename);
void renameFile(tecnicofs *fs, char *f1, char *f2);
void print_tecnicofs_tree(FILE * fp, tecnicofs *fs);

#endif /* FS_H */
