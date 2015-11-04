all: producer consumer

pc.o: ph.h

clean:
	rm -f producer consumer
