#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "RFID-RC522"
#define ENABLE 1
#define DISABLE 0

struct rc522_device {
    struct device_node * of_node;
    struct cdev cdev;
    struct class * class;
    struct device * device;
    int major;
    dev_t dev_id;
    struct spi_device *spi;
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
  * @brief 写RFID-RC522 一定数量的寄存器 (已修复：使用 spi_sync 和动态内存)
  **/
static int rfid_rc522_write_regs(struct rc522_device * rfid_rc522_dev, unsigned char reg, 
                                            unsigned char *dat, unsigned char len)
{
    int ret;
    unsigned char * buf;
    struct spi_device * spi;
    struct spi_message msg;
    struct spi_transfer xfer;

    if (!rfid_rc522_dev || !rfid_rc522_dev->spi)
        return -ENODEV;
    
    spi = rfid_rc522_dev->spi;
    
    buf = kzalloc(1 + len, GFP_KERNEL);
    if (!buf) 
        return -ENOMEM;

    /* 第一字节：寄存器地址，写操作 bit7=0，bit0=0 */
    buf[0] = (reg << 1) & 0x7E; 
    memcpy(buf + 1, dat, len);

    spi_message_init(&msg);
    memset(&xfer, 0, sizeof(xfer));
    xfer.tx_buf = buf;
    xfer.len = len + 1;
    spi_message_add_tail(&xfer, &msg);

    /* 同步发送，确保时序连续 */
    ret = spi_sync(spi, &msg);
    
    kfree(buf);
    return ret;
}

/**
  * @brief 读RFID-RC522 一定数量的寄存器 (已修复：彻底解决片选拉高导致全0x00的Bug)
  **/
static int rfid_rc522_read_regs(struct rc522_device *rfid_rc522_dev, unsigned char reg, 
                                        unsigned char *dat, unsigned char len)
{
    int ret = -1;
    unsigned char *txbuf;
    unsigned char *rxbuf;
    struct spi_device *spi;
    struct spi_message msg;
    struct spi_transfer xfer;

    if (!rfid_rc522_dev || !rfid_rc522_dev->spi)
        return -ENODEV;
    
    spi = rfid_rc522_dev->spi;

    /* 必须动态分配 tx 和 rx 缓冲区，满足 DMA 要求 */
    txbuf = kzalloc(len + 1, GFP_KERNEL);
    rxbuf = kzalloc(len + 1, GFP_KERNEL);
    if (!txbuf || !rxbuf) {
        kfree(txbuf);
        kfree(rxbuf);
        return -ENOMEM;
    }

    /* 读操作地址格式：bit7=1(读), bit6-1=地址, bit0=0 */
    txbuf[0] = ((reg << 1) & 0x7E) | 0x80; 

    spi_message_init(&msg);
    memset(&xfer, 0, sizeof(xfer));

    xfer.tx_buf = txbuf;
    xfer.rx_buf = rxbuf;
    xfer.len = len + 1; 

    spi_message_add_tail(&xfer, &msg);
    
    /* spi_sync 会保证在整个读写过程中，CS片选线始终保持拉低 */
    ret = spi_sync(spi, &msg);
    if (ret < 0) {
        printk(KERN_ERR "SPI read failed: %d\n", ret);
        goto cleanup;
    }
    
    /* 拷贝有效数据（跳过第 0 字节，因为发送地址时返回的是无用状态位） */
    memcpy(dat, rxbuf + 1, len);
    ret = 0; // 成功

cleanup:
    kfree(txbuf);
    kfree(rxbuf);
    return ret;
}

/**
  * @brief 读/写单寄存器封装
  **/
static unsigned char rfid_rc522_read_one_reg(struct rc522_device *rfid_rc522_dev, unsigned char reg)
{
    int ret;
    unsigned char data = 0;
    
    ret = rfid_rc522_read_regs(rfid_rc522_dev, reg, &data, 1);
    if (!ret)
        return data;
    return ret; // 如果底层失败，此时返回的是负数错误码
}

static int rfid_rc522_write_one_reg(struct rc522_device *rfid_rc522_dev,unsigned char reg, unsigned char value)
{
    return rfid_rc522_write_regs(rfid_rc522_dev, reg, &value, 1);
}


int rfid_rc522_open(struct inode * inode, struct file * filp)
{
    struct rc522_device * rfid_rc522_dev;

    rfid_rc522_dev = container_of(inode->i_cdev, struct rc522_device, cdev);
    filp->private_data = rfid_rc522_dev;

    /* 硬件复位流程 */
    control_rfid_rc522_reset_pin(rfid_rc522_dev, 0); // 强行拉低复位
    mdelay(50); // 等待 50ms 确保芯片完全放电
    control_rfid_rc522_reset_pin(rfid_rc522_dev, 1); // 拉高，唤醒芯片
    mdelay(50); // 晶振起振需要时间，等待 50ms
    
    // 绝对不能再拉低了！！！芯片现在必须保持唤醒状态！！！

    printk(KERN_INFO "Open RFID-RC522 successfully!\n");
    return 0;
}

ssize_t rfid_rc522_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned char w_buf[2]; // 使用栈内存，彻底告别内存泄漏
    int hw_ret;
    struct rc522_device * rfid_rc522_dev = filp->private_data;

