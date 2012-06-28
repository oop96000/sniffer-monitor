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
#define __KERNEL_SYSCALLS__
#define MY_FILE  "/home/administrator/log_file.log"

int tmp = 0;
int a(char *buff){
    struct file* filp = NULL;
    mm_segment_t old_fs;	
filp = filp_open(MY_FILE,O_CREAT|O_RDWR,777);   //创建文件    
if (filp)
        {
            old_fs = get_fs();
            set_fs(get_ds());
            filp->f_op->write(filp,buff,strlen(buff),&filp->f_pos);  //写文件
            set_fs(old_fs);
	    filp_close(filp,NULL);
            }
    return 0;
}
static int write_log_file(char *filename, char *data_w) {
    mm_segment_t fs;
    struct file *nef_filp = NULL;
    struct inode *inode = NULL;
    loff_t nef_size = 0;
    loff_t tmp_file_offset;
    ssize_t nwritten;
    int rc = -EINVAL;
    int str_len;
    fs = get_fs();
    set_fs(get_ds());
    nef_filp = filp_open(filename, O_CREAT | O_RDWR, 0);
    if (IS_ERR(nef_filp)) {
        printk(KERN_ERR "filp_open error: ""Unable to open nand emu file!\n");
        return PTR_ERR(nef_filp);
    }
    if (nef_filp->f_path.dentry) {
        inode = nef_filp->f_path.dentry->d_inode;
    } else {
        printk(KERN_ERR "Invalid " "nef_filp->f_path.dentry value!\n");
        goto out;
    }
    nef_size = i_size_read(inode->i_mapping->host);
    if (nef_size < 0) {
        printk(KERN_ERR "Invalid ""nand emu file size: 0x%llx\n", nef_size);
        goto out;
    } else {
        printk(KERN_ERR "nand emu file size: ""%lld\n", nef_size);
    }
    str_len = strlen(data_w) + 1;
    tmp_file_offset = nef_size;
    nwritten = vfs_write(nef_filp,
                         (char __user *)data_w,
                         str_len, &tmp_file_offset);
    if (nwritten < str_len) {
        printk(KERN_ERR "%s, Line %d - ""nand emu file partial write: ""%d bytes\n", __FILE__, __LINE__, (int)nwritten);
        goto out;
    }
    rc = 0;
    out:
        filp_close(nef_filp, current->files);
    set_fs(fs);
    return rc;
}
/* 用于注册我们的函数的数据结构 */
static struct nf_hook_ops nfho;

/* 我们要丢弃的数据包来自的地址，网络字节序 */
static unsigned char *drop_ip = "\x7f\x00\x00\x01";
    struct file* filp = NULL;
/* 注册的hook函数的实现 */
unsigned int hook_func(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
char *buff = "12345656";

    mm_segment_t old_fs;	
old_fs = get_fs();
            set_fs(get_ds());

//filp = filp_open(MY_FILE,O_RDWR,0);   //创建文件   
//filp_close(filp,NULL);

vfs_write(filp,buff,strlen(buff),&filp->f_pos);     
set_fs(old_fs); 
/*if (filp)
        {
            old_fs = get_fs();
            set_fs(get_ds());
            filp->f_op->write(filp,buff,strlen(buff),&filp->f_pos);  //写文件
            set_fs(old_fs);
	    filp_close(filp,NULL);
            }*/
        return NF_ACCEPT;
}

/* 初始化程序 */
int init_module()
{
    /* 填充我们的hook数据结构 */
    nfho.hook       = hook_func;         /* 处理函数 */
    nfho.hooknum  = NF_INET_PRE_ROUTING; /* 使用IPv4的第一个hook */
    nfho.pf       = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;   /* 让我们的函数首先执行 */

filp = filp_open(MY_FILE,O_RDWR,0);   //创建文件   
    nf_register_hook(&nfho);
    return 0;
}

/* 清除程序 */
void cleanup_module()
{
    nf_unregister_hook(&nfho);
}

