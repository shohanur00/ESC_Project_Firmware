#include "main.h"
#include "drv8301.h"
#include "timebase.h"
#include <stdio.h>   // for sprintf
#include <string.h>  // for strlen
#include "debug.h"
#include "sensor.h"
#include "motor.h"

DRV8301_HandleTypeDef drv;

uint8_t current_step = 1;

// ---------------- Application Setup ----------------
void App_Setup(void)
{
    // Init periodic timebase (1 kHz)
    Timebase_Init(1000);
    // Enable gate driver
    HAL_GPIO_WritePin(EN_GATE_GPIO_Port, EN_GATE_Pin, GPIO_PIN_SET);
    HAL_Delay(10); // small delay to ensure DRV8301 sees EN_GATE high
    // SPI & CS pin for DRV8301
    drv.hspi = &hspi1;
    drv.CS_Port = SPI1_CS_GPIO_Port;
    drv.CS_Pin  = SPI1_CS_Pin;

    HAL_GPIO_WritePin(drv.CS_Port, drv.CS_Pin, GPIO_PIN_SET); // CS idle high

    Motor_Init();
    Motor_Start();

    //Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_OFF, 40);
    //Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_PWM, 40);
    //Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_PWM, 60);

    DRV8301_Init(&drv);
    DRV8301_SetCSAGain(&drv,DRV8301_CSA_GAIN_40);
    Sensor_ADC_Init();
    Sensor_Current_Amp_Offset_Measure();
    // Align rotor
    Timebase_DownCounter_SS_Set_Securely(1, 500);
    Timebase_DownCounter_SS_Set_Securely(2, 2);
    Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_PWM, 100);
    Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_LOW, 0);

    //DRV8301_DC_Cal_High(&drv);
}

// ---------------- Application Main Loop ----------------
void App_Main_Loop(void)
{
    if(Timebase_DownCounter_SS_Continuous_Expired_Event(1))
    {

    	Sensor_ADC_Debug_Print();
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        Debug_Add_Log("Offset_A: %ld mA | Offset_B: %ld mA\r\n",
                      (long)(current_offset_a * 1000.0f),
                      (long)(current_offset_b * 1000.0f));

        Debug_Add_Log("CurrA_A: %ld mA | Curr_B: %ld mA\r\n",
                              (long)(Sensor_Get_Phase_A_Current() * 1000.0f),
                              (long)(Sensor_Get_Phase_B_Current() * 1000.0f));
        Debug_Send_Log();
    }

//    if(Timebase_DownCounter_SS_Continuous_Expired_Event(2))
//    {
//
//    	Motor_Commutate_Step(current_step, 30.0f); // 10% duty cycle
//    	current_step++;
//    	if (current_step > 6) current_step = 1;
//
//    }

    Timebase_Main_Loop_Executables();
}
