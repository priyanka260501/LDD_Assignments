#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define MAX 32
static char mybuf[MAX];

static dev_t devno;
static int major = 250;
static struct class *pclass;
static struct cdev pchar_cdev;


static int device_open = 0;

static int pchar_open(struct inode *pinode, struct file *pfile) {
    printk(KERN_INFO "%s: pchar_open() called.\n", THIS_MODULE->name);

    
    if (device_open) {
        printk(KERN_ERR "%s: Device is already open by another process.\n", THIS_MODULE->name);
        return -EBUSY; 
    }

    
    device_open = 1;
    printk(KERN_INFO "%s: Device opened successfully.\n", THIS_MODULE->name);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile) {
    printk(KERN_INFO "%s: pchar_close() called.\n", THIS_MODULE->name);

    
    device_open = 0;
    printk(KERN_INFO "%s: Device closed successfully.\n", THIS_MODULE->name);
    return 0;
}

static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t bufsize, loff_t *pf_pos) {
    int max_bytes, bytes_to_write, nbytes;
    printk(KERN_INFO "%s: pchar_write() called.\n", THIS_MODULE->name);

    max_bytes = MAX - *pf_pos;
    bytes_to_write = (max_bytes < bufsize) ? max_bytes : bufsize;
    if (bytes_to_write <= 0) {
        printk(KERN_ERR "%s: pchar_write() - mybuf is full.\n", THIS_MODULE->name);
        return -ENOSPC;
    }
    nbytes = bytes_to_write - copy_from_user(mybuf + *pf_pos, ubuf, bytes_to_write);
    *pf_pos = *pf_pos + nbytes;
    printk(KERN_INFO "%s: pchar_write() written %d bytes to mybuf.\n", THIS_MODULE->name, nbytes);
    return nbytes;
}

static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t bufsize, loff_t *pf_pos) {
    int max_bytes, bytes_to_read, nbytes;
    printk(KERN_INFO "%s: pchar_read() called.\n", THIS_MODULE->name);

    max_bytes = MAX - *pf_pos;
    bytes_to_read = (max_bytes < bufsize) ? max_bytes : bufsize;
    if (bytes_to_read <= 0) {
        printk(KERN_INFO "%s: pchar_read() - no more to read from mybuf.\n", THIS_MODULE->name);
        return 0;
    }
    nbytes = bytes_to_read - copy_to_user(ubuf, mybuf + *pf_pos, bytes_to_read);
    *pf_pos = *pf_pos + nbytes;
    printk(KERN_INFO "%s: pchar_read() read %d bytes from mybuf.\n", THIS_MODULE->name, nbytes);
    return nbytes;
}

static struct file_operations pchar_fops = {
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read
};

static int __init pchar_init(void) {
    int ret;
    struct device *pdevice;

    printk(KERN_INFO "%s: pchar_init() called.\n", THIS_MODULE->name);
    devno = MKDEV(major, 0);
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    if (ret != 0) {
        printk(KERN_ERR "%s: alloc_chrdev_region() failed.\n", THIS_MODULE->name);
        return ret;
    }
    major = MAJOR(devno);
    pclass = class_create(THIS_MODULE, "pchar_class");
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar");
    pchar_cdev.owner = THIS_MODULE;
    cdev_init(&pchar_cdev, &pchar_fops);
    ret = cdev_add(&pchar_cdev, devno, 1);
    return 0;
}

static void __exit pchar_exit(void) {
    cdev_del(&pchar_cdev);
    device_destroy(pclass, devno);
    class_destroy(pclass);
    unregister_chrdev_region(devno, 1);
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Pseudo Char Device Driver");
MODULE_AUTHOR("Nilesh Ghule <nilesh@sunbeaminfo.com>");

