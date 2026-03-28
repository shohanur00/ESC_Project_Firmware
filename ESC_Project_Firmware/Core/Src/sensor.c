#include "sensor.h"
#include "motor.h"

#define ADC_MAX        4095.0f
#define VREF           3.3f
#define CSA_GAIN       40.0f
#define SHUNT_RES      0.001f   // <-- change if different
#define ADC2_CURRENT_A_INDEX   0   // IN3
#define ADC2_CURRENT_B_INDEX   1   // IN4


// ------------------- Current Offset Calibration -------------------
void Sensor_Current_Amp_Offset_Measure(void)
{
    const uint16_t samples = 500;
    uint32_t sum_a = 0;
    uint32_t sum_b = 0;

    // 1️⃣ Stop motor / PWM before calibration
    Motor_Stop();

    // 2️⃣ Enable DC calibration mode on DRV8301
    HAL_GPIO_WritePin(DC_CAL_GPIO_Port, DC_CAL_Pin, GPIO_PIN_SET);
    HAL_Delay(20);  // allow amplifier to settle

    // 3️⃣ Collect samples from DMA buffer
    for (uint16_t i = 0; i < samples; i++)
    {
        sum_a += adc2_buffer[ADC2_CURRENT_A_INDEX];
        sum_b += adc2_buffer[ADC2_CURRENT_B_INDEX];

        HAL_Delay(1);  // allow new DMA samples
    }

    // 4️⃣ Disable DC calibration
    HAL_GPIO_WritePin(DC_CAL_GPIO_Port, DC_CAL_Pin, GPIO_PIN_RESET);

    // 5️⃣ Compute average ADC value
    float adc_avg_a = (float)sum_a / samples;
    float adc_avg_b = (float)sum_b / samples;

    // 6️⃣ Convert ADC → voltage
    float voltage_a = (adc_avg_a / ADC_MAX) * VREF;
    float voltage_b = (adc_avg_b / ADC_MAX) * VREF;

    // 7️⃣ Compute current offset (accounting for 1.65V midpoint)
    current_offset_a = (voltage_a - VREF / 2.0f) / (CSA_GAIN * SHUNT_RES);
    current_offset_b = (voltage_b - VREF / 2.0f) / (CSA_GAIN * SHUNT_RES);
}

// ------------------- Read Phase Currents -------------------
float Sensor_Get_Phase_A_Current(void)
{
    float voltage = ((float)adc2_buffer[ADC2_CURRENT_A_INDEX] / ADC_MAX) * VREF;
    float current = (voltage - VREF / 2.0f) / (CSA_GAIN * SHUNT_RES);
    return current - current_offset_a;
}

float Sensor_Get_Phase_B_Current(void)
{
    float voltage = ((float)adc2_buffer[ADC2_CURRENT_B_INDEX] / ADC_MAX) * VREF;
    float current = (voltage - VREF / 2.0f) / (CSA_GAIN * SHUNT_RES);
    return current;// - current_offset_b;
}

// Phase C can be computed by Kirchhoff: Ic = -(Ia + Ib)
float Sensor_Get_Phase_C_Current(void)
{
    return -(Sensor_Get_Phase_A_Current() + Sensor_Get_Phase_B_Current());
}


void Sensor_ADC2_DMA_Start(void){
	 // 1. Disable DMA
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;

	// 2. Setup DMA
	DMA1_Channel2->CPAR = (uint32_t)&ADC2->DR;
	DMA1_Channel2->CMAR = (uint32_t)adc2_buffer;
	DMA1_Channel2->CNDTR = 5;

	DMA1_Channel2->CCR =
		  DMA_CCR_MINC
		| DMA_CCR_CIRC
		| DMA_CCR_PSIZE_0
		| DMA_CCR_MSIZE_0;

	DMA1_Channel2->CCR |= DMA_CCR_EN;

	// 3. Enable ADC DMA
	ADC2->CFGR |= ADC_CFGR_DMAEN | ADC_CFGR_DMACFG;

	// 4. Enable ADC
	if (!(ADC2->CR & ADC_CR_ADEN))
	{
		ADC2->CR |= ADC_CR_ADEN;
		while (!(ADC2->ISR & ADC_ISR_ADRDY));
	}

	// 5. Start conversion
	ADC2->CR |= ADC_CR_ADSTART;
}


// ADC debug print function
void Sensor_ADC_Debug_Print(void)
{
    Debug_Add_Log(
        "================= ADC Readings ================\r\n"
        "ADC1:\r\n"
        "  Voltage_A_Sense = %u  (IN4)\r\n"
        "  Temp_Sense      = %u  (IN11)\r\n"
        "  Bus_Voltage     = %u  (IN12)\r\n"
        "  Total_Current   = %u  (IN15)\r\n"
        "ADC2:\r\n"
        "  Current_A       = %u  (IN3)\r\n"
        "  Current_B       = %u  (IN4)\r\n"
        "  Virtual_N       = %u  (IN12)\r\n"
        "  Voltage_C_Sense = %u  (IN13)\r\n"
        "  Voltage_B_Sense = %u  (IN17)\r\n"
        "===============================================\r\n",
        adc1_buffer[0], adc1_buffer[1], adc1_buffer[2], adc1_buffer[3],
        adc2_buffer[0], adc2_buffer[1], adc2_buffer[2], adc2_buffer[3], adc2_buffer[4]
    );
}


void Sensor_ADC_Init(void){
    // 1. Calibration (once)
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

    HAL_Delay(10);

    // 2. Start ADC1 first
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_buffer, 4) != HAL_OK)
        Error_Handler();

    Sensor_ADC2_DMA_Start();
}





