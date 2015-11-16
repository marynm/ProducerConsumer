/* 
*	SYSC 4001 - Assignment 2
*	Maryn Marsland
*	100854916
*	Producer
*/ 

#include "pc.h"

int BytesWritten = 0;	//count of the total number of bytes written to the shared memory
int TotalBytes;	//the total number of bytes in the file

char input_file [100];
FILE* in;
char input_buffer [BUFSIZ];

int main(int argc, const char * argv[]) 
{
	/* get input file name */
	if(argc < 2)	//if no command line arguments input
	{
		strcpy(input_file, "input.txt");	//use defult file input.txt
	}
	else
	{
		strcpy(input_file, argv[1]);	//get file name from command like argument
	}


	/* open file */
	printf("Opening file: %s\n", input_file);
	
	in = fopen(input_file, "r");

	if(in == NULL)
	{
		printf("File '%s' does not exist in this location.\n Closing producer.\n", input_file);
		return;
	}
	
	//get the total number of bytes in the file
	fseek(in, 0, SEEK_END);
	TotalBytes = ftell(in);
	fseek(in, 0, SEEK_SET);	


	printf("Total bytes in file = %d\n", TotalBytes);


	/* set up/connect to shared memory */
 	void *shared_memory = (void *)0;
    	char buffer[BUFSIZ];
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
	

	/* set up/connect to semaphores	*/
	union semun sem_union;
	
	S = semget((key_t) 1234, 1, 0666 | IPC_CREAT);
	sem_union.val = 1;
	semctl(S, 0, SETVAL, sem_union);

	N = semget((key_t) 4321, 1, 0666 | IPC_CREAT);
	sem_union.val = 0;
	semctl(N, 0, SETVAL, sem_union);
	
	E = semget((key_t) 1234, 1, 0666 | IPC_CREAT);
	sem_union.val = 100;
	semctl(E, 0, SETVAL, sem_union);


	/* prepare variables for counts and connection to shared memory */
	size_t count;
	int begin_index;
	int buffer_index = 0;
	
	struct buffer *producer_buffer;
	producer_buffer = (struct buffer *)shared_memory;
	

	/* send total bytes in the file the consumer */
	sem_w(S);	
	producer_buffer->buf[buffer_index].size = TotalBytes;
	sem_s(S);
	sem_s(N);
	sleep(1);

	/* read the bytes from the file in section of size BUFSIZ into the input_buffer, 
	divide this buffer into 128 byte parts and write these to the shared memory */
	while(BytesWritten < TotalBytes)	//while not at the end of the input file
	{	
		/* read from file into input buffer */
		count = fread(input_buffer, 1, BUFSIZ + BytesWritten, in);

					//printf("Read %d bytes from file: %s\n", count, input_buffer); 
		begin_index = 0;
		int i = count / 128;	//two indexes to send the count of bytes per buffer
		int f = i;
		//printf("i = %d", i);
		/* Divide the data in the input buffer into 128 byte parts and write to the shared memory */
		while(begin_index < count)	
		{	
			buffer_index ++;

			sem_w(E);	//wait on semaphore E: wait for buffer not full
			sem_w(S);	//wait on semaphore S: wait for shared memory not in use

			//"read" from input buffer into shared memory buffer
			strncpy(producer_buffer->buf[buffer_index].data, input_buffer + begin_index, 128);
			
			//put count of the number of bytes written in shared buffer into the buffer
			if(i == 0) {
				//printf("Bytes sent = %d\n", count-(128*f));
				producer_buffer->buf[buffer_index].size = count-(128*f);
			}
			else {
				//printf("Bytes sent = %d\n", 128);
				producer_buffer->buf[buffer_index].size = 128;
				i--;
			}
				//printf("Written to buffer[%d]: %s\n", buffer_index, producer_buffer->buf[buffer_index].data);
			sem_s(S);	//signal semaphore S: shared memory now available
			sem_s(N);	//signal semaphore N: additional full buffer available in shared memory
			begin_index += 128;		
		}

		BytesWritten += count;
	}
	//once reached EOF, print and then close file 
		printf("Reached end of file. %d bytes written to shared memory.\n", BytesWritten);
	fclose(in);
}

