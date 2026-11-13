#ifndef PID_H
#define PID_H

typedef struct
{
  float kp;             /*!< 比例系数，单位：CAN 电压指令值 / rpm */
  float ki;             /*!< 积分系数，单位：CAN 电压指令值 / (rpm·次) */
  float kd;             /*!< 微分系数，单位：CAN 电压指令值 / rpm */
  float target;         /*!< 目标转速，单位 rpm */
  float err;            /*!< 当前误差，单位 rpm */
  float last_err;       /*!< 上一次误差，单位 rpm */
  float err_integral;   /*!< 误差累加值，单位 rpm·次 */
  float out;            /*!< 上一次控制量，单位：CAN 电压指令值 */
  float max_out;        /*!< 输出限幅，单位：CAN 电压指令值 */
  float max_integral;   /*!< 误差累加限幅，单位 rpm·次 */
  float actual;         /*!< 实际转速，单位 rpm */
} PID_Data;

void PID_Init(PID_Data *pid, float kp, float ki, float kd, float max_out, float max_integral);
float PID_Calc(PID_Data *pid, float target, float actual);
void PID_Reset(PID_Data *pid);

#endif