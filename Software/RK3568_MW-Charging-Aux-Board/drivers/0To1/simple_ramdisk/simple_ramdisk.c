#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/vmalloc.h>
#include <linux/highmem.h>  // 【必须包含】提供 kmap_atomic 和 flush_dcache_page


/* 定义模拟的磁盘大小 16MB */
#define SIMPLE_RAMDISK_SIZE (16 * 1024 * 1024)
/* 定义模拟磁盘名称 */
#define SIMPLE_RAMDISK_NAME "simple_ramdisk"
/* 保存内核动态分配的主设备号 */
int simple_ramdisk_major = 0;


/* 定义一个结构体来代表模拟磁盘 */
struct simple_ramdisk_device {
	void* data;
	struct gendisk* disk;
	struct request_queue *queue;
};

static struct simple_ramdisk_device* simple_ramdisk_dev = NULL;


/**
 * @brief: 核心读写逻辑，当用户要在磁盘上读写数据时，内核最终会调用到这
 * @detail: 因为我们是内存模拟，所以所谓的“内存读写”，其实就是内存拷贝（memcpy）
 * @param: bio - 上层传递给块层的I/O请求
 */
	static blk_qc_t simple_ramdisk_submit_bio(struct bio *bio)
	{
		struct bio_vec bvec;
		struct bvec_iter iter;
		sector_t sector = bio->bi_iter.bi_sector;
		u8 *mem_addr = simple_ramdisk_dev->data + (sector * 512); 
	
		// 1. 越界检查防线：防止 mkfs 扫描时越界导致死机
		if ((sector * 512) + bio->bi_iter.bi_size > SIMPLE_RAMDISK_SIZE) {
			printk(KERN_ERR "simple_ramdisk: 读写越界拦截！\n");
			bio_io_error(bio); 
			return BLK_QC_T_NONE;
		}
	
		// 2. 忽略丢弃/刷新等控制命令（纯内存盘直接当做成功处理即可）
		if (bio_op(bio) == REQ_OP_DISCARD || bio_op(bio) == REQ_OP_FLUSH) {
			bio_endio(bio);
			return BLK_QC_T_NONE;
		}
	
		// 3. 核心搬砖逻辑
		bio_for_each_segment(bvec, bio, iter) {
			unsigned int len = bvec.bv_len;
			
			// 获取映射的基地址（用于正确解除映射）
			void *kmem = kmap_atomic(bvec.bv_page);
			// 加上偏移量才是我们真正要操作的数据起始点
			void *buffer = kmem + bvec.bv_offset;
	
			// 【巨坑修复】使用更精准的 op_is_write 宏判断操作方向
			if (op_is_write(bio_op(bio))) {
				// 用户写：把用户发来的 buffer 拷贝到我们的内存盘
				memcpy(mem_addr, buffer, len);
			} else {
				// 用户读：把我们的内存盘数据拷贝回 buffer 给用户
				memcpy(buffer, mem_addr, len);
				// 【核心灵魂】强制刷新 D-Cache！保证 ARM64 架构下用户态能看到刚读出的数据！
				flush_dcache_page(bvec.bv_page); 
			}
	
			// 必须使用最原始的 kmem 地址解除映射
			kunmap_atomic(kmem);
			mem_addr += len;
		}
	
		// 告诉内核：完美收工
		bio_endio(bio);
		return BLK_QC_T_NONE;
	}



struct block_device_operations simple_ramdisk_fops = {
	.owner = THIS_MODULE,
	.submit_bio = simple_ramdisk_submit_bio,
}; 

static int __init simple_ramdisk_init(void)
{
	int ret = 0;

	printk(KERN_INFO "simple_ramdisk: 正在初始化... ...\n");

	/* 申请主设备号 */
	simple_ramdisk_major = register_blkdev(0, SIMPLE_RAMDISK_NAME);
	if (simple_ramdisk_major < 0) {
		printk(KERN_ERR "simple_ramdisk: 无法注册块设备号\n");
		return simple_ramdisk_major;
	}

	/* 分配内存 */
	simple_ramdisk_dev = kzalloc(sizeof(struct simple_ramdisk_device), GFP_KERNEL);
	if (!simple_ramdisk_dev) return -ENOMEM;

	/* 核心：分配 16MB 的真实物理内存，这就是我们的磁盘本体              */
	simple_ramdisk_dev->data = vzalloc(SIMPLE_RAMDISK_SIZE);
	if (!simple_ramdisk_dev->data) {
		ret = - ENOMEM;
		goto err_alloc_data;
	}

	/* 分配请求队列 */
	simple_ramdisk_dev->queue = blk_alloc_queue(NUMA_NO_NODE);
	if (!simple_ramdisk_dev->queue) {
		ret = - ENOMEM;
		goto err_alloc_queue;
	}

	/* 申请gendisk(身份证)   */
	simple_ramdisk_dev->disk = alloc_disk(1);
	if (!simple_ramdisk_dev->disk) {
		ret = -ENOMEM;
		goto err_alloc_disk;
	}

	/* 填写gendisk的详细信息 */
	simple_ramdisk_dev->disk->major = simple_ramdisk_major; // 让内核动态分配一个主设备号
	simple_ramdisk_dev->disk->first_minor = 0; // 次设备号从0开始
	simple_ramdisk_dev->disk->fops = &simple_ramdisk_fops; // 绑定操作数据集
	simple_ramdisk_dev->disk->queue = simple_ramdisk_dev->queue;
	simple_ramdisk_dev->disk->private_data = simple_ramdisk_dev;
	sprintf(simple_ramdisk_dev->disk->disk_name, SIMPLE_RAMDISK_NAME); // 名字，到时候会显示为 /dev/simple_ramdisk

	/* 告诉内核这个磁盘的容量有多少个扇区（16MB/512字节） */
	set_capacity(simple_ramdisk_dev->disk, SIMPLE_RAMDISK_SIZE / 512);

	/* 注册并添加该磁盘 */
	add_disk(simple_ramdisk_dev->disk);

	printk(KERN_INFO "simple_ramdisk: 初始化成功！主设备号: %d\n", simple_ramdisk_major);
	return 0;

err_alloc_disk:
	blk_cleanup_queue(simple_ramdisk_dev->queue);
err_alloc_queue:
	vfree(simple_ramdisk_dev->data);
err_alloc_data:
	kfree(simple_ramdisk_dev);
	unregister_blkdev(simple_ramdisk_major, SIMPLE_RAMDISK_NAME);
	return ret;
}

static void __exit simple_ramdisk_exit(void) 
{
	/* 把磁盘从系统中摘除 */
	del_gendisk(simple_ramdisk_dev->disk);
	/* 释放gendisk结构体 */
	put_disk(simple_ramdisk_dev->disk);
	/* 清理队列 */
	blk_cleanup_queue(simple_ramdisk_dev->queue);
	/* 释放分配的16MB内存 */
	vfree(simple_ramdisk_dev->data);
	kfree(simple_ramdisk_dev);
	unregister_blkdev(simple_ramdisk_major, SIMPLE_RAMDISK_NAME);

	printk(KERN_INFO "simple_ramdisk_dev: 模块已卸载，内存已释放\n");
}

module_init(simple_ramdisk_init);
module_exit(simple_ramdisk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("L1angGM");
MODULE_DESCRIPTION("A simple ramdisk block driver");

