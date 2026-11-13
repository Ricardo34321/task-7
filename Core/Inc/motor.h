//
// Created by Ricardo on 2026/11/1.
//

#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"

#define MOTOR_CAN_ID    1
#define MOTOR_CAN_RX_ID 0x205
#define MOTOR_CAN_TX_ID 0x1FF

typedef struct
{
  volatile uint16_t angle;    /*!< 机械角度，单位：编码值，0~8191 对应 0~360° */
  volatile int16_t speed;     /*!< 转速，单位 rpm */
  volatile int16_t current;   /*!< 实际电流，单位：CAN 电流指令值，±16384 对应 ±3A */
  volatile int16_t temp;      /*!< 温度，单位 ℃ */
} Motor_Data;

extern Motor_Data motor;
extern uint32_t last_rx_time;

void Motor_Init(void);
void Motor_SendVoltage(int16_t voltage);

#endif