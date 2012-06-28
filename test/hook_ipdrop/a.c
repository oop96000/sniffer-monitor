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
#include <linux/in.h> 
#include <linux/socket.h>  
#include <linux/inet.h> 
#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]
#include <linux/inet.h> 
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
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>

#define MY_FILE  "/home/administrator/log_file.log"
#define READ_FILE  "/home/administrator/log_file.txt"
char *read_buf;
char *store;
char *tmp;
char list[100][100];
unsigned int ip;
unsigned int ip_list[100];
struct file* filp = NULL;
mm_segment_t old_fs;
int count;
/* 用于注册我们的函数的数据结构 */
static struct nf_hook_ops nfho;

/* 我们要丢弃的数据包来自的地址，网络字节序 */
static char *deny_ip = "\x7f\x00\x00\x01";

int chuli(char *tmp){
printk("!!!!!!!!!!!!!!!!%s",tmp);
//char *tmp =&buff[0];
int i = 0,j = 0;
while(tmp[0] != '\0'){
printk("@@@@@@@");
j = 0;
//sscanf(tmp, "%[^'\n']", list[i]); 
while(tmp[j]!='\n'){
list[i][j]=tmp[j];
j++;
//printf("%c@",tmp[j]);
}
//list[i][strlen(list[i])] = '\0';
tmp = strstr(tmp,"\n");
tmp++;
printk("%s",list[i++]);
}
//fflush(stdout);
return i;
}

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

    filp = filp_open(READ_FILE,O_RDWR,777);   //创建文件    
    off_t size = filp->f_path.dentry->d_inode->i_size;
    read_buf = (char *)kmalloc(size,GFP_KERNEL);
    if (filp)
    {
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        filp->f_op->read(filp,read_buf,size,&filp->f_pos);  //写文件
        //vfs_write(filp,buff,strlen(buff),&filp->f_pos);
        read_buf[size] = '\0';
int jj = chuli(read_buf);
count = jj;
printk("TTTTTTTTTT%s",list[0]);
int i = 0;

        set_fs(old_fs);
        //printk("!!!%d",(int)size);
int m = 0;
for(m = 0;m<jj;m++){
ip_list[m] = in_aton(list[m]);
}
        ip = in_aton(list[0]);
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
    //printk("###%s",read_buf);
    int i;
struct timex  txc;
struct rtc_time tm;
do_gettimeofday(&(txc.time));
rtc_time_to_tm(txc.time.tv_sec,&tm);
//printk("UTC time :%d-%d-%d %d:%d:%d \n",tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
// struct sk_buff *a = *skb;
for(i = 0;i<count;i++)
//printk("COUNT:%d",count);
    if (ip_hdr(skb)->saddr == ip_list[i]) {
        sprintf(tmp,"Dropped packet from... %s\tUTC time :%d-%d-%d %d:%d:%d \n",list[i],tm.tm_year+1900,tm.tm_mon+1, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        strcat(store,tmp);
        tmp[0] = '\n';
        //        printk("Dropped packet from... %d.%d.%d.%d/n",*drop_ip, *(drop_ip + 1),*(drop_ip + 2), *(drop_ip + 3));
        return NF_DROP;
    }
//      sprintf(tmp,"Receive packet from... %s\tUTC time :%d-%d-%d %d:%d:%d \n",NIPQUAD(ip_hdr(*skb)->saddr),tm.tm_year+1900,tm.tm_mon+1, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
sprintf(tmp,"Receive packet from... %s\tUTC time :%d-%d-%d %d:%d:%d \n","@@@@@@@@@@@@",tm.tm_year+1900,tm.tm_mon+1, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
//printk("%s",(char *)(ip_hdr(a)->saddr));
        strcat(store,tmp);
        tmp[0] = '\n';
  return NF_ACCEPT;
    
}

/* 初始化程序 */
int init_module()
{
    /* hook数据结构 */
    nfho.hook       = hook_func;         /* 处理函数 */
    nfho.hooknum  = NF_INET_PRE_ROUTING; /* 使用IPv4的第一个hook */
    nfho.pf       = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;   /* 先执行 */
    
read();


    store = (char *)kmalloc(65536,GFP_KERNEL);
    tmp = (char *)kmalloc(1000,GFP_KERNEL);
memset(store,0,65536);
memset(tmp,0,1000);
    nf_register_hook(&nfho);

    return 0;
}

/* 清除程序 */
void cleanup_module()
{
    printk("wocao%s",store);
    write(store);
    nf_unregister_hook(&nfho);
}

