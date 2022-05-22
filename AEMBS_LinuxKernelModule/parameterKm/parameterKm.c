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

/* parameter definitions */
static unsigned int blinkMs = 1000;
module_param(blinkMs, int, S_IRUGO);
MODULE_PARM_DESC(blinkMs, " blink duration in milliseconds (default=1000)");
static char *ledName = "blue";
module_param(ledName, charp, S_IRUGO);
MODULE_PARM_DESC(ledName, " Color of LED to blink: red, green, blue, yellow (default=blue)");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Arnold <jonas.arnold@stud.hslu.ch>");
MODULE_DESCRIPTION("Simple parameter kernel module for demonstration");
MODULE_VERSION("0.5");


static unsigned int gpioLed;
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
    msleep(blinkMs);
  }
  return 0;
}

static int __init simple_init_module(void) {
  printk(KERN_INFO "Init my parameter kernel module.\n");
  /* Read parameters */
  // check if blinkMs is out of range
  if(blinkMs < 10 || blinkMs > 2000){
    printk(KERN_WARNING "LED Blink delay parameter out of range, using default 1000 ms\n");
    blinkMs = 1000;
  }
  // check led name and set the gpio pin
  if      (strcmp(ledName, "red") == 0)    { gpioLed = LED_RED_PIN_BCM; }
  else if (strcmp(ledName, "blue") == 0)   { gpioLed = LED_BLUE_PIN_BCM; }
  else if (strcmp(ledName, "green") == 0)  { gpioLed = LED_GREEN_PIN_BCM; }
  else if (strcmp(ledName, "yellow") == 0) { gpioLed = LED_YELLOW_PIN_BCM; }
  else {
      printk(KERN_WARNING "LED Name unknown, using default led blue\n");
      gpioLed = LED_BLUE_PIN_BCM;
  }

  /* Initialize LED */
  ledOn = false;
  gpio_request(gpioLed, "LED_ParameterKM");
  gpio_direction_output(gpioLed, 0);

  /* Initialize thread */
  task = kthread_run(thread, NULL, THREAD_NAME);

  /* A non-0 return means init_module failed; module can't be loaded.  */
  return 0;
}

static void __exit simple_cleanup_module(void) {
  printk(KERN_INFO "Disabling my parameter kernel module.\n");

  // disable led and free memory
  gpio_set_value(gpioLed, 0);
  gpio_free(gpioLed);

  // stop thread
  kthread_stop(task);
}

module_init(simple_init_module);
module_exit(simple_cleanup_module);

