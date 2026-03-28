/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;

extern UART_HandleTypeDef huart3;

extern DMA_HandleTypeDef hdma_usart3_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;

extern TIM_HandleTypeDef htim17;  // <-- Declare here for global access
extern uint16_t adc1_buffer[4]; // ADC1
extern uint16_t adc2_buffer[5]; // ADC2
extern float current_offset_a;
extern float current_offset_b;
extern float voltage_a;
extern float voltage_b;
extern float voltage_c;
extern float current_a;
extern float current_b;
extern float current_c;
extern float temperature;
extern float bus_voltage;
extern float total_current;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_14
#define LED_GPIO_Port GPIOC
#define Btn_Pin GPIO_PIN_15
#define Btn_GPIO_Port GPIOC
#define U_ZCD_Pin GPIO_PIN_0
#define U_ZCD_GPIO_Port GPIOA
#define V_ZCD_Pin GPIO_PIN_1
#define V_ZCD_GPIO_Port GPIOA
#define W_ZCD_Pin GPIO_PIN_2
#define W_ZCD_GPIO_Port GPIOA
#define Voltage_A_SENSE_Pin GPIO_PIN_3
#define Voltage_A_SENSE_GPIO_Port GPIOA
#define Voltage_B_SENSE_Pin GPIO_PIN_4
#define Voltage_B_SENSE_GPIO_Port GPIOA
#define Voltage_C_SENSE_Pin GPIO_PIN_5
#define Voltage_C_SENSE_GPIO_Port GPIOA
#define Current_A_SENSE_Pin GPIO_PIN_6
#define Current_A_SENSE_GPIO_Port GPIOA
#define Current_B_SENSE_Pin GPIO_PIN_7
#define Current_B_SENSE_GPIO_Port GPIOA
#define Total_Current_SENSE_Pin GPIO_PIN_0
#define Total_Current_SENSE_GPIO_Port GPIOB
#define POWER_Bus_Voltage_SENSE_Pin GPIO_PIN_1
#define POWER_Bus_Voltage_SENSE_GPIO_Port GPIOB
#define Virtual_N_Pin GPIO_PIN_2
#define Virtual_N_GPIO_Port GPIOB
#define Temp_SENSE_Pin GPIO_PIN_12
#define Temp_SENSE_GPIO_Port GPIOB
#define NFAULT_Pin GPIO_PIN_13
#define NFAULT_GPIO_Port GPIOB
#define INL_B_Pin GPIO_PIN_14
#define INL_B_GPIO_Port GPIOB
#define EN_GATE_Pin GPIO_PIN_15
#define EN_GATE_GPIO_Port GPIOB
#define INH_A_Pin GPIO_PIN_8
#define INH_A_GPIO_Port GPIOA
#define INH_B_Pin GPIO_PIN_9
#define INH_B_GPIO_Port GPIOA
#define INH_C_Pin GPIO_PIN_10
#define INH_C_GPIO_Port GPIOA
#define INL_A_Pin GPIO_PIN_11
#define INL_A_GPIO_Port GPIOA
#define DC_CAL_Pin GPIO_PIN_15
#define DC_CAL_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_6
#define SPI1_CS_GPIO_Port GPIOB
#define NOCTW_Pin GPIO_PIN_7
#define NOCTW_GPIO_Port GPIOB
#define INL_C_Pin GPIO_PIN_9
#define INL_C_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
