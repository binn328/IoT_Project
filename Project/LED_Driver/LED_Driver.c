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


