/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "pid.h"
#include <stdio.h>
#include "usart.h"
#include <stdlib.h>
#include <string.h>
#include <tim.h>
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern PID_Data pid_speed;
extern volatile float target_rpm;
uint8_t rx_buf[64];
volatile uint16_t rx_len = 0;
volatile uint8_t rx_ready = 0;
volatile uint8_t key_flag = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
  .name = "MotorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for CommTask */
osThreadId_t CommTaskHandle;
const osThreadAttr_t CommTask_attributes = {
  .name = "CommTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for LedTask */
osThreadId_t LedTaskHandle;
const osThreadAttr_t LedTask_attributes = {
  .name = "LedTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartMotorTask(void *argument);
void StartCommTask(void *argument);
void StartLedTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of MotorTask */
  MotorTaskHandle = osThreadNew(StartMotorTask, NULL, &MotorTask_attributes);

  /* creation of CommTask */
  CommTaskHandle = osThreadNew(StartCommTask, NULL, &CommTask_attributes);

  /* creation of LedTask */
  LedTaskHandle = osThreadNew(StartLedTask, NULL, &LedTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartMotorTask */
/**
* @brief Function implementing the MotorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMotorTask */
void StartMotorTask(void *argument)
{
  /* USER CODE BEGIN StartMotorTask */
  /* Infinite loop */
  for(;;)
  {
    float out = PID_Calc(&pid_speed, target_rpm, (float)motor.speed);
    Motor_SendVoltage((int16_t)out);
    osDelay(10);


  }
  /* USER CODE END StartMotorTask */
}

/* USER CODE BEGIN Header_StartCommTask */
/**
* @brief Function implementing the CommTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommTask */
void StartCommTask(void *argument)
{
  /* USER CODE BEGIN StartCommTask */
  /* Infinite loop */
  static uint32_t last_print_time = 0;
  char buf[64];
  HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx_buf, sizeof(rx_buf));
  for(;;)
  {
    if (rx_ready)
  {
    rx_ready = 0;
    if (rx_len >= 6 && strncmp((char*)rx_buf, "motor", 5) == 0)
    {
      target_rpm = (float)atoi((char*)rx_buf + 5);
    }
  }
    if (HAL_GetTick()-last_print_time > 2000) {
      last_print_time = HAL_GetTick();
      int n= sprintf(buf,"speed=%d rpm\r\n",motor.speed);
      HAL_UART_Transmit(&huart6, (uint8_t*)buf, n, 100);


    }

    osDelay(10);
  }
  /* USER CODE END StartCommTask */
}

/* USER CODE BEGIN Header_StartLedTask */
/**
* @brief Function implementing the LedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN StartLedTask */
  /* Infinite loop */
  static uint32_t last_key_tick = 0;
  static float servo_angle = 0.0f;
  static float servo_dir   = 20.0f;
  static uint32_t breath_val = 0;
  static int32_t breath_dir = 10;

  for(;;)
  {
    if (key_flag) {
      key_flag = 0;
      if (HAL_GetTick()-last_key_tick > 200) {
        last_key_tick = HAL_GetTick();
        if (servo_angle >= 180.0f) servo_dir = -20.0f;
        if (servo_angle <= 0.0f)   servo_dir =  20.0f;

        servo_angle += servo_dir;

        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
                              (uint32_t)(500.0f + servo_angle * 2000.0f / 180.0f));
      }
    } if (HAL_GetTick() - last_rx_time > 100)
    {
      __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, 0);
      breath_val += breath_dir;
      if (breath_val >= 1000) breath_dir = -10;
      if (breath_val <= 0)    breath_dir =  10;
      __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, (uint32_t)breath_val);
    }
    else
    {
      __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, 1000);
      __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
      breath_val = 0;
      breath_dir = 10;
    }

    osDelay(10);
  }
  /* USER CODE END StartLedTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART6)
  {
    rx_len = Size;
    rx_ready = 1;
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx_buf, sizeof(rx_buf));
  }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_0)
  {
    key_flag = 1;
  }
}
/* USER CODE END Application */

