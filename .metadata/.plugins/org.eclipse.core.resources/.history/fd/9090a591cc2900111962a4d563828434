#include "drv8301.h"

/* ================= Internal CS Control ================= */
static inline void _CS_Low(DRV8301_HandleTypeDef *drv)
{
    HAL_GPIO_WritePin(drv->CS_Port, drv->CS_Pin, GPIO_PIN_RESET);
    for(volatile int i=0; i<20; i++); // Short setup delay for G4 high clock
}

static inline void _CS_High(DRV8301_HandleTypeDef *drv)
{
    for(volatile int i=0; i<20; i++); // Hold time
    HAL_GPIO_WritePin(drv->CS_Port, drv->CS_Pin, GPIO_PIN_SET);
}

/* ================= SPI Write ================= */
HAL_StatusTypeDef DRV8301_WriteRegister(DRV8301_HandleTypeDef *drv, drv8301_reg_t reg, uint16_t data)
{
    // Frame: [W=0][Addr=4 bit][Data=11 bit]
    uint16_t tx = ((uint16_t)(reg & 0x0F) << 11) | (data & 0x07FF);

    _CS_Low(drv);
    // Since SPI is in 16-bit mode, size is 1 (one 16-bit word)
    HAL_StatusTypeDef status = HAL_SPI_Transmit(drv->hspi, (uint8_t*)&tx, 1, DRV8301_SPI_TIMEOUT);
    _CS_High(drv);

    return status;
}

/* ================= SPI Read ================= */
HAL_StatusTypeDef DRV8301_ReadRegister(DRV8301_HandleTypeDef *drv, drv8301_reg_t reg, uint16_t *data)
{
    // Cycle 1: Send Read Command [R=1][Addr=4 bit][Dummy Data]
    uint16_t tx = DRV8301_SPI_READ_BIT | ((uint16_t)(reg & 0x0F) << 11);
    uint16_t rx = 0;
    uint16_t dummy = 0x8000; // Read Status 1 command as dummy for the next cycle

    // Step 1: Transmit Address to read
    _CS_Low(drv);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(drv->hspi, (uint8_t*)&tx, 1, DRV8301_SPI_TIMEOUT);
    _CS_High(drv);

    if(status != HAL_OK) return status;

    // Small delay between SPI frames
    for(volatile int i=0; i<30; i++);

    // Step 2: Transmit dummy and receive the data from the previous command
    _CS_Low(drv);
    status = HAL_SPI_TransmitReceive(drv->hspi, (uint8_t*)&dummy, (uint8_t*)&rx, 1, DRV8301_SPI_TIMEOUT);
    _CS_High(drv);

    if(status == HAL_OK) {
        *data = rx & 0x07FF; // Mask to get 11-bit data
    }

    return status;
}

/* ================= Init ================= */
HAL_StatusTypeDef DRV8301_Init(DRV8301_HandleTypeDef *drv)
{
    // EN_GATE pin must be high before calling this
    HAL_Delay(20); // Wait for DRV8301 to wake up

    // Clear any power-up faults
    return DRV8301_ClearFaults(drv);
}

/* ================= Status Functions ================= */
HAL_StatusTypeDef DRV8301_GetStatus1(DRV8301_HandleTypeDef *drv, uint16_t *status)
{
    return DRV8301_ReadRegister(drv, DRV8301_REG_STATUS1, status);
}

HAL_StatusTypeDef DRV8301_GetStatus2(DRV8301_HandleTypeDef *drv, uint16_t *status)
{
    return DRV8301_ReadRegister(drv, DRV8301_REG_STATUS2, status);
}

/* ================= CSA Gain Control ================= */
HAL_StatusTypeDef DRV8301_SetCSAGain(DRV8301_HandleTypeDef *drv, DRV8301_CSA_Gain_t gain)
{
    uint16_t reg;

    // Read current Control Register 2
    if(DRV8301_ReadRegister(drv, DRV8301_REG_CTRL2, &reg) != HAL_OK)
        return HAL_ERROR;

    // Mask bits 2-3 (GAIN) and set new value
    reg &= ~(0x03 << 2);
    reg |= ((uint16_t)gain << 2);

    return DRV8301_WriteRegister(drv, DRV8301_REG_CTRL2, reg);
}

uint8_t DRV8301_GetCSAGain(DRV8301_HandleTypeDef *drv)
{
    uint16_t reg;
    if(DRV8301_ReadRegister(drv, DRV8301_REG_CTRL2, &reg) != HAL_OK)
        return 0xFF;

    return (uint8_t)((reg >> 2) & 0x03);
}

/* ================= Fault Management ================= */
HAL_StatusTypeDef DRV8301_ClearFaults(DRV8301_HandleTypeDef *drv)
{
    uint16_t reg;

    // CLR_FLT bit is in Control Register 1 (Bit 0)
    if(DRV8301_ReadRegister(drv, DRV8301_REG_CTRL1, &reg) != HAL_OK)
        return HAL_ERROR;

    reg |= (1 << 0); // Set CLR_FLT bit

    return DRV8301_WriteRegister(drv, DRV8301_REG_CTRL1, reg);
}

bool DRV8301_IsFaulted(DRV8301_HandleTypeDef *drv)
{
    uint16_t status1;
    // Status Register 1 has general fault bits
    if(DRV8301_GetStatus1(drv, &status1) != HAL_OK)
        return true;

    // If any bit in Status Register 1 is set (besides reserved), it's a fault
    return (status1 != 0);
}