    if (count != 2) {
        printk(KERN_ERR "RC522 write requires exactly 2 bytes.\n");
        return -EINVAL; 
    }

    if (copy_from_user(w_buf, buf, 2)) {
        printk(KERN_ERR "Fail to copy data from user.\n");
        return -EFAULT; 
    }

    hw_ret = rfid_rc522_write_one_reg(rfid_rc522_dev, w_buf[0], w_buf[1]);
    if (hw_ret < 0) {
        printk(KERN_ERR "Fail to write to RFID-RC522 hardware.\n");
        return -EIO;    
    }

    return 2; // 成功时必须返回实际写入字节数
}

ssize_t rfid_rc522_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned char reg;
    int hw_ret;
    unsigned char value;
    struct rc522_device * rfid_rc522_dev = filp->private_data;

    if (copy_from_user(&reg, buf, 1)) {
        printk(KERN_ERR "Fail to copy data from userspace.\n");
        return -EFAULT;
    }

    hw_ret = (int)rfid_rc522_read_one_reg(rfid_rc522_dev, reg);
    if (hw_ret < 0) {
        printk(KERN_ERR "Fail to read data from RFID-RC522.\n");
        return -EIO;
    }

    value = (unsigned char)hw_ret;

    if (copy_to_user(buf, &value, 1)) {
        printk(KERN_ERR "Fail to copy data to userspace.\n");
        return -EFAULT;
    }

    return 1; // 成功时必须返回实际读取的字节数
}

