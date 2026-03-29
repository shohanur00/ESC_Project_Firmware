// llrlpf.h
#ifndef LLRLPF_H
#define LLRLPF_H

#include <stdint.h>

// Number of LPF channels
#define LPF_COUNT 9

// Function declarations

// Filter initialize
void LPF_Init(void);

// Set alpha for a specific filter (0-100)
void LPF_Set_Alpha(uint8_t index, uint8_t alpha);

// Run filter for new input, return filtered value
int32_t LPF_Run(uint8_t index, int32_t adc_val);

#endif // LLRLPF_H
