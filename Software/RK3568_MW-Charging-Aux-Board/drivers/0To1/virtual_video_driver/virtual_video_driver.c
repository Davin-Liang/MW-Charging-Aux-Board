/*
 * 参考 airspy.c
 */

//Notice: vvd = virtual_video_driver

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>

struct vvd_frame_buf {
	struct vb2_v4l2_buffer vb;
	struct list_head list;
};

extern unsigned char red[8230];
extern unsigned char blue[8267];
extern unsigned char green[8265];

static struct list_head queued_bufs;
static struct timer_list vvd_timer;
static struct mutex vb_queue_lock;
static struct mutex v4l2_lock;
static int copy_cnt;

/* 这个结构体中的函数都不需要自己提供 */
static const struct v4l2_file_operations vvd_fops = {
	.owner          = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll           = vb2_fop_poll,
	.mmap           = vb2_fop_mmap,
	.unlocked_ioctl = video_ioctl2,
};

/* 查询能力IOCTL */
static int vvd_querycap(struct file *file, void *fh, struct v4l2_capability *cap)
{
	printk("[%s] func: %s, line: %d\n", __FILE__, __func__, __LINE__);
	strlcpy(cap->driver, "L1ang_virtual_video", sizeof(cap->driver));
	strlcpy(cap->card, "no-card", sizeof(cap->card));
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;

	return 0;
}

/* 枚举格式IOCTL */
static int vvd_enum_fmt_vid_cap(struct file *file, void *priv,
struct v4l2_fmtdesc *f)
{
	printk("[%s] func: %s, line: %d\n", __FILE__, __func__, __LINE__);
	if (f->index > 0)
		return -EINVAL;

	strlcpy(f->description, "Motion JPEG", sizeof(f->description));
	f->pixelformat = V4L2_PIX_FMT_MJPEG;

	return 0;
}

/* 设置参数IOCTL */
static int vvd_s_fmt_vid_cap(struct file *file, void *priv,
		struct v4l2_format *f)
{
	/*
	 * 分辩用户传入的参数是否可用
	 * 如果不可用，给APP传入最接近的、硬件支持的参数
     */
    printk("[%s] func: %s, line: %d\n", __FILE__, __func__, __LINE__);
	if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG)
		return -EINVAL;
    f->fmt.pix.width = 800;
    f->fmt.pix.height = 600;

	return 0;
}

/* 枚举设备支持的格式对应的帧大小IOCTL */
static int vvd_enum_framesizes(struct file *file, void *fh,
				   struct v4l2_frmsizeenum *fsize)
{
	if (fsize->index > 0)
		return -EINVAL;

	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = 800;
	fsize->discrete.height = 600;
	return 0;
}

static int vvd_g_fmt(struct file *file, void *priv, struct v4l2_format *f)
{
   struct v4l2_pix_format *pix = &f->fmt.pix;

   pix->width = 800;
   pix->height = 600;
   pix->field = V4L2_FIELD_NONE;
   pix->pixelformat = V4L2_PIX_FMT_MJPEG;
   pix->bytesperline = 0;
   return 0;
}


/* 这个结构体中的大部分函数也是内核已经提供好了的 */
static const struct v4l2_ioctl_ops vvd_ops = {
	.vidioc_querycap          = vvd_querycap,

	.vidioc_enum_fmt_vid_cap  = vvd_enum_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vvd_s_fmt_vid_cap,
	.vidioc_enum_framesizes	  = vvd_enum_framesizes,
	.vidioc_g_fmt_vid_cap     = vvd_g_fmt,

	.vidioc_reqbufs           = vb2_ioctl_reqbufs,
	.vidioc_create_bufs       = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf       = vb2_ioctl_prepare_buf,
	.vidioc_querybuf          = vb2_ioctl_querybuf,
	.vidioc_qbuf              = vb2_ioctl_qbuf,
	.vidioc_dqbuf             = vb2_ioctl_dqbuf,

	.vidioc_streamon          = vb2_ioctl_streamon,
	.vidioc_streamoff         = vb2_ioctl_streamoff,
};