int rfid_rc522_release(struct inode* inode ,struct file *filp)
{
    struct rc522_device * rfid_rc522_dev = filp->private_data;

    control_rfid_rc522_reset_pin(rfid_rc522_dev, ENABLE);
    printk(KERN_INFO "Release RFID-RC522 device successfully.\n");

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

    printk(KERN_INFO "Enter rfid_rc522_probe().\n");

    rfid_rc522_dev = kzalloc(sizeof(struct rc522_device), GFP_KERNEL);
    if (!rfid_rc522_dev)
        return -ENOMEM;

    rfid_rc522_dev->of_node = spi->dev.of_node;
    if (rfid_rc522_dev->of_node == NULL) {
        printk(KERN_ERR "Fail to get device tree node.\n");
        goto err_get_of_node;
    }

    rfid_rc522_dev->reset_gpio = gpiod_get(&spi->dev, "rst", GPIOD_OUT_HIGH);
    if (IS_ERR(rfid_rc522_dev->reset_gpio)) {
        printk(KERN_ERR "Cannot get reset gpio.\n");
        ret = PTR_ERR(rfid_rc522_dev->reset_gpio);
        goto err_get_pin;
    };
    
    ret = alloc_chrdev_region(&rfid_rc522_dev->dev_id, 0, 1, DEVICE_NAME);
    if (ret) {
        printk(KERN_ERR "Fail to request device ID(Error: %d).\n", ret);
        goto err_alloc_region;
    }

    cdev_init(&rfid_rc522_dev->cdev, &rfid_rc522_fops);

    ret = cdev_add(&rfid_rc522_dev->cdev, rfid_rc522_dev->dev_id, 1);
    if (ret) {
        printk(KERN_ERR "Fail to add device to cdev(Error: %d).\n", ret);
        goto err_add_cdev;
    }
    
    rfid_rc522_dev->class = class_create(THIS_MODULE, "nfc_class");
    if (IS_ERR(rfid_rc522_dev->class)) {
       printk(KERN_ERR "Fail to create nfc class.\n");
       ret = PTR_ERR(rfid_rc522_dev->class);
       goto err_create_class;
    }
    
    rfid_rc522_dev->device = device_create(rfid_rc522_dev->class, NULL, 
                                            rfid_rc522_dev->dev_id, 
                                            NULL, DEVICE_NAME); 
    if (IS_ERR(rfid_rc522_dev->device)) {
       printk(KERN_ERR "Fail to create device in nfc class.\n");
       ret = PTR_ERR(rfid_rc522_dev->device);
       goto err_create_device;
    }
        
    rfid_rc522_dev->spi = spi;
    spi_set_drvdata(spi, rfid_rc522_dev);
    
    /* ======== 已修复：必须设置 SPI 通讯参数 ======== */
    spi->mode = SPI_MODE_0;         // CPOL=0, CPHA=0
    spi->max_speed_hz = 5000000;    // RC522最高支持10MHz，这里用5MHz保稳
    spi->bits_per_word = 8;         // 数据宽度8位
    /* ============================================== */

    ret = spi_setup(rfid_rc522_dev->spi);   
    if (ret) {
       printk(KERN_ERR "Fail to setup spi.\n");
       goto err_setup_spi;  
    }

    printk(KERN_INFO "The RFID-RC522 driver has been successfully installed.\n");
    return 0;

err_get_of_node:
    kfree(rfid_rc522_dev);
    return -EINVAL;

err_get_pin:
    kfree(rfid_rc522_dev);
    return ret;

err_alloc_region:
    gpiod_put(rfid_rc522_dev->reset_gpio);
    kfree(rfid_rc522_dev);
    return ret;

err_add_cdev:
    unregister_chrdev_region(rfid_rc522_dev->dev_id, 1);
    gpiod_put(rfid_rc522_dev->reset_gpio);
    kfree(rfid_rc522_dev);
    return ret; 

err_create_class:
    cdev_del(&rfid_rc522_dev->cdev);
    unregister_chrdev_region(rfid_rc522_dev->dev_id, 1);
    gpiod_put(rfid_rc522_dev->reset_gpio);
    kfree(rfid_rc522_dev);
    return ret;

err_create_device:
    class_destroy(rfid_rc522_dev->class);
    cdev_del(&rfid_rc522_dev->cdev);
    unregister_chrdev_region(rfid_rc522_dev->dev_id, 1);
    gpiod_put(rfid_rc522_dev->reset_gpio);
    kfree(rfid_rc522_dev);
    return ret; 

err_setup_spi:
    device_destroy(rfid_rc522_dev->class, rfid_rc522_dev->dev_id);
    class_destroy(rfid_rc522_dev->class);
    cdev_del(&rfid_rc522_dev->cdev);
    unregister_chrdev_region(rfid_rc522_dev->dev_id, 1);
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
    unregister_chrdev_region(rfid_rc522_dev->dev_id, 1);
    gpiod_put(rfid_rc522_dev->reset_gpio);
    kfree(rfid_rc522_dev);

    printk(KERN_INFO "The RFID-RC522 driver has been successfully uninstalled.\n");
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
        printk(KERN_ERR "Fail to register spi driver unsucess(Error: %d).\n", ret);
    else
        printk(KERN_INFO "Register spi driver successfully.\n");

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