#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define LED_FILE_NAME "/dev/led_driver"
#define SERVO_MOTOR_NAME ""
#define WEATHER_INFO_FILE ""
#define WEATHER_TEST_01_FILE ""
#define WEATHER_TEST_02_FILE ""
#define WEATHER_TEST_03_FILE ""

/* 각 led 설정하는 함수 */
void led_setting(int fd_led, char red, char yellow, char green, char blue);

int main(int argc, char **argv){
    /* 장치 파일 디스크럽터 */
    int fd_led;
    int fd_servo_motor;
    
    /* led를 위한 on off 변수 */
    char on = '1';
    char off = '0';

    /* led 파일 열기 */
    fd_led = open(LED_FILE_NAME, O_RDWR);
    if(fd_led < 0){
        printf("%d", fd_led);
        fprintf(stderr, "Can't open %s\n", LED_FILE_NAME);
        return -1;
    }

    /* servo_motor 열기 */

    /* 날씨 정보 열기 */

    /* led 테스트하기 */
    led_setting(fd_led, on, on, on, on);
    sleep(1);
    led_setting(fd_led, off, off, off, off);
    sleep(1);
    for(int i = 0; i < 10; i++){
        led_setting(fd_led, on, off, off, off);
        sleep(1);
        led_setting(fd_led, off, on, off, off);
        sleep(1);
        led_setting(fd_led, off, off, on, off);
        sleep(1);
        led_setting(fd_led, off, off, off, on);
        sleep(1);
    }
    led_setting(fd_led, off, off, off, off);

    close(fd_led);
    return 0;
}

void led_setting(int fd_led, char red, char yellow, char green, char blue){
    char leds[4] = {0, };
    leds[0] = red;
    leds[1] = yellow;
    leds[2] = green;
    leds[3] = blue;

    write(fd_led, leds, sizeof(leds));
}
