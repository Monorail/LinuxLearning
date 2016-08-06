#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "i2c_hw.h"

int main(int argc, char *argv[]){
	/* mpu setup */
	int accelArr[3];
	int gyroArr[3];
	int gyroCalib[3];

	float accAngles[2] = {0.0};
	float cfAngles[2] = {0.0};
	struct timeval tvBegin, tvEnd,tvDiff;

	int mpu_file_desc = setup_mpu();
	int startInt  = mymillis();
	/* end mpu setup */


	/* client setup */
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	float buff[1024];
	int i = 0;
	int ms = 0;
	/* end client setup */

	while(1) {

		readAccelerometer(mpu_file_desc, accelArr);
		readGyro(mpu_file_desc, gyroArr);
		//Convert Gyro raw to degrees per second
		float rate_gyr_x = (float) gyroArr[0] * G_GAIN;
		float rate_gyr_y = (float) gyroArr[1] * G_GAIN;
		float rate_gyr_z = (float) gyroArr[2] * G_GAIN;

		accAngles[0] = (float) (atan2(accelArr[1],accelArr[2])+M_PI)*RAD_TO_DEG;
		accAngles[1] = (float) (atan2(accelArr[2],accelArr[0])+M_PI)*RAD_TO_DEG;
		//Change the rotation value of the accelerometer to -/+ 180
		if(accAngles[0] > 180) accAngles[0] -= (float)360.0;
        if(accAngles[1] > 180) accAngles[1] -= (float)360.0;

        cfAngles[0] = AA*(cfAngles[0]+rate_gyr_x*DT) +(1 - AA) * accAngles[0];
		cfAngles[1] = AA*(cfAngles[1]+rate_gyr_y*DT) +(1 - AA) * accAngles[1];
		printf("X %f Y %f\n", cfAngles[0], cfAngles[1]);
		
		buff[0] = cfAngles[0];
		buff[1] = cfAngles[1];
		buff[2] = 0.0;



		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock < 0){
			perror("Couldnt create socket");
			close(sock);
			exit(1);
		}

		server.sin_family = AF_INET;

		hp = gethostbyname(argv[1]);
		if(hp == 0){
			perror("gethostbyname failed");
			close(sock);
			exit(1);
		}

		memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
		server.sin_port = htons(5000);

		if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
			perror("connect failed");
			close(sock);
			exit(1);
		}
		if(send(sock, buff, sizeof(buff), 0) < 0){
			perror("gethostbyname failed");
			close(sock);
			exit(1);
		}

		close(sock);

        while(mymillis() - startInt < 20) {
            usleep(100);
        }
	}


	return 0;
}