#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

struct usbkbd {
	struct input_dev * input_dev;
	struct urb * urb;
	int size;
	unsigned char * buf;
	unsigned char old_buf[8];
	dma_addr_t dma;
}

/* 键盘的键码表，包含标准键盘的按键映射 */
static const unsigned char usb_kbd_keycode[256] = {
    /* 0x00 */   0,   0,   0,   0,  30,  48,  46,  32,  18,  33,  34,  35,  23,  36,  37,  38,
    /* 0x10 */  50,  49,  24,  25,  16,  19,  31,  20,  22,  47,  17,  45,  21,  44,   2,   3,
    /* 0x20 */   4,   5,   6,   7,   8,   9,  10,  11,  28,   1,  14,  15,  57,  12,  13,  26,
    /* 0x30 */  27,  43,  43,  39,  40,  41,  51,  52,  53,  58,  59,  60,  61,  62,  63,  64,
    /* 0x40 */  65,  66,  67,  68,  87,  88,  99,  70, 119, 110, 102, 104, 111, 107, 109, 106,
    /* 0x50 */ 105, 108, 103,  69,  98,  55,  74,  78,  96,  79,  80,  81,  75,  76,  77,  71,
    /* 0x60 */  72,  73,  82,  83,  86, 127, 116, 117, 183, 184, 185, 186, 187, 188, 189, 190,
    /* 0x70 */ 191, 192, 193, 194, 134, 138, 130, 132, 128, 129, 131, 137, 133, 135, 136, 113,
    /* 0x80 */ 115, 114,   0,   0,   0, 121,   0,  89,  93, 124,  92,  94,  95,   0,   0,   0,
    /* 0x90 */ 122, 123,  90,  91,  85,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 0xA0 */   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 0xB0 */   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 0xC0 */   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 0xD0 */   0,   0,   0,   0,   0,   0,   0,   0,   0,  29,  42,  56, 125,  97,  54, 100,
    /* 0xE0 */ 126, 164, 166, 165, 163, 161, 115, 114, 113, 150, 158, 159, 128, 136, 177, 178,
    /* 0xF0 */ 176, 142, 152, 173, 140
};


