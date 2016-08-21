//TODO: switch system to use semaphores with character to switch consumer producer to avoid hard limit on chars

#include <sched.h>	
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // sleep
#include <sys/types.h> // kill
#include <sys/stat.h>
#include <sys/mman.h>  // mmap, munmap
#include <signal.h>    // kill


#define MESSAGE_LENGTH 30
#define ERROR -1
#define SLEEP_TIME 1

#define TERMCHAR '-'
// declaration of data structure for the processes
// this object is mapped to memory containing data and the count of objects in the array

// p1: read from stdin and puts chars in an array until it's full
// p2: print out the whole array

typedef struct {
	char MSG[MESSAGE_LENGTH];
	int count;
} messageObject;

void producer(messageObject *state);
void consumer(messageObject *state);
void waitForChildren(pid_t*);
pid_t forkChild(void(*func)(messageObject *), messageObject* state);
messageObject* createMMAP(size_t size);
void deleteMMAP(void*);

int main() {
	// get address of memory mapped location
	messageObject* state = createMMAP(sizeof(messageObject));
	state->count = 0;

	//fork children
	pid_t childpids[2];
	childpids[0] = forkChild(producer, state);
	childpids[1] = forkChild(consumer, state);

	waitForChildren(childpids);

	deleteMMAP(state);
	exit(EXIT_SUCCESS);
}

// allocates memory for a messageObject with mmap
messageObject* createMMAP(size_t size){
	void* addr = 0;
	int protections = PROT_READ|PROT_WRITE;
	int flags = MAP_SHARED | MAP_ANONYMOUS;
	fclose(fopen("streamland", "w+"));
	int fd = open("streamland", O_WRONLY | O_APPEND);
	off_t offset = 0;

	if(fd == -1){
		perror("fd failed");
		exit(1);
	}

	// create memory map
	messageObject* state = mmap(addr, size, protections, flags, fd, offset); 

	if( (void *) ERROR == state ){ // if mmap returns -1
		perror("mmap error");
		exit(EXIT_FAILURE);
	}
	return state;
}

// mmap cleanup
void deleteMMAP(void* addr){
	// deletes memory map at given address
	if (ERROR == munmap(addr, sizeof(messageObject))){
		perror("munmap error");
		exit(EXIT_FAILURE);
	}
}

// passes a function producer or consumer and the state object for creation
pid_t forkChild(void (*function)(messageObject *), messageObject* state){
	pid_t childpid;
	switch( childpid = fork()){
		case ERROR:
			perror("fork error");
			exit(EXIT_FAILURE);
		case 0:
			//cast function pointer to function and call with state
			(*function)(state);
		default:
			printf("returning childpid");
			return childpid;
	}
}

void waitForChildren(pid_t* childpids){
	int status;
	//wait: waits on a process to change state
	while(ERROR < wait(&status)){ // parent waits on any child
		// WIFEXITED(stat_val): Evaluates to a non-zero value if status was returned for a child process that terminated normally.
		if(!WIFEXITED(status)){   // if the child terminated unsuccessfully, kill all children
			kill(childpids[0], SIGKILL);
			kill(childpids[1], SIGKILL);
			break;
		}
	}
}

// execution path for producer
void producer(messageObject* state){
	// reads and writes until state is full
	int pct = 0;
	while(1){
		// printf("\npct: %d\n", pct++);
		while(MESSAGE_LENGTH == state->count){
			// discard input after full buffer and then yield 
			// fgetc(stdin);
			sleep(1); //sched_yield();
		}

		// add next char to state count
		char c = fgetc(stdin);
		state->MSG[state->count] = c;
		state->count++;

		// terminate cond
		if ( c == TERMCHAR ) {
			state->count = MESSAGE_LENGTH;
			exit(EXIT_SUCCESS);
		}
	}

	return;
}

void consumer(messageObject* state){
	// print chars until array is empty
	int localCount = 0; // keep track of what's next to read
	int cct = 0;
	while(1){
		// printf("\ncct: %d\n", cct++);

		// allow producer to produce while buffer isnt full
		while(state->count < MESSAGE_LENGTH){
			sleep(1); //sched_yield();
		}

		char c = state->MSG[localCount];

		if ( c == TERMCHAR ) {
			exit(EXIT_SUCCESS);
		}

		putchar(c);
		localCount++;
		if(MESSAGE_LENGTH == localCount){
			localCount = 0;
			state->count = 0;
		}
	}
}