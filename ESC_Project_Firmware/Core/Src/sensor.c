#include "sensor.h"
#include "motor.h"
#include <math.h>
#include "llrlpf.h"


#define ADC_MAX        4095.0f
#define VREF           3.3f
#define CSA_GAIN       40.0f
#define SHUNT_RES      0.001f   // <-- change if different
#define ADC2_CURRENT_A_INDEX   0   // IN3
#define ADC2_CURRENT_B_INDEX   1   // IN4
#define INA180A2_GAIN		  50

// Bus voltage divider
#define R_UP           34800.0f
#define R_DOWN         5100.0f

// NTC
#define NTC_R_FIXED    10000.0f
#define NTC_BETA       3950.0f
#define NTC_T0         298.15f    // 25°C in Kelvin
#define NTC_R0         10000.0f


// ---------- LPF INDEX ----------
#define LPF_VOLTAGE_A      0
#define LPF_VOLTAGE_B      1
#define LPF_VOLTAGE_C      2
#define LPF_BUS_VOLTAGE    3
#define LPF_CURRENT_A      4
#define LPF_CURRENT_B      5
#define LPF_CURRENT_C      6
#define LPF_TOTAL_CURRENT  7
#define LPF_TEMPERATURE    8

// ---------- ADC1 LPF CHANNELS ----------
#define LPF_ADC1_0    0   // ADC1 Channel IN4
#define LPF_ADC1_1    1   // ADC1 Channel IN11
#define LPF_ADC1_2    2   // ADC1 Channel IN12
#define LPF_ADC1_3    3   // ADC1 Channel IN15

// ---------- ADC2 LPF CHANNELS ----------
#define LPF_ADC2_0    4   // ADC2 Channel IN3
#define LPF_ADC2_1    5   // ADC2 Channel IN4
#define LPF_ADC2_2    6   // ADC2 Channel IN12 (Virtual N)
#define LPF_ADC2_3    7   // ADC2 Channel IN13
#define LPF_ADC2_4    8   // ADC2 Channel IN17
#define LPF_OFFSET_A  9
#define LPF_OFFSET_B  10


#define VOLTAGE_ALPHA	10
#define CURRENT_ALPHA	10
#define TEMP_ALPHA		10


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

    uint16_t collected = 0;
    uint16_t last_index = 0xFFFF; // store last DMA index read

    while (collected < samples)
    {
        // Assuming circular DMA with 5 elements as in your Sensor_ADC2_DMA_Start
        uint16_t dma_index = ADC2->DR; // or get current DMA index if using HAL
        // For STM32 without HAL helper, you can read NDTR register
        // dma_index = 5 - DMA1_Channel2->CNDTR;

        if (dma_index != last_index)
        {
            // New sample available
            sum_a += LPF_Run(LPF_OFFSET_A, adc2_buffer[0]); // Current A
            sum_b += LPF_Run(LPF_OFFSET_B, adc2_buffer[1]); // Current B

            last_index = dma_index;
            collected++;
        }
        __NOP(); // small delay to prevent tight loop
    }

    // 4️⃣ Disable DC calibration
    HAL_GPIO_WritePin(DC_CAL_GPIO_Port, DC_CAL_Pin, GPIO_PIN_RESET);

    // 5️⃣ Compute average ADC value
    current_offset_a_adc = sum_a / samples;
    current_offset_b_adc = sum_b / samples;
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
    return current - current_offset_b;
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
        "\r\n================= ADC Readings =================\r\n"
        "ADC1:\r\n"
        "  Voltage_A_Sense = %4u  (IN4)\r\n"
        "  Temp_Sense      = %4u  (IN11)\r\n"
        "  Bus_Voltage     = %4u  (IN12)\r\n"
        "  Total_Current   = %4u  (IN15)\r\n"
        "ADC2:\r\n"
        "  Current_A       = %4u  (IN3)\r\n"
        "  Current_B       = %4u  (IN4)\r\n"
        "  Virtual_N       = %4u  (IN12)\r\n"
        "  Voltage_C_Sense = %4u  (IN13)\r\n"
        "  Voltage_B_Sense = %4u  (IN17)\r\n"
        "------------------------------------------------\r\n"
        "Calculated (scaled):\r\n"
        "  Va=%4d  Vb=%4d  Vc=%4d   (x0.1V)\r\n"
        "  Ia=%5d  Ib=%5d  Ic=%5d   (mA)\r\n"
        "  Bus=%4d  Total_I=%5d     (0.1V / mA)\r\n"
        "  Temp=%4d                (0.1C)\r\n"
        "================================================\r\n",

        // ADC RAW
        adc1_buffer[0], adc1_buffer[1], adc1_buffer[2], adc1_buffer[3],
        adc2_buffer[0], adc2_buffer[1], adc2_buffer[2], adc2_buffer[3], adc2_buffer[4],

        // Scaled Calculated
        (int)(voltage_a * 10),
        (int)(voltage_b * 10),
        (int)(voltage_c * 10),

        (int)(current_a * 1000),
        (int)(current_b * 1000),
        (int)(current_c * 1000),

        (int)(bus_voltage * 10),
        (int)(total_current * 1000),

        (int)(temperature * 10)
    );
}



