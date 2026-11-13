//
// Created by Ricardo on 2026/11/13.
//

#ifndef CONFIG_H
#define CONFIG_H

/* 任务周期 */
#define MOTOR_TASK_PERIOD_MS      10
#define COMM_TASK_PERIOD_MS       10
#define LED_TASK_PERIOD_MS        10

/* 串口命令 */
#define MOTOR_CMD_PREFIX          "motor"
#define MOTOR_CMD_PREFIX_LEN      5
#define MOTOR_CMD_MIN_LEN         6

/* 串口通信 */
#define PRINT_PERIOD_MS           2000
#define UART_TX_TIMEOUT_MS        100

/* 按键与离线 */
#define KEY_DEBOUNCE_MS           200
#define MOTOR_OFFLINE_MS          100

/* 舵机 */
#define SERVO_RANGE_DEG           180.0f
#define SERVO_STEP_DEG            20.0f
#define SERVO_PULSE_MIN_US        500.0f
#define SERVO_PULSE_SPAN_US       2000.0f

/* 呼吸灯 */
#define BREATH_MAX                1000
#define BREATH_STEP               10

/* PID */
#define SPEED_KP                  50.0f
#define SPEED_KI                  0.7f
#define SPEED_KD                  0.2f
#define SPEED_MAX_OUT             25000.0f
#define SPEED_MAX_INTEGRAL        28000.0f

/* CAN */
#define MOTOR_CAN_FILTER_BANK     0

#endif
