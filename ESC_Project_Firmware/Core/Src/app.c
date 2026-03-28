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

#include "stm32g4xx_hal.h"

void TIM1_Disable_All(void)
{
    // 1️⃣ Stop all PWM channels
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);

    // 2️⃣ Stop complementary outputs (N channels)
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_4);

    // 3️⃣ Disable TIM1 counter
    __HAL_TIM_DISABLE(&htim1);

    // 4️⃣ Reset all timer registers (optional, ensures timer is off)
    htim1.Instance->CR1 = 0;
    htim1.Instance->CR2 = 0;
    htim1.Instance->CCER = 0;
    htim1.Instance->BDTR = 0;
}


void TIM1_Pins_To_GPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // PA8, PA9, PA10, PA11
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // adjust as needed
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PB9, PB14
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_14;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Optional: set all pins LOW initially
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9 | GPIO_PIN_14, GPIO_PIN_RESET);
}

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

    //Motor_Init();
    //Motor_Start();

    TIM1_Disable_All();
    TIM1_Pins_To_GPIO();
    // Set PA8 and PB14 HIGH
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    // 3️⃣ Set outputs
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // Phase A HIGH
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); // Phase B LOW
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
    //Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_PWM, 100);
    //Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_LOW, 0);

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
