#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/vmalloc.h>

/* 定义模拟的磁盘大小 16MB */
#define SIMPLE_RAMDISK_SIZE (16 * 1024 * 1024)
/* 定义模拟磁盘名称 */
#define SIMPLE_RAMDISK_NAME "simple_ramdisk"

/* 定义一个结构体来代表模拟磁盘 */
struct simple_ramdisk_device {
	void* data;
	struct gendisk* disk;
};

static struct simple_ramdisk_device* simple_ramdisk_dev = NULL;


/**
 * @brief: 核心读写逻辑，当用户要在磁盘上读写数据时，内核最终会调用到这
 * @detail: 因为我们是内存模拟，所以所谓的“内存读写”，其实就是内存拷贝（memcpy）
 * @param: bio - 上层传递给块层的I/O请求
 */
blk_qc_t simple_ramdisk_submit_bio(struct bio *bio)
{
	struct bio_vec bvec; // 描述与这个bio请求对应的所有的内存，它不可能全在一页里，所以需要一个向量来表示
	struct bvec_iter iter; // 描述I/O请求的开始扇区
	sector_t sector;
	u8* mem_addr = NULL;
	int dir = 0;

	/* 获取这次读写请求对应的开始扇区的编号 */
	sector = bio->bi_iter.bi_sector;
	/* 计算出该扇区在我们的内存空间中对应的具体位置（偏移量） */
	mem_addr = simple_ramdisk_dev->data + (sector * 512);
	/* 判断本次请求是读还是写(write = 1, read = 0) */
	dir = bio_data_dir(bio);

	/* 一个bio请求可能包含多段内存数据，需要循环遍历他们 */
	bio_for_each_segment(bvec, bio, iter) {
		/* 获取当前这段数据的长度 */
		unsigned int len = bvec.bv_len;
		/* 把这段数据映射到虚拟地址空间，方便操作 */
		void* buffer = kmap_atomic(bvec.bv_page) + bvec.bv_offset;
		
	}
}


struct block_device_operations simple_ramdisk_fops = {
	.owner = THIS_MODULE,
	.submit_bio = simple_ramdisk_submit_bio,
}; 

static int __init simple_ramdisk_init(void)
{
	int ret = 0;

	printk(KERN_INFO, "simple_ramdisk: 正在初始化... ...\n");

	/* 分配内存 */
	simple_ramdisk_dev = kzalloc(sizeof(struct simple_ramdisk_device), GFP_KERNEL);
	if (!simple_ramdisk_dev) return -ENOMEM;

	/* 核心：分配 16MB 的真实物理内存，这就是我们的磁盘本体              */
	simple_ramdisk_dev->data = vmalloc(SIMPLE_RAMDISK_SIZE);
	if (!simple_ramdisk_dev->data) {
		ret = - ENOMEM;
		goto err_alloc_data;
	}

	/* 申请gendisk(身份证)   */
	simple_ramdisk_dev->disk = blk_alloc_disk(NUMA_NO_NODE);
	if (!simple_ramdisk_dev->disk) {
		ret = -ENOMEM;
		goto err_alloc_disk;
	}

	/* 填写gendisk的详细信息 */
	simple_ramdisk_dev->disk->major = 0; // 让内核动态分配一个主设备号
	simple_ramdisk_dev->disk->first_minor = 0; // 次设备号从0开始
	simple_ramdisk_dev->disk->fops = ; // 绑定操作数据集

err_alloc_disk:
	vfree(simple_ramdisk_dev->data);
err_alloc_data:
	kfree(simple_ramdisk_dev);
}

