#ifndef PID_H
#define PID_H
#include <stdint.h>

typedef struct
{
  float kp;
  float ki;
  float kd;
  float target;
  float err;
  float last_err;
  float err_integral;
  float out;
  float max_out;
  float max_integral;
  float actual;
} PID_Data;

void PID_Init(PID_Data *pid, float kp, float ki, float kd, float max_out, float max_integral);
float PID_Calc(PID_Data *pid, float target, float actual);
void PID_Reset(PID_Data *pid);

#endif