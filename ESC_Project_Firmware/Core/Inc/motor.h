#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

/* --- Configuration --- */
// STM32G4 ক্লক ১৭০ মেগাহার্টজ (যদি আপনার সেটিংস আলাদা হয় তবে এটি পরিবর্তন করুন)
#define MOTOR_TIMER_CLK      170000000U
#define DEFAULT_PWM_FREQ     20000U      // ডিফল্ট ২০ কিলোহার্জ

/* --- Enums for Clean Code --- */

/**
 * @brief ফেজ সিলেকশন এনুম
 */
typedef enum {
    MOTOR_PHASE_A = 1,
    MOTOR_PHASE_B = 2,
    MOTOR_PHASE_C = 3
} Motor_Phase_t;

/**
 * @brief প্রতিটি ফেজের ড্রাইভ মোড
 * 6-Step Commutation এর জন্য এটি অত্যন্ত জরুরি
 */
typedef enum {
    PHASE_MODE_OFF   = 0, // High and Low both OFF (Floating)
    PHASE_MODE_PWM   = 1, // High-Side PWM active
    PHASE_MODE_LOW   = 2  // Low-Side fully ON (GND connection)
} Phase_Mode_t;

/**
 * @brief গেট ড্রাইভার স্ট্যাটাস
 */
typedef enum {
    GATE_DISABLE = 0,
    GATE_ENABLE  = 1
} Gate_Status_t;

/* --- External Handlers --- */
extern TIM_HandleTypeDef htim1;

/* --- Function Prototypes --- */

// সিস্টেম কন্ট্রোল
void Motor_Init(void);
void Motor_Start(void);
void Motor_Stop(void);
void Motor_Set_Gate(Gate_Status_t status);

// প্যারামিটার কন্ট্রোল
void Motor_Set_Frequency(uint32_t freq_hz);
void Motor_Set_All_Duty(float duty_percent);

// ইন্ডিভিজুয়াল ফেজ কন্ট্রোল (Low-Level)
void Motor_Apply_Phase_Control(Motor_Phase_t phase, Phase_Mode_t mode, float duty);

// সিক্স-স্টেপ কমিউটেশন হেল্পার
void Motor_Commutate_Step(uint8_t step, float duty);

#endif /* __MOTOR_H */
