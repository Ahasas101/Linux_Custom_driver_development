#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>


static int __init my_kernel_module_init(void)
{
	pr_info("Module loaded successfully\n");	
	return 0;
}


static void __exit my_kernel_module_exit(void)
{

	pr_info("Module removed successfully\n");

}

module_init(my_kernel_module_init);
module_exit(my_kernel_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AHASAS");
MODULE_DESCRIPTION("Test Module");
MODULE_INFO(board, "Beaglebone black REV C3");
