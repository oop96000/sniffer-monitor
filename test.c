#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

static struct nf_hook_ops nfho;
static unsigned char *drop_ip = "\x7f\x00\x00\x01";

unsigned int hook_func(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
       struct sk_buff *sb = *skb;
       struct iphdr     *iph ;
   
       iph = ip_hdr(sb);
       pr_info("Packet from %d.%d.%d.%d\n",NIPQUAD(iph->saddr));
       if ( iph->saddr == *(__be32 *) drop_ip)
       {
             pr_info("Dropped packet from ... %d.%d.%d.%d\n",*drop_ip, *(drop_ip+1), *(drop_ip+2), *(drop_ip+3) );
             return NF_DROP;
       }else {
           
             return NF_ACCEPT;
       }
}

int init_module()
{
       pr_info("i'm now in the kernel space!\n");
       nfho.hook      = hook_func;
       nfho.hooknum   = NF_IP_PRE_ROUTING;
       nfho.pf                = PF_INET;
       nfho.priority      = NF_IP_PRI_FIRST;
   
       nf_register_hook(&nfho);

       return 0;
}

void cleanup_module()
{
    nf_unregister_hook(&nfho);
    pr_info("module removed from kernel!\n");
}
