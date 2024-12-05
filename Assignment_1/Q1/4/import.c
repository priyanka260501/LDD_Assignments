#include<linux/init.h>
#include<linux/module.h>
#include "export.h"

static int __init _init(void)
{
    exported_fu();
    printk(KERN_INFO "Importd symbol 'num'=%d from export.ko\n",num);
    printk(KERN_INFO "this is module which is using exported symbol...\n");
    return(0);
}

static void __exit _exit(void)
{
    printk(KERN_INFO "exiting from  module which is using exported symbol...\n");
}

module_init(_init);
module_exit(_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PRIYANKA");
MODULE_DESCRIPTION("MODULE USING EXPORTED SYMBOLS");