#include <linux/module.h>//模块加载卸载函数
#include <linux/kernel.h>//内核头文件
#include <linux/types.h>//数据类型定义
#include <linux/fs.h>//file_operations结构体
#include <linux/device.h>//class_create等函数
#include <linux/ioctl.h>
#include <linux/kernel.h>/*包含printk等操作函数*/
#include <linux/of.h>/*设备树操作相关的函数*/
#include <linux/gpio.h>/*gpio接口函数*/
#include <linux/of_gpio.h>
#include <linux/platform_device.h>/*platform device*/
#include <linux/spi/spi.h> /*spi相关api*/
#include <linux/delay.h> /*内核延时函数*/
#include <linux/slab.h> /*kmalloc、kfree函数*/
#include <linux/cdev.h>/*cdev_init cdev_add等函数*/
#include <linux/gpio.h>/*gpio接口函数*/
#include <linux/uaccess.h>/*__copy_from_user 接口函数*/

#define DEVICE_NAME "RFID-RC522"
#define ENABLE 1
#define DISABLE 0

struct rc522_device {
	struct device_node * of_node; // 设备树节点
	struct cdev cdev;        	 // 定义一个cdev结构体
	struct class * class;     	 // 创建一个rc522类
	struct device * device;   	 // 创建一个rc522设备 该设备是需要挂在rc522类下面的
	int major;               	 // 主设备号
	dev_t dev_id;
	struct spi_device *spi;  	 // spi设备
	struct gpio_desc * reset_gpio;
	struct mutex lock;
	void *private_data;
};

/**
  * @brief  设置 RFID-RC522 的复位引脚 
  * @param  value: 1 输出高电平 / 0 输出低电平
  * @return void
  **/
void control_rfid_rc522_reset_pin(struct rc522_device * rfid_rc522_dev, int value)
{
	gpiod_direction_output(rfid_rc522_dev->reset_gpio, value);
}

/**
  * @brief 写RFID-RC522 一定数量的寄存器
  * @param dev: rc522_device结构体
  * @param reg: 要写入的寄存器的地址
  * @param dat: 要写入的值的指针
  * @param len: 要写入的寄存器的数量
  * @return 0 写入成功 / 负数 写入失败
  **/
static int rfid_rc522_write_regs(struct rc522_device * rfid_rc522_dev, unsigned char reg, 
											unsigned char *dat, unsigned char len)
{
    int ret;
    unsigned char * buf;

	if (!rfid_rc522_dev || !rfid_rc522_dev->spi)
		return -ENODEV;
	
    struct spi_device * spi = rfid_rc522_dev->spi;
    
    buf = (unsigned char *)kzalloc(1 + len, GFP_KERNEL);
    if (!buf) 
		return -ENOMEM;

	/* 填充要发送的数据 */
    // 一共发送len+1个字节的数据，第一个字节为寄存器首地址，len为要写入的寄存器的集合
    *buf = (reg << 1) & 0x7e; // 写数据的时候首寄存器地址bit8要清零
    memcpy(buf + 1, dat, len);

	/* SPI 发送 */
	ret = spi_write(spi, buf, 1 + len);
    kfree(buf);

	return ret;
}

/**
  * @brief 读RFID-RC522 一定数量的寄存器
  * @param dev: rc522_device结构体
  * @param reg: 要读取的寄存器的地址
  * @param dat: 读取到的数据的存放地址
  * @param len: 要读取的寄存器的数量
  * @return 0 写入成功 / 负数 写入失败
  **/
static int rfid_rc522_read_regs(struct rc522_device *rfid_rc522_dev, unsigned char reg, 
										unsigned char *dat, unsigned char len)
{
	int ret = -1;
	unsigned char txbuf[1];
	unsigned char * rxbuf;

	if (!rfid_rc522_dev || !rfid_rc522_dev->spi)
		return -ENODEV;
	
	struct spi_device *spi = rfid_rc522_dev->spi;

	/* 申请缓冲区内存 */
	rxbuf = kzalloc(len + 1, GFP_KERNEL);
	if (!rxbuf)
		return -ENOMEM;
 
	txbuf[0] = ((reg << 1) & 0x7e) | 0x80; // 或上 0x80 代表读操作
	
	/* 执行SPI传输：先发送1字节命令，再接收len+1字节数据 */
	ret = spi_write_then_read(spi, txbuf, 1, rxbuf, 1 + len);
	if (ret < 0) {
		printk("SPI read failed: %d\n", ret);
		goto cleanup;
	}
	
	/* 跳过第一个字节（通常是垃圾数据或状态），复制有效数据 */
	memcpy(dat, rxbuf + 1, len);
	kfree(rxbuf);
	ret = 0; // 成功

	return 0;
	
cleanup:
	kfree(rxbuf);
	return ret;
}

