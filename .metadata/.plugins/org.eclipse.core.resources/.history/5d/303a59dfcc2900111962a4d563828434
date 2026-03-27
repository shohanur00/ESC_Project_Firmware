#ifndef __DRV8301_H
#define __DRV8301_H

#include "main.h"
#include "drv8301_types.h"
#include "drv8301_config.h"

/* ============================================================
 * Initialization
 * ============================================================ */
HAL_StatusTypeDef DRV8301_Init(DRV8301_HandleTypeDef *drv);

/* ============================================================
 * SPI Register Access (Low-Level)
 * ============================================================ */
HAL_StatusTypeDef DRV8301_WriteRegister(DRV8301_HandleTypeDef *drv, drv8301_reg_t reg, uint16_t data);
HAL_StatusTypeDef DRV8301_ReadRegister (DRV8301_HandleTypeDef *drv, drv8301_reg_t reg, uint16_t *data);

/* ============================================================
 * High-Level API
 * ============================================================ */
HAL_StatusTypeDef DRV8301_SetGateCurrent(DRV8301_HandleTypeDef *drv, uint16_t value);
HAL_StatusTypeDef DRV8301_Enable        (DRV8301_HandleTypeDef *drv, bool enable);
HAL_StatusTypeDef DRV8301_GetStatus1(DRV8301_HandleTypeDef *drv, uint16_t *status);
HAL_StatusTypeDef DRV8301_GetStatus2(DRV8301_HandleTypeDef *drv, uint16_t *status);
HAL_StatusTypeDef DRV8301_SetCSAGain    (DRV8301_HandleTypeDef *drv, DRV8301_CSA_Gain_t gain);
uint8_t           DRV8301_GetCSAGain    (DRV8301_HandleTypeDef *drv);

/* ============================================================
 * Fault Handling
 * ============================================================ */
HAL_StatusTypeDef DRV8301_ClearFaults   (DRV8301_HandleTypeDef *drv);
bool              DRV8301_IsFaulted     (DRV8301_HandleTypeDef *drv);

#endif /* __DRV8301_H */
