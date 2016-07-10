#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), create() */
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
// #include <linux/i2c.h>

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
int fd;
int acclX, acclY, acclZ;
int gyroX, gyroY, gyroZ;
double acclX_scaled, acclY_scaled, acclZ_scaled;
double gyroX_scaled, gyroY_scaled, gyroZ_scaled;
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
int read_word_2c(int addr) {
	int val = wiringPiI2CReadReg8(fd, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(fd, addr+1);
	if (val >= 0x8000)
		val = -(65536 - val);
	return val;
}
//
// int read_word_2c(int device, __u8 reg){
// 	int val = i2c_smbus_read_word_data(device, reg);
// 	val = val << 8;
// 	val += i2c_smbus_read_word_data(device, reg + 1);
// 	if (val >= 0x8000)
// 		val = -(65536 - val);
// 	return val;
// }


/*
 * wiringPiI2CSetup:
 *	Open the I2C device, and regsiter the target device
 *********************************************************************************
 */

int wiringPiI2CSetup (const int devId)
{
  int rev ;
  const char *device ;

  rev = 0;

  if (rev == 1)
    device = "/dev/i2c-0" ;
  else
    device = "/dev/i2c-1" ;
  // ("/dev/i2c-1", 0x68) int fd ;

  if ((fd = open (device, O_RDWR)) < 0)
    return -1; //wiringPiFailure (WPI_ALMOST, "Unable to open I2C device: %s\n", strerror (errno)) ;

  if (ioctl (fd, I2C_SLAVE, devId) < 0)
    return -1; //wiringPiFailure (WPI_ALMOST, "Unable to select I2C device: %s\n", strerror (errno)) ;

  return fd ;
}



static inline int i2c_smbus_write ( int fd, int data) {
  struct i2c_smbus_ioctl_data args ;

  args.read_write = I2C_SMBUS_WRITE ;
  args.command    = data ;
  args.size       = I2C_SMBUS_BYTE ;
  args.data       = NULL ;
  ioctl (fd, I2C_SMBUS, &args) ;
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


int main(void) {
	fd = wiringPiI2CSetup (0x68);
	wiringPiI2CWriteReg8 (fd,0x6B,0x00);//disable sleep mode 
	printf("set 0x6B=%X\n",wiringPiI2CReadReg8 (fd,0x6B));

	while(1) {
		acclX = read_word_2c(0x3B);
		acclY = read_word_2c(0x3D);
		acclZ = read_word_2c(0x3F);
		acclX_scaled = acclX / 16384.0;
		acclY_scaled = acclY / 16384.0;
		acclZ_scaled = acclZ / 16384.0;

		printf("My acclX_scaled: %f\n", acclX_scaled);
		printf("My acclY_scaled: %f\n", acclY_scaled);
		printf("My acclZ_scaled: %f\n", acclZ_scaled);
		printf("My X rotation: %f\n", get_x_rotation(acclX_scaled, acclY_scaled, acclZ_scaled));
		printf("My Y rotation: %f\n", get_y_rotation(acclX_scaled, acclY_scaled, acclZ_scaled));
		printf("\n\n\n\n\n");
		sleep(1);
	}

	// yes, unreachable
	close(fd);
	return 0;

}