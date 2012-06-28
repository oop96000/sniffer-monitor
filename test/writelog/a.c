#define __KERNEL__
#define MODULE
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

int write(char *buff){
    filp = filp_open(MY_FILE,O_CREAT|O_RDWR,777);   //创建文件    
if (filp)
        {
            old_fs = get_fs();
            set_fs(KERNEL_DS);
            //filp->f_op->write(filp,buff,strlen(buff),&filp->f_pos);  //写文件
vfs_write(filp,buff,strlen(buff),&filp->f_pos);            
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
int b(){
printk("!!!%s",read_buf);  

return 0;
}
int init_module(void)
{
read();
b();
return 0;
}
void cleanup_module(void)
{
    printk(KERN_INFO"Unloading the module...KK...\n");
    return ;
}