// ---------- SENSOR LOOP ----------
void Sensor_Main_Loop_Executable(void)
{
    // --------- FILTER RAW ADC VALUES ---------
    adc1_buffer_filtered[0] = LPF_Run(LPF_ADC1_0, adc1_buffer[0]);
    adc1_buffer_filtered[1] = LPF_Run(LPF_ADC1_1, adc1_buffer[1]);
    adc1_buffer_filtered[2] = LPF_Run(LPF_ADC1_2, adc1_buffer[2]);
    adc1_buffer_filtered[3] = LPF_Run(LPF_ADC1_3, adc1_buffer[3]);

    adc2_buffer_filtered[0] = LPF_Run(LPF_ADC2_0, adc2_buffer[0]);
    adc2_buffer_filtered[1] = LPF_Run(LPF_ADC2_1, adc2_buffer[1]);
    adc2_buffer_filtered[2] = LPF_Run(LPF_ADC2_2, adc2_buffer[2]);
    adc2_buffer_filtered[3] = LPF_Run(LPF_ADC2_3, adc2_buffer[3]);
    adc2_buffer_filtered[4] = LPF_Run(LPF_ADC2_4, adc2_buffer[4]);

    // -------- ADC → Voltage (use filtered values) --------
    float v_adc1_0 = ((float)adc1_buffer_filtered[0] / ADC_MAX) * VREF;
    float v_adc1_1 = ((float)adc1_buffer_filtered[1] / ADC_MAX) * VREF;
    float v_adc1_2 = ((float)adc1_buffer_filtered[2] / ADC_MAX) * VREF;
    float v_adc1_3 = ((float)adc1_buffer_filtered[3] / ADC_MAX) * VREF;

    float v_adc2_0 = ((float)adc2_buffer_filtered[0] / ADC_MAX) * VREF;
    float v_adc2_1 = ((float)adc2_buffer_filtered[1] / ADC_MAX) * VREF;
    float v_adc2_3 = ((float)adc2_buffer_filtered[3] / ADC_MAX) * VREF;
    float v_adc2_4 = ((float)adc2_buffer_filtered[4] / ADC_MAX) * VREF;

    // -------- Phase Voltages (scaled ×100) --------
    voltage_a = v_adc1_0 * ((R_UP + R_DOWN) / R_DOWN);
    voltage_b = v_adc2_4 * ((R_UP + R_DOWN) / R_DOWN);
    voltage_c = v_adc2_3 * ((R_UP + R_DOWN) / R_DOWN);
    bus_voltage = v_adc1_2 * ((R_UP + R_DOWN) / R_DOWN);

    // -------- Phase Currents (scaled ×1000) --------
    current_a = -((v_adc2_0 - VREF/2.0f) / (CSA_GAIN * SHUNT_RES) - current_offset_a);
    current_b = -((v_adc2_1 - VREF/2.0f) / (CSA_GAIN * SHUNT_RES) - current_offset_b);
    current_c = -(current_a + current_b);

    // -------- Total Current (scaled ×1000) --------
    total_current = v_adc1_3 / (INA180A2_GAIN * SHUNT_RES);

    // -------- Temperature (scaled ×10) --------
    float r_ntc = (v_adc1_1 * NTC_R_FIXED) / (VREF - v_adc1_1);
    float temp_kelvin = 1.0f / ((1.0f/NTC_T0) + (1.0f/NTC_BETA) * logf(r_ntc / NTC_R0));
    temperature = temp_kelvin - 273.15f;
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
    // ---------- Initialize LPF ----------
    LPF_Init();

    // -------- ADC RAW FILTER (light smoothing) --------
    LPF_Set_Alpha(LPF_ADC1_0, VOLTAGE_ALPHA);
    LPF_Set_Alpha(LPF_ADC1_1, TEMP_ALPHA);
    LPF_Set_Alpha(LPF_ADC1_2, VOLTAGE_ALPHA);
    LPF_Set_Alpha(LPF_ADC1_3, CURRENT_ALPHA);

    LPF_Set_Alpha(LPF_ADC2_0, CURRENT_ALPHA);   // current channels → faster
    LPF_Set_Alpha(LPF_ADC2_1, CURRENT_ALPHA);
    LPF_Set_Alpha(LPF_ADC2_2, VOLTAGE_ALPHA);
    LPF_Set_Alpha(LPF_ADC2_3, VOLTAGE_ALPHA);
    LPF_Set_Alpha(LPF_ADC2_4, VOLTAGE_ALPHA);
    LPF_Set_Alpha(LPF_OFFSET_A, CURRENT_ALPHA);
    LPF_Set_Alpha(LPF_OFFSET_B, CURRENT_ALPHA);
}





