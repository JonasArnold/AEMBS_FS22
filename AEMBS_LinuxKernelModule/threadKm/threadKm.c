#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>

/* LED definitions */
#define LED_BLUE_PIN_BCM    12 /* wPi 26 */
#define LED_GREEN_PIN_BCM   16 /* wPi 27 */
#define LED_YELLOW_PIN_BCM  20 /* wPi 28 */
#define LED_RED_PIN_BCM     21 /* wPi 29 */

/* thread definitions */
#define THREAD_NAME "ledThread"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Arnold <jonas.arnold@stud.hslu.ch>");
MODULE_DESCRIPTION("Simple thread kernel module for demonstration");
MODULE_VERSION("0.4");

static unsigned int gpioLed = LED_BLUE_PIN_BCM;
static bool ledOn;
struct task_struct *task;

/* Thread actions */
static int thread(void *data){
  for(;;) {
    if (kthread_should_stop()) {
      break;
    }
    /* toggle led */
    ledOn = !ledOn;
    gpio_set_value(gpioLed, ledOn);
    msleep(500);
  }
  return 0;
}

static int __init simple_init_module(void) {
  printk(KERN_INFO "Init my thread kernel module.\n");

  /* Initialize LED */
  ledOn = false;
  gpio_request(gpioLed, "LED_InterruptKM");
  gpio_direction_output(gpioLed, 0);

  /* Initialize thread */
  task = kthread_run(thread, NULL, THREAD_NAME);

  /* A non-0 return means init_module failed; module can't be loaded.  */
  return 0;
}

static void __exit simple_cleanup_module(void) {
  printk(KERN_INFO "Disabling my thread kernel module.\n");

  // disable led and free memory
  gpio_set_value(gpioLed, 0);
  gpio_free(gpioLed);

  // stop thread
  kthread_stop(task);
}

module_init(simple_init_module);
module_exit(simple_cleanup_module);

