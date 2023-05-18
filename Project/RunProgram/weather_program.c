#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define LED_FILE_NAME "/dev/led_driver"
#define SERVO_MOTOR_NAME "/dev/motor_driver"
#define WEATHER_INFO_FILE ""
#define WEATHER_TEST_01_FILE ""
#define WEATHER_TEST_02_FILE ""
#define WEATHER_TEST_03_FILE ""

/* led를 위한 on off 변수 */
char stay = ' ';
char on = '1';
char off = '0';

/* led 파일 여는 함수 */
int open_led();
/* servo_motor 파일 여는 함수 */
int open_servo_motor();

/* 각 led 설정하는 함수 */
void led_setting(int fd_led, char red, char yellow, char green, char blue);

/* 서보모터 돌리는 함수
 * 0,    1,   2, 3,   4,    5,    6 순서로
 * 90, 60, 30, 0, -30, -60, -90 순서  */
void motor_write(int fd_servo, char data);

/* 최초 구동 시 장치의 작동을 테스트하는 함수 */
void device_init(int fd_led, int fd_servo);

int main(int argc, char **argv){
    /* 장치 파일 디스크럽터 */
    int fd_led;
    int fd_servo_motor;

    /* led 파일 열기 */
    fd_led = open_led();

    /* servo_motor 열기 */
    fd_servo_motor = open_servo_motor();

    /* 날씨 정보 열어서 정보 가져오기 */

    /* 장치 정상작동 확인하기 */   


    /* 종료하기 */
    //close(fd_led);
    close(fd_servo_motor);
    return 0;
}

/* led 파일 여는 함수 */
int open_led() {
        int fd;
        fd = open(LED_FILE_NAME, O_RDWR);
        if (fd < 0) {
                fprintf(stderr, "Can't open %s\n", LED_FILE_NAME);
                return -1;
        }
        return fd;
}
/* servo_motor 파일 여는 함수 */
int open_servo_motor() {
        int fd;
        fd= open(SERVO_MOTOR_NAME, O_RDWR);
        if (fd< 0) {
                fprintf(stderr, "Can't open %s\n", SERVO_MOTOR_NAME);
                return -1;
        }
        return fd;
}

/* led 설정하는 함수 */
void led_setting(int fd_led, char red, char yellow, char green, char blue){
    char leds[4] = {0, };
    leds[0] = red;
    leds[1] = yellow;
    leds[2] = green;
    leds[3] = blue;

    write(fd_led, leds, 4);
}

/* 모터 설정하는 함수 */
void motor_write(int fd_servo, char data){
    write(fd_servo, &data, 1);
}
/* 최초구동시 장치 작동여부 확인하는 함수 */
void device_init(int fd_led, int fd_servo) {
        /* led 작동을 테스트 */
        /* led 모두 끄기*/
        led_setting(fd_led, off, off, off, off);
        sleep(0.1);
        /* led 하나씩 키고 끄기 */
        led_setting(fd_led, on, off, off, off);
        sleep(0.1);
        led_setting(fd_led, off, on, off, off);
        sleep(0.1);
        led_setting(fd_led, off, off, on, off);
        sleep(0.1);
        led_setting(fd_led, off, off, off, on);
        sleep(0.2);
        led_setting(fd_led, off, off, on, off);
        sleep(0.1);
        led_setting(fd_led, off, on, off, off);
        sleep(0.1);
        led_setting(fd_led, on, off, off, off);
        sleep(0.1);
        led_setting(fd_led, off, off, off, off);
        sleep(0.1);
        /* led 모두 껐다켰다 2회 반복 */
        for (int i = 0; i < 2; i++) {
                led_setting(fd_led, on, on, on, on);
                sleep(0.3);
                led_setting(fd_led, off, off, off, off);
                sleep(0.3);
        }
        /* led 작동 확인 완료 */

        /* servo_motor 작동을 테스트 */
        /* 90' 에서 -90도로, -90도에서 90도로 돌리기 */
        char angle[] = { '0', '1', '2', '3', '4', '5', '6' };
        for (int i = 0; i < 7; i++) {
                motor_write(fd_servo, angle[i]);
                sleep(0.1);
        }
        for (int i = 6; i > -1; i++) {
                motor_write(fd_servo, angle[i]);
                sleep(0.1);
        }
        /* 작동 확인 완료 */
}