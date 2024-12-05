#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("PRIYANKA");
MODULE_DESCRIPTION("TWO FILE KERNAL MODULE");

static int priya_init(void)
{
    printk(KERN_INFO "This is module with 2 files...\n");
    return 0;
}
module_init(priya_init);