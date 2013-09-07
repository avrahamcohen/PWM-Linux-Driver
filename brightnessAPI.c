#include <linux/kernel.h> // Standard
#include <linux/module.h> // Always Needed For Modules !
#include <linux/init.h>  // Use For PRINTK();
#include <linux/types.h> // Use For 'struct file_operarions'
#include <linux/fs.h> // Handling FileSystem
#include <linux/miscdevice.h> // Use For Minor
#include <asm/uaccess.h> // Use for access user-space
#include <asm/io.h> //Use for read and write registers
#include "brightnessAPI.h"

#if (__GNUC__>=3) && (__GNUC_MINOR__>=4)
#include <linux/vermagic.h>
#endif

MODULE_AUTHOR("Avi Cohen, Linux4biz ltd.  avic@linux4biz.com");
MODULE_DESCRIPTION("PWM kernel module for OMAP3530");
MODULE_LICENSE("GPL");

#define TCLR 0x48086024 //Disables/Enableds the timer completely before any configuration changes.
#define CONTROL_PADCONF_UART2_CTS 0x48002176 //Change the PAD multiplexing to allow PWM10 off this pin instead of its current configuration as GPIO_145.
#define TLDR 0x4808602C //Set the frequency of the PWM signal to 1024 Hz.
#define TMAR 0x48086038 //This sets the duty cycle of the PWM signal to 50%, ON time one half of cycle time.
#define TCRR 0x48086028 //This primes the timer counter for the first cycle. Primed with the same value as the frequency.
#define READ_BIT_FROM_USER 1
#define READ_BIT_TO_USER 1
#define TOUCH_LONG 4
#define TOUCH_SHORT 2


/* Prototype */
static void initPin(void);
static ssize_t module_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t module_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos);

/* Globals */
static int _major;
static int _minor;
static char fromUser;

/* Kernel Structures */
static struct file_operations mod_fops = {
	.owner = THIS_MODULE,
	.read = module_read,
	.write = module_write,
};

static struct miscdevice mod_misc = {
	MISC_DYNAMIC_MINOR,
	"brightnessMinor",
	&mod_fops,
};

int __init load_module(void)
{
	  _major = register_chrdev (0,"brightnessMajor",&mod_fops);
	  if (_major < 0) {
		  printk("Registering the character device failed with (Major)-  %d\n",  _major);
		  return _major;
	  }
	  if (_major > 0) {
		  printk("Registering the character device success with (Major)-  %d\n",  _major);
	  }

	  _minor=misc_register(&mod_misc);

	  if (_minor){
		printk("Registering the character device failed with (Minor) -  %d\n",  _minor);
		return -EIO;
	  }
	  printk("Registering the character device success with (Minor) -  %d\n",  _minor);

	  initPin();

	  printk(KERN_ALERT "brightness Module Load Successfuly\n");
	  return 0;
}

static void initPin(void){
	int * ptr;
	
	//Disables the timer completely before any configuration changes.
	ptr = ioremap_nocache(TCLR,TOUCH_LONG);
	iowrite32(0x0,ptr);
	iounmap(ptr);

	//Change the PAD multiplexing to allow PWM10 off this pin instead of its current configuration as GPIO_145.
	ptr = ioremap_nocache(CONTROL_PADCONF_UART2_CTS,TOUCH_SHORT);
	iowrite16(0x0002,ptr);
	iounmap(ptr);

	//Set the frequency of the PWM signal to 1024 Hz.
	ptr = ioremap_nocache(TLDR,TOUCH_LONG);
	iowrite32(0xFFFFFFE0,ptr);
	iounmap(ptr);

	//This sets the duty cycle of the PWM signal to 100%, ON time one half of cycle time.
	ptr = ioremap_nocache(TMAR,TOUCH_LONG);
	iowrite32(0x0,ptr);
	iounmap(ptr);

	//This primes the timer counter for the first cycle. Primed with the same value as the frequency.
	ptr = ioremap_nocache(TCRR,TOUCH_LONG);
	iowrite32(0xFFFFFFE0,ptr);
	iounmap(ptr);

	//This starts the timer in PWM mode.
	ptr = ioremap_nocache(TCLR,TOUCH_LONG);
	iowrite32(0x01843,ptr);
	iounmap(ptr);

}

