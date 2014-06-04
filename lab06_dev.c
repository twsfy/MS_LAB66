#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/timer.h>

static struct timer_list timer;

static long timeout = 2000;

void time_handler(unsigned long data)
{
	int ret;

	printk(KERN_INFO "Lab06 time handler, timeout=%ld \n", timeout);
	ret = mod_timer(&timer, jiffies + msecs_to_jiffies(timeout));
	if (ret) {
		printk(KERN_ERR "Fail to mod timer\n");
	}
}

static ssize_t write_handler(struct file *filp, const char *buff,
		size_t count, loff_t *offp)
{
	long t = -1l;
	kstrtol(buff, 10, &t);
	if (t == -1l) {
		printk(KERN_INFO "Wrong argument.\n");
		return count;
	}
	timeout = t;
	return count;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = write_handler
};

static struct miscdevice lab06_dev = {
	MISC_DYNAMIC_MINOR,
	"lab06",
	&fops
};

static int __init lab_init(void)
{
	int ret;
	ret = misc_register(&lab06_dev);
	if (ret) {
		printk(KERN_ERR "Unable to register \"Lab06\" device\n");
	}

	setup_timer(&timer, time_handler, 0);

	ret = mod_timer(&timer, jiffies + msecs_to_jiffies(timeout));
	if (ret) {
		printk(KERN_ERR "Error in mod_timer\n");
	}


	return ret;
}

module_init(lab_init);

static void __exit lab_exit(void)
{
	misc_deregister(&lab06_dev);
	del_timer(&timer);
}

module_exit(lab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Stabrovsky");
MODULE_DESCRIPTION("\"Lab06\" module");
MODULE_VERSION("dev");