#include<linux/init.h>
#include<linux/module.h>
#include "export.h"

static int __init _init(void)
{
    num=100;
    printk(KERN_INFO "THIS IS MODULE WITH MULTIPLE EXPORTED SYMBOLS...\n");
    return (0);
}
static void __exit _exit(void)
{
    printk(KERN_INFO "EXITING FROM MODULE WITH MULTIPLE EXPORTED SYMBOLS...\n");
}
void exported_fu(void)
{
    printk(KERN_INFO "EXPORTED FUNCTION FROM MODULE.KO ..\n");
}
module_init(_init);
module_exit(_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PRIYANKA");
MODULE_DESCRIPTION("MODULE WITH MULTIPLE EXPORTED SYMBOLS");

EXPORT_SYMBOL(exported_fu);
EXPORT_SYMBOL(num);
