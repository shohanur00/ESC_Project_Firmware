#ifndef __DRV8301_CONFIG_H
#define __DRV8301_CONFIG_H

/* ============================================================
 * DRV8301 Register Bit Definitions
 * ============================================================ */

/* --- Control Register (0x00) --- */
#define DRV8301_CTRL_CLR_FLT        (1 << 0)   // Clear fault bit
#define DRV8301_CTRL_BRAKE          (1 << 1)   // Coast/Brake
#define DRV8301_CTRL_3PWM_6PWM      (1 << 2)   // 0=6PWM, 1=3PWM
#define DRV8301_CTRL_OTW_REP        (1 << 3)   // OTW report to nFAULT

/* --- Gate Drive HS Register (0x01) --- */
/* IDRIVEP_HS bits [3:0]: peak source current */
/* IDRIVEN_HS bits [7:4]: peak sink current   */
#define DRV8301_GATE_HS_IDRIVEP(x)  ((x) & 0x0F)
#define DRV8301_GATE_HS_IDRIVEN(x)  (((x) & 0x0F) << 4)

/* --- Gate Drive LS Register (0x02) --- */
#define DRV8301_GATE_LS_IDRIVEP(x)  ((x) & 0x0F)
#define DRV8301_GATE_LS_IDRIVEN(x)  (((x) & 0x0F) << 4)
#define DRV8301_GATE_LS_TDRIVE(x)   (((x) & 0x03) << 8)  // peak drive time

/* --- OCP Control Register (0x03) --- */
#define DRV8301_OCP_TRETRY          (1 << 10)  // retry time
#define DRV8301_OCP_DEAD_TIME(x)    (((x) & 0x03) << 8)
#define DRV8301_OCP_OCP_MODE(x)     (((x) & 0x03) << 6)
#define DRV8301_OCP_OCP_DEG(x)      (((x) & 0x03) << 4)
#define DRV8301_OCP_VDS_LVL(x)      ((x) & 0x0F)

/* --- CSA Control Register (0x04) --- */
#define DRV8301_CSA_GAIN_BITS(x)    (((x) & 0x03) << 2)  // bits [3:2]
#define DRV8301_CSA_GAIN_MASK       (0x03 << 2)
#define DRV8301_CSA_DIS_SEN         (1 << 4)   // disable sense overcurrent
#define DRV8301_CSA_CSA_CAL_A       (1 << 5)   // calibrate phase A
#define DRV8301_CSA_CSA_CAL_B       (1 << 6)   // calibrate phase B
#define DRV8301_CSA_CSA_CAL_C       (1 << 7)   // calibrate phase C
#define DRV8301_CSA_VREF_DIV        (1 << 8)   // VREF divider
#define DRV8301_CSA_LS_REF          (1 << 9)   // low-side reference

/* ============================================================
 * Default Register Values
 * ============================================================ */
#define DRV8301_DEFAULT_CTRL        0x0000
#define DRV8301_DEFAULT_GATE_HS     0x0000
#define DRV8301_DEFAULT_GATE_LS     0x0000
#define DRV8301_DEFAULT_OCP_CTRL    0x0000
#define DRV8301_DEFAULT_CSA_CTRL    0x0000

/* ============================================================
 * SPI Timing & Masks
 * ============================================================ */
#define DRV8301_SPI_DATA_MASK       0x7FFU   // 11-bit data mask
#define DRV8301_SPI_ADDR_SHIFT      11
#define DRV8301_SPI_READ_BIT        0x8000U
#define DRV8301_SPI_TIMEOUT         100U     // ms

#endif /* __DRV8301_CONFIG_H */
