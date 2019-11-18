#ifndef SYNC_H
#define SYNC_H
#include <semaphore.h>
#include <pthread.h>

//Mechanism to abstract and this way we manage to reuse code in sync.c
#ifdef MUTEX
    #define LOCK pthread_mutex_t
#elif RWLOCK
    #define LOCK pthread_rwlock_t
#else
    #define LOCK int
#endif


void locksInit(LOCK* mainLock);
void destroyLocks(LOCK* mainLock);
void conditionsToLockRead(LOCK* mainLock);
void conditionsToLockWrite(LOCK* mainLock);
void conditionsToUnlock(LOCK* mainLock);
void semWait(sem_t* sem);
void semPost(sem_t* sem);
void initMainSync(pthread_mutex_t* consMutex, pthread_mutex_t* prodMutex, sem_t* producer, sem_t* consumer);
void destroyMainSync(pthread_mutex_t* consMutex, pthread_mutex_t* prodMutex, sem_t* producer, sem_t* consumer);
void lockMutex(pthread_mutex_t* mainLock);
void unlockMutex(pthread_mutex_t* mainLock);

#endif