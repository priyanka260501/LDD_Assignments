
obj-m = module_param.o


module_param.ko: module_param.c
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean

.PHONY: clean
