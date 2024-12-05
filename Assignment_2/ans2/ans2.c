#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kfifo.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

// private device structs
typedef struct pchardev {
    struct kfifo mybuf;
    dev_t devno;        
    struct cdev cdev;
    int id;
    int buffer_size;
    // ...
}pchardev_t;

#define MAX 32
#define DEVCNT  4
static pchardev_t devices[DEVCNT];

// IOCTL commands
#define IOCTL_CLEAR_BUFFER _IO('p', 1)         // Clear the device buffer
#define IOCTL_SET_MAX_SIZE _IOW('p', 2, int)  // Set maximum buffer size
#define IOCTL_GET_MAX_SIZE _IOR('p', 3, int)  // Get maximum buffer size

// device operations
static int pchar_open(struct inode *pinode, struct file *pfile) {
    pchardev_t *dev = container_of(pinode->i_cdev, pchardev_t, cdev);
    pfile->private_data = dev;
    pr_info("%s: pchar_open() called for pchar%d.\n", THIS_MODULE->name, dev->id);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile) {
    pchardev_t *dev = (pchardev_t *)pfile->private_data;
    pr_info("%s: pchar_close() called for pchar%d.\n", THIS_MODULE->name, dev->id);
    return 0;
}

static ssize_t pchar_write(struct file *pfile, const char * __user ubuf, size_t bufsize, loff_t *pf_pos) {
    pchardev_t *dev = (pchardev_t *)pfile->private_data;
    int nbytes;
    pr_info("%s: pchar_write() called for pchar%d.\n", THIS_MODULE->name, dev->id);
    if (bufsize > dev->buffer_size) {
        pr_warn("pchar%d: Buffer size exceeded\n", dev->id);
        return -ENOMEM;
    }
    kfifo_from_user(&dev->mybuf, ubuf, bufsize, &nbytes);
    pr_info("%s: pchar_write() written %d bytes in pchar%d.\n", THIS_MODULE->name, nbytes, dev->id);
    return nbytes;
}

static ssize_t pchar_read(struct file *pfile, char * __user ubuf, size_t bufsize, loff_t *pf_pos) {
    pchardev_t *dev = (pchardev_t *)pfile->private_data;
    int nbytes;
    pr_info("%s: pchar_read() called for pchar%d.\n", THIS_MODULE->name, dev->id);
    kfifo_to_user(&dev->mybuf, ubuf, bufsize, &nbytes);
    pr_info("%s: pchar_read() read %d bytes in pchar%d.\n", THIS_MODULE->name, nbytes, dev->id);
    return nbytes;
}

static long pchar_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg) {
    pchardev_t *dev = (pchardev_t *)pfile->private_data;
    int max_size, ret;

    switch (cmd) {
    case IOCTL_CLEAR_BUFFER:
        kfifo_reset(&dev->mybuf);
        pr_info("pchar%d: Buffer cleared\n", dev->id);
        break;
    case IOCTL_SET_MAX_SIZE:
        if (copy_from_user(&max_size, (int __user *)arg, sizeof(max_size))) {
            return -EFAULT;
        }
        if (max_size > MAX || max_size < 0) {
            return -EINVAL;
        }
        dev->buffer_size = max_size;
        pr_info("pchar%d: Max buffer size set to %d\n", dev->id, max_size);
        break;
    case IOCTL_GET_MAX_SIZE:
        max_size = dev->buffer_size;
        if (copy_to_user((int __user *)arg, &max_size, sizeof(max_size))) {
            return -EFAULT;
        }
        pr_info("pchar%d: Max buffer size is %d\n", dev->id, max_size);
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

static struct file_operations pchar_fops = {
    .owner = THIS_MODULE, 
    .open = pchar_open,
    .release = pchar_close,
    .read = pchar_read,
    .write = pchar_write,
    .unlocked_ioctl = pchar_ioctl,
};

// other global vars
static dev_t devno;
static int major;
static struct class *pclass;

static int __init pchar_init(void) {
    int ret, i;
    struct device *pdevice;
    dev_t devnum;

    pr_info("%s: pchar_init() called.\n", THIS_MODULE->name);

    // allocate device numbers
    ret = alloc_chrdev_region(&devno, 0, DEVCNT, "pchar");
    major = MAJOR(devno);
    pr_info("%s: alloc_chrdev_region() allocated device number: major = %d\n", THIS_MODULE->name, major);

    // create device class
    pclass = class_create(THIS_MODULE, "pchar_class");

    // create device files
    for(i = 0; i < DEVCNT; i++) {
        devnum = MKDEV(major, i);
        pdevice = device_create(pclass, NULL, devnum, NULL, "pchar%d", i);
    }

    // initialize and add cdevs into kernel
    for(i = 0; i < DEVCNT; i++) {
        devnum = MKDEV(major, i);
        devices[i].cdev.owner = THIS_MODULE;
        cdev_init(&devices[i].cdev, &pchar_fops);
        ret = cdev_add(&devices[i].cdev, devnum, 1);
    }

    // initialize device info 
    for(i = 0; i < DEVCNT; i++) {
        devices[i].id = i;
        devices[i].devno = MKDEV(major, i);
        devices[i].buffer_size = MAX; // Default buffer size
        ret = kfifo_alloc(&devices[i].mybuf, MAX, GFP_KERNEL);
    }

    return 0;
}

static void __exit pchar_exit(void) {
    int i;

    pr_info("%s: pchar_exit() called.\n", THIS_MODULE->name);

    for(i = 0; i < DEVCNT; i++) {
        kfifo_free(&devices[i].mybuf);
        cdev_del(&devices[i].cdev);
        device_destroy(pclass, devices[i].devno);
    }

    class_destroy(pclass);
    unregister_chrdev_region(devno, DEVCNT);
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Multiple devices - Pseudo Char Driver with ioctl");
MODULE_AUTHOR("Unknown <anonymous@gmail.com>");

