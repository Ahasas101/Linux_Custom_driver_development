obj-m := pcd_n.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERNEL_DIR=/home/ahasas/LDD1_Workspace/source/linux-6.6.20-ti-rt-arm32-r3
HOST_KERNEL_DIR=/lib/modules/6.17.0-19-generic/build/

all:
	 make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules

clean: 
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) clean

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) help

host:
	make -C $(HOST_KERNEL_DIR) M=$(PWD) modules

