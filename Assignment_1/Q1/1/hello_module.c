#include<linux/init.h>
#include<linux/module.h>

static int _init(void)
{
    printk(KERN_INFO "Hello World...\n");
    return(0);
}

static void _exit(void)
{
    printk(KERN_INFO "Exiting from module.....\n");
}
module_init(_init);
module_exit(_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Priyanka");
MODULE_DESCRIPTION("First linux kernel module");
 