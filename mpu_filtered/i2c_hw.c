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

#define M_PI 3.14159265358979323846
#define RAD_TO_DEG 57.29578

#define G_GAIN 0.07
#define DT 0.02
#define AA 0.98
int openDevice (const int devId);
int read_word_2c(int addr);
int wiringPiI2CReadReg8 (int fd, int reg);
int wiringPiI2CWriteReg8 (int fd, int reg, int value);
int mymillis();
double dist(double a, double b);
double get_y_rotation(double x, double y, double z);
double get_x_rotation(double x, double y, double z);
void readAccelerometer(int *accArr);
void readGyro(int *gyrArr);
void calibGyro(int *gyrDef);

int fd;
int acclX, acclY, acclZ;
int gyroX, gyroY, gyroZ;
double acclX_scaled, acclY_scaled, acclZ_scaled;
double gyroX_scaled, gyroY_scaled, gyroZ_scaled;

int main(void) {
	fd = openDevice (0x68);
	wiringPiI2CWriteReg8 (fd,0x6B,0x00);//disable sleep mode 
	printf("set 0x6B=%X\n",wiringPiI2CReadReg8 (fd,0x6B));
	
	int gyroConf = read_word_2c(27);
	gyroConf |= 0b00011000;
	wiringPiI2CWriteReg8(fd, 27, gyroConf);
	
	int accelArr[3];
	int gyroArr[3];
	int gyroCalib[3];

	float gyroXangle = 0.0;
	float gyroYangle = 0.0;
	float gyroZangle = 0.0;
	float AccYangle = 0.0;
	float AccXangle = 0.0;
	float CFangleX = 0.0;
	float CFangleY = 0.0;

	int startInt  = mymillis();
	struct  timeval tvBegin, tvEnd,tvDiff;

	signed int acc_y = 0;
	signed int acc_x = 0;
	signed int acc_z = 0;
	signed int gyr_x = 0;
	signed int gyr_y = 0;
	signed int gyr_z = 0;
	while(1) {

		readAccelerometer(accelArr);
		readGyro(gyroArr);
		//Convert Gyro raw to degrees per second
		float rate_gyr_x = (float) gyroArr[0] * G_GAIN;
		float rate_gyr_y = (float) gyroArr[1] * G_GAIN;
		float rate_gyr_z = (float) gyroArr[2] * G_GAIN;

		gyroXangle+=rate_gyr_x*DT;
		gyroYangle+=rate_gyr_y*DT;
		gyroZangle+=rate_gyr_z*DT;
		
		AccXangle = (float) (atan2(accelArr[1],accelArr[2])+M_PI)*RAD_TO_DEG;
		AccYangle = (float) (atan2(accelArr[2],accelArr[0])+M_PI)*RAD_TO_DEG;
		//Change the rotation value of the accelerometer to -/+ 180
		if (AccXangle > 180) AccXangle -= (float)360.0;
        if (AccYangle > 180) AccYangle -= (float)360.0;

        CFangleX = AA*(CFangleX+rate_gyr_x*DT) +(1 - AA) * AccXangle;
		CFangleY = AA*(CFangleY+rate_gyr_y*DT) +(1 - AA) * AccYangle;

		// acclX_scaled = acclX / 16384.0;
		// acclY_scaled = acclY / 16384.0;
		// acclZ_scaled = acclZ / 16384.0;

		// printf("My acclX_scaled: %f\n", acclX_scaled);
		// printf("My acclY_scaled: %f\n", acclY_scaled);
		// printf("My acclZ_scaled: %f\n", acclZ_scaled);
		// printf("My X rotation: %f\n", get_x_rotation(acclX_scaled, acclY_scaled, acclZ_scaled));
		// printf("My Y rotation: %f\n", get_y_rotation(acclX_scaled, acclY_scaled, acclZ_scaled));
		// printf("ping\n");
		printf("X %f Y %f\n", CFangleX, CFangleY);
        while(mymillis() - startInt < 20) {
            usleep(100);
        }

		// printf("Loop Time %d\t\n", mymillis()- startInt);

		// usleep(DT * 1000 * 1000);
	}

	// yes, unreachable
	close(fd);
	return 0;

}

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
 * openDevice:
 *	Open the I2C device, and regsiter the target device
 *********************************************************************************
 */

int openDevice (const int devId) {
	int rev ;
	const char *device ;

	device = "/dev/i2c-1" ;
  // ("/dev/i2c-1", 0x68) int fd ;

	if ((fd = open (device, O_RDWR)) < 0)
		return -1; //wiringPiFailure (WPI_ALMOST, "Unable to open I2C device: %s\n", strerror (errno)) ;

	if (ioctl (fd, I2C_SLAVE, devId) < 0)
		return -1; //wiringPiFailure (WPI_ALMOST, "Unable to select I2C device: %s\n", strerror (errno)) ;

	return fd ;
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

void readAccelerometer(int *accArr) {
	accArr[0] = read_word_2c(0x3B);
	accArr[1] = read_word_2c(0x3D);
	accArr[2] = read_word_2c(0x3F);
	// return accArr;
}

void readGyro(int *gyrArr) {
	gyrArr[0] = read_word_2c(0x3B);
	gyrArr[1] = read_word_2c(0x3D);
	gyrArr[2] = read_word_2c(0x3F);
	// return gyrArr;
}