void usbkbd_complete_fn(struct urb * urb)
{
	struct usbkbd * usbkbd = urb->context;

	/* 处理功能键（F1~F8等）：字节0的每位bit代表一个功能键状态 */
	for (int i = 0; i < 8; i++)
		input_report_key(usbkbd->input_dev, 
							usb_kbd_keycode[i + 224], // 功能键键码偏移 224
							(usbkbd->buf[0] >> i) & 1); // 按bit解析按键状态

	/* 处理普通按键（字节2~7为按键扫描值） */
	for (int i = 0; i < 8; i++) {
		/* 旧按键释放检测：旧数据存在且未出现在新数据中 */
		if (usbkbd->old_buf[i] > 3 && memscan(usbkbd->buf + 2, usbkbd->old_buf[i], 6) == usbkbd->buf + 8) {
			if (usb_kbd_keycode[usbkbd->old_buf[i]])
				input_report_key(usbkbd->input_dev, usb_kbd_keycode[usbkbd->old_buf[i], 0); // 0 代表按键释放
		}

		/* 新按键按下检测：新数据存在且未出现在旧数据中 */
		if (usbkbd->buf[i] > 3 && memscan(usbkbd->old_buf + 2, usbkbd->buf[i], 6) == usbkbd->old_buf + 8) {
			if (usb_kbd_keycode[usbkbd->buf[i]])
				input_report_key(usbkbd->input_dev, usb_kbd_keycode[usbkbd->buf[i], 1); // 1 代表按键按下
		}
	}

	/* 提交所有按键事键 */
	input_sync(usbkbd->input_dev);

	/* 保存当前状态供下次比对 */
	memcpy(usbkbd->old_buf, usbkbd->buf, 8);

	/* 重新提交urb继续监听 */
	usb_submit_urb(usbkbd->urb, GFP_KERNEL);
}


int usbkbd_probe(struct usb_interface *intf, 
						const struct usb_device_id *id) 
{
	printk("Enter usbkbd_probe() sucessfully.\n");

	int ret;
	int pipe;
	struct usbkbd * usbkbd;
	struct usb_host_interface * usbkbd_interface;
	struct usb_endpoint_descriptor * usbkbd_endpoint;
	struct usb_device * usbkbd_dev;

	/* 分配 usbkbd */
	usbkbd = kzalloc(sizeof(struct usbkbd), GFP_KERNEL);
	if (usbkbd == NULL) {
		printk("Fail to alloc usbkbd.\n");
		return -1;
	}

	/* 为输入设备分配内存 */
	usbkbd->input_dev = input_allocate_device();
	if (usbkbd->input_dev == NULL) {
		printk("Fail to allocate input device.\n");
		goto fail_to_alloc_input_dev;
	}

	/* 设置输入设备 */
	usbkbd->input_dev->name = "usbkbd_input_dev"; // 设置名称
	__set_bit(EV_KEY, usbkbd->input_dev->evbit); // 按键事件
	__set_bit(EV_REP, usbkbd->input_dev->evbit); // 重复事件
	for (int i = 0; i < 255; i++)
		__set_bit(usb_kbd_keycode[i], usbkbd->input_dev->keybit); // 设置按键位图
	_set_bit(0, usbkbd->input_dev->keybit); // 清除无效的按键位

	/* 注册输入设备 */
	ret = input_register_device(usbkbd->input_dev);
	if (ret) {
		printk("Fail to register input device.\n");
		goto fail_to_register_inputdev;
	}

	/* 分配 urb */
	usbkbd->urb = usb_alloc_urb(0, GFP_KERNEL);
	if (usbkbd->urb == NULL) {
		printk("Fail to alloc urb.\n");
		goto fail_to_alloc_urb;
	}

	/* 获得 USB 设备指针 */
	usbkbd_dev = interface_to_usbdev(intf);
	if (usbkbd_dev == NULL) {
		printk("Fail to get usb device.\n");
		goto fail_to_get_usbdev;
	}

	/* 获得端点描述符并获得端点最大数据包大小 */
	usbkbd_interface = intf->act_altsetting;
	usbkbd_endpoint = usbkbd_interface->endpoint[0].desc;
	usbkbd->size = usbkbd_endpoint->wMaxPacketSize;

	/* 分配一致性内存缓冲区 */
	usbkbd->buf = usb_alloc_coherent(usbkbd_dev, usbkbd->size, GFP_ATOMIC, &usbkbd->dma);
	if (usbkbd->buf == NULL) {
		printk("Fail to alloc usb coherent.\n");
		goto fail_to_alloc_usb_coherent;
	}

	/* 获取接收中断管道 */
	pipe = usb_rcvintpipe(usbkbd_dev, usbkbd_endpoint->bEndpointAddress);

	/* 初始化URB为中断URB */
	usb_fill_int_urb(usbkbd->urb, 
					usbkbd_dev, // 指定这个 URB 是发给哪个物理 USB 设备
					pipe, // 告诉内核数据要在哪条“管道”流动
					usbkbd->buf, // 当键盘有按键数据时，数据会被写入这里
					usbkbd->size, // 告诉 USB 核心最多可以读取多少数据，防止缓冲区溢出
					usbkbd_complete_fn, 
					usbkbd, // 给回调函数使用
					usbkbd_endpoint->bInterval); // 轮询间隔

	/* 配置URB相关的DMA设置 */
	usbkbd->urb->transfer_dma = usbkbd->dma; // 设置 URB 的 DMA 地址
	usbkbd->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP; // 设置 URB 的标志，不使用 DMA 映射
	
	/* 提交URB进行数据传输 */
	ret = usb_submit_urb(usbkbd->urb, GFP_KERNEL);
	if (ret) {
		printk("Fail to submit usb urb.\n");
		goto fail_to_submit_urb;
	}

	/* 设置device私有数据 */
	dev_set_drvdata(&intf->dev, usbkbd);

	return 0;

fail_to_alloc_input_dev:
	kfree(usbkbd);
	return -1;

fail_to_register_inputdev:
	input_free_device(usbkbd->input_dev);
	kfree(usbkbd);
	return ret;

fail_to_alloc_urb:
	input_unregister_device(usbkbd->input_dev);
	input_free_device(usbkbd->input_dev);
	kfree(usbkbd);
	return -1;

fail_to_get_usbdev:
	usb_free_urb(usbkbd->urb);
	input_unregister_device(usbkbd->input_dev);
	input_free_device(usbkbd->input_dev);
	kfree(usbkbd);
	return -1;

fail_to_alloc_usb_coherent:
	usb_free_urb(usbkbd->urb);
	input_unregister_device(usbkbd->input_dev);
	input_free_device(usbkbd->input_dev);
	kfree(usbkbd);
	return -1;

fail_to_submit_urb:
	usb_free_urb(usbkbd->urb);
	usb_free_coherent(usbkbd_dev, usbkbd->size, usbkbd->buf, &usbkbd->dma);
	input_unregister_device(usbkbd->input_dev);
	input_free_device(usbkbd->input_dev);
	kfree(usbkbd);
	return ret;
}

/* 断开函数：设备移除或驱动卸载时调用 */
void usbkbd_disconnect(struct usb_interface *intf)
{
	struct usb_device * usbkbd_dev;
	struct usbkbd * usbkbd; 

	usbkbd = dev_get_drvdata(&intf->dev);
	usbkbd_dev = interface_to_usbdev(intf);
	usb_kill_urb(usbkbd->urb);
	usb_free_urb(usbkbd->urb);
	usb_free_coherent(usbkbd_dev, usbkbd->size, usbkbd->buf, &usbkbd->dma);
	input_unregister_device(usbkbd->input_dev);
	input_free_device(usbkbd->input_dev);
	kfree(usbkbd);
}

const struct usb_device_id usbkbd_id_table = {
	{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, // 人机交互设备
						USB_INTERFACE_SUBCLASS_BOOT, // 启动子类
						USB_INTERFACE_PROTOCOL_KEYBOARD))}, // 键盘协议
	{}	
};

struct usb_driver usbkbd_driver = {
	.name = "L1angGM-usbkbd",
	.probe = usbkbd_probe,
	.disconnect = usbkbd_disconnect,
	.id_table = usbkbd_id_table,
};

static int __init usbkbd_init(void)
{
	int ret;

	/* 注册 USB 驱动到内核子系统  */
	ret = usb_register(&usbkbd_driver);
	if (!ret) {
		printk("Fail to register usbkbd driver.\n");
		return ret;
	}

	printk("Succeed to register usbkbd driver.\n");
	return 0;
}

static void usbkbd_exit(void)
{
	/* 注销 USB 驱动 */
	usb_deregister(&usbkbd_driver);
	printk("usbkbd driver deregistered successfully.\n");
}

module_init(usbkbd_init);
module_exit(usbkbd_exit);
MODULEAUTHOR("L1angGM");
MODULE_LICENSE("GPL");


