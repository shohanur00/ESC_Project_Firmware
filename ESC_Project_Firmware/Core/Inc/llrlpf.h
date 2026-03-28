/*
 * llrlpg.h
 *
 *  Created on: Mar 28, 2026
 *      Author: WALTON
 */

#ifndef INC_LLRLPF_H_
#define INC_LLRLPF_H_
#include "main.h"

void    LPF_Struct_Init(void);
void    LPF_Set_Alpha(uint8_t lpf_index, uint8_t val);
uint8_t LPF_Get_Alpha(uint8_t lpf_index);
int32_t LPF_Get_Filtered_Value(uint8_t lpf_index, int32_t val);
void    LPF_Init(void);



#endif /* INC_LLRLPF_H_ */
