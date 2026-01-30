export ARCH=arm64
export CROSS_COMPILE = aarch64-none-linux-gnu-
obj-m += rfid-rc522-driver.o
KDIR := /home/topeet/Linux/rk3568_linux_5.10/kernel
PWD ?= $(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean