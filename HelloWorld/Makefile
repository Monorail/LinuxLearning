obj-m += hello.o

KDIR = /usr/src/linux-headers-3.16.0-38-generic


all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
