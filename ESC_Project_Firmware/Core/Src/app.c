#include "main.h"
#include "drv8301.h"
#include "timebase.h"
#include <stdio.h>   // for sprintf
#include <string.h>  // for strlen
#include "debug.h"
#include "sensor.h"

DRV8301_HandleTypeDef drv;



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
    // -------- 3-phase PWM setup --------
    /*HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, htim1.Init.Period*20/100);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Init.Period*20/100);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period*20/100);

	*/
    DRV8301_Init(&drv);
    DRV8301_SetCSAGain(&drv,DRV8301_CSA_GAIN_40);
    Sensor_ADC_Init();
    Timebase_DownCounter_SS_Set_Securely(1, 500);
    //DRV8301_DC_Cal_High(&drv);
}

// ---------------- Application Main Loop ----------------
void App_Main_Loop(void)
{
    if(Timebase_DownCounter_SS_Continuous_Expired_Event(1))
    {

    	Sensor_ADC_Debug_Print();
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        Debug_Send_Log();
    }

    Timebase_Main_Loop_Executables();
}
