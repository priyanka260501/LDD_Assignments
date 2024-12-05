
obj-m = module.o
module-objs=split_module.o split_module2.o 

split_module.ko split_module2.ko: split_module.c split_module2.c
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean

.PHONY: clean
