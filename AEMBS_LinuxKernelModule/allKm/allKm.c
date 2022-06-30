#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/gpio.h>
#include <linux/interrupt.h> /* IRQ interface */

/* Button definition */
#define UP_PIN_BCM          5  /* wPi 21 */
#define DOWN_PIN_BCM        6  /* wPi 22 */
#define RIGHT_PIN_BCM      13  /* wPi 23 */
#define LEFT_PIN_BCM       19  /* wPi 24 */
#define PUSH_PIN_BCM       26  /* wPi 25 */

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
MODULE_DESCRIPTION("Simple Interrupt kernel module for demonstration");
MODULE_VERSION("0.3");

static int irqNumber;
static unsigned int numberPresses = 0;
static unsigned int gpioButton = UP_PIN_BCM;
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
    msleep(blinkMs);
  }
  return 0;
}

/* Interrupt handler */
static irqreturn_t my_gpio_irq_handler(int irq, void *dev_id)
{
  /* do work here */
  printk(KERN_INFO "Button interrupt nr. %d! Button state: %d\n",
    ++numberPresses, gpio_get_value(gpioButton));
  ledOn = !ledOn;  // invert
  gpio_set_value(gpioLed, ledOn);
  return IRQ_HANDLED; // IRQ was handled correctly
}

static int __init simple_init_module(void) {
  int result;
  printk(KERN_INFO "Init my interrupt kernel module.\n");
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
  gpio_request(gpioLed, "LED_InterruptKM");
  gpio_direction_output(gpioLed, 0);
  gpio_set_value(gpioLed, 1);  // set to 1

  /* Initialize interrupt */
  gpio_direction_input(gpioButton); // configure as input
  gpio_set_debounce(gpioButton, 200); // debounce the button with given millisceond time
  irqNumber = gpio_to_irq(gpioButton); // get IRQ number
  result = request_irq(irqNumber, // the interrupt number
    my_gpio_irq_handler,
    IRQF_TRIGGER_FALLING, // falling edge
    "button_handler", // used in /proc/interrupts
    NULL); // the *dev_id for shared lines

  /* Initialize thread */
  task = kthread_run(thread, NULL, THREAD_NAME);

  /* A non-0 return means init_module failed; module can't be loaded.  */
  return 0;
}

static void __exit simple_cleanup_module(void) {
  printk(KERN_INFO "Disabling my interrupt kernel module.\n");

  // disable led and free memory
  gpio_set_value(gpioLed, 0);
  gpio_free(gpioLed);

  // disable interrupt and free memory
  free_irq(irqNumber, NULL); // free the IRQ number
  gpio_free(gpioButton); // unexport the GPIO pin

  // stop thread
  kthread_stop(task);
}

module_init(simple_init_module);
module_exit(simple_cleanup_module);

