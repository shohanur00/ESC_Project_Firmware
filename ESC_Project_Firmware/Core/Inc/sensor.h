#ifndef __SENSOR_H
#define __SENSOR_H

#include "main.h"
#include "debug.h"
#include <stdio.h>

/********************************************
 * Voltage_A_Sense = adc1_buff[0]	IN4
 * Temp_Sense	   = adc1_buff[1]	IN11
 * Bus_Voltage	   = adc1_buff[2]	IN12
 * Total_Current   = adc1_buff[3]   IN15
 * Current_A	   = adc2_buff[0]   IN3
 * Current_B	   = adc2_buff[1]   IN4
 * Virtual_N	   = adc2_buff[2]   IN12
 * Voltage_C_Sense = adc2_buff[3]	IN13
 * Voltage_B_Sense = adc2_buff[4]	IN17
 *******************************************/



void Sensor_ADC2_DMA_Start(void);
void Sensor_ADC_Debug_Print(void);
void Sensor_ADC_Init(void);
void Sensor_Current_Amp_Offset_Measure(void);
float Sensor_Get_Phase_A_Current(void);
float Sensor_Get_Phase_B_Current(void);
float Sensor_Get_Phase_C_Current(void);
void Sensor_Main_Loop_Executable(void);

#endif
