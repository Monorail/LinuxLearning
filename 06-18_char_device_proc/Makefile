obj-m += char_device_proc.o

KDIR = /usr/src/linux-headers-3.19.0-32-generic

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
    
clean:
	rm -rf *.o *.ok *.mod.* *.symvers *.order
