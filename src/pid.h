/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : iap
                                      
 Hardware      : ADuC702x

 Description   : iap
*************************************************************************************************/

#ifndef PID_H
#define PID_H

#include "stdint.h"
#include "AppTypes.h"

#define SCALING_FACTOR  128

/*typedef struct PID_DATA
{
    //! Last process value, used to find derivative of process value.
    int16_t lastProcessValue;
    //! Summation of errors, used for integrate calculations
    int32_t sumError;
    //! The Proportional tuning constant, multiplied with SCALING_FACTOR
    int16_t P_Factor;
    //! The Integral tuning constant, multiplied with SCALING_FACTOR
    int16_t I_Factor;
    //! The Derivative tuning constant, multiplied with SCALING_FACTOR
    int16_t D_Factor;
    //! Maximum allowed error, avoid overflow
    int16_t maxError;
    //! Maximum allowed sumerror, avoid overflow
    int32_t maxSumError;
} pidData_t;*/

// Maximum value of variables
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      (MAX_LONG / 2)

// Boolean values
#define FALSE           0
#define TRUE            1

void pid_init(int16_t p_factor, int16_t i_factor, int16_t d_factor, UNIT_CFG *pid);
int16_t pid_controller(int16_t setPoint, int16_t processValue, UNIT_CFG *pid);
void pid_reset_integrator(UNIT_CFG *pid);

#endif
