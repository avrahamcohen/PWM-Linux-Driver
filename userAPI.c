#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "brightnessAPI.h"

#define TRUE 1

int main (int argc, char* argv[])
{
	int fd;
	char buf;

	fd = open("/dev/brightnessAPI", O_RDWR);
	if (fd < 0)
	{
		perror("Cannot open screen module (userspace) \n");
		printf("Cannot open screen module (userspace) \n");
		return EXIT_FAILURE;
	}
	/*This are the values the brightness driver can handle  
	enum BRIGHTNESS_LEVEL {BRIGHTNESS_LEVEL_1=1 , BRIGHTNESS_LEVEL_2 , BRIGHTNESS_LEVEL_3 , BRIGHTNESS_LEVEL_4 , BRIGHTNESS_LEVEL_5};
	For Example : 
		int value = BRIGHTNESS_LEVEL_1;				
		write(fd,&value,1);
	*/
	buf = (char)BRIGHTNESS_LEVEL_1;
	write(fd,&buf,1);
	read(fd,&buf,1);

	return EXIT_SUCCESS;
}
