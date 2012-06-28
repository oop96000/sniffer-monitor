#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x60d90935, "module_layout" },
	{ 0x50eedeb8, "printk" },
	{ 0x75307f84, "filp_close" },
	{ 0x3de31dbd, "vfs_write" },
	{ 0x52412bcc, "filp_open" },
	{ 0xd0d8621b, "strlen" },
	{ 0x28d179d0, "vfs_read" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B77A9671659572A64B15E8E");
