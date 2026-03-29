#include <stdint.h>
#include "llrlpf.h"

#define LPF_COUNT 11

// প্রতিটা filter er জন্য input, output এবং alpha আলাদা array
uint8_t LPF_Alpha[LPF_COUNT];
int32_t LPF_Output[LPF_COUNT];

// Filter initialize
void LPF_Init(void){
    for(uint8_t i=0; i<LPF_COUNT; i++){
        LPF_Alpha[i] = 0;
        LPF_Output[i] = 0;
    }
}

// Filter alpha set করা
void LPF_Set_Alpha(uint8_t index, uint8_t alpha){
    if(index < LPF_COUNT) LPF_Alpha[index] = alpha;
}

// Filter চালানো (new ADC value input)
int32_t LPF_Run(uint8_t index, int32_t adc_val){
    if(index >= LPF_COUNT) return 0;
    // 1st order LPF: Output = alpha*input + (1-alpha)*previous_output
    LPF_Output[index] = (LPF_Alpha[index]*adc_val + (100 - LPF_Alpha[index])*LPF_Output[index])/100;
    return LPF_Output[index];
}