void __exit unload_module(void){
	unregister_chrdev(_major, "brightnessMajor");
	misc_deregister(&mod_misc);
	printk(KERN_ALERT "Module Unload !!! \n");
}

static ssize_t module_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){

	ssize_t retval = 1;
	
	printk(KERN_ALERT "\n	Value in kernel is : %d\n",fromUser);
	if (__copy_to_user(buf,&fromUser,READ_BIT_TO_USER))
	{
		retval = -EFAULT;
		return retval;
	}
	

	return retval;
}
static ssize_t module_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos){
	ssize_t retval = 1;
	int * ptr;
	unsigned int checkData;

	checkData = __copy_from_user(&fromUser,buf,READ_BIT_FROM_USER);

	if(checkData){
		printk(KERN_ALERT "Cant copy from user !!! \n");
		return -EFAULT;
	}
	
	printk(KERN_ALERT "Value From User Is : fromUser %d",(int)fromUser);
	
	
	//Disables the timer completely before any configuration changes.
	ptr = ioremap_nocache(TCLR,TOUCH_LONG);
	iowrite32(0x0,ptr);
	iounmap(ptr);
	//Change the PAD multiplexing to allow PWM10 off this pin instead of its current configuration as GPIO_145.
	ptr = ioremap_nocache(CONTROL_PADCONF_UART2_CTS,TOUCH_SHORT);
	iowrite16(0x0002,ptr);
	iounmap(ptr);
	//Set the frequency of the PWM signal to 1024 Hz.
	ptr = ioremap_nocache(TLDR,TOUCH_LONG);
	iowrite32(0xFFFFFFE0,ptr);
	iounmap(ptr);	
	
	switch((int)fromUser){
		case BRIGHTNESS_LEVEL_1:
			printk(KERN_ALERT "Change to LEVEL 1\n");			
			//This sets the duty cycle of the PWM signal to 0%, ON time one half of cycle time.
			ptr = ioremap_nocache(TMAR,TOUCH_LONG);
			iowrite32(0xFFFFFFFF,ptr);
			iounmap(ptr);
			break;
		case BRIGHTNESS_LEVEL_2:
			printk(KERN_ALERT "Change to LEVEL 2\n");
			//This sets the duty cycle of the PWM signal to 25%, ON time one half of cycle time.
			ptr = ioremap_nocache(TMAR,TOUCH_LONG);
			//iowrite32(0xFFFFFFFE,ptr);
			iowrite32(0xFFFFFFFE,ptr);
			iounmap(ptr);
			break;
		case BRIGHTNESS_LEVEL_3:
			printk(KERN_ALERT "Change to LEVEL 3\n");
			//This sets the duty cycle of the PWM signal to 50%, ON time one half of cycle time.
			ptr = ioremap_nocache(TMAR,TOUCH_LONG);
			iowrite32(0xFFFFFFEF,ptr);
			iounmap(ptr);
			break;
		case BRIGHTNESS_LEVEL_4:
			printk(KERN_ALERT "Change to LEVEL 3\n");
			//This sets the duty cycle of the PWM signal to 75%, ON time one half of cycle time.
			ptr = ioremap_nocache(TMAR,TOUCH_LONG);
			iowrite32(0xFFFFFEFF,ptr);
			iounmap(ptr);
			break;
		case BRIGHTNESS_LEVEL_5:
			printk(KERN_ALERT "Change to LEVEL 5\n");
			//This sets the duty cycle of the PWM signal to 100%, ON time one half of cycle time.
			ptr = ioremap_nocache(TMAR,TOUCH_LONG);
			iowrite32(0x0,ptr);
			iounmap(ptr);
			break;
		default:
			printk(KERN_ALERT "Input from user -ERROR\n");
			break;
	}
	
	//This primes the timer counter for the first cycle. Primed with the same value as the frequency.
	ptr = ioremap_nocache(TCRR,TOUCH_LONG);
	iowrite32(0xFFFFFFE0,ptr);
	iounmap(ptr);
	//This starts the timer in PWM mode.
	ptr = ioremap_nocache(TCLR,TOUCH_LONG);
	iowrite32(0x01843,ptr);
	iounmap(ptr);

	return retval;
}

module_init(load_module);
module_exit(unload_module);
