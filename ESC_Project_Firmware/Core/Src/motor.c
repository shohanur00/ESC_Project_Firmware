#include "motor.h"

/**
 * @brief Enable or Disable the Gate Driver (e.g., DRV8301)
 * @param status: GATE_ENABLE or GATE_DISABLE
 */
void Motor_Set_Gate(Gate_Status_t status) {
    if (status == GATE_ENABLE) {
        HAL_GPIO_WritePin(EN_GATE_GPIO_Port, EN_GATE_Pin, GPIO_PIN_SET);
        HAL_Delay(10); // Wait for gate driver to stabilize
    } else {
        HAL_GPIO_WritePin(EN_GATE_GPIO_Port, EN_GATE_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief Set the PWM Frequency dynamically
 * @param freq_hz: Desired frequency in Hz (e.g., 20000 for 20kHz)
 */
void Motor_Set_Frequency(uint32_t freq_hz) {
    if (freq_hz < 1000) return; // Minimum frequency limit

    uint32_t timer_clk = MOTOR_TIMER_CLK;
    uint32_t prescaler = htim1.Instance->PSC;

    // ARR Calculation: (Clock / ((PSC + 1) * Freq)) - 1
    uint32_t new_arr = (timer_clk / ((prescaler + 1) * freq_hz)) - 1;

    htim1.Instance->ARR = new_arr;
}

/**
 * @brief Apply control logic to a specific phase
 * @param phase: MOTOR_PHASE_A, B, or C
 * @param mode: PWM (High-side), LOW (Low-side ON), or OFF (Floating)
 * @param duty: Duty cycle percentage (0.0 to 100.0)
 */
void Motor_Apply_Phase_Control(Motor_Phase_t phase, Phase_Mode_t mode, float duty) {
    uint32_t channel;

    if (phase == MOTOR_PHASE_A)      channel = TIM_CHANNEL_1;
    else if (phase == MOTOR_PHASE_B) channel = TIM_CHANNEL_2;
    else                             channel = TIM_CHANNEL_3;

    // Constrain duty cycle between 0 and 100%
    if (duty > 100.0f) duty = 100.0f;
    if (duty < 0.0f)   duty = 0.0f;

    uint32_t compare_val = (uint32_t)((htim1.Instance->ARR) * (duty / 100.0f));

    switch (mode) {
        case PHASE_MODE_PWM:
            // Activate High-side PWM, ensure Low-side is OFF
            HAL_TIM_PWM_Start(&htim1, channel);
            HAL_TIMEx_PWMN_Start(&htim1, channel);
            __HAL_TIM_SET_COMPARE(&htim1, channel, compare_val);
            break;

        case PHASE_MODE_LOW:
            // Force Low-side MOSFET ON, High-side OFF
            HAL_TIM_PWM_Stop(&htim1, channel);
            HAL_TIMEx_PWMN_Start(&htim1, channel);
            __HAL_TIM_SET_COMPARE(&htim1, channel, 0);
            break;

        case PHASE_MODE_OFF:
        default:
            // Both High-side and Low-side OFF (High Impedance / Floating)
            HAL_TIM_PWM_Stop(&htim1, channel);
            HAL_TIMEx_PWMN_Stop(&htim1, channel);
            __HAL_TIM_SET_COMPARE(&htim1, channel, 0);
            break;
    }
}

/**
 * @brief Standard 6-Step Commutation Sequence for BLDC
 * @param step: Step number (1 to 6)
 * @param duty: PWM Duty Cycle for the active high-side phase
 */
void Motor_Commutate_Step(uint8_t step, float duty) {
    switch (step) {
        case 1: // U+ (PWM), V- (GND), W (Float)
            Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_PWM, duty);
            Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_LOW, 100.0f);
            Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_OFF, 0.0f);
            break;
        case 2: // U+ (PWM), W- (GND), V (Float)
            Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_PWM, duty);
            Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_LOW, 100.0f);
            Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_OFF, 0.0f);
            break;
        case 3: // V+ (PWM), W- (GND), U (Float)
            Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_PWM, duty);
            Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_LOW, 100.0f);
            Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_OFF, 0.0f);
            break;
        case 4: // V+ (PWM), U- (GND), W (Float)
            Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_PWM, duty);
            Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_LOW, 100.0f);
            Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_OFF, 0.0f);
            break;
        case 5: // W+ (PWM), U- (GND), V (Float)
            Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_PWM, duty);
            Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_LOW, 100.0f);
            Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_OFF, 0.0f);
            break;
        case 6: // W+ (PWM), V- (GND), U (Float)
            Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_PWM, duty);
            Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_LOW, 100.0f);
            Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_OFF, 0.0f);
            break;
        default:
            Motor_Stop();
            break;
    }
}

/**
 * @brief Initialize Motor System and Gate Driver
 */
void Motor_Init(void) {
    Motor_Set_Gate(GATE_ENABLE);
    Motor_Stop();
    Motor_Set_Frequency(DEFAULT_PWM_FREQ);
}

/**
 * @brief Set system to Ready state with all phases floating
 */
void Motor_Start(void) {
    Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_OFF, 0);
    Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_OFF, 0);
    Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_OFF, 0);
}

/**
 * @brief Stop motor by floating all phases (Coast to Stop)
 */
void Motor_Stop(void) {
    Motor_Apply_Phase_Control(MOTOR_PHASE_A, PHASE_MODE_OFF, 0);
    Motor_Apply_Phase_Control(MOTOR_PHASE_B, PHASE_MODE_OFF, 0);
    Motor_Apply_Phase_Control(MOTOR_PHASE_C, PHASE_MODE_OFF, 0);
}



// -------- 3-phase PWM setup --------
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
//    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, htim1.Init.Period*20/100);
//
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
//    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Init.Period*20/100);
//
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
//    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period*20/100);
