#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h> // changed from asm/uaccess.h to linux/uaccess.h
#include <linux/time.h>
#include <linux/io.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/fcntl.h> /*Helps fix O_ACCMODE*/
#include <linux/sched.h> /*Helps fix TASK_UNINTERRUPTIBLE */
#include <linux/fs.h> /*Helps fix the struct intializer */

int __init init_module(void);
void __exit cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "hsdk"// Dev name
#define BUF_LEN 80//Max length of device message

/* These parameters depend on the RPI edition */

#define BCM2708_PERI_BASE       0x3F000000 // value needs to be changed to 0x3F000000 for a RPi3. 0x20000000 works for Pi W.
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)	// GPIO controller

// Defines  GPIO macros to control GPIOs.
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3)) //001
#define SET_GPIO_ALT(g,a) *(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))
#define GPIO_SET  *(gpio.addr + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(gpio.addr + 10) // clears bits which are 1 ignores bits which are 0
#define GPIO_READ(g)  *(gpio.addr + 13) &= (1<<(g))

//GPIO Clock. AT
#define CLOCK_BASE              (BCM2708_PERI_BASE + 0x00101000)
#define GZ_CLK_BUSY (1 << 7)


//How many samples to capture
#define SAMPLE_SIZE 	2500 // 2x2500 pts in one line
#define REPEAT_SIZE 	10 // 10 captures

//static int SAMPLE_SIZE = 2500;
//static int REPEAT_SIZE = 10;

//module_param(SAMPLE_SIZE, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
//module_param(REPEAT_SIZE, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

// in this setup, there will be REPEAT_SIZE captures, of each SAMPLE_SIZE length, with ADCs interleaved,
// that means 2x SAMPLE_SIZE length

//Define GPIO Pins

//ADC 1
#define BIT0_ADC1 9
#define BIT1_ADC1 25
#define BIT2_ADC1 10
#define BIT3_ADC1 22
#define BIT4_ADC1 27
#define BIT5_ADC1 17
#define BIT6_ADC1 18
#define BIT7_ADC1 15
#define BIT8_ADC1 14
#define BIT9_ADC1 24

//ADC 2
#define BIT0_ADC2 20
#define BIT1_ADC2 26
#define BIT2_ADC2 16
#define BIT3_ADC2 19
#define BIT4_ADC2 13
#define BIT5_ADC2 12
#define BIT6_ADC2 7
#define BIT7_ADC2 8
#define BIT8_ADC2 11
#define BIT9_ADC2 21

//CLOCK
#define CLOCK_GPIO 4

// IO Access
struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;

};

static int map_peripheral(struct bcm2835_peripheral *p);
static void unmap_peripheral(struct bcm2835_peripheral *p);
static void readScope(void);

static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;
static char msg[BUF_LEN];	/* The msg the device will give when asked */

// changed from unsigned char
static uint32_t *buf_p;

//changed
static struct file_operations fops = {
read : device_read,
       write : device_write,
       open : device_open,
       release : device_release
};

static struct bcm2835_peripheral myclock = {CLOCK_BASE};

static struct bcm2835_peripheral gpio = {GPIO_BASE};

typedef struct DataStruct{
    uint32_t Buffer[REPEAT_SIZE*SAMPLE_SIZE];
    uint32_t time;
}Ds;
Ds dataStruct;

static uint32_t *ScopeBufferStart;
static uint32_t *ScopeBufferStop;

static int map_peripheral(struct bcm2835_peripheral *p)
{
    p->addr=(uint32_t *)ioremap(GPIO_BASE, 41*4); //41 GPIO register with 32 bit (4*8)
    return 0;
}

static void unmap_peripheral(struct bcm2835_peripheral *p) {
    iounmap(p->addr);//unmap the address
}

/*
   In our case we are only taking 10k samples so not too much time. Before the sample taking we take a time stamp. Then we read out 10k times the GPIO register and save it in our data structure. The GPIO register is a 32bit value so it is made out of 32 ‘1’s and ‘0’s each defining if the GPIO port is high (3.3V) or low (GND). After the read out we take another time stamp and turn on all interrupts again. The two time stamps we took are important since we can calculate how long it took to read in the 10k samples. The time difference divided by 10k gives us the time between each sample point. In case the sample frequency is too high and should be reduced one can add some delay and waste some time during each readout step. Here the aim is to achieve the maximal performance.
   */