/**
  * @brief 读RFID-RC522 的一个寄存器
  * @param dev: rc522_device结构体
  * @param reg: 要读取的寄存器的地址
  * @return 0 写入成功 / 负数 写入失败
  **/
static unsigned char rfid_rc522_read_one_reg(struct rc522_device *rfid_rc522_dev, unsigned char reg)
{
	int ret;
	unsigned char data = 0;
	
	ret = rfid_rc522_read_regs(rfid_rc522_dev, reg, &data, 1);
	if (!ret)
		return data;
	return ret;
}

/**
  * @brief 写RFID-RC522 的一个寄存器
  * @param dev: rc522_device结构体
  * @param reg: 要写入的寄存器的地址
  * @param value: 要写入的值
  * @return 0 写入成功 / 负数 写入失败
  **/
static int rfid_rc522_write_one_reg(struct rc522_device *rfid_rc522_dev,unsigned char reg, unsigned char value)
{
	return rfid_rc522_write_regs(rfid_rc522_dev, reg, &value, 1);
}


int rfid_rc522_open(struct inode * inode, struct file * filp)
{
	struct rc522_device * rfid_rc522_dev;

	/* 从 inode 找到包含它的私有结构体 */
    rfid_rc522_dev = container_of(inode->i_cdev, struct rc522_device, cdev);

    /* 保存到 file 私有数据，后面 read/write 都能用 */
	filp->private_data = rfid_rc522_dev;

	/* 初始化RC522 */
    control_rfid_rc522_reset_pin(rfid_rc522_dev, DISABLE);
    udelay(10);
    control_rfid_rc522_reset_pin(rfid_rc522_dev, ENABLE);
    udelay(10);
    control_rfid_rc522_reset_pin(rfid_rc522_dev, DISABLE);

    printk("Open RFID-RC522 successfully!\n");
    return 0;
}

ssize_t rfid_rc522_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	unsigned char * w_buf;
	int ret;
	struct rc522_device * rfid_rc522_dev = filp->private_data;

	w_buf = (unsigned char *)kzalloc(count, GFP_KERNEL);
	if (!w_buf)
		return -ENOMEM;

	ret = copy_from_user(w_buf, buf, count);
	if (ret) {
		kfree(w_buf);
		printk("Fail to copy data from user.\n");
		return ret;
	}

	ret = rfid_rc522_write_one_reg(rfid_rc522_dev, w_buf[0], w_buf[1]);
	
	kfree(w_buf);
	return ret;
}

ssize_t rfid_rc522_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int ret;
	unsigned char reg, value;
	struct rc522_device * rfid_rc522_dev = filp->private_data;

	ret = copy_from_user(&reg, buf, 1);
	if (ret < 0) {
		printk("Fail to copy data from userspace.\n");
		return ret;
	}

	value = rfid_rc522_read_one_reg(rfid_rc522_dev, reg);
	if (value < 0) {
		printk("Fail to read data from RFID-RC522.\n");
		return value;
	}

	ret = copy_to_user(buf, &value, count);
	if (ret < 0) {
		printk("Fail to copy data to userspace.\n");
		return ret;
	}

	return ret;
}

int rfid_rc522_release(struct inode* inode ,struct file *filp)
{
	struct rc522_device * rfid_rc522_dev = filp->private_data;

	control_rfid_rc522_reset_pin(rfid_rc522_dev, ENABLE);
	printk("Release RFID-RC522 device successfully.\n");

	return 0;
}

static struct file_operations rfid_rc522_fops={
    .owner      = THIS_MODULE,
    .open       = rfid_rc522_open,
    .write      = rfid_rc522_write,
    .read       = rfid_rc522_read,
    .release    = rfid_rc522_release,
};

static const struct of_device_id rfid_rc522_of_match[] = {
	{.compatible = "L1angGM,RFID-RC522"},
	{}
};

static const struct spi_device_id rfid_rc522_id[] = {
    { "xxxx", 0 },
    {},
};

