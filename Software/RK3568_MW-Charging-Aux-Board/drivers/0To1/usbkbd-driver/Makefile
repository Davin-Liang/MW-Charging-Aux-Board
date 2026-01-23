export ARCH=arm64
export CROSS_COMPILE = aarch64-linux-gnu-
obj-m += usbkbd_driver.o
KDIR := /home/topeet/Linux/rk356x_linux/kernel
PWD ?= $(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean