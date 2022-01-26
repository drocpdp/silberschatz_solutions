/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>
#include <asm/param.h>

/* This function is called when the module is loaded. */
static int simple_init(void)
{
       printk(KERN_INFO "OS -- Loading Module\n");
       printk(KERN_INFO "%llu", GOLDEN_RATIO_PRIME);
       printk(KERN_INFO "%llu", jiffies);
       return 0;
}

/* This function is called when the module is removed. */
static void simple_exit(void) {
    printk(KERN_INFO "%lu\n", gcd(3300,40));
    printk(KERN_INFO "OS -- Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");