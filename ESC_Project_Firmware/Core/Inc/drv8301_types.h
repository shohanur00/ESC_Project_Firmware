#ifndef __DRV8301_TYPES_H
#define __DRV8301_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* DRV8301 Register Addresses */
typedef enum {
    DRV8301_REG_STATUS1 = 0x00,
    DRV8301_REG_STATUS2 = 0x01,
    DRV8301_REG_CTRL1   = 0x02,
    DRV8301_REG_CTRL2   = 0x03
} drv8301_reg_t;

/* DRV8301 Handle */
typedef struct {
    SPI_HandleTypeDef *hspi;  // SPI handle
    GPIO_TypeDef      *CS_Port; // Chip select GPIO port
    uint16_t           CS_Pin;  // Chip select GPIO pin
} DRV8301_HandleTypeDef;

/* CSA Gain options */
typedef enum {
    DRV8301_CSA_GAIN_10 = 0,
    DRV8301_CSA_GAIN_20 = 1,
    DRV8301_CSA_GAIN_40 = 2,
    DRV8301_CSA_GAIN_80 = 3
} DRV8301_CSA_Gain_t;

#endif /* __DRV8301_TYPES_H */
