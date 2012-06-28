obj-m += a.o
KDIR:=/lib/modules/$(shell uname -r)/build
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf Module.makers modules.order Module.symvers
