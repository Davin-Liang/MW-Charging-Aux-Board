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

/* 这个结构体中的函数都不需要自己提供 */
static const struct v4l2_file_operations vvd_fops = {
	.owner                    = THIS_MODULE,
	.open                     = v4l2_fh_open,
	.release                  = vb2_fop_release,
	.read                     = vb2_fop_read,
	.poll                     = vb2_fop_poll,
	.mmap                     = vb2_fop_mmap,
	.unlocked_ioctl           = video_ioctl2,
};

/* 这个结构体中的大部分函数也是内核已经提供好了的 */
static const struct v4l2_ioctl_ops vvd_ops = {
	.vidioc_querycap          = airspy_querycap,

	.vidioc_enum_fmt_sdr_cap  = airspy_enum_fmt_sdr_cap,
	.vidioc_g_fmt_sdr_cap     = airspy_g_fmt_sdr_cap,
	.vidioc_s_fmt_sdr_cap     = airspy_s_fmt_sdr_cap,
	.vidioc_try_fmt_sdr_cap   = airspy_try_fmt_sdr_cap,

	.vidioc_reqbufs           = vb2_ioctl_reqbufs,
	.vidioc_create_bufs       = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf       = vb2_ioctl_prepare_buf,
	.vidioc_querybuf          = vb2_ioctl_querybuf,
	.vidioc_qbuf              = vb2_ioctl_qbuf,
	.vidioc_dqbuf             = vb2_ioctl_dqbuf,

	.vidioc_streamon          = vb2_ioctl_streamon,
	.vidioc_streamoff         = vb2_ioctl_streamoff,

	.vidioc_g_tuner           = airspy_g_tuner,
	.vidioc_s_tuner           = airspy_s_tuner,

	.vidioc_g_frequency       = airspy_g_frequency,
	.vidioc_s_frequency       = airspy_s_frequency,
	.vidioc_enum_freq_bands   = airspy_enum_freq_bands,

	.vidioc_subscribe_event   = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
	.vidioc_log_status        = v4l2_ctrl_log_status,
};

/* 这个结构体中的大部分函数也是内核已经提供好了的 */
static const struct vb2_ops vvd_vb2_ops = {
	.queue_setup            = airspy_queue_setup,
	.buf_queue              = airspy_buf_queue,
	.start_streaming        = airspy_start_streaming,
	.stop_streaming         = airspy_stop_streaming,
	.wait_prepare           = vb2_ops_wait_prepare,
	.wait_finish            = vb2_ops_wait_finish,
};


static const struct video_device vvd_vdev = {
	.name                     = "L1angGM_virtual_video_driver",
	.release                  = video_device_release_empty,
	.fops                     = &vvd_fops,
	.ioctl_ops                = &vvd_ops,
};

static struct v4l2_device vvd_v4l2_dev;
static struct vb2_queue vvd_vb2_queue;

static int vvd_init(void)
{
	int ret;

	/* 分配/设置/注册video_device结构体 */
	/* 
	 * 设置：
	 * 1. 函数调用（比如ioctl）
	 * 2. 队列/buffer的管理
	 */
	vvd_vb2_queue.type = V4L2_BUF_TYPE_SDR_CAPTURE;
	vvd_vb2_queue.io_modes = VB2_MMAP | VB2_USERPTR | VB2_READ;
	vvd_vb2_queue.drv_priv = NULL;
	vvd_vb2_queue.buf_struct_size = sizeof(struct airspy_frame_buf);
	vvd_vb2_queue.ops = &vvd_vb2_ops; // 使用自己提供的结构体
	vvd_vb2_queue.mem_ops = &vb2_vmalloc_memops; // 使用内核提供好的结构体
	vvd_vb2_queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	ret = vb2_queue_init(&vvd_vb2_queue);
	if (ret) {
		printk("Could not initialize vb2 queue\n");
		return -1;
	}

	vvd_vdev.queue = &vvd_vb2_queue; // queue和video_device 挂钩
	vvd_vdev.queue->lock = &s->vb_queue_lock;

	/* Register the v4l2_device structure */
	vvd_v4l2_dev.release = airspy_video_release;
	ret = v4l2_device_register(NULL, &vvd_v4l2_dev);
	if (ret) {
		printk("Failed to register v4l2-device (%d)\n", ret);
		return -1;
	}

	vvd_vdev.v4l2_dev = &vvd_v4l2_dev;
	vvd_vdev.lock = &s->v4l2_lock;
	ret = video_register_device(&vvd_vdev, VFL_TYPE_SDR, -1);
	if (ret) {
		printk("Failed to register as video device (%d)\n", ret);
		return -1;
	}

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

