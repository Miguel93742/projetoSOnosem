#include "fs.h"
#include "lib/hash.h"
#include "sync.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>



int obtainNewInumber(tecnicofs* fs) {
	int newInumber = ++(fs->nextINumber);
	return newInumber;
}

tecnicofs* new_tecnicofs(int num){

	int e;

	tecnicofs*fs = malloc(sizeof(tecnicofs));

	if (!fs) {
		perror("failed to allocate tecnicofs");
		exit(EXIT_FAILURE);
	}
	
	fs->numBuckets = num;

	fs->hashRoot = malloc(sizeof(node*) * fs->numBuckets); // Since we now have a hash table of bst we need to alloc space for them
	countLock = malloc(sizeof(LOCK) * fs->numBuckets); // Same for the locks one for each bst

	for (e = 0; e < fs->numBuckets; e++){ // Initiating locks and bst
		fs->hashRoot[e] = NULL;
		locksInit(&countLock[e]);
	}
	
	return fs;
}

void free_tecnicofs(tecnicofs* fs){ 
	
	//Freeing the memory allocated by the file system

	int e;

	for (e = 0; e < fs->numBuckets; e++){ 
		free_tree(fs->hashRoot[e]);
		destroyLocks(&countLock[e]);
	}
	free(countLock);
	free(fs->hashRoot);
	free(fs);
}


void create(tecnicofs* fs, char *name, int inumber, int isRename){ 

	//We block the acess to the bst with the lock according to the index of the hash table we want to access
	
	int ind = hash(name, fs->numBuckets);

	if (isRename != COMMAND_REN)
		conditionsToLockWrite(&countLock[ind]);

	fs->hashRoot[ind] = insert(fs->hashRoot[ind], name, inumber);
	
	if (isRename != COMMAND_REN)
		conditionsToUnlock(&countLock[ind]);
}


void delete(tecnicofs* fs, char *name, int isRename){
	
	//We block the acess to the bst with the lock according to the index of the hash table we want to access

	int ind = hash(name, fs->numBuckets);
	
	if (isRename != COMMAND_REN)
		conditionsToLockWrite(&countLock[ind]);
	
	fs->hashRoot[ind] = remove_item(fs->hashRoot[ind], name);
	
	if (isRename != COMMAND_REN)
		conditionsToUnlock(&countLock[ind]);
}

int lookup(tecnicofs* fs, char *name, int isRename){
	
	//We block the acess to the bst with the lock according to the index of the hash table we want to access

	int ind = hash(name, fs->numBuckets);

	if (isRename != COMMAND_REN)
		conditionsToLockRead(&countLock[ind]);
	
	node* searchNode = search(fs->hashRoot[ind], name);
	
	if (isRename != COMMAND_REN)
		conditionsToUnlock(&countLock[ind]);

	if ( searchNode ) return searchNode->inumber;
	return 0;
}

void renameFile(tecnicofs *fs, char *f1, char *f2){
	/*
	
	Params:tecnicofs *fs, char *f1, char *f2 (f1 is the name of the existent file and f2 the new name)
	Return: void
	Description: In this function since were acessing two different trees (in some cases) we have to
	block them both. To avoid deadlocks we have chose to order the index of the locks in crescent order
	avoiding deadlocks this way. If the names correspond to the same index we just lock it normally.

	*/


	int a = 0, b = 0;

	int hashF1 = hash(f1, fs->numBuckets); 
	int hashF2 = hash(f2, fs->numBuckets);
	int auxINumber = 0;
	if (hashF1 == hashF2)
		a = hashF1;
	if (hashF1 < hashF2){
		a = hashF1;
		b = hashF2;
	}
	else{
		a  = hashF2;
		b = hashF1;
	}

	conditionsToLockWrite(&countLock[a]);
    if (hashF1 == hashF2)	
    	conditionsToLockWrite(&countLock[b]);

    if (lookup(fs, f1, COMMAND_REN) == 0){
        printf("Nao foi possivel renomear\n");
        if (hashF1 != hashF2)
        	conditionsToUnlock(&countLock[b]);
        conditionsToUnlock(&countLock[a]);
        return;
    }
    if (lookup(fs, f2, COMMAND_REN) != 0){
        printf("Nao foi possivel renomear\n");
        if (hashF1 != hashF2)
        	conditionsToUnlock(&countLock[b]);
        conditionsToUnlock(&countLock[a]);
        return;
    }
                
    auxINumber = lookup(fs, f1, COMMAND_REN);

    delete(fs, f1, COMMAND_REN);

    create(fs, f2, auxINumber, COMMAND_REN);
    if (hashF1 == hashF2)
    	conditionsToUnlock(&countLock[b]);
    conditionsToUnlock(&countLock[a]);
}

void print_tecnicofs_tree(FILE * fp, tecnicofs *fs){
	int e;
	for (e = 0; e < fs->numBuckets; e++){
		print_tree(fp, fs->hashRoot[e]);
	}
}
