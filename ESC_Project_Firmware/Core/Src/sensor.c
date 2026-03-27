#include "sensor.h"


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
