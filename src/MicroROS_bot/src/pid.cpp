#include "pid.h"

float pid::calculate(float setpoint, float input){
    float error = setpoint - input;
    integral += error;
    float derivative = error - previous_error;
    float output = kp * error + ki * integral + kd * derivative;
    previous_error = error;

    if(output < output_min)
        output = 0;
  
    if(output > output_max)
        output = output_max;
    return output;
}

void pid::reset(){
    integral = 0;
    previous_error = 0;
}

pid::pid(float kp, float ki, float kd)
{
    kp = kp;
    ki = ki;
    kd = kd; 
}

pid::~pid()
{
}

