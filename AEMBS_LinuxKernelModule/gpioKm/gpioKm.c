#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/gpio.h>

#define LED_BLUE_PIN_BCM    12 /* wPi 26 */
#define LED_GREEN_PIN_BCM   16 /* wPi 27 */
#define LED_YELLOW_PIN_BCM  20 /* wPi 28 */
#define LED_RED_PIN_BCM     21 /* wPi 29 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Arnold <jonas.arnold@stud.hslu.ch>");
MODULE_DESCRIPTION("Simple GPIO kernel module for demonstration");
MODULE_VERSION("0.2");

static int __init simple_init_module(void) {
  printk(KERN_INFO "Init my GPIO kernel module.\n");
  gpio_request(LED_BLUE_PIN_BCM, "LED_BLUE");
  gpio_direction_output(LED_BLUE_PIN_BCM, 0);

  // Set to 1
  gpio_set_value(LED_BLUE_PIN_BCM, 1);
  /* A non-0 return means init_module failed; module can't be loaded.  */
  return 0;
}

static void __exit simple_cleanup_module(void) {
  printk(KERN_INFO "Disabling my GPIO kernel module.\n");

  // disable led and free memory
  gpio_set_value(LED_BLUE_PIN_BCM, 1);  
  gpio_free(LED_BLUE_PIN_BCM);
}

module_init(simple_init_module);
module_exit(simple_cleanup_module);

