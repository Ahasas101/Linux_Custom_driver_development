#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include "platform.h"
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include<linux/of.h>
#include<linux/of_device.h>



#define RDONLY	0x01
#define WRONLY	0x10
#define RDWR	0x11

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__



/*Device private data structure*/
struct pcdev_private_data
{
	struct pcdev_platform_data pdata;
	char *buffer;
	dev_t dev_num;
	struct cdev cdev;
};

/*Driver private data structure*/
struct pcdrv_private_data
{
	int total_devices;
	dev_t device_num_base;
	struct class *class_pcd;
	struct  device *device_pcd;
};

struct pcdrv_private_data pcdrv_data;


enum pcdev_names
{
	PCDEVA1X,
	PCDEVB1X,
	PCDEVC1X,
	PCDEVD1X
};

struct device_config
{
	int config_item1;
	int config_item2;

};

struct device_config pcdev_config[] = {
	[PCDEVA1X] = {.config_item1 = 60, .config_item2 = 21},
	[PCDEVB1X] = {.config_item1 = 65, .config_item2 = 22},
	[PCDEVC1X] = {.config_item1 = 30, .config_item2 = 23},
	[PCDEVD1X] = {.config_item1 = 90, .config_item2 = 25}

};

loff_t pcd_lseek (struct file *filp, loff_t offset, int whence);
ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int check_permission(int dev_perm, int acc_mode);
int pcd_open (struct inode *inode, struct file *filp);
int pcd_release (struct inode *inode, struct file *filp);
int pcd_platform_driver_probe(struct platform_device *pdev);
void pcd_platform_driver_remove(struct platform_device *pdev);





loff_t pcd_lseek (struct file *filp, loff_t offset, int whence)
{	
	struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*)filp->private_data;
	
	int max_size = pcdev_data->pdata.size;
	
	loff_t temp;
	
	pr_info("lseek requested\n");
	pr_info("Current value of file position = %lld\n",filp->f_pos);
	
	switch(whence)
	{
	case SEEK_SET:
		if( (offset > max_size) || (offset < 0))
			return -EINVAL;
		filp->f_pos=offset;
	break;
	
	case SEEK_CUR:
		temp = filp->f_pos + offset;
		if((temp > max_size) || (temp < 0))
			return -EINVAL;
			
		filp->f_pos = temp;
	break;
	
	case SEEK_END:
		temp = max_size + offset;
		if((temp > max_size) || (temp < 0))
			return -EINVAL;
		filp->f_pos = temp;
	break;
	
	default:
		return -EINVAL;
	}
	
	pr_info("New value of the file position = %lld\n",filp->f_pos);
	return filp->f_pos;
	
}

ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*)filp->private_data;
	int size = pcdev_data->pdata.size;
	pr_info("Write requested for %zu bytes\n",count);
	pr_info("current file position = %lld\n", *f_pos);
	if(count + *f_pos > size)
	{
		count = size - *f_pos;
	}
	
	if(copy_to_user(buff, (pcdev_data->buffer + *f_pos), count))
	{
		return -EFAULT;
	}
	*f_pos += count;
	pr_info("Number of bytes successfully read = %zu\n",count);
	pr_info("Update file position = %lld\n",*f_pos);
	
	/* Return number of bytes which have been successfully read*/
	return count;
	
	
		return -EFAULT;
	
}
ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{	
	struct pcdev_private_data *pcdev_data= (struct pcdev_private_data*)filp->private_data;
	int size = pcdev_data->pdata.size;
	pr_info("Write requested for %zu bytes\n",count);
	pr_info("current file position = %lld\n", *f_pos);
	if(count + *f_pos > size)
	{
		count = size - *f_pos;
	}
	if(!count) {
		pr_err("No space left on the device\n");
		return -ENOMEM;
	}
	if(copy_from_user(pcdev_data->buffer+(*f_pos), buff, count))
	{
		return -EFAULT;
	}
		*f_pos += count;
	
	pr_info("Number of bytes successfully written = %zu\n",count);
	pr_info("Update file position = %lld\n",*f_pos);
	pr_info("write requested for %zu bytes\n",count);
	
	return count;
	
}



int check_permission(int dev_perm, int acc_mode)
{
	if(dev_perm == RDWR)
		return 0;
	if( (dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE) ))
		return 0;
	
	if( (dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ) ))
		return 0;
		
	return -EPERM;
}


int pcd_open (struct inode *inode, struct file *filp)
{	int ret;
	int minor_n;
	minor_n = MINOR(inode->i_rdev);
	pr_info("Device Minor no. = %d", minor_n);
	
	struct pcdev_private_data *pcdev_data;
	pcdev_data = container_of(inode->i_cdev, struct pcdev_private_data, cdev);
	filp->private_data = pcdev_data;
	ret = check_permission(pcdev_data->pdata.perm , filp->f_mode);
	(!ret)?pr_info("open was successful\n") : pr_info("open was unsuccessful\n");
	return ret;
}