int rfid_rc522_probe(struct spi_device *spi)
{
	struct rc522_device * rfid_rc522_dev;
	int ret;

	printk("Enter rfid_rc522_probe().");

	/* 分配 rc522_device  */
	rfid_rc522_dev = kzalloc(sizeof(struct rc522_device), GFP_KERNEL);
	if (!rfid_rc522_dev)
		return -ENOMEM;

	/* 获取设备树节点 */
	rfid_rc522_dev->of_node = spi->dev.of_node;
	if (rfid_rc522_dev->of_node == NULL) {
		printk("Fail to get device tree node.\n");
		goto err_get_of_node;
	}

	/* 获取reset引脚的GPIO描述符 */
	rfid_rc522_dev->reset_gpio = gpiod_get(&spi->dev, "rst", 0);
	if (!rfid_rc522_dev->reset_gpio) {
		printk("Cannot get reset gpio.\n");
		goto err_get_pin;
	};
	
	gpiod_direction_output(rfid_rc522_dev->reset_gpio, 1);

	/* 分配设备号 */
	ret = alloc_chrdev_region(&rfid_rc522_dev->dev_id, 0, 1, DEVICE_NAME);
	if (ret) {
		printk("Fail to request device ID(Error: %d).\n", ret);
		goto err_alloc_region;
	}

	/* 初始化 cdev */
	cdev_init(&rfid_rc522_dev->cdev, &rfid_rc522_fops);

	/* 向内核中添加一个cdev */
	ret = cdev_add(&rfid_rc522_dev->cdev, rfid_rc522_dev->dev_id, 1);
	if (ret) {
		printk("Fail to add device to cdev(Error: %d).\n", ret);
		goto err_add_cdev;
	}
	
	/* 创建一个nfc_class类 */
	rfid_rc522_dev->class = class_create(THIS_MODULE, "nfc_class");
	if (rfid_rc522_dev->class == NULL) {
	   printk("Fail to create nfc class.\n");
	   goto err_create_class;
	}
	
	/* 在nfc_class类下创建一个NFC_class设备 */
	rfid_rc522_dev->device = device_create(rfid_rc522_dev->class, NULL, 
											rfid_rc522_dev->dev_id, 
											NULL, DEVICE_NAME);	
	if (rfid_rc522_dev->device == NULL) {
	   printk("Fail to create device in nfc class.\n");
	   goto err_create_device;
	}
		
	/* 获取与本驱动匹配的spi设备 */
	rfid_rc522_dev->spi = spi;

	/* 设置SPI私有数据 */
	spi_set_drvdata(spi, rfid_rc522_dev);
	
	ret = spi_setup(rfid_rc522_dev->spi);	
	if (ret) {
	   printk("Fail to setup spi.\n");
	   goto err_setup_spi;	
	}

	printk("The RFID-RC522 driver has been successfully installed.\n");
	return 0;

err_get_of_node:
	kfree(rfid_rc522_dev);
	ret = -EINVAL;
	return ret;

err_get_pin:
	kfree(rfid_rc522_dev);
	ret = -EINVAL;
	return ret;

err_alloc_region:
	gpiod_put(rfid_rc522_dev->reset_gpio);
	kfree(rfid_rc522_dev);
	return ret;

err_add_cdev:
	gpiod_put(rfid_rc522_dev->reset_gpio);
	kfree(rfid_rc522_dev);
	return ret;	

err_create_class:
	cdev_del(&rfid_rc522_dev->cdev);
	gpiod_put(rfid_rc522_dev->reset_gpio);
	kfree(rfid_rc522_dev);
	ret = -EINVAL;
	return ret;

err_create_device:
	class_destroy(rfid_rc522_dev->class);
	cdev_del(&rfid_rc522_dev->cdev);
	gpiod_put(rfid_rc522_dev->reset_gpio);
	kfree(rfid_rc522_dev);
	ret = -EINVAL;
	return ret;	

err_setup_spi:
	device_destroy(rfid_rc522_dev->class, rfid_rc522_dev->dev_id);
	class_destroy(rfid_rc522_dev->class);
	cdev_del(&rfid_rc522_dev->cdev);
	gpiod_put(rfid_rc522_dev->reset_gpio);
	kfree(rfid_rc522_dev);
	return ret;		
}

int rfid_rc522_remove(struct spi_device *spi)
{
	struct rc522_device * rfid_rc522_dev = spi_get_drvdata(spi);

	rfid_rc522_dev->spi = NULL;
	device_destroy(rfid_rc522_dev->class, rfid_rc522_dev->dev_id);
	class_destroy(rfid_rc522_dev->class);
	cdev_del(&rfid_rc522_dev->cdev);
	gpiod_put(rfid_rc522_dev->reset_gpio);
	kfree(rfid_rc522_dev);

	printk("The RFID-RC522 driver has been successfully uninstalled.\n");
	return 0;
}


static struct spi_driver rfid_rc522_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "RFID-RC522",
		.of_match_table = rfid_rc522_of_match,
	},
	.probe = rfid_rc522_probe,
	.remove = rfid_rc522_remove,
	.id_table = rfid_rc522_id,
};

static int __init rfid_rc522_driver_init(void)
{
	int ret;

	ret = spi_register_driver(&rfid_rc522_driver);
	if (ret)
		printk("Fail to egister spi driver unsucess(Error: %d).\n", ret);
	else
		printk("Register spi driver successfully.\n");

	return ret;
}

static void rfid_rc522_driver_exit(void)
{
	spi_unregister_driver(&rfid_rc522_driver);
}

module_init(rfid_rc522_driver_init);
module_exit(rfid_rc522_driver_exit);
MODULE_AUTHOR("L1angGM");
MODULE_LICENSE("GPL");

