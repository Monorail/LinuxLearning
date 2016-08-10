#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>
#include "i2c_hw.h"
#include "six_axis_comp_filter.c"

int main(int argc, char *argv[]){
	/* mpu setup */
	int accelArr[3]  = {0.0};
	int gyroArr[3]   = {0.0};
	int gyroCalib[3] = {0.0};

	float accAngles[2] = {0.0};
	float cfAngles[2]  = {0.0};
	struct SixAxis* sixaxisctrls;
	sixaxisctrls = malloc(sizeof(struct SixAxis));
	struct timeval tvBegin, tvEnd,tvDiff;

	int mpu_file_desc = setup_mpu();
	int startInt  = mymillis();
	/* end mpu setup */

	/* start comp filter lib */
	CompInit(sixaxisctrls, DT, TAU);
	CompAccelUpdate(sixaxisctrls, accelArr[0], accelArr[1], accelArr[2]);
	CompGyroUpdate(sixaxisctrls, gyroArr[0], gyroArr[1], gyroArr[2]);
	CompStart(sixaxisctrls);
	/* end comp filter lib */

	/* client setup */
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	float buff[1024];
	int i = 0;
	int ms = 0;
	/* end client setup */

	while(1) {

		CompStart(sixaxisctrls);
		readAccelerometer(mpu_file_desc, accelArr);
		readGyro(mpu_file_desc, gyroArr);
		//Convert Gyro raw to degrees per second
		// float rate_gyr_x = (float) gyroArr[0] * G_GAIN;
		// float rate_gyr_y = (float) gyroArr[1] * G_GAIN;
		// float rate_gyr_z = (float) gyroArr[2] * G_GAIN;

		// accAngles[0] = (float) (atan2(accelArr[1],accelArr[2])+M_PI)*RAD_TO_DEG;
		// accAngles[1] = (float) (atan2(accelArr[2],accelArr[0])+M_PI)*RAD_TO_DEG;
		// //Change the rotation value of the accelerometer to -/+ 180
		// if(accAngles[0] > 180) accAngles[0] -= (float)360.0;
  //       if(accAngles[1] > 180) accAngles[1] -= (float)360.0;

  //       cfAngles[0] = AA*(cfAngles[0]+rate_gyr_x*DT) +(1 - AA) * accAngles[0];
		// cfAngles[1] = AA*(cfAngles[1]+rate_gyr_y*DT) +(1 - AA) * accAngles[1];
		
		//grab IMU junk
		CompAccelUpdate(sixaxisctrls, accelArr[0], accelArr[1], accelArr[2]);
		CompGyroUpdate(sixaxisctrls, gyroArr[0], gyroArr[1], gyroArr[2]);
		CompAnglesGet(sixaxisctrls, &cfAngles[0], &cfAngles[1]);
		

		// upright
		cfAngles[1] += 3*PI/2;
		cfAngles[1] = fmod(cfAngles[1], 2 * PI);
		// calc mouse screen pos
		float mouseX = 1920/2 + 1920/2*(cosf(cfAngles[1]))* cosf(cfAngles[0]);// * 1920/2 ;
		float mouseY = 1080/2 + 1080/2*(sinf(cfAngles[1]))* cosf(cfAngles[0]);// * 1080/2 ;
		printf("Mag %3.3f\tDeg %3.3f\tX %3.3f\tY %3.3f\n", sinf(cfAngles[0]), RAD_TO_DEG * cfAngles[1], mouseX, mouseY);
		// buff[0] = cfAngles[0];
		// buff[1] = cfAngles[1];
		// buff[2] = 0.0;



		// sock = socket(AF_INET, SOCK_STREAM, 0);
		// if(sock < 0){
		// 	perror("Couldnt create socket");
		// 	close(sock);
		// 	exit(1);
		// }

		// server.sin_family = AF_INET;

		// hp = gethostbyname(argv[1]);
		// if(hp == 0){
		// 	perror("gethostbyname failed");
		// 	close(sock);
		// 	exit(1);
		// }

		// memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
		// server.sin_port = htons(5000);

		// if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
		// 	perror("connect failed");
		// 	close(sock);
		// 	exit(1);
		// }
		// if(send(sock, buff, sizeof(buff), 0) < 0){
		// 	perror("gethostbyname failed");
		// 	close(sock);
		// 	exit(1);
		// }

		// close(sock);

        while(mymillis() - startInt < 20) {
            usleep(100);
        }
	}


	return 0;
}