#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x92997ed8, "_printk" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0xea341aa5, "device_destroy" },
	{ 0xe9d84471, "cdev_del" },
	{ 0x37a0cba, "kfree" },
	{ 0xec094555, "_dev_info" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x8c503be5, "class_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xcaac2ecc, "__platform_driver_register" },
	{ 0xde770c48, "platform_driver_unregister" },
	{ 0x7a84233c, "class_destroy" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x5f754e5a, "memset" },
	{ 0x1d52fd96, "devm_kmalloc" },
	{ 0x31221d01, "of_property_read_string" },
	{ 0x788fefec, "of_property_read_variable_u32_array" },
	{ 0xee6be8f1, "of_device_get_match_data" },
	{ 0xb6ebc6f0, "kmalloc_caches" },
	{ 0x235b7ab5, "kmalloc_trace" },
	{ 0x2d6fcc06, "__kmalloc" },
	{ 0xdd039098, "cdev_init" },
	{ 0xdd5d8d12, "cdev_add" },
	{ 0x7d00c2e3, "_dev_err" },
	{ 0x2a894890, "device_create" },
	{ 0x6d62e7a5, "module_layout" },
};

MODULE_INFO(depends, "");

