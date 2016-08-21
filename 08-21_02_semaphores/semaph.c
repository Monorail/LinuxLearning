// simple semaphore example. thread 1 sets sharedCounter to 10
// then thread 2 triples it and the result is checked.

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void myfunc1( void *ptr );
void myfunc2( void *ptr );
/*
int pthread_create(
						pthread_t *thread, 					// pthread to execute on
						const pthread_attr_t *attr, 		// a pthread_attr_t structure whose contents are used at thread creation time to determine attributes for the new thread
						void *(*start_routine) (void *), 	// function to execute
						void *arg							// anonymous functions of passed functions
					);
*/
char buf[24];
sem_t mutex;
int sharedCounter = 0;
int main() {
	pthread_t thread1;
	pthread_t thread2;

	char *msg1 = "Thread 1";
	char *msg2 = "Thread 2";

	/* int sem_init(
						sem_t *sem, 		// 
						int pshared, 		// 0:  means that the semaphore is shared between threads and should be visible to all of them
											// !0: the semaphore is shared between processes and must be stored in shared memory
						unsigned int value	// 
					);
	*/ 
	sem_init(&mutex, 0, 0); // make a thread shared semaphore with 1 in pool

	pthread_create (&thread1, NULL, (void *) &myfunc1, (void *) msg1);
	pthread_create (&thread2, NULL, (void *) &myfunc2, (void *) msg2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	sem_destroy(&mutex);

	if(sharedCounter == 30){
		printf("operations successful!\n");
	}

	return 0;
}

void myfunc1(void *ptr){
	char *msg = (char *) ptr;
	
	printf("Thread 1 at gate\n");
	
	// sem_wait(&mutex);

	printf("Thread 1 past gate\n");
	
	sharedCounter = 10;

	sem_post(&mutex); // release semaphore
	
	pthread_exit(0); // terminates calling thread
}

void myfunc2(void *ptr){
	char *msg = (char *) ptr;
	
	printf("Thread 2 at gate\n");	
	
	sem_wait(&mutex);

	printf("Thread 2 past gate\n");
	
	sharedCounter *= 3;

	sem_post(&mutex);
	
	pthread_exit(0); // terminates calling thread
}