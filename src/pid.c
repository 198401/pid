/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : iap
                                      
 Hardware      : ADuC702x

 Description   : iap
*************************************************************************************************/
#include "pid.h"

void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, UNIT_CFG *pid)
{
    // Start values for PID controller
    pid->dat.sumError = 0;
    pid->dat.lastProcessValue = 0;
    // Tuning constants for PID loop
    pid->dat.P_Factor = p_factor;
    pid->dat.I_Factor = i_factor;
    pid->dat.D_Factor = d_factor;
    // Limits to avoid overflow
    pid->dat.maxError = MAX_INT / (pid->dat.P_Factor + 1);
    pid->dat.maxSumError = MAX_I_TERM / (pid->dat.I_Factor + 1);
}

int16_t pid_Controller(int16_t setPoint, int16_t processValue, UNIT_CFG *pid)
{
    int16_t error, p_term, d_term;
    int32_t i_term, ret, temp;

    error = setPoint - processValue;

    // Calculate Pterm and limit error overflow
    if (error > pid->dat.maxError)
    {
        p_term = MAX_INT;
    }
    else if (error < -pid->dat.maxError)
    {
        p_term = -MAX_INT;
    }
    else
    {
        p_term = pid->dat.P_Factor * error;
    }

    // Calculate Iterm and limit integral runaway
    temp = pid->dat.sumError + error;
    if (temp > pid->dat.maxSumError)
    {
        i_term = MAX_I_TERM;
        pid->dat.sumError = pid->dat.maxSumError;
    }
    else if (temp < -pid->dat.maxSumError)
    {
        i_term = -MAX_I_TERM;
        pid->dat.sumError = -pid->dat.maxSumError;
    }
    else
    {
        pid->dat.sumError = temp;
        i_term = pid->dat.I_Factor * pid->dat.sumError;
    }

    // Calculate Dterm
    d_term = pid->dat.D_Factor * (pid->dat.lastProcessValue - processValue);

    pid->dat.lastProcessValue = processValue;

    ret = (p_term + i_term + d_term) / SCALING_FACTOR;
    if (ret > MAX_INT)
    {
        ret = MAX_INT;
    }
    else if (ret < -MAX_INT)
    {
        ret = -MAX_INT;
    }

    return((int16_t)ret);
}

void pid_Reset_Integrator(UNIT_CFG *pid)
{
    pid->dat.sumError = 0;
}
