#include<linux/init.h>
#include<linux/module.h>

int num=100;
EXPORT_SYMBOL(num);

static int __init _init(void)
{
    printk(KERN_INFO "This is module with one exported symbol...\n");
    return 0;
}
 static void __exit _exit(void)
 {
    printk(KERN_INFO "Exiting from module with one exported symbol....\n");
 }

 module_init(_init);
 module_exit(_exit);

 MODULE_LICENSE("Dual BSD/GPL");
 MODULE_AUTHOR(" PRIYANKA");
 MODULE_DESCRIPTION("MODULE WITH ONE EXPORTED SYMBOL");
