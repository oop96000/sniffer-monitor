#define __KERNEL__
#define MODULE
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#define NF_IP_PRE_ROUTING	0
/* 用于注册我们的函数的数据结构 */
static struct nf_hook_ops nfho;

/* 我们丢弃的数据包来自的接口的名字 */
static char *drop_if = "\x00\x19";

/* 注册的hook函数的实现 */
unsigned int hook_func(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
    if (strcmp(in->name, drop_if) == 0) {
        printk("Dropped packet on %s.../n", drop_if);
        return NF_DROP;
    } else {
        return NF_ACCEPT;
    }
}

/* 初始化程序 */
int init_module()
{
    /* 填充我们的hook数据结构 */
    nfho.hook     = hook_func;         /* 处理函数 */
    nfho.hooknum  = NF_IP_PRE_ROUTING; /* 使用IPv4的第一个hook */
    nfho.pf       = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;   /* 让我们的函数首先执行 */

    nf_register_hook(&nfho);
    
    return 0;
}
    
/* 清除程序 */
void cleanup_module()
{
    nf_unregister_hook(&nfho);
}
