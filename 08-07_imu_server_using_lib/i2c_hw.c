// https://github.com/tcleg/Six_Axis_Complementary_Filter/tree/master/C
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include "six_axis_comp_filter.c"

#define I2C_SMBUS_I2C_BLOCK_MAX 32  /* Not specified but we use same structure */
#define I2C_SMBUS	0x0720

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

#define I2C_SMBUS_QUICK		    	0
#define I2C_SMBUS_BYTE		    	1
#define I2C_SMBUS_BYTE_DATA	    	2
#define I2C_SMBUS_WORD_DATA	    	3
#define I2C_SMBUS_PROC_CALL	    	4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7
#define I2C_SMBUS_I2C_BLOCK_DATA    8

#define DT 0.02
#define TAU 2.0

int fd;
int wiringPiI2CReadReg8 (int, int);
int read_word_2c(int addr);
int openDevice (const int devId);
int wiringPiI2CWriteReg8 (int fd, int reg, int value);
int MPUInit();
void configGyro();
void readAccelerometer(int *accArr);
void readGyro(int *gyrArr);


int main(void) {
	fd = MPUInit();
	configGyro();
	int accelArr[3];
	int gyroArr[3];
	struct SixAxis *sixaxisctrls;
	CompInit(sixaxisctrls, DT, TAU);
	while(1) {
		readAccelerometer(accelArr);
		readGyro(gyroArr);
	}

	// yes, unreachable
	close(fd);
	return 0;

}

int MPUInit() {
	int fd = openDevice (0x68);
	wiringPiI2CWriteReg8 (fd,0x6B,0x00);//disable sleep mode 
	printf("set 0x6B=%X\n",wiringPiI2CReadReg8 (fd,0x6B));
	return fd;
}

void configGyro(){
	
	int gyroConf = read_word_2c(27);
	gyroConf |= 0b00011000;
	wiringPiI2CWriteReg8(fd, 27, gyroConf);

}

int wiringPiI2CReadReg8 (int fd, int reg) {
	union i2c_smbus_data data;

	if (i2c_smbus_access (fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data))
		return -1 ;
	else
		return data.byte & 0xFF ;
}

int read_word_2c(int addr) {
	int val = wiringPiI2CReadReg8(fd, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(fd, addr+1);
	if (val >= 0x8000)
		val = -(65536 - val);
	return val;
}

int openDevice (const int devId) {
	int rev ;
	const char *device ;
	device = "/dev/i2c-1" ;

	if ((fd = open (device, O_RDWR)) < 0)
		return -1; //wiringPiFailure (WPI_ALMOST, "Unable to open I2C device: %s\n", strerror (errno)) ;

	if (ioctl (fd, I2C_SLAVE, devId) < 0)
		return -1; //wiringPiFailure (WPI_ALMOST, "Unable to select I2C device: %s\n", strerror (errno)) ;

	return fd ;
}

int wiringPiI2CWriteReg8 (int fd, int reg, int value) {
	union i2c_smbus_data data ;

	data.byte = value ;
	return i2c_smbus_access (fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data) ;
}

void readAccelerometer(int *accArr) {
	accArr[0] = read_word_2c(0x3B);
	accArr[1] = read_word_2c(0x3D);
	accArr[2] = read_word_2c(0x3F);
}

void readGyro(int *gyrArr) {
	gyrArr[0] = read_word_2c(0x3B);
	gyrArr[1] = read_word_2c(0x3D);
	gyrArr[2] = read_word_2c(0x3F);
}