#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHSIZE 100

#define PORTALSTRING "Hello world\n"

int main(int argc, char *argv[]) {
	int shmid;
	key_t key;
	char *shm;
	char *s;

	key = 9876;

	shmid = shmget(key, SHSIZE, IPC_CREAT | 0666);

	if(shmid < 0){
		perror("shmget failed");
		exit(1);
	}

	shm = shmat(shmid, NULL, 0);

	if (shm == (char *) -1) {
		perror("shmat failed");
		exit(1);
	}
	int sizeofPORTALSTRING = sizeof(PORTALSTRING);
	memcpy(shm, PORTALSTRING, sizeofPORTALSTRING);

	// weird terminate condition
	while(*shm != '*')
		sleep(1);

	return 0;
} 