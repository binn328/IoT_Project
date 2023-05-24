#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define LED_FILE_NAME "/dev/led_driver"
#define SERVO_MOTOR_NAME "/dev/motor_driver"
#define WEATHER_INFO_FILE "./java_api.txt"
#define WEATHER_TEST_01_FILE "./test01.txt"
#define WEATHER_TEST_02_FILE "./test02.txt"
#define WEATHER_TEST_03_FILE "./test03.txt"

/* led를 위한 on off 변수 */
char stay = ' ';        //이전 상태를 유지
char on = '1';
char off = '0';

/* led 파일 여는 함수 */
int open_led();
/* servo_motor 파일 여는 함수 */
int open_servo_motor();
/* 날씨 정보 파일 여는 함수 */
int open_weather_file();

/* 각 led 설정하는 함수 */
void led_setting(int fd_led, char red, char yellow, char green, char blue);

/* 서보모터 돌리는 함수
 * 0,    1,   2, 3,   4,    5,    6 순서로
 * 90, 60, 30, 0, -30, -60, -90 순서  */
void motor_write(int fd_servo, char data);

/* 최초 구동 시 장치의 작동을 테스트하는 함수 */
void device_init(int fd_led, int fd_servo);

/* 날씨 정보 얻어오기 */
int get_weather(int input, int* weather, int* is_rain, int* dust);

/* 읽어온 정보를 토대로 led와 모터 설정 */
void set_weather(int fd_led, int fd_servo, int weather, int is_rain, int dust);

/* jar 파일을 실행해 날씨 정보를 얻어옴 */
int exec_java();

int main(int argc, char **argv){
    /* 장치 파일 디스크럽터 */
    int fd_led;
    int fd_servo_motor;
    int fd_weather_info;

    /* led 파일 열기 */
    fd_led = open_led();

    /* servo_motor 열기 */
    fd_servo_motor = open_servo_motor();

    /* 날씨 정보 열어서 정보 가져오기 */
    int weather = 0;          //날씨 : (0, 맑음) (1, 흐림) (2, 눈/비)
    int is_rain = 0;            //비소식 : (0, 없음) (1, 있음)
    int dust = 0;               //미세먼지 농도 : (0 좋음 : 0~15 / 1 보통 : 16~35 / 2 나쁨 : 36~)

    /* 장치 정상작동 확인하기 */   
    //device_init(fd_led, fd_servo_motor);
    
    /* 동작 선택하기 */
    int input = 0;
    int result;
    printf("1. run, 2. test01, 3. test02, 4. test03, 5. device_init() >> ");
    scanf("%d", &input);
    
    switch(input){
        /* run */
        case 1:
            while (1) {
                /* 날씨 정보 파일 생성 */
                exec_java();
                /* 날씨 정보 파일 열기 */
                result = get_weather(1, &weather, &is_rain, &dust);
                if (result) {
                    printf("날씨 정보 열기 실패\n");
                    return -1;
                }

                /* 얻어온대로 장치 설정하기 */
                set_weather(fd_led, fd_servo_motor, weather, is_rain, dust);

                /* 한시간마다 정보를 읽어와 적용한다. */
                sleep(3600);
            }
            break;
        /* test01 */
        case 2:
            /* 날씨 테스트01 열기 
             *  맑음 비옴 좋음 */
            result = get_weather(2, &weather, &is_rain, &dust);
            if (result) {
                printf("날씨 정보 열기 실패\n");
                return -1;
            }

            /* 얻어온대로 장치 설정하기 */
            set_weather(fd_led, fd_servo_motor, weather, is_rain, dust);
            break;
        /* test 02 */
        case 3:
            /* 날씨 테스트02 열기 
             * 흐림 비안옴 보통 */
            result = get_weather(3, &weather, &is_rain, &dust);
            if (result) {
                printf("날씨 정보 열기 실패\n");
                return -1;
            }

            /* 얻어온대로 장치 설정하기 */
            set_weather(fd_led, fd_servo_motor, weather, is_rain, dust);
            break;
        /* test 03 */
        case 4:
            /* 날씨 테스트03 열기 
             * 비옴 비옴 나쁨*/
            result = get_weather(2, &weather, &is_rain, &dust);
            if (result) {
                printf("날씨 정보 열기 실패\n");
                return -1;
            }

            /* 얻어온대로 장치 설정하기 */
            set_weather(fd_led, fd_servo_motor, weather, is_rain, dust);
            break;
        case 5:
                /* device init 함수 실행 */
                device_init(fd_led, fd_servo_motor);
                break;
        /* 이상한 값 */
        default:
            return -1;
    }
    
    /* 종료하기 */
    close(fd_led);
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
        fd = open(SERVO_MOTOR_NAME, O_RDWR);
        if (fd< 0) {
                fprintf(stderr, "Can't open %s\n", SERVO_MOTOR_NAME);
                return -1;
        }
        return fd;
}

/* 날씨 정보 파일 여는 함수 */
int open_weather_file() {
        int fd;
        fd = open(WEATHER_INFO_FILE, O_RDWR);
        if (fd < 0) {
                fprintf(stderr, "Can't open %s\n", WEATHER_INFO_FILE);
                return -1;
        }
        return fd;
}

/* led 설정하는 함수 */
void led_setting(int fd_led, char red, char yellow, char green, char blue){
    // '0' 꺼짐, '1' 켜짐
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
        printf("led_test\n");
        /* led 모두 끄기*/
        led_setting(fd_led, off, off, off, off);
        sleep(1);
        /* led 하나씩 키고 끄기 */
        led_setting(fd_led, on, off, off, off);
        sleep(1);
        led_setting(fd_led, off, on, off, off);
        sleep(1);
        led_setting(fd_led, off, off, on, off);
        sleep(1);
        led_setting(fd_led, off, off, off, on);
        sleep(1);
        led_setting(fd_led, off, off, on, off);
        sleep(1);
        led_setting(fd_led, off, on, off, off);
        sleep(1);
        led_setting(fd_led, on, off, off, off);
        sleep(1);
        led_setting(fd_led, off, off, off, off);
        sleep(1);
        /* led 모두 껐다켰다 2회 반복 */
        for (int i = 0; i < 2; i++) {
                led_setting(fd_led, on, on, on, on);
                sleep(1);
                led_setting(fd_led, off, off, off, off);
                sleep(1);
        }
        /* led 작동 확인 완료 */

        /* servo_motor 작동을 테스트 */
        printf("motor test\n");
        /* 90' 에서 -90도로, -90도에서 90도로 돌리기 */
        printf("test01\n");
        char angle[] = { '0', '1', '2', '3', '4', '5', '6' };
        for (int i = 0; i < 7; i++) {
                motor_write(fd_servo, angle[i]);
                sleep(1);
        }
        printf("test02\n");
        for (int i = 6; i > -1; i--) {
                motor_write(fd_servo, angle[i]);
                sleep(1);
        }
        /* 작동 확인 완료 */
        printf("ok\n");
}

/* 날씨 받아오는 프로그램*/
int get_weather(int input, int* weather, int* is_rain, int* dust) {
    int fd;
    switch(input){
        case 1:
            fd = open(WEATHER_INFO_FILE, O_RDWR);
            break;
        case 2:
            fd = open(WEATHER_TEST_01_FILE, O_RDWR);
            break;
        case 3:
            fd = open(WEATHER_TEST_02_FILE, O_RDWR);
            break;
        case 4:
            fd = open(WEATHER_TEST_03_FILE, O_RDWR);
            break;
        default:
            return -1;
    }

        if (fd < 0) {
                fprintf(stderr, "날씨 파일을 여는데 실패했습니다!\n");
                return -1;
        }
        
        char buffer[5] = { '\0', };
        int result = read(fd, buffer, 5);
        if (!result) {
                fprintf(stderr, "파일 읽기 실패\n");
                return -1;
        }
        /* 읽어온거 표시 */
        for(int i = 0; i < 5; i++){
            printf("%c", buffer[i]);
        }
        printf("\n");

        close(fd);
        *weather = (int)(buffer[0] - '0');
        *is_rain = (int)(buffer[2] - '0');
        *dust = (int)(buffer[4] - '0');

        return 0;
}

/* 읽어온 정보를 토대로 led와 모터 설정 */
void set_weather(int fd_led, int fd_servo, int weather, int is_rain, int dust) {
        char led_info[4] = { '\0', };

        /* 비가 오는가? */
        switch (is_rain) {
        case 0: //안와요
                led_info[3] = off;
                break;
        case 1: //와요
                led_info[3] = on;
                break;
        default:
                led_info[3] = off;
                break;
        }
        /* 먼지는 어떠한가? */
        switch (dust) {
        case 0: //좋음
                led_info[0] = on;
                led_info[1] = off;
                led_info[2] = off;
                break;
        case 1: //보통
                led_info[0] = off;
                led_info[1] = on;
                led_info[2] = off;
                break;
        case 2: //나쁨
                led_info[0] = off;
                led_info[1] = off;
                led_info[2] = on;
                break;
        default:
                led_info[0] = off;
                led_info[1] = off;
                led_info[2] = off;
                break;
        }
        led_setting(fd_led, led_info[0], led_info[1], led_info[2], led_info[3]);

        /* 서보모터 설정 시작 */
        char motor = '0';

        switch (weather) {
        case 0: //맑음
                motor = '3';
                break;
        case 1: //흐림
                motor = '5';
                break;
        case 2: //눈, 비옴
                motor = '1';
                break;
        default:
                motor = '0';
                break;
        }

        motor_write(fd_servo, motor);
}

/* jar 파일을 실행해 날씨 정보를 얻어옴 */
int exec_java() {
        pid_t pid;
        pid = fork();
        int status, child;
        
        if (pid == 0) {
                /* 자식이면 java -jar ./weather_jar.jar 명령어를 실행 */
                execl("/usr/bin/java", "java", "-jar", "./weather_jar.jar", NULL);
        }
        else {
                child = waitpid(pid, &status, 0);
        }

        return 0;
}
