#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "farawayfromdeathstar"
// create our fake device structure
struct fake_device {
	char data[100];
	struct semaphore sem;
} virtual_device;

//create objects for registering device later
struct cdev *mcdev; // cdev is a char device; resides inside inode object
int major_number;
int ret;

dev_t dev_num;


// beg proc code area
int proc_len, proc_temp;
char *proc_msg;

int read_proc(struct file *filp, char *buf, size_t count, loff_t offp){
	printk(KERN_INFO "ALEX:::32::read_proc(): count: u %zu | d %zd", count, count);
	if(count > proc_temp)
		count = proc_temp;

	proc_temp = proc_temp - count;
	copy_to_user(buf, proc_msg, count);
	if(count == 0)
		proc_temp = proc_len;

	return count;
}

struct file_operations proc_fops = {
	read: read_proc
};

void create_new_proc_entry(void){
	proc_create(DEVICE_NAME, 0, NULL, &proc_fops);
	proc_msg = virtual_device.data;
	// proc_msg = kmalloc(sizeof(char) * 50, 0);
	// strcpy(proc_msg, "Hello World\n");
	proc_len = strlen(proc_msg);
	proc_temp = proc_len;
	printk(KERN_INFO "1.len=%d", proc_len);
}

int proc_init(void){
	printk(KERN_INFO "ALEX: PROC START");
	create_new_proc_entry();
	return 0;
}

void proc_cleanup(void){
	kfree(proc_msg);
	remove_proc_entry(DEVICE_NAME, NULL);
}

// end proc code area

int device_open(struct inode *inode, struct file *filp){
	// use semaphore to allow only one process to access this device at a time
	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "ALEX: could not lock device during open");
		return -1;
	}
	return 0;
}

//called when user wants to get information about device
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset){
	//transfer data from kernel space to user space
	printk(KERN_INFO "ALEX: reading from device");
	ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
	return ret;
}

ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//send data from user to kernel
	printk(KERN_INFO "ALEX: writing to device");
	ret = copy_from_user(virtual_device.data, bufSourceData, bufCount);
	return ret;
}

int device_close(struct inode *inode, struct file *filp){
	//release device semaphore
	up(&virtual_device.sem);
	printk(KERN_INFO "ALEX: closed device");
	return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read
};

static int driver_entry(void){
	//register driver number
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if(ret < 0){
		printk(KERN_ALERT "ALEX: failed to allocate a major number");
		return ret;
	}

	major_number = MAJOR(dev_num);
	printk(KERN_ALERT "ALEX: the major number is %d", major_number);
	printk(KERN_ALERT "ALEX: \t run \"sudo mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number);

	// initialize cdev structure
	mcdev = cdev_alloc();
	//set operations of our char device to fops
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	//attempt to add our char device to the kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0){
		printk(KERN_ALERT "ALEX: unable to add cdev to kernel");
		return ret;
	}
	sema_init(&virtual_device.sem, 1);
	proc_init();
	return 0;
}

static void driver_exit(void){
	cdev_del(mcdev);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "ALEX: unloaded module");
	proc_cleanup();
}

module_init(driver_entry);
module_exit(driver_exit);