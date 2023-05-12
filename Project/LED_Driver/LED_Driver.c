#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* 모듈 정보 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("binn328");
MODULE_DESCRIPTION("LED를 끄고 키는 GPIO 장치드라이버");

<<<<<<< HEAD
/* led 핀 번호 define */
#define RED 14     //빨간 LED 
#define YELLOW 15   //노란 LED
#define GREEN 18    //초록 LED
#define BLUE 23     //파란 LED

/* write 구현 */
static ssize_t led_write(struct file *File, const char *buffer, size_t count, loff_t *offs){
    /* data[0] = RED
     * data[1] = YELLOW
     * data[2] = GREEN
     * data[3] = BLUE
     */
    char data[4];
    int result;

    result = copy_from_user(data, buffer, count);
  
    for(int i = 0; i < 4; i++){
        printk("%c", data[i]);
    }
    printk("\n");

    if(result < 0){
        printk("copy_from_user Error: 데이터를 읽어오는데 실패");
        return result;
    }
    
    /* LED 설정하기 */
    /* RED */
    switch(data[0]){
        case '0':
            gpio_set_value(RED, 0);
            break;
        case '1':
            gpio_set_value(RED, 1);
            break;
        default:
            printk("잘못된 값입니다!");
    }
    /* YELLOW */
    switch(data[1]){
        case '0':
            gpio_set_value(YELLOW, 0);
            break;
        case '1':
            gpio_set_value(YELLOW, 1);
            break;
        default:
            printk("잘못된 값입니다!");
    }
    /* GREEN */
    switch(data[2]){
        case '0':
            gpio_set_value(GREEN, 0);
            break;
        case '1':
            gpio_set_value(GREEN, 1);
            break;
        default:
            printk("잘못된 값입니다!");
    }
    /* BLUE */
    switch(data[3]){
        case '0':
            gpio_set_value(BLUE, 0);
            break;
        case '1':
            gpio_set_value(BLUE, 1);
            break;
        default:
            printk("잘못된 값입니다!");
    }

    printk("RED : %c, YELLOW : %c, GREEN : %c, BLUE : %c\n", data[0], data[1], data[2], data[3]);

    return 0;
}

/* open 구현 */
static int led_open(struct inode *device_file, struct file *instance){
    printk("led_open이 호출됨\n");
    return 0;
}

/* close 구현 */
static int led_close(struct inode *device_file, struct file *instance){
    printk("led_close가 호출됨\n");
    return 0;
}

/* 동작 명시 */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_close,
    .write = led_write
};

/* 장치 클래스 */
static dev_t led_nr;
static struct class *led_class;
static struct cdev led;

#define DRIVER_NAME "led_driver"
#define DRIVER_CLASS "led_calss"

/* init 구현 */
static int __init led_init(void){
    printk("Hello, led_driver!\n");
 
    if( alloc_chrdev_region(&led_nr, 0, 1, DRIVER_NAME) < 0){
        printk("Error : Driver Nr Failed!\n");
        return -1;
    }

    printk("LED Nr Major : %d, Minor : %d was registered!\n", led_nr>>20, led_nr&0xfffff);

    /* 클래스 파일을 만든다. */
    if((led_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL){
        printk("장치 클래스 파일을 만들 수 없습니다!\n");
        return -1;
    }

    if(device_create(led_class, NULL, led_nr, NULL, DRIVER_NAME) == NULL){
        printk("장치 파일을 만들 수 없습니다!\n");
        return -1;
    }

    /* 장치 파일 초기화 */
    cdev_init(&led, &fops);

    /* 장치를 커널에 등록 */
    if(cdev_add(&led, led_nr, 1) == -1){
        printk("장치를 커널에 등록하는데 실패했습니다\n");
        return -1;
    }

    //GPIO 등록하기
    int result;
    /* RED */
    result = gpio_request(RED, "RED_LED");
    if(result){
        printk("RED핀 할당 실패");
        gpio_free(RED);
        return result;
    }
    result = gpio_direction_output(RED, 0);
    if(result){
        printk("RED핀 출력 실패");
        gpio_free(RED);
        return result;
    }
    /* YELLOW */
    result = gpio_request(YELLOW, "YELLOW_LED");
    if(result){
        printk("YELLOW핀 할당 실패");
        gpio_free(YELLOW);
        return result;
    }
    result = gpio_direction_output(YELLOW, 0);
    if(result){
        printk("YELLOW핀 출력 실패");
        gpio_free(YELLOW);
        return result;
    }
    /* GREEN */
    result = gpio_request(GREEN, "GREEN_LED");
    if(result){
        printk("GREEN핀 할당 실패");
        gpio_free(GREEN);
        return result;
    }
    result = gpio_direction_output(GREEN, 0);
    if(result){
        printk("GREEN핀 출력 실패");
        gpio_free(GREEN);
        return result;
    }
    /* BLUE */
    result = gpio_request(BLUE, "BLUE_LED");
    if(result){
        printk("BLUE핀 할당 실패");
        gpio_free(BLUE);
        return result;
    }
    result = gpio_direction_output(BLUE, 0);
    if(result){
        printk("BLUE핀 출력 실패");
        gpio_free(BLUE);
        return result;
    }

    return 0;
}

/* exit 구현 */
static void __exit led_exit(void){
    gpio_set_value(RED, 0);
    gpio_set_value(YELLOW, 0);
    gpio_set_value(GREEN, 0);
    gpio_set_value(BLUE, 0);

    gpio_free(RED);
    gpio_free(YELLOW);
    gpio_free(GREEN);
    gpio_free(BLUE);
    
    cdev_del(&led);
    device_destroy(led_class, led_nr);
    class_destroy(led_class);
    unregister_chrdev(led_nr, DRIVER_NAME);
    printk("Bye LED Driver\n");
}

module_init(led_init);
module_exit(led_exit);
=======
>>>>>>> 128fd28fae70afcdb06250493ddb6cbceeb16a38

