#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>

#define DRIVER_NAME "motor_driver"
#define DRIVER_CLASS "motor_class"
/* META Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("binn328");
MODULE_DESCRIPTION("서보모터를 조작하는 장치 드라이버");

/* pwm을 위한 변수 */
#define DUTY_MIN 400000
#define DUTY_MAX 2600000
#define PERIOD 20000000

struct pwm_device *pwm0 = NULL;
/* duty_cycle은 1 ~ 2ms = 350,000 ~ 2,350,000ns = 90' ~ -90'
 *  90 = 350,000
 *  60 = 683,333
 *  30 = 1,016,666
 *   0 = 1,350,000
 * -30 = 1,683,333
 * -60 = 2,016,665
 * -90 = 2,350,000
 */


/* 장치와 장치 클래스를 위한 변수 */
static dev_t servo_motor_nr;
static struct class *servo_motor_class;
static struct cdev servo_motor;

/* write 구현 */
static ssize_t servo_motor_write(struct file *File, const char *buffer, size_t count, loff_t *offs){
    char data;
    int result;
    int duty = DUTY_MIN;
    int period = PERIOD;
    result = copy_from_user(&data, buffer, count);
    if(result < 0){
        printk("write error\n");
        return -1;
    }

    switch(data){
        case '0':
            //90도
            duty = DUTY_MIN;    
            break;
        case '1':
            //60도
            duty = 750000;
            break;
        case '2':
            //30도
            duty = 1100000;
            break;
        case '3':
            //0도
            duty = 1450000;
            break;
        case '4':
            //-30도
            duty = 1800000;
            break;
        case '5':
            //-60도
            duty = 2150000;
            break;
        case '6':
            //-90도
            duty = DUTY_MAX;
            break;
        default:
            printk("잘못된 data 값입니다.\n");
    }

    pwm_config(pwm0, duty, period);
    printk("duty : %d\n", duty);

    return 0;
}

/* open 함수 */
static int servo_motor_open(struct inode * device_file, struct file *instance){
    printk("servo_motor_open()이 호출됨\n");
    return 0;
}

/* close 함수 */
static int servo_motor_close(struct inode * device_file, struct file *instance){
    printk("servo_motor_close()가 호출됨\n");
    return 0;
}

/* 사용할 함수 명시 */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = servo_motor_open,
    .release = servo_motor_close,
    .write = servo_motor_write
};

/* init 구현 */
static int __init servo_motor_init(void){
    printk("Hello, Servo!\n");

    int duty_init = DUTY_MIN;
    int period_init = PERIOD;

    /* 장치 등록하기 */
    if( alloc_chrdev_region(&servo_motor_nr, 0, 1, DRIVER_NAME) < 0){
        printk("Error : 장치 등록 실패\n");
        return -1;
    }

    printk("Servo Motor Major : %d, Minor : %d 가 등록되었습니다.\n", servo_motor_nr>>20, servo_motor_nr&0xfffff);

    /* 클래스 파일 만들기 */
    if((servo_motor_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL){
        printk("장치 클래스 파일을 만들 수 없습니다!\n");
        return -1;
    }

    if(device_create(servo_motor_class, NULL, servo_motor_nr, NULL, DRIVER_NAME) == NULL){
        printk("장치 파일을 만들 수 없습니다!\n");
        return -1;
    }

    /* 장치 파일 초기화 */
    cdev_init(&servo_motor, &fops);

    /* 장치를 커널에 등록 */
    if(cdev_add(&servo_motor, servo_motor_nr, 1) == -1){
        printk("장치를 커널에 등록하는데 실패했습니다!\n");
        return -1;
    }

    pwm0 = pwm_request(0, "servo_motor_pwm");
    if(pwm0 == NULL){
        printk("PWM0 요청이 실패했습니다!\n");
        return -1;
    }

    pwm_config(pwm0, duty_init, period_init);
    pwm_enable(pwm0);

    return 0;
}

/* exit 구현 */
static void __exit servo_motor_exit(void){
    pwm_disable(pwm0);
    pwm_free(pwm0);
    cdev_del(&servo_motor);
    device_destroy(servo_motor_class, DRIVER_NAME);
    class_destroy(servo_motor_class);
    unregister_chrdev(servo_motor_nr, DRIVER_NAME);
    printk("Goodbye, Servo Motor!\n");
}

module_init(servo_motor_init);
module_exit(servo_motor_exit);

















