/* 
*	SYSC 4001 - Assignment 2
*	Maryn Marsland
*	100854916
*	Shared components for producer and consumer
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>

//buffers of shared memory
struct buffer{
	struct buf{
	int size;
	char data [128];
	}buf[100];
};  

//for semaphores
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int S;		//semaphore S: initialized to 1, to allow 1 processs to access the shared memory
int N;		//semaphore N: initialized to 0, as the shared memory begins empty
int E;		//semaphore E: initialized to 100 as the shared memory begins with 100 available buffers

int sizeOfFile;

//semaphore wait
int sem_w(int semaphore)
{
	//printf("Waiting on a semaphore\n");
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* "wait" operation: decrese semaphore valuse by 1 */
    sem_b.sem_flg = 0;
    if (semop(semaphore, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);	
}

//semaphore signal
int sem_s(int semaphore)
{
	//printf("Signalling a semaphore\n");
struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* "signal" operation: increases semaphore value by 1 */
    sem_b.sem_flg = 0;
    if (semop(semaphore, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return(0);
    }
    return(1);
}


