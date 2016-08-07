//guide used: http://ozzmaker.com/guide-to-interfacing-a-gyro-and-accelerometer-with-a-raspberry-pi/
// https://github.com/tcleg/Six_Axis_Complementary_Filter/tree/master/C
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), create() */
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>

#define I2C_SMBUS_I2C_BLOCK_MAX 32  /* Not specified but we use same structure */
#define I2C_SMBUS	0x0720

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2 
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

#define M_PI 3.14159265358979323846
#define RAD_TO_DEG 57.29578

#define G_GAIN 0.07
#define DT 0.02
#define AA 0.98

void openDevice (int *fd, const int devId);
int read_word_2c(int fd, int addr);
int wiringPiI2CReadReg8 (int fd, int reg);
int wiringPiI2CWriteReg8 (int fd, int reg, int value);
int mymillis();
double dist(double a, double b);
double get_y_rotation(double x, double y, double z);
double get_x_rotation(double x, double y, double z);
void readAccelerometer(int fd, int *accArr);
void readGyro(int fd, int *gyrArr);
void calibGyro(int *gyrDef);
int setup_mpu();

int mymillis() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}

// union i2c_smbus_data {
//   uint8_t  byte ;
//   uint16_t word ;
//   uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;  // block [0] is used for length + one more for PEC
// } ;

// struct i2c_smbus_ioctl_data {
//   char read_write ;
//   uint8_t command ;
//   int size ;
//   union i2c_smbus_data *data ;
// } ;
int wiringPiI2CReadReg8 (int fd, int reg)
{
  union i2c_smbus_data data;

  if (i2c_smbus_access (fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data))
    return -1 ;
  else
    return data.byte & 0xFF ;
}


int read_word_2c(int fd, int addr) {
	int val = wiringPiI2CReadReg8(fd, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(fd, addr+1);
	if (val >= 0x8000)
		val = -(65536 - val);
	return val;
}


/*
 * openDevice:
 *	Open the I2C device, and regsiter the target device
 *********************************************************************************
 */

void openDevice (int *fd ,const int devId) {
	int rev ;
	const char *device ;

	device = "/dev/i2c-1" ;
  // ("/dev/i2c-1", 0x68) int fd ;

	if ((*fd = open (device, O_RDWR)) < 0)
		return; //wiringPiFailure (WPI_ALMOST, "Unable to open I2C device: %s\n", strerror (errno)) ;

	if (ioctl (*fd, I2C_SLAVE, devId) < 0)
		return; //wiringPiFailure (WPI_ALMOST, "Unable to select I2C device: %s\n", strerror (errno)) ;
}



double dist(double a, double b) {
	return sqrt((a*a) + (b*b));
}

double get_y_rotation(double x, double y, double z) {
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}

double get_x_rotation(double x, double y, double z) {
	double radians;
	radians = atan2(y, dist(x, z));
	return (radians * (180.0 / M_PI));
}
int wiringPiI2CWriteReg8 (int fd, int reg, int value)
{
  union i2c_smbus_data data ;

  data.byte = value ;
  return i2c_smbus_access (fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data) ;
}

void readAccelerometer(int fd, int *accArr) {
	accArr[0] = read_word_2c(fd, 0x3B);
	accArr[1] = read_word_2c(fd, 0x3D);
	accArr[2] = read_word_2c(fd, 0x3F);
	// return accArr;
}

void readGyro(int fd, int *gyrArr) {
	gyrArr[0] = read_word_2c(fd, 0x3B);
	gyrArr[1] = read_word_2c(fd, 0x3D);
	gyrArr[2] = read_word_2c(fd, 0x3F);
	// return gyrArr;
}

int setup_mpu() {
	int mpu_file_desc;
	openDevice(&mpu_file_desc, 0x68);
	
	wiringPiI2CWriteReg8 (mpu_file_desc, 0x6B, 0x00);//disable sleep mode 
	
	int gyroConf = read_word_2c(mpu_file_desc, 27);
	gyroConf |= 0b00011000;
	wiringPiI2CWriteReg8(mpu_file_desc, 27, gyroConf);	
	
	return mpu_file_desc;
}