#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>

static void ft5x06_report_data(struct work_struct * work);
static int ft5x06_driver_probe(struct i2c_client * client, const struct i2c_device_id * id);
static int ft5x06_driver_remove(struct i2c_client * client);
static irqreturn_t ft5x06_driver_handler(int irq, void *args);
static int read_ft5x06_reg(u8 reg_addr);
static void write_ft5x06_reg(u8 reg_addr, u8 * data, u8 len);

static DECLARE_WORK(ft5x06_report_work, ft5x06_report_data);
static struct gpio_desc * reset_gpio, irq_gpio; // reset 和 interrupt 引脚的 GPIO 描述符
static struct i2c_client * ft5x06_client;
static struct input_dev * ft5x06_input_dev; // ft5x06触摸芯片的输入设备dev指针
static struct i2c_driver ft5x06_driver = {
	.driver = {
		.name = "my-ft5x06",
		.owner = THIS_MODULE,
	},
	.probe = ft5x06_driver_probe,
	.remove = ft5x06_driver_remove,
};

static int ft5x06_driver_probe(struct i2c_client * client, const struct i2c_device_id * id)
{
	int ret;
	int size_x, size_y;
	struct device_node * np = client->dev->of_node;
	ft5x06_client = client;

	/* 获取reset引脚的GPIO描述符 */
	reset_gpio = gpiod_get(&client.dev, "reset", 0);
	if (!reset_gpio) {
		printk("Cannot get reset gpio.");
		goto error_0;
	};

	irq_gpio = gpiod_get(&client.dev, "interrupt", 0);
	if (!irq_gpio) {
		printk("Cannot get interrupt gpio.");
		goto error_1;
	};

	/* 复位ft5x06：设置 reset GPIO 为输出,并拉低 5ms 后拉高 */
	gpiod_direction_output(reset_gpio, 0);
	msleep(5);
	gpiod_direction_output(reset_gpio, 1);

	/* 申请中断 */
	ret = request_irq(client.irq, 
						ft5x06_driver_handler,
						IRQ_TYPE_EDGE_FALLING | IRQF_ONESHOT, // 下降沿触发且为单次触发
						"ft5x06 IRQ", 
						NULL);
	if (ret < 0) {
		printk("Cannot request interrupt.");
		goto error_2;
	};

	/* 分配input_dev */
	ft5x06_input_dev = input_allocate_device();
	if (!ft5x06_input_dev) {
		printk("Cannot allocate input device.");
		goto error_3;
	};

	/* 设置分配input_dev */
	ft5x06_input_dev->name = "ft5x06_dev";
	set_bit(EV_KEY, ft5x06_input_dev->evbit); // 支持按键事件
	set_bit(BTN_TOUCH, ft5x06_input_dev->keybit); // 按键为触摸类型
	set_bit(EV_ABS, ft5x06_input_dev->evbit); // 支持绝对事件
	set_bit(ABS_X, ft5x06_input_dev->absbit);
	set_bit(ABS_Y, ft5x06_input_dev->absbit);

	/* 设置触摸绝对值范围 */
	ret = of_property_read_u32(np, "touchscreen-size-x", &size_x);
	if (ret < 0) {
		printk("Cannot get touchscreen-size-x from devicetree.");
		return -2;
	};
	ret = of_property_read_u32(np, "touchscreen-size-y", &size_y);
	if (ret < 0) {
		printk("Cannot get touchscreen-size-x from devicetree.");
		return -2;
	};
	input_set_abs_params(ft5x06_input_dev, ABS_X, 0, size_x, 0, 0);
	input_set_abs_params(ft5x06_input_dev, ABS_Y, 0, size_y, 0, 0);

	/* 注册input_dev设备 */
	ret = input_register_device(ft5x06_input_dev);
	if (ret < 0) {
		printk("Cannot register input device.");
		goto error_3;
	};

	return 0;

error_0:
	gpiod_put(reset_gpio);
error_1:
	gpiod_put(irq_gpio);
	gpiod_put(reset_gpio);
error_2:
	free_irq(client->irq, NULL);
	gpiod_put(irq_gpio);
	gpiod_put(reset_gpio);
error_3:
	input_free_device(ft5x06_input_dev);
	free_irq(client->irq, NULL);
	gpiod_put(irq_gpio);
	gpiod_put(reset_gpio);

	return ret;
}

static int ft5x06_driver_remove(struct i2c_client * client)
{
	input_free_device(ft5x06_input_dev);
	free_irq(client->irq, NULL);
	gpiod_put(irq_gpio);
	gpiod_put(reset_gpio);
}

static irqreturn_t ft5x06_driver_handler(int irq, void *args)
{
	schedule_work(&ft5x06_report_work);

	return IRQ_RETVAL(IRQ_HANDLED); // 表示中断已经被处理	
}

static void ft5x06_report_data(struct work_struct * work)
{

}

static int read_ft5x06_reg(u8 reg_addr)
{
	u8 data;
	int ret;

#if 1
	struct i2c_msg msgs[2] = {
		[0] = {
			.addr = ft5x06_client->addr; // 设置设备地址
			.flag = 0, // 写操作：写入寄存器地址
			.len = sizeof(reg_addr),
			.buf = &reg_addr,
		},
		[1] = {
			.addr = ft5x06_client->addr; // 设置设备地址
			.flag = 0, // 读操作：写入地址
			.len = sizeof(reg_addr),
			.buf = &data,
		},
	};

	ret = i2c_transfer(ft5x06_client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret != 1)
		return -EIO;
#else
	ret = i2c_master_send(ft5x06_client->adapter, &reg_addr, 1);
	if (ret != 1) {
		printk("Cannot write register address.");
		return -EIO;
	};
	i2c_master_recv(ft5x06_client->adapter, &data, 1);
	if (ret != 1) {
		printk("Cannot read data from ft5x06.");
		return -EIO;
	};
#endif

	return data;
}

static void write_ft5x06_reg(u8 reg_addr, u8 * data, u8 len)
{
#if 1
	u8 buf[256];

	struct i2c_msg msgs[] = {
		[0] = {
			.addr = ft5x06_client->addr, // 设置设备地址
			.flags = 0, // 写操作：写入要写入的寄存器地址
			.len = len + 1, // 数据长度 + 寄存器地址长度
			.buf = buf, // 数据缓冲区
		},
	};

	buf[0] = reg_addr;
	memcpy(&buf[1], data, len);

	ret = i2c_transfer(ft5x06_client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret != len + 1) {
		printk("Cannot read data from ft5x06.");
		return;
	};
#else
	ret = i2c_master_send(ft5x06_client->adapter, &reg_addr, 1);
	if (ret != 1) {
		printk("Cannot write register address.");
		return;
	};
	ret = i2c_master_send(ft5x06_client->adapter, data, len);
	if (ret != len) {
		printk("Cannot write data.");
		return;
	};
#endif
}

static int __init ft5x06_driver_init(void)
{
	int ret;

	/* 注册ft5x06 i2c驱动 */
	ret = i2c_add_driver(&ft5x06_driver);
	if (ret < 0) {
		printk("Ft5x06 driver cannot be added.");
		return ret;
	}

	return 0;
}

static void ft5x06_driver_exit(void)
{
	/* 删除ft5x06 i2c驱动 */
	i2c_del_driver(&ft5x06_driver);
}

module_init(ft5x06_driver_init);
module_exit(ft5x06_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("L1angGM");