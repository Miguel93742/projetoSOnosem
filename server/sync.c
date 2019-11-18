#include "sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*

Params:number of locks needed   
Return: void
Description: Allocates memory for the locks necessary and intializes them

*/

void locksInit(LOCK* mainLock){

    if (pthread_rwlock_init(mainLock, NULL) != 0){
        printf("Erro a inicializar cadeado\n");
        exit(EXIT_FAILURE);
    }
}


/*

Params:number of locks needed
Return: void 
Description: Destroys the locks, closes the files used in the prgram
 and frees the space alocated for the locks

*/

void destroyLocks(LOCK* mainLock){

    if (pthread_rwlock_destroy(mainLock) != 0){
        printf("Erro a destruir cadeado\n");
        exit(EXIT_FAILURE);
    }
}


void lockMutex(pthread_mutex_t *mainLock){
/*

Params:phtread_mutex_t *mainLock
Return:void
Description: This function locks the mutex passed as an argument.

*/

    if (pthread_mutex_lock(mainLock) != 0){
        printf("Error locking!\n");
        exit(EXIT_FAILURE);
    }
}


void unlockMutex(pthread_mutex_t *mainLock){
/*

Params:phtread_mutex_t *mainLock
Return:void
Description: This function unlocks the mutex passed as an argument.

*/
    
    if (pthread_mutex_unlock(mainLock) != 0){
        printf("Error locking!\n");
        exit(EXIT_FAILURE);
    }
}

void conditionsToLockRead(LOCK* mainLock){
/*

Params:Depends on compilation flags (LOCK is defined in sync.h)
Return:void
Description:Based on the compilation flags it decides if it blocks
the critical sections of the code with MUTEXES or with RWLOCKS
or neither if no flag is passed.

*/

    if (pthread_rwlock_rdlock(mainLock) != 0){
        printf("Error locking!\n");
        exit(EXIT_FAILURE);
    }
}


void conditionsToLockWrite(LOCK* mainLock){
/*

Params:Depends on compilation flags (LOCK is defined in sync.h)
Return:void
Description:Based on the compilation flags it decides if it blocks
the critical sections of the code with MUTEXES or with RWLOCKS
or neither if no flag is passed.

*/
    if (pthread_rwlock_wrlock(mainLock) != 0){
        printf("Error locking!\n");
        exit(EXIT_FAILURE);
    }
}


void conditionsToUnlock(LOCK* mainLock){
/*

Params:Depends on compilation flags (LOCK defined in sync.h)
Return:void
Description: Depending on the compilation flag it unlocks a MUTEX
or a RWLOCK.

*/
    if (pthread_rwlock_unlock(mainLock) != 0){
        printf("Error locking!\n");
        exit(EXIT_FAILURE);
    }
}
