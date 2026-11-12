//
// Created by Ricardo on 2026/11/1.
//

#ifndef HUXIDENG_MOTOR_H
#define HUXIDENG_MOTOR_H

#endif //HUXIDENG_MOTOR_H
#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

#define MOTOR_CAN_ID     1
#define MOTOR_CAN_RX_ID  0x205
#define MOTOR_CAN_TX_ID  0x1FF


void Motor_Init(void);
void Motor_Senddianya(int16_t dianya);
typedef struct
{
    volatile uint16_t angle;
    volatile int16_t  speed;
    volatile int16_t  current;
    volatile int16_t  temp;
} Motor_Data;

extern Motor_Data motor;
extern uint32_t  lasttime ;


#endif

