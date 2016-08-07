#include "i2c_hw.h"

int fd;
int acclX, acclY, acclZ;
int gyroX, gyroY, gyroZ;
double acclX_scaled, acclY_scaled, acclZ_scaled;
double gyroX_scaled, gyroY_scaled, gyroZ_scaled;

int main(void) {
	fd = openDevice (&fd, 0x68);
	wiringPiI2CWriteReg8 (fd,0x6B,0x00);//disable sleep mode 
	printf("set 0x6B=%X\n",wiringPiI2CReadReg8 (fd,0x6B));
	
	int gyroConf = read_word_2c(fd, 27);
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