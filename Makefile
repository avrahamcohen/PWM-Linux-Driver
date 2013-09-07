CC=/home/linux4biz/Develop/arm-2009q1/bin/arm-none-linux-gnueabi-gcc
ARCH=arm
LIB="/home/linux4biz/Develop/arm-2009q1/arm-none-linux-gnueabi/libc/usr/include"
TARGET = module-driver
DEST = "/home/linux4biz/Develop/Raptor-Project/raptor-kernel/drivers/misc"
KDIR = "/home/linux4biz/Develop/Raptor-Project/raptor-kernel"

obj-m += brightnessAPI.o

default:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	
brightnessAPI : brightnessAPI.c
	$(CC) -g -Wall -o $@ $<

$(TARGET).o: $(OBJS)
	$(LD) $(LD_RFLAG) -r -o $@ $(OBJS)

clean:	
	rm -Rf  *.o  *.ko