static void readScope(){

    int counter=0;
    int counterline = 0;
    int limit = 0;

    int Pon=0;
    int Poff=0;
    struct timespec ts_start,ts_stop;

    msleep(10);

    //disable IRQ
    local_irq_disable();
    local_fiq_disable();

    //Start time
    set_current_state(TASK_UNINTERRUPTIBLE);
    getnstimeofday(&ts_start);

    while(counterline<REPEAT_SIZE){
        Pon = 0;
        Poff = 0;
        limit = (counterline+1)*SAMPLE_SIZE;

        printk(KERN_INFO "scope: capturing repeat %d\n", counterline);

        while(counter<(limit) ){
            dataStruct.Buffer[counter++]= *(gpio.addr + 13);
        }

        // to avoid freezes
        msleep(0.5);

        counterline++;
    }
    printk(KERN_INFO "scope: finished collecting data\n");

    //Stop time
    getnstimeofday(&ts_stop);

    set_current_state(TASK_INTERRUPTIBLE);
    //enable IRQ
    local_fiq_enable();
    local_irq_enable();

    //save the time difference
    dataStruct.time=timespec_to_ns(&ts_stop)-timespec_to_ns(&ts_start);//ns resolution
    buf_p= dataStruct.Buffer;//cound maybe removed

    //accessing memeber of the structure that is already pointer by its nature
    ScopeBufferStart= dataStruct.Buffer;
    ScopeBufferStop=ScopeBufferStart+sizeof(struct DataStruct);
    printk(KERN_INFO "scope: finished playing with time\n");
}

int init_module(void)
{

    struct bcm2835_peripheral *p=&myclock;
    int speed_id = 6; //1 for to start with 19Mhz or 6 to start with 500 MHz

    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    //Map GPIO

    if(map_peripheral(&gpio) == -1)
    {
        // goes without comma
        printk(KERN_ALERT "Failed to map the physical GPIO registers into the virtual memory space.\n");
        return -1;
    }

    //Define Scope pins as inputs
    // ADC1
    INP_GPIO(BIT0_ADC1);
    INP_GPIO(BIT1_ADC1);
    INP_GPIO(BIT2_ADC1);
    INP_GPIO(BIT3_ADC1);
    INP_GPIO(BIT4_ADC1);
    INP_GPIO(BIT5_ADC1);
    INP_GPIO(BIT6_ADC1);
    INP_GPIO(BIT7_ADC1);
    INP_GPIO(BIT8_ADC1);
    INP_GPIO(BIT9_ADC1);
    // ADC2
    INP_GPIO(BIT0_ADC2);
    INP_GPIO(BIT1_ADC2);
    INP_GPIO(BIT2_ADC2);
    INP_GPIO(BIT3_ADC2);
    INP_GPIO(BIT4_ADC2);
    INP_GPIO(BIT5_ADC2);
    INP_GPIO(BIT6_ADC2);
    INP_GPIO(BIT7_ADC2);
    INP_GPIO(BIT8_ADC2);
    INP_GPIO(BIT9_ADC2);

    //Set a clock signal on Pin 4
    p->addr=(uint32_t *)ioremap(CLOCK_BASE, 41*CLOCK_GPIO);
    INP_GPIO(CLOCK_GPIO);
    SET_GPIO_ALT(CLOCK_GPIO,0);
    // Preparing the clock

    *(myclock.addr+28)=0x5A000000 | speed_id; //Turn off the clock
    while (*(myclock.addr+28) & GZ_CLK_BUSY) {}; //Wait until clock is no longer busy (BUSY flag)
    // Set divider //divide by 50 (0x32) -- ideally 41 (29) to fall on 12MHz clock
    *(myclock.addr+29)= 0x5A000000 | (0x29 << 12) | 0;
    // And let's turn clock on
    *(myclock.addr+28)=0x5A000010 | speed_id;

    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void){
    unregister_chrdev(Major, DEVICE_NAME);
    unmap_peripheral(&gpio);
    unmap_peripheral(&myclock);
}

static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 0;

    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    printk(KERN_INFO "scope: Device has been opened.\n");
    readScope();//Read n Samples into memory
    printk(KERN_INFO "scope: returned from readScope()\n");
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp,
        char *buffer,
        size_t length,
        loff_t * offset)
{

    printk(KERN_INFO "scope: about to copy data to userspace\n");
    // Number of bytes actually written to the buffer
    printk(KERN_INFO "scope: copying up %d bytes of data.\n", sizeof(struct DataStruct));
    int bytes_read = 0;
    bytes_read = copy_to_user(buffer, buf_p, sizeof(struct DataStruct));

    if (bytes_read==0){            // if true then have success
        printk(KERN_INFO "scope: Sent %d characters to the user\n", sizeof(struct DataStruct));
        return bytes_read;  // clear the position to the start and return 0
    }
    else {
        printk(KERN_INFO "scope: Failed to send %d characters to the user\n", bytes_read);
        return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
    }

    return bytes_read;
}

    static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
    return -EINVAL;
}


MODULE_AUTHOR("kelu124 and jholtom");
MODULE_LICENSE("GPL");
MODULE_VERSION("3");
