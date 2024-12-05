#include<linux/init.h>
#include<linux/module.h>

static void priya_exit(void)
{
    printk(KERN_INFO "Exiting From the module with two files....\n");

}

module_exit(priya_exit);
