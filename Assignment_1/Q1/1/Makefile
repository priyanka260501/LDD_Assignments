
obj-m = hello_module.o


hello_module.ko: hello_module.c
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean

.PHONY: clean
