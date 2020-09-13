/***********************************
* BLG413E-System Programming       *
* Project-1                        *
* Group Number:54                  *
* Group Members:                   *
* Mehmet Gencay Ertürk - 150130118 *
* Bora Yöntem - 150130806          *
* Ahmet Metehan Yaman - 150140030  *
************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/moduleparam.h>
static char guess[4],*mmind_number,message[4096];
static int mmind_max_guesses=10,cur=0;
int major=0;
module_param(mmind_max_guesses,int,0000);
module_param(mmind_number,charp,0000);
volatile static int is_open=0;
static int mastermind_open(struct inode* inodep,struct file* filep)
{
	if(is_open==1)
	{
		printk(KERN_INFO "Error! Device is already open.\n");
		return -EBUSY;
	}
	is_open=1;
	try_module_get(THIS_MODULE);
	return 0;
}
static int mastermind_release(struct inode* inodep,struct file* filep)
{
	if(is_open==0)
	{
		printk(KERN_INFO "Error! Device is not open.\n");
		return -EBUSY;
	}	
	is_open=0;
	module_put(THIS_MODULE);
	return 0;
}
ssize_t mastermind_read(struct file* filep,char __user* outb,size_t nbytes,loff_t* offset)
{
	int bytes_read=0,i,j,k=16*cur;
	char true_location='0',false_location='0';
	if(cur==mmind_max_guesses) 
	{
		printk(KERN_INFO "Error! Quota exceeded.\n");
		return -EDQUOT;
	}
	for(i=0;i<4;i++)
	{
		while(true)
		{
			if(mmind_number[i]==guess[i])
			{
				true_location++;
				break;
			}
			else
			{
				for(j=0;j<4;j++)
				{
					if(i!=j && mmind_number[i]==guess[j]){false_location++;}
				}
				break;
			}	
		}
	}
	
	cur++;
	for(i=0;i<4;i++){message[k+i]=guess[i];}
	message[k+4]=' ';
	message[k+5]=true_location;
	message[k+6]='+';
	message[k+7]=' ';
	message[k+8]=false_location;
	message[k+9]='-';
	message[k+10]=' ';
	message[k+11]='0';
	message[k+12]='0'+(cur/100);
	message[k+13]='0'+((cur%100)/10);
	message[k+14]='0'+(cur%10);
	message[k+15]='\n';
	if(offset==NULL){return -EINVAL;}
	if(*offset>=k+16)	{return -EINVAL;}
	while((bytes_read<nbytes) && (*offset<k+16))
	{
		copy_to_user(&outb[bytes_read],&message[*offset],1);
		*offset=*offset+1;
		bytes_read++;
	}
	return bytes_read;
}
ssize_t mastermind_write(struct file* filep,const char __user* inpb,size_t nbytes,loff_t* offset)
{
	int bytes_write=0;
	if(offset==NULL){return -EINVAL;}
	if(*offset>=4){return -EINVAL;}
	while((bytes_write<nbytes) && (*offset<4))
	{
		copy_from_user(&guess[*offset],&inpb[bytes_write],1);
		*offset=*offset+1;
		bytes_write++;
	}
	return bytes_write;
}
struct file_operations fops = {
	open: mastermind_open,
	release: mastermind_release,
	read: mastermind_read,
	write: mastermind_write
};
static int __init mastermind_start(void)
{
	int x;
	major=register_chrdev(0,"mastermind",&fops);
	if(strlen(mmind_number)!=4)
	{
		printk(KERN_INFO "Error! Number should be 4-digit.\n");
		return -EINVAL;
	}
	for(x=0;x<4;x++)
	{
		if(mmind_number[x]<'0' || mmind_number[x]>'9')
		{
			printk(KERN_INFO "Error! This is not a number.\n");
			return -EINVAL;
		}
	}
	if(mmind_max_guesses>256)
	{
		printk(KERN_INFO "Error! Max number of guesses is 256.\n");
		return -EINVAL;
	}
	return 0;
}
static void __exit mastermind_end(void)
{
	kfree(message);
	unregister_chrdev(major,"mastermind");
}
module_init(mastermind_start);
module_exit(mastermind_end);