int pcd_release (struct inode *inode, struct file *filp)
{
	pr_info("close was successful\n");
	return 0;
}

/* file operations of the driver*/

struct file_operations pcd_fops =
{
	.open = pcd_open,
	.write = pcd_write,
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
	.owner = THIS_MODULE
};

struct pcdev_platform_data* pcdev_get_platdata_from_dt(struct device *dev)
{
	struct device_node *dev_node = dev->of_node;
	struct pcdev_platform_data *pdata;
	
	if(!dev_node) {return NULL;}
		/*this probe didnt happed because of device tree node*/
		
	pdata = devm_kzalloc(dev,sizeof(*pdata),GFP_KERNEL);
	if(!pdata){
		dev_info(dev,"Cannot allocate memory \n");
		return ERR_PTR(-ENOMEM);
	}
	if(of_property_read_string(dev_node, "org,device-serial-num",&pdata->serial_number)){
		dev_info(dev,"Missing serial number property\n");
		return ERR_PTR(-EINVAL);
	}
	
	if(of_property_read_u32(dev_node,"org,size",&pdata->size)){
		dev_info(dev,"Missing size property\n");
		return ERR_PTR(-EINVAL);
	}
	
	if(of_property_read_u32(dev_node,"org,perm",&pdata->size)){
		dev_info(dev,"Missing permission property\n");
		return ERR_PTR(-EINVAL);
	}
	
	return pdata;

}
/*This is called when matched platform device is found*/
int pcd_platform_driver_probe(struct platform_device *pdev)
{
	pr_info("Device detected");
	
	int ret; 
	struct pcdev_private_data *dev_data;
	struct pcdev_platform_data *pdata;
	//struct of_device_id *match;
	
	struct device *dev = &pdev->dev;
	int driver_data;
	
	dev_info(dev,"Device detected");
	
	pdata = pcdev_get_platdata_from_dt(&pdev->dev);
	if(IS_ERR(pdata))
		return PTR_ERR(pdata);
		
	if(!pdata)
	{
		/*1. Get the platform data as device was not instantiated using dt*/
		pdata = pdev->dev.platform_data;
		/*You can also use the below inbuilt macro to obtain the data*/
		//pdata =(struct pcdev_platform_data*)dev_get_platdata(&pdev->dev);
		if(!pdata){
			pr_info("No platform data available\n");
			ret = -EINVAL;
			goto out;
		}	
		driver_data =  pdev->id_entry->driver_data;
	}
	else
	{
		 //match = of_match_device(pdev->dev.driver->of_match_table, &pdev->dev);
		 //driver_data = (int)match->data;
		 //or
		 driver_data = (int) of_device_get_match_data(&pdev->dev);
	 
	}
	
	
	
	/*2. Dynamically allocate memmory for the device private data*/
	dev_data = kzalloc(sizeof(struct pcdev_private_data), GFP_KERNEL);
	if(!dev_data){
		dev_info(dev, "Cannot allcoate memory \n");
		ret = -ENOMEM;
		goto out;
	}
	
	// we are doing this as we need free dev_data in the remove function and it is local variable so it is passed like this 
	//do this or you can use helper function .
	//pdev->dev.driver_data = dev_data;
	dev_set_drvdata(&pdev->dev,dev_data);
	 
	dev_data->pdata.size = pdata->size;
	dev_data->pdata.perm = pdata->perm;
	dev_data->pdata.serial_number = pdata->serial_number;
	
	/*pr_info("Device serial number = %s\n",dev_data->pdata.serial_number);
	pr_info("Device size = %d\n",dev_data->pdata.size);
	pr_info("Device permission = %d\n",dev_data->pdata.perm);*/
	dev_info(dev,"Device serial number = %s\n",pdata->serial_number);
	dev_info(dev,"Device size = %d\n",pdata->size);
	dev_info(dev,"Device permission = %d\n",pdata->perm);
	dev_info(dev,"Config item 1 = %d\n", pcdev_config[driver_data].config_item1);
	dev_info(dev,"Config item 2 = %d\n", pcdev_config[driver_data].config_item2);
	
	/*3. Dynamically allocate memory for the device buffer using size information from the platform data*/
	dev_data->buffer = kzalloc(dev_data->pdata.size, GFP_KERNEL);
	if(!(dev_data->buffer)){
		dev_info(dev,"Cannot allcoate memory \n");
		ret = -ENOMEM;
		goto dev_data_free;
	}
	
	/*4 Get the device number*/
	dev_data->dev_num = pcdrv_data.device_num_base + pcdrv_data.total_devices;
	
	/*5 Do cdev init and cdev add*/
	cdev_init(&dev_data->cdev, &pcd_fops);
	dev_data->cdev.owner = THIS_MODULE;
	ret = cdev_add(&dev_data->cdev, dev_data->dev_num,1);
	if(ret < 0)
	{
		dev_err(dev,"Cdev add failed \n");
		goto buffer_free;
	}
	
	/*6 Create device file for the detected platform device*/
	pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd,dev,dev_data->dev_num,NULL,"pcdev-%d",pcdrv_data.total_devices);
	if(IS_ERR(pcdrv_data.device_pcd))
	{
		dev_err(dev,"Device create failed\n");
		ret = PTR_ERR(pcdrv_data.device_pcd);
		goto cdev_del;
	}
	
	
	pcdrv_data.total_devices++;
	dev_info(dev,"Probing successful \n");
	return 0;
	


