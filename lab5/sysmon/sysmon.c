#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/sysinfo.h>
#include <linux/mm.h>
#include <linux/sched.h>

#define DEVICE_NAME "sysmon"
#define CLASS_NAME "sysmon_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A.A");
MODULE_DESCRIPTION("A simple system monitoring device driver");
MODULE_VERSION("0.1");

static int majorNumber;
static struct class* sysmonClass = NULL;
static struct device* sysmonDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};

static int __init sysmon_init(void)
{
    printk(KERN_INFO "sysmon: Initializing the sysmon LKM\n");

    // Try to dynamically allocate a major number for the device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "sysmon failed to register a major number\n");
        return majorNumber;
    }

    // Register the device class
    sysmonClass = class_create(DEVICE_NAME);
    if (IS_ERR(sysmonClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(sysmonClass);
    }

    // Register the device driver
    sysmonDevice = device_create(sysmonClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(sysmonDevice))
    {
        class_destroy(sysmonClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(sysmonDevice);
    }

    printk(KERN_INFO "sysmon: device class created correctly\n");
    return 0;
}

static void __exit sysmon_exit(void)
{
    device_destroy(sysmonClass, MKDEV(majorNumber, 0));
    class_destroy(sysmonClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "sysmon: Goodbye from the sysmon LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "sysmon: Device has been opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    struct sysinfo info;

    // Get system information
    si_meminfo(&info);

    // Calculate total RAM and total swap in kilobytes
    unsigned long totalram_kb = (info.totalram * (info.mem_unit / 1024));
    unsigned long freeram_kb = (info.freeram * (info.mem_unit / 1024));
    unsigned long totalswap_kb = (info.totalswap * (info.mem_unit / 1024));
    unsigned long freeswap_kb = (info.freeswap * (info.mem_unit / 1024));

    // Initialize bytes_read variable
    int bytes_read = 0;

    // Copy data to user space
    char temp_buff[512];
    bytes_read = snprintf(temp_buff, sizeof(temp_buff),
                          "Total RAM: %lu KB\n"
                          "Free RAM: %lu KB\n"
                          "Total Swap: %lu KB\n"
                          "Free Swap: %lu KB\n"
                          "\x03",
                          totalram_kb, freeram_kb, totalswap_kb, freeswap_kb);

    if (bytes_read < 0) {
        return bytes_read;
    }

    // Make sure we don't write more than len bytes
    bytes_read = min_t(int, bytes_read, len);

    // Copy data to user space
    if (copy_to_user(buffer, temp_buff, bytes_read)) {
        return -EFAULT;
    }

    return bytes_read;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "sysmon: Device successfully closed\n");
    return 0;
}

module_init(sysmon_init);
module_exit(sysmon_exit);
