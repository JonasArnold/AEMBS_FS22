#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Arnold <jonas.arnold@stud.hslu.ch>");
MODULE_DESCRIPTION("Simple kernel module for demonstration");
MODULE_VERSION("0.1");

static int __init simple_init_module(void) {
  printk(KERN_INFO "Hello world from kernel module.\n");
  /* A non-0 return means init_module failed; module can't be loaded.  */
  return 0;
}

static void __exit simple_cleanup_module(void) {
  printk(KERN_INFO "Goodbye from a kernel module.\n");
}

module_init(simple_init_module);
module_exit(simple_cleanup_module);

