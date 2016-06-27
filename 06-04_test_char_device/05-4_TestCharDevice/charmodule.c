#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>           // device read/write
#include <linux/cdev.h>         // helps register device to kernel
#include <linux/semaphore.h>    
#include <asm/uaccess.h>        // copy_to_user; copy_from_user

//(1) create a structure for our fake device
struct fake_device {
    char data[100];
    struct semaphore sem;
} virtual_device;

// (2) to later register our device we need a cdev object and some other variables
struct cdev *mcdev;
int major_number; 
int ret; //used to hold return values for functions

dev_t dev_num;

#define DEVICE_NAME     "neardeathstar"

int device_open(struct inode *inode, struct file *filp){
    //only allow one process to open this device by using a semaphore as mutual exclusive lock - mutex
    if(down_interruptible(&virtual_device.sem) != 0){
        printk(KERN_ALERT "ALEX: could not lock device during open");
        return -1;
    }
    return 0;
}

//called when the user wants to get information from the device
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset){
    //take data from kernel space and transfer to user space

    printk(KERN_INFO "ALEX: reading from device");
    ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
    return ret;
}

ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset){
	// send data from user to kernel
	//copy_from_user(dest, source, count)

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



//tell the kernel which functions to call when the user operates on our device file
struct file_operations fops = {
    .owner = THIS_MODULE,           //prevent unloading of this module when operations are in use
    .open = device_open,            //function pointer used for opening the device
    .release = device_close,        //function pointer used for closing the device
    .write = device_write,          //function pointer used for writing to the device
    .read = device_read             //function pointer used for reading from the device
};


static int driver_entry(void){
    //allocate major number dynamically
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if(ret < 0){ //an error occured
        printk(KERN_ALERT "ALEX: failed to allocate a major number");
        return ret; // propagate error
    }
    
    major_number = MAJOR(dev_num); //extract major number and store in our var
    printk(KERN_ALERT "ALEX: the major number is %d", major_number);
    printk(KERN_ALERT "ALEX: \t use \"mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number); //dmesg
    
    mcdev = cdev_alloc();       // initialize our cdev
    mcdev->ops = &fops;         // struct file_operations
    mcdev->owner = THIS_MODULE;
    
    
    // add cdev to kernel
    ret = cdev_add(mcdev, dev_num, 1);
    if(ret < 0){ //an error occured
        printk(KERN_ALERT "ALEX: unable to add cdev to kernel");
        return ret; // propagate error
    }
    
    // create a semaphore
    sema_init(&virtual_device.sem, 1);
    
    
    return 0;
}


static void driver_exit(void){
    // unregister everything in reverse order
    cdev_del(mcdev);
    
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_ALERT "ALEX: unloaded module");

}

module_init(driver_entry);
module_exit(driver_exit);