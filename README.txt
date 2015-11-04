README:

SYSC 4001 - Assignment 2
Maryn Marsland - 100854916

========================================
Files:
	pc.h
	consumer.c
	producer.c
	Makefile

-----------------------------------------
Setup:

1. Open xterm window and navigate to containing directory
2. Enter command 'make all'
3. Use command 'xterm&' to open additional xterm windows, one for each process

-----------------------------------------
Run Instructions:

Note: Run each of these commands in a separate xterm window

1. Run ./consumer <output file name> 
		(run without command line argument to use default "output.txt")

2. Run ./producer <input file name> 
		(run without command line argument to use defult "input.txt" -> **file must exist**)


Note: For one producer and one consumer can be succesfully run without Semaphore S



Note: If shmget fails, may need to remove previous shared memore by running "ipcrm -m <key>" with key 1234
