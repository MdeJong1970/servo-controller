// servo: device driver for controlling servos through a PCA9685
//
// Mechanismen:
//  * Dateischnittstelle: open(), close(), write()
//  * GPIO's
//  * dynamisch oder statisch vergebene Major-Number
//  * Generierung eines /sys-FS-Eintrages inkl. UEVENT für udevd bzw. mdev
//

#include <linux/version.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ctype.h>

static dev_t servo_dev;
static struct cdev servo_cdev;

// Referenzen fuer /sys-FS
static struct class* servo_class;
static struct device* servo_device;

static const int servo_oe_pgio = 105;

#define MODULE_NAME "servo"

static ssize_t servo_write(struct file *file, const char *buf, size_t count, loff_t *offset)
{
	return (count);
}

static int servo_open(struct inode *node, struct file *file)
{
	return 0;
}


static int servo_close(struct inode *node, struct file *file)
{
	return 0;
}


static struct file_operations servo_fops = {
	.owner   = THIS_MODULE,
	.open    = servo_open,
	.release = servo_close,
	.write   = servo_write,
};


static int __init servo_init(void)
{
	int ret = 0;

	printk (KERN_INFO "servo_init()\n");

	servo_dev = MKDEV(240, 0);
	if ((ret = register_chrdev_region(servo_dev, 1, MODULE_NAME)) < 0) 
	{
		printk(KERN_ERR "servo_init(): register_chrdev_region(): ret=%d\n", ret);
		ret = -1;
		goto out;
	}

	cdev_init(&servo_cdev, &servo_fops);
	if ((ret = cdev_add(&servo_cdev, servo_dev, 1)) < 0) 
	{
		printk(KERN_ERR "servo_init(): cdev_add(): ret=%d\n", ret);
		ret = -2;
		goto out_unalloc_region;
	}

	// Eintrag fuer uevent in /sys
	servo_class = class_create (THIS_MODULE, MODULE_NAME);
	if (IS_ERR(servo_class))
	{
		printk (KERN_ERR "servo_init(): class_create(): err=%ld\n", PTR_ERR(servo_class));
		ret = -3;
		goto out_cdev_del;
	}

	servo_device = device_create (servo_class, NULL, servo_dev, NULL, "servo%d", MINOR(servo_dev));
	if (IS_ERR(servo_device))
	{
		printk (KERN_ERR "servo_init(): device_create(): err=%ld\n", PTR_ERR(servo_device));
		ret = -4;
		goto out_class_destroy;
	}

	ret = gpio_request (servo_oe_pgio, MODULE_NAME);
	if (ret)
	{
		printk(KERN_ERR "servo_init(): gpio_request: ret=%d\n", ret);
		ret = -6;
		goto out_device_destroy;
	}
	return ret;

//out_gpio_free:
//	gpio_free(servo_oe_pgio);
out_device_destroy:
	device_destroy (servo_class, servo_dev);
out_class_destroy:
	class_destroy (servo_class);
out_cdev_del:
	cdev_del(&servo_cdev);
out_unalloc_region:
	unregister_chrdev_region(servo_dev, 1);
out:
	return ret;
}


static void __exit servo_exit(void)
{

	gpio_free(servo_oe_pgio);

	device_destroy (servo_class, servo_dev);
	class_destroy (servo_class);

	cdev_del(&servo_cdev);
	unregister_chrdev_region(servo_dev, 1);

	printk (KERN_INFO "servo_exit()\n");
}


module_init(servo_init);
module_exit(servo_exit);


MODULE_DESCRIPTION("Servo Controller for PCA9685");
MODULE_AUTHOR("Mark de Jonger <mark@mdejong.de>");
MODULE_LICENSE("GPL");

