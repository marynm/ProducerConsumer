/* 
*	SYSC 4001 - Assignment 2
*	Maryn Marsland
*	100854916
*	Consumer
*/


#include "pc.h"

int main(int argc, const char * argv[]) 
{
	char output_file [100];
	FILE* out;

	//establish output file
	if(argc < 2)	//if no command line arguments input
	{
		strcpy(output_file, "output.txt");	//use defult file input.txt
	}
	else
	{
		strcpy(output_file, argv[1]);	//get file name from command like argument
	}
	printf("Opening file: %s\n", output_file);
	
	out = fopen(output_file, "w");
	//open file
	if(out == NULL)
	{
		printf("File '%s' does not exist in this location.\n Closing consumer.\n", output_file);
		return;
	}


	//set up/connect to shared memory
	void *shared_memory = (void *)0;
    	int shmid;
    	shmid = shmget((key_t)1234, sizeof(struct buffer), 0666 | IPC_CREAT);

   	 if (shmid == -1) {
        	fprintf(stderr, "shmget failed\n");
        	exit(EXIT_FAILURE);
    	}

    	shared_memory = shmat(shmid, (void *)0, 0);
   	 if (shared_memory == (void *)-1) {
       	 	fprintf(stderr, "shmat failed\n");
        	exit(EXIT_FAILURE);
    	}

    	printf("Memory attached at %X\n", (int)shared_memory);

	//set up/connect to semaphores
	union semun sem_union;
	
	S = semget((key_t) 1234, 1, 0666 | IPC_CREAT);
	N = semget((key_t) 4321, 1, 0666 | IPC_CREAT);
	E = semget((key_t) 1234, 1, 0666 | IPC_CREAT);
	
	sem_union.val = 1;
	semctl(S, 0, SETVAL, sem_union);

	sem_union.val = 0;
	semctl(N, 0, SETVAL, sem_union);
	
	sem_union.val = 100;
	semctl(E, 0, SETVAL, sem_union);

	struct buffer * consumer_buffer;
	consumer_buffer = (struct buffer *)shared_memory;
	
	int buffer_index = 0;

	//get total bytes in file from producer
	printf("Waiting for producer to connect and send the size of its input file...\n");
	sem_w(N);
	sem_s(S);
	int totalBytesInFile = consumer_buffer->buf[buffer_index].size;
	sem_w(S);
	
	int totalByteCount = 0;
	int count;

	while(totalByteCount < totalBytesInFile)
	{
		buffer_index ++;
		//printf("waitng on N\n");
		sem_w(N);	//wait on semaphore N: shared memory not empty
		//printf("waiting on S\n");
		//sem_w(S);	//wait on semaphore S: shared memory not in use

		//"consume" buffer from shared memory
		//printf("Read from buffer[%d]: %s\n", buffer_index, consumer_buffer->buf[buffer_index].data);
		//printf("Bytes read = %d\n", consumer_buffer->buf[buffer_index].size);
		count = fwrite(consumer_buffer->buf[buffer_index].data, 1, consumer_buffer->buf[buffer_index].size, out);
		if(count != consumer_buffer->buf[buffer_index].size)
			printf("Error in writing to file!!");
		totalByteCount += count;

		//sem_s(S);	//signal semaphore S
		sem_s(E);	//signal semaphore E

	}
	//once reached end of file, close output file
	printf("Total bytes written to file %s = %d.\n", output_file, totalByteCount);
	fclose(out);

}


