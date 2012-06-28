
#define __KERNEL__
#define MODULE

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#define NF_IP_PRE_ROUTING	0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <asm/fcntl.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/types.h>
#include<linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#define MY_FILE  "/home/administrator/log_file.log"
    char *read_buf;
    struct file* filp = NULL;
    mm_segment_t old_fs;
/* 用于注册我们的函数的数据结构 */
static struct nf_hook_ops nfho;
int write(char *buff){
    filp = filp_open(MY_FILE,O_CREAT|O_RDWR,777);   //创建文件    
if (filp)
        {
            old_fs = get_fs();
            set_fs(KERNEL_DS);
            filp->f_op->write(filp,buff,strlen(buff),&filp->f_pos);  //写文件
//vfs_write(filp,buff,strlen(buff),&filp->f_pos);            
set_fs(old_fs);
            filp_close(filp,NULL);
            }
    return 0;
}
int read(){

    filp = filp_open(MY_FILE,O_RDWR,777);   //创建文件    
off_t size = filp->f_path.dentry->d_inode->i_size;
read_buf = (char *)kmalloc(size,GFP_KERNEL);
if (filp)
        {
            old_fs = get_fs();
            set_fs(KERNEL_DS);
            filp->f_op->read(filp,read_buf,size,&filp->f_pos);  //写文件
//vfs_write(filp,buff,strlen(buff),&filp->f_pos);       
   read_buf[size] = '\0';
set_fs(old_fs);
//printk("!!!%d",(int)size);  

            filp_close(filp,NULL);
            }
    return 0;
}

/* 注册的hook函数的实现 */
unsigned int hook_func(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
printk("!!!%s",read_buf);  
    return NF_DROP;           /* 丢弃所有的数据包 */
}


/* 初始化程序 */
int init_module()
{
    /* 填充我们的hook数据结构 */
    nfho.hook = hook_func;         /* 处理函数 */
    nfho.hooknum  = NF_IP_PRE_ROUTING; /* 使用IPv4的第一个hook */
    nfho.pf       = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;   /* 让我们的函数首先执行 */
read();
    nf_register_hook(&nfho);

    return 0;
}

/* 清除程序 */
void cleanup_module()
{
    nf_unregister_hook(&nfho);
}

