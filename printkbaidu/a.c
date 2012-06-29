#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>                  /* For IP header */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#define NF_IP_PRE_ROUTING 0
/* 用于注册我们的函数的数据结构 */
static struct nf_hook_ops nfho;

/* 我们要丢弃的数据包来自的地址，网络字节序 */
unsigned char *deny_ip = "/xac/x10/xe0/x5d";  /* 127.0.0.1 */

/* 注册的hook函数的实现 */
unsigned int hook_func(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
              if (!skb )return NF_ACCEPT;
              if (!(ip_hdr(skb)))  return NF_ACCEPT;     
              if (ip_hdr(skb)->saddr == *(unsigned int *)deny_ip) { 
printk("baidu.com\n");
              return NF_DROP;
              }

              return NF_ACCEPT;
}

/* 初始化程序 */
int init_module()
{
    /* 填充我们的hook数据结构 */
    nfho.hook       = hook_func;         /* 处理函数 */
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

