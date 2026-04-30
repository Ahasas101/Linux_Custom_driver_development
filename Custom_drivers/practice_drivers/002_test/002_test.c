#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

#define SIZE 512
dev_t device_no;


char device_buff[SIZE];
ssize_t test_device_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int test_device_open (struct inode *inode, struct file *filp);
int test_device_release (struct inode *inode, struct file *filp);
ssize_t test_device_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
	


struct cdev test_cdev;
struct file_operations test_fops = { .owner = THIS_MODULE,
	//.llseek = test_device_lseek,
	.read = test_device_read,
	.write = test_device_write,
	.open = test_device_open,
	.release = test_device_release			
};

ssize_t test_device_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Read requested for %zu bytes \n",count);
	pr_info("Current position = %lld \n",*f_pos);
	if((count + *f_pos) > SIZE)
	{
		count = SIZE - *f_pos;
	}
	
	if(copy_to_user(buff, &device_buff[*f_pos],count))
	{
		return -EFAULT;
	}	
	
	
	*f_pos += count;
	
	pr_info("Number of bytes successfully read = %zu\n",count);
	pr_info("Update file position = %lld\n",*f_pos);
	
	
	return count;
}

ssize_t test_device_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Write requested for %zu bytes \n",count);
	pr_info("Current position = %lld \n",*f_pos);
	if((count + *f_pos) > SIZE)
	{
		count = SIZE - *f_pos;
	}
	
	if(!count) 
	{
		pr_err("Memory exhausted \n");
		return -ENOMEM;
	}
	if(copy_from_user((void*)&device_buff[*f_pos], buff, count))
	{
		return -EFAULT;
	}
	*f_pos += count;
	pr_info("Number of bytes successfully written = %zu\n",count);
	pr_info("Update file position = %lld\n",*f_pos);
	pr_info("write requested for %zu bytes\n",count);
	
	return count;
}
	

int test_device_open (struct inode *inode, struct file *filp)
{
	pr_info("open was successful\n");
	return 0;
}

int test_device_release (struct inode *inode, struct file *filp)
{
	pr_info("close was successful\n");
	return 0;
}


struct class *test_class;
struct device *test_device;



static int __init my_kernel_module_init(void)
{
	int ret;
	/*1. Creating device numbers*/
	ret = alloc_chrdev_region(&device_no, 0, 1, "char_test_device"); 
	if(ret < 0)
	{
		pr_err("alloc_chrdev_region failed \n");
		goto alloc_failure;
	}
	/*2. Registering with VFS*/
	cdev_init(&test_cdev, &test_fops);
	test_cdev.owner = THIS_MODULE;
	ret = cdev_add(&test_cdev, device_no, 1);
	if(ret < 0)
	{
		pr_err("Registration with VFS failed \n");
		goto cdev_failure;
	}
	/*3. Creating Class under /sys/class */
	test_class = class_create("char-test-class");
	if(IS_ERR(test_class))
	{
		pr_err("Class creation failed \n");
		ret = PTR_ERR(test_class);
		goto class_failure;
	}
	/*4. Creating device sys/class/*/
	test_device = device_create(test_class, NULL, device_no,NULL, "test_device_1");
	if(IS_ERR(test_device))
	{
		pr_err("Device creation failed \n");
		ret = PTR_ERR(test_device);
		goto device_failure;
	}
	
	pr_info("Module loaded successfully\n");
	return 0;	

device_failure:
	device_destroy(test_class, device_no);
class_failure:
	class_destroy(test_class);
cdev_failure:
	cdev_del(&test_cdev);
alloc_failure:
	unregister_chrdev_region(device_no, 1);
	return ret;

}


static void __exit my_kernel_module_exit(void)
{

	device_destroy(test_class, device_no);
	class_destroy(test_class);
	cdev_del(&test_cdev);
	unregister_chrdev_region(device_no, 1);
	
	pr_info("Module removed successfully\n");

}

module_init(my_kernel_module_init);
module_exit(my_kernel_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AHASAS");
MODULE_DESCRIPTION("Test Module");
MODULE_INFO(board, "Beaglebone black REV C3");
