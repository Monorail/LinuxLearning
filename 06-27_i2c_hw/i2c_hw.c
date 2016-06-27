#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */
#include <linux/i2c.h>

int main(void){
	int file;
	int adapter_nr = 1;
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if(file < 0){
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	int addr = 0x40;

	if(ioctl(file, I2C_SLAVE, addr) < 0){
		/* something really baaaad happened */
		exit(1);
	}

	__u8 reg = 0x10;
	__s32 res;
	char buf[10];

	//  using SMBus commands 
	// res = i2c_smbus_read_word_data(file, reg);
	// if(res < 0)

	// buf[0] = reg;
	// buf[1] = 0x43;
	// buf[2] = 0x65;
	// if(write(file, buf, 3) != 3){

	// }
	// unsigned long *funcs;
	// ioctl(file, I2C_FUNC, funcs);
	// printf("%lu", *funcs);
	if (!i2c_check_functionality(file, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
		printf("wubbalubba dub dub");
	else
		printf("that's the waaaaaaaaaaay the news goes");
	return 0;

}