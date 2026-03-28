#include "llrlpf.h"

#define NUMBER_OF_LPF 9

typedef struct lpf_t{
  uint8_t  Alpha ;
  int32_t  Input ;
  int32_t  Output;
}lpf_t;

lpf_t LPF[NUMBER_OF_LPF];

void LPF_Struct_Init(void){
  for(uint8_t i=0; i<NUMBER_OF_LPF; i++){
    LPF[i].Alpha = 0;
    LPF[i].Input = 0;
	LPF[i].Output = 0;
  }
}

void LPF_Set_Alpha(uint8_t lpf_index, uint8_t val){
  LPF[lpf_index].Alpha = val;
}

uint8_t LPF_Get_Alpha(uint8_t lpf_index){
  return LPF[lpf_index].Alpha;
}

int32_t LPF_Get_Filtered_Value(uint8_t lpf_index, int32_t val){
  int32_t tmp1=LPF_Get_Alpha(lpf_index);
  tmp1*=val;
  int32_t tmp2=(100 - LPF_Get_Alpha(lpf_index));
  tmp2*=LPF[lpf_index].Output ;
	LPF[lpf_index].Output = tmp1 + tmp2;
	LPF[lpf_index].Output/= 100;
  return LPF[lpf_index].Output;
}

void LPF_Init(void){
  LPF_Struct_Init();
}