cdev_del:
	cdev_del(&dev_data->cdev);
buffer_free:
	kfree(dev_data->buffer);
		
dev_data_free:
 	kfree(dev_data);	

out:
	dev_info(dev,"Device probe failed\n");
	return ret;  
	



}

/*This is called when the device is removed from the system*/
void pcd_platform_driver_remove(struct platform_device *pdev)
{

	struct pcdev_private_data *dev_data = dev_get_drvdata(&pdev->dev);
	//alternatively: struct pcdev_private_data *dev_data = pdev->dev.driver;
	
	/*1 Remove a devie that was created with device_create()*/
	device_destroy(pcdrv_data.class_pcd,dev_data->dev_num);

	/*2 Remove a cdev entry from the system */
	cdev_del(&dev_data->cdev);
	
	/*3 Free the memory held by the device*/
	kfree(dev_data->buffer);
	kfree(dev_data);
	dev_info(&pdev->dev,"A device is removed \n");
	
	pcdrv_data.total_devices--;

	


}



struct platform_device_id pcdevs_ids[] = {
	[0] = {.name = "pcdev-A1x", .driver_data = PCDEVA1X},
	[1] = {.name = "pcdev-B1x", .driver_data = PCDEVB1X},
	[2] = {.name = "pcdev-C1x", .driver_data = PCDEVC1X},
	{ }
	
};

struct of_device_id org_pcdev_dt_match[]=
{
	{.compatible = "pcdev-A1x", .data = (void*)PCDEVA1X},
	{.compatible = "pcdev-B1x", .data = (void*)PCDEVB1X},
	{.compatible = "pcdev-C1x", .data = (void*)PCDEVC1X},
	{.compatible = "pcdev-D1x", .data = (void*)PCDEVD1X},
	{}
};

struct platform_driver pcd_platform_driver = {
	.probe = pcd_platform_driver_probe,
	.remove = (void*)pcd_platform_driver_remove,
	.id_table = pcdevs_ids,
	.driver = {
		.name = "pseudo-char-device",
		.of_match_table = org_pcdev_dt_match
	}
};

#define MAX_DEVICES	10 

static int __init pcd_driver_init(void)
{	
	int ret; 
	/*1. Dynamically allocate a device number for MAX_DEVICES*/
	ret = alloc_chrdev_region(&pcdrv_data.device_num_base,0,MAX_DEVICES,"pcdevs");
	if(ret < 0){
		pr_err("Alloc chrdev region failed\n");
	}
	
	/*2. Creating device class under /sys/class */
	pcdrv_data.class_pcd = class_create("pcd_class");
	if(IS_ERR(pcdrv_data.class_pcd)){
		pr_err("Class creation failed \n");
		ret = PTR_ERR(pcdrv_data.class_pcd);
		unregister_chrdev_region(pcdrv_data.device_num_base,MAX_DEVICES);
		return ret; 
	}
	
	/*3. Register a platform driver*/
	
	platform_driver_register(&pcd_platform_driver);
	pr_info("pcd platform driver loaded \n");
	return 0;
}

static void __exit pcd_driver_cleanup(void)
{	
	/*1.Unregister the platform driver*/
	platform_driver_unregister(&pcd_platform_driver);
	
	/*2. Class destroy*/
	class_destroy(pcdrv_data.class_pcd);
	
	/*3.Unregister device number for MAX_DEVICES*/
	unregister_chrdev_region(pcdrv_data.device_num_base,MAX_DEVICES);
	
	pr_info("pcd platform driver unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AHASAS");
MODULE_DESCRIPTION("A pseudo character driver for multiple devices");
