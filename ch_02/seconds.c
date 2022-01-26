#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/unistd.h>
#include <linux/limits.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

unsigned long initial_jiffies;
unsigned long jiffies_rate;

/**
 * Function Prototypes
 */
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static const struct proc_ops proc_ops2= {
	.proc_read = proc_read
};

/* This function is called when the module is loaded */
static int proc_init(void)
{   

    initial_jiffies = jiffies;

    
    // the following is a wrapper for 
    // proc_create data() passing NULL as the last argument
    proc_create(PROC_NAME, 0, NULL, &proc_ops2);

    printk(KERN_INFO "/proc/%s/ created\n", PROC_NAME);

    return 0;
}

/* This function is called when the module is removed */
static void proc_exit(void)
{
    // removes the /proc/jiffies entry
    remove_proc_entry(PROC_NAME, NULL);

    printk(KERN_INFO "/proc/%s/ removed\n", PROC_NAME);
}

/** 
 * This function is called each time the /proc/jiffies is read
 * 
 * This function is called repeatedly until it returns 0, so there
 * must be logic that ensures it ultimately returns 0 once it
 * has collected the data that is to go into the corresponding
 * /proc file.
 * 
 * params:
 * 
 * file:
 * buf: buffer in user space
 * 
*/
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    unsigned long rslt;
    int seconds_elapsed;

    if (completed){
        completed = 0;
        return 0;
    }

    completed = 1;

    seconds_elapsed = jiffies_to_msecs((jiffies-initial_jiffies) / 1000);

    rv = sprintf(buffer, "%lu, %lu, %i", initial_jiffies, jiffies, seconds_elapsed);

    //copies the contents of buffer to userspace usr_buf
    rslt = copy_to_user(usr_buf, buffer, rv);

    return rv;
} 

/* Macros for registering module entry and exit points */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("DRE");

