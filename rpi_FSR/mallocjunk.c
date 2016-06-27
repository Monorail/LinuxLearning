#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
/// Size of memory page on RPi
#define BCM2835_PAGE_SIZE               (4*1024)
/// Size of memory block on RPi
#define BCM2835_BLOCK_SIZE              (4*1024)

static 	uint8_t *gpioMem = NULL;

int main(void){
	// printf("%u\n", malloc(0));
	// printf("%u\n", malloc(0));
	// printf("%u\n", malloc(0));
	// printf("%u\n", malloc(0));
	printf("\n");
	// printf("%u\n", malloc(0));
	gpioMem = malloc(BCM2835_BLOCK_SIZE + (BCM2835_PAGE_SIZE - 1));
	uint8_t *empty = malloc(0);
	printf("gpiostart %u\n", gpioMem);
	printf("gpioend %u\n", empty);
	printf("size: %u\n", empty - gpioMem);
	// printf("%u\n", ((uint32_t)gpioMem % BCM2835_PAGE_SIZE));

	// uint8_t *mapaddr = gpioMem;
	// mapaddr += BCM2835_PAGE_SIZE - ((uint32_t)mapaddr % BCM2835_PAGE_SIZE) ;
	// printf("%u\n", ((uint32_t)mapaddr));
	
	return 0;
}