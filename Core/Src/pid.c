#include "pid.h"

/**
  * @brief  初始化 PID 控制器并清零内部状态
  * @param  pid           PID 控制器结构体指针
  * @param  kp            比例系数，单位：CAN 电压指令值 / rpm
  * @param  ki            积分系数，单位：CAN 电压指令值 / (rpm·次)
  * @param  kd            微分系数，单位：CAN 电压指令值 / rpm
  * @param  max_out       输出限幅，单位：CAN 电压指令值，±30000 对应 ±24V
  * @param  max_integral  误差累加限幅，单位：rpm·次，应满足 ki*max_integral <= max_out
  * @retval None
  */
void PID_Init(PID_Data *pid, float kp, float ki, float kd, float max_out, float max_integral)
{
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->max_out = max_out;
  pid->max_integral = max_integral;

  PID_Reset(pid);
}

/**
  * @brief  计算一次 PID 输出，位置式，调用周期固定时无需显式乘 dt
  * @param  pid     PID 控制器结构体指针
  * @param  target  目标转速，单位 rpm
  * @param  actual  实际转速，单位 rpm
  * @retval 限幅后的控制量，单位：CAN 电压指令值，±30000 对应 ±24V
  */
float PID_Calc(PID_Data *pid, float target, float actual)
{
  pid->actual = actual;
  pid->target = target;
  pid->err = target - actual;
  pid->err_integral = pid->err + pid->err_integral;

  if (pid->err_integral > pid->max_integral)
  {
    pid->err_integral = pid->max_integral;
  }
  else if (pid->err_integral < -pid->max_integral)
  {
    pid->err_integral = -pid->max_integral;
  }

  pid->out = pid->kp * pid->err + pid->ki * pid->err_integral + pid->kd * (pid->err - pid->last_err);

  if (pid->out > pid->max_out)
  {
    pid->out = pid->max_out;
  }
  else if (pid->out < -pid->max_out)
  {
    pid->out = -pid->max_out;
  }
  pid->last_err = pid->err;

  return pid->out;
}

/**
  * @brief  清零 PID 控制器的误差、积分与输出
  * @param  pid  PID 控制器结构体指针
  * @retval None
  */
void PID_Reset(PID_Data *pid)
{
  pid->err = 0;
  pid->last_err = 0;
  pid->err_integral = 0;
  pid->target = 0;
  pid->out = 0;
  pid->actual = 0;
}