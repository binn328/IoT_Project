cmd_/home/user/IOT/Project/Servo_Motor_Driver/motor_driver.mod := printf '%s\n'   motor_driver.o | awk '!x[$$0]++ { print("/home/user/IOT/Project/Servo_Motor_Driver/"$$0) }' > /home/user/IOT/Project/Servo_Motor_Driver/motor_driver.mod