static int vvd_queue_setup(struct vb2_queue *vq,
		unsigned int *nbuffers, unsigned int *nplanes, 
		unsigned int sizes[], struct device *alloc_devs[])
{
	/* 假设：至少需要8个buffer，每个buffer只有1个plane */
	if (vq->num_buffers + *nbuffers < 8)
		*nbuffers = 8 - vq->num_buffers; // 如果当前申请的buffer加上当前已有buffer不够8个，就调整当前申请的buffer个数
	*nplanes = 1; // 直接设定每个buffer只有1个plane
	sizes[0] = PAGE_ALIGN(800 * 600 * 2);

	printk("nbuffers=%d sizes[0]=%d\n", *nbuffers, sizes[0]);
	return 0;
}

static void vvd_buf_queue(struct vb2_buffer *vb)
{	
	/* 结构体转换：vb2_buffer --> vb2_v4l2_buffer     */
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vvd_frame_buf *buf = container_of(vbuf, struct vvd_frame_buf, vb);
	// unsigned long flags;
	/* 将申请到的buffer添加到队列中 */
	// spin_lock_irqsave(&s->queued_bufs_lock, flags);
	list_add_tail(&buf->list, &queued_bufs);
	//spin_unlock_irqrestore(&s->queued_bufs_lock, flags);
}

static struct vvd_frame_buf *vvd_get_next_fill_buf(void)
{
	struct vvd_frame_buf *buf = NULL;

	// spin_lock_irqsave(&s->queued_bufs_lock, flags);
	if (list_empty(&queued_bufs))
		goto leave;

	buf = list_entry(queued_bufs.next, struct vvd_frame_buf, list); // 从空闲链表中的得到空闲buffer
	list_del(&buf->list); // 从空闲链表中移除
leave:
	// spin_unlock_irqrestore(&s->queued_bufs_lock, flags);
	return buf;
}


static void vvd_timer_expire(struct timer_list *t)
{
	struct vvd_frame_buf *buf;
	void *ptr;

	/* 从硬件上读到数据，使用（red/green/blue来模拟，构造（伪造）数据） */
	/* 写入空闲buffer */
	buf = vvd_get_next_fill_buf(); // 得到空闲buffer

	if (buf) {
		ptr = vb2_plane_vaddr(&buf->vb.vb2_buf, 0); // 得到 struct vb2_buffer 指针

		/* 每60帧（2s）拷贝不同颜色  */
		if (copy_cnt < 60) {
			memcpy(ptr, red, sizeof(red));
			vb2_set_plane_payload(&buf->vb.vb2_buf, 0, sizeof(red)); // 设置每次拷贝的数据，因为可能数据每次的长度可能不一样
		} else if (copy_cnt >= 60 && copy_cnt < 120) {
			memcpy(ptr, green, sizeof(green));
			vb2_set_plane_payload(&buf->vb.vb2_buf, 0, sizeof(green)); // 设置每次拷贝的数据，因为可能数据每次的长度可能不一样
		} else if (copy_cnt >= 120 && copy_cnt < 180) {
			memcpy(ptr, blue, sizeof(blue));
			vb2_set_plane_payload(&buf->vb.vb2_buf, 0, sizeof(blue)); // 设置每次拷贝的数据，因为可能数据每次的长度可能不一样
		}

		/* 将buffer放入完成链表：vb2_buffer_done */
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
	}

	copy_cnt += 1;
	if (copy_cnt >= 180)
		copy_cnt = 0;
	/* 再次设置 timer 的超时时间       */
	mod_timer(&vvd_timer, jiffies + HZ / 30); // 期望每秒钟产生30帧数据
}

