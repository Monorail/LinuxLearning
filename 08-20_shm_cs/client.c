#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHSIZE 100

int main(int argc, char *argv[]) {
	int shmid;
	key_t key;
	char *shm;

	key = 9876;

	shmid = shmget(key, SHSIZE,  0666);

	if(shmid < 0){
		perror("shmget failed");
		exit(1);
	}

	shm = shmat(shmid, NULL, 0);

	printf("%s", shm);

	*shm = '*';
	printf("%s", shm);

	return 0;
} 