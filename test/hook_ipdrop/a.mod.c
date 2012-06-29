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
	{ 0xb435d865, "nf_unregister_hook" },
	{ 0x6408b9d3, "nf_register_hook" },
	{ 0x2bc95bd4, "memset" },
	{ 0xba236513, "kmem_cache_alloc_trace" },
	{ 0xc6177942, "kmalloc_caches" },
	{ 0x86a4889a, "kmalloc_order_trace" },
	{ 0x1b6314fd, "in_aton" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x75307f84, "filp_close" },
	{ 0xd0d8621b, "strlen" },
	{ 0x52412bcc, "filp_open" },
	{ 0x1e6d26a8, "strstr" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x50eedeb8, "printk" },
	{ 0x61651be, "strcat" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0xabd0c91c, "rtc_time_to_tm" },
	{ 0x1d2e87c6, "do_gettimeofday" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B0EF73E10F2BF8A734E5DB0");