static int vvd_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	/* 启动硬件传输 */
	/* 使用 timer 来模拟硬件中断 */
	// 设置 timer
	timer_setup(&vvd_timer, vvd_timer_expire, 0); // 4.19.232使用该版本函数

	// 启动timer
	printk(KERN_INFO "Starting timer...\n");
	mod_timer(&vvd_timer, jiffies + HZ / 30); // 期望每秒钟产生30帧数据
	
	return 0;
}

static void vvd_stop_streaming(struct vb2_queue *vq)
{
	/* 停止硬件传输 */
	del_timer(&vvd_timer);

	while (!list_empty(&queued_bufs)) {
		struct vvd_frame_buf *buf;

		buf = list_entry(queued_bufs.next,
				struct vvd_frame_buf, list);
		list_del(&buf->list);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}


}


/* 这个结构体中的大部分函数也是内核已经提供好了的 */
static const struct vb2_ops vvd_vb2_ops = {
	.queue_setup            = vvd_queue_setup,
	.buf_queue              = vvd_buf_queue,
	.start_streaming        = vvd_start_streaming,
	.stop_streaming         = vvd_stop_streaming,
	.wait_prepare           = vb2_ops_wait_prepare,
	.wait_finish            = vb2_ops_wait_finish,
};


static struct video_device vvd_vdev = {
	.name                     = "L1angGM_virtual_video_driver",
	.release                  = video_device_release_empty,
	.fops                     = &vvd_fops,
	.ioctl_ops                = &vvd_ops,
};

static struct v4l2_device vvd_v4l2_dev;
static struct vb2_queue vvd_vb2_queue;

static void vvd_video_release(struct v4l2_device *v)
{

}


static int vvd_init(void)
{
	int ret;

	/* 分配/设置/注册video_device结构体 */
	/* 
	 * 设置：
	 * 1. 函数调用（比如ioctl）
	 * 2. 队列/buffer的管理
	 */
	vvd_vb2_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vvd_vb2_queue.io_modes = VB2_MMAP | VB2_USERPTR | VB2_READ;
	vvd_vb2_queue.drv_priv = NULL;
	vvd_vb2_queue.buf_struct_size = sizeof(struct vvd_frame_buf); // 分配vb时，分配的空间大小为buf_struct_size
	vvd_vb2_queue.ops = &vvd_vb2_ops; // 使用自己提供的结构体
	vvd_vb2_queue.mem_ops = &vb2_vmalloc_memops; // 使用内核提供好的结构体
	vvd_vb2_queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	ret = vb2_queue_init(&vvd_vb2_queue);
	if (ret) {
		printk("Could not initialize vb2 queue\n");
		return -1;
	}

	/* 初始化锁 */
	mutex_init(&vb_queue_lock);
	mutex_init(&v4l2_lock);

	vvd_vdev.queue = &vvd_vb2_queue; // queue和video_device 挂钩
	vvd_vdev.queue->lock = &vb_queue_lock;

	/* Register the v4l2_device structure */
	vvd_v4l2_dev.release = vvd_video_release;
	strcpy(vvd_v4l2_dev.name, "L1angGM_vvd_v4l2");
	ret = v4l2_device_register(NULL, &vvd_v4l2_dev);
	if (ret) {
		printk("Failed to register v4l2-device (%d)\n", ret);
		return -1;
	}

	vvd_vdev.v4l2_dev = &vvd_v4l2_dev;
	vvd_vdev.lock = &v4l2_lock;
	ret = video_register_device(&vvd_vdev, VFL_TYPE_GRABBER, -1);
	if (ret) {
		printk("Failed to register as video device (%d)\n", ret);
		return -1;
	}

	/* 初始化链表 */
	INIT_LIST_HEAD(&queued_bufs);

	return 0;
}

static void vvd_exit(void)
{
	/* 反注册/释放video_device结构体 */
	v4l2_device_unregister(&vvd_v4l2_dev);
	video_unregister_device(&vvd_vdev);
}

module_init(vvd_init);
module_exit(vvd_exit);
MODULE_LICENSE("GPL");

