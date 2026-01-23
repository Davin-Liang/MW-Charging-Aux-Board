# 1. 设置交叉编译环境
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

# 2. 定义最终生成的模块名称
# 不要直接写 virtual_video_driver.o，因为它会与你的源文件 virtual_video_driver.c 冲突
obj-m += vvd.o

# 3. 指定该模块依赖的所有 .o 文件列表
vvd-y := virtual_video_driver.o blue.o red.o green.o

# 4. 内核路径
KDIR := /home/topeet/Linux/rk356x_linux/kernel
PWD ?= $(shell pwd)

# 5. 编译规则
all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean