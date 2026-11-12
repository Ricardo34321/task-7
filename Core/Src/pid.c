#include "pid.h"



void PID_Init(PID_Data *pid,float kp, float ki, float kd , float max_out, float max_integral) {
        pid->kp = kp;
        pid->ki = ki;
        pid->kd = kd;
        pid->max_out = max_out;
        pid->max_integral = max_integral;

        PID_Reset(pid);


}

float PID_Calc(PID_Data *pid,float target, float actual) {
        pid->actual=actual;
        pid->target=target;
        pid->err=target-actual;
        pid->err_integral=pid->err+pid->err_integral;

        if(pid->err_integral>pid->max_integral) {
            pid->err_integral=pid->max_integral;
        }else if (pid->err_integral<-pid->max_integral) {
            pid->err_integral=-pid->max_integral;
        }

        pid->out=pid->kp*pid->err+pid->ki*pid->err_integral+pid->kd*(pid->err-pid->last_err);

        if(pid->out>pid->max_out) {
            pid->out=pid->max_out;
        }else if (pid->out<-pid->max_out) {
            pid->out=-pid->max_out;
        }
       pid->last_err=pid->err;

        return pid->out;




}
void PID_Reset(PID_Data *pid) {

          pid->err = 0;
          pid->last_err = 0;
          pid->err_integral=0;
          pid->target=0;
          pid->out=0;
          pid->actual=0;




}
