#include "motor.h"
#include "config.h"
#include "can.h"

uint32_t last_rx_time = 0;
Motor_Data motor;

/**
  * @brief  启动 CAN1、配置验收滤波器并开启接收中断
  * @param  None
  * @retval None
  */
void Motor_Init(void)
{
  HAL_CAN_Start(&hcan1);

  CAN_FilterTypeDef filter;
  filter.FilterBank = MOTOR_CAN_FILTER_BANK;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = MOTOR_CAN_RX_ID << 5;
  filter.FilterIdLow = 0;
  filter.FilterMaskIdHigh = 0x7FF << 5;
  filter.FilterMaskIdLow = 0;
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;

  HAL_CAN_ConfigFilter(&hcan1, &filter);

  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
  * @brief  向 GM6020 发送一帧电压控制指令
  * @param  voltage  电压指令值，范围 ±30000，对应 ±24V
  * @retval None
  */
void Motor_SendVoltage(int16_t voltage)
{
  CAN_TxHeaderTypeDef tx;
  uint8_t data[8] = {0};
  uint32_t mailbox = 0;

  data[0] = (uint8_t)(voltage >> 8);
  data[1] = (uint8_t)(voltage & 0xFF);

  tx.StdId = MOTOR_CAN_TX_ID;
  tx.ExtId = 0;
  tx.IDE = CAN_ID_STD;
  tx.RTR = CAN_RTR_DATA;
  tx.DLC = 8;
  tx.TransmitGlobalTime = DISABLE;

  if (HAL_CAN_AddTxMessage(&hcan1, &tx, data, &mailbox) != HAL_OK)
  {
    return;
  }
}

/**
  * @brief  CAN1 接收中断回调，解析 GM6020 反馈帧并更新电机状态
  * @param  hcan  CAN 句柄指针
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef rx;
  uint8_t data[8];

  if (hcan->Instance != CAN1)
  {
    return;
  }
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx, data) != HAL_OK)
  {
    return;
  }
  if (rx.StdId != MOTOR_CAN_RX_ID)
  {
    return;
  }

  motor.angle = (uint16_t)(data[0] << 8 | data[1]);
  motor.speed = (int16_t)(data[2] << 8 | data[3]);
  motor.current = (int16_t)(data[4] << 8 | data[5]);
  motor.temp = data[6];

  last_rx_time = HAL_GetTick();
}