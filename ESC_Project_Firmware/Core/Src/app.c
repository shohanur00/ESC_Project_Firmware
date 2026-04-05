#include "main.h"
#include "drv8301.h"
#include "timebase.h"
#include <stdio.h>   // for sprintf
#include <string.h>  // for strlen
#include "debug.h"
#include "sensor.h"
#include "motor.h"
#include "foc_transforms.h"

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

void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable DWT
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Start counter
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - start) < ticks);
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

    Motor_Init();
    Motor_Start();
    DRV8301_Init(&drv);
    DRV8301_SetCSAGain(&drv,DRV8301_CSA_GAIN_20);
    Sensor_ADC_Init();
    //Sensor_Current_Amp_Offset_Measure();
    // Align rotor
    Timebase_DownCounter_SS_Set_Securely(1, 500);
    Timebase_DownCounter_SS_Set_Securely(2, 1);
    Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_PWM, 100);
    Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_LOW, 100);
    Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_LOW, 100);
    //DRV8301_DC_Cal_High(&drv);
}




// ---------------- Application Main Loop ----------------
void App_Main_Loop(void)
{
	int gain;

    if(Timebase_DownCounter_SS_Continuous_Expired_Event(1))
    {

//    	Clarke_t out =  FOC_ClarkeTransform();
    	//Sensor_ADC_Debug_Print();
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//        Debug_Add_Log("Offset_A: %d  | Offset_B: %d \r\n",
//                      (current_offset_a_adc),
//                      (current_offset_b_adc));
////
//        Debug_Add_Log("CurrA_A: %ld mA | Curr_B: %ld mA\r\n",
//                              (long)(Sensor_Get_Phase_A_Current() * 1000.0f),
//                              (long)(Sensor_Get_Phase_B_Current() * 1000.0f));
//        Debug_Add_Log("Curr_A ADC Ac = %d  Curr_B ADC = %d  Gain:%d \r\n",adc2_buffer[0],adc2_buffer[1],DRV8301_GetCSAGain(&drv));




    	/* Read gain once */
    	gain = DRV8301_GetCSAGain(&drv);

    	/* Debug print (ADC values) */
//    	Debug_Add_Log("Curr_A ADC = %d  Curr_B ADC = %d  Gain:%d\r\n",
//    	              adc2_buffer_filtered[0],
//    	              adc2_buffer_filtered[1],
//    	              gain);
//
//    	/* Debug print (Current values) */
//    	Debug_Add_Log("Ialpha = %d   Ibeta = %d   Gain:%d\r\n",
//    	              (int)out.Ialpha,
//    	              (int)out.Ibeta,
//    	              (int)gain);
    	//Sensor_ADC_Debug_Print();
        // Header
//        Debug_Add_Log("Curr_A_Ac  Curr_A_Fi  Curr_B_Ac  Curr_B_Fi\r\n");
//        Debug_Add_Log("-----------------------------------------\r\n");

        // Data row example
//        Debug_Add_Log("%4d        %4d        %4d        %4d\r\n",
//                      adc2_buffer[0], adc2_buffer_filtered[0],
//                      adc2_buffer[1], adc2_buffer_filtered[1]);
//        Debug_Send_Log();
    }

    if(updateFlag){
    	Sensor_Main_Loop_Executable();
    	/* Read gain once */
    	gain = DRV8301_GetCSAGain(&drv);

    	/* Debug print (ADC values) */
//    	Debug_Add_Log("Curr_A ADC = %d  Curr_B ADC = %d  Gain:%d\r\n",
//    	              adc2_buffer_filtered[0],
//    	              adc2_buffer_filtered[1],
//    	              gain);
    	Sensor_ADC_Debug_Print();
    	Debug_Send_Log();
    	updateFlag = 0;
    }

//    if(Timebase_DownCounter_SS_Continuous_Expired_Event(2))
//    {
//    	//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//    	Motor_Commutate_Step(current_step, 40.0f); // 10% duty cycle
//    	current_step++;
//    	if (current_step > 6) current_step = 1;
//
//    }

    Timebase_Main_Loop_Executables();
}
