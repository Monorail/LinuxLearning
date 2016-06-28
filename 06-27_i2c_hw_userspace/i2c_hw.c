#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), create() */
// #include <linux/i2c.h>

int main(void){
	int myI2cDevice;
	int adapter_nr = 1;
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	myI2cDevice = open(filename, O_RDWR);
	if(myI2cDevice < 0){
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("Failed to open device i2c-%d\n", adapter_nr);
		exit(1);
	}

	int addr = 68;

	if(ioctl(myI2cDevice, I2C_SLAVE, addr) < 0){
		/* something really baaaad happened */
		printf("something really baaaad happened\n");
		exit(1);
	}

	__u8 reg = 0;
	__s32 res;
	char buf[10];

	//  using SMBus commands 
	res = i2c_smbus_read_word_data(myI2cDevice, reg);
	// if(res < 0)

	// buf[0] = reg;
	// buf[1] = 0x43;
	// buf[2] = 0x65;
	// if(write(myI2cDevice, buf, 3) != 3){

	// }
	// unsigned long *funcs;
	// ioctl(myI2cDevice, I2C_FUNC, funcs);
	// printf("%lu", *funcs);
	// if (!i2c_check_functionality(myI2cDevice, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
	// 	printf("wubbalubba dub dub");
	// else
	// 	printf("that's the waaaaaaaaaaay the news goes");
	close(myI2cDevice);
	return 0;

}