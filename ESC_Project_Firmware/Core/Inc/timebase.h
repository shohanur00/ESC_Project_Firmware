#ifndef TIMEBASE_H
#define TIMEBASE_H

#include "main.h"

//#define  TIMEBASE_UPCOUNTER                1
//#define  TIMEBASE_UPCOUNTER_SUBSECONDS     1
#define  TIMEBASE_DOWNCOUNTER               25
#define  TIMEBASE_DOWNCOUNTER_SUBSECONDS    10
//#define  TIMEBASE_LP_DOWNCOUNTER           1


//Config Functions
void     Timebase_Struct_Init(void);
void     Timebase_Timer_Enable(uint16_t UpdateRateHz);
void     Timebase_Timer_Disable(void);
void     Timebase_LPTimer_Enable(uint16_t UpdateRateHz);
void     Timebase_LPTimer_Disable(void);


//Atomic Operations
void     Timebase_Atomic_Operation_Start(void);
void     Timebase_Atomic_Operation_End(void);


//Token Functions
uint8_t  Timebase_Token_Executing(void);
void     Timebase_Token_Add(void);
void     Timebase_Token_Remove(void);
void     Timebase_Token_Remove_All(void);


//Timer Functions
uint16_t Timebase_Timer_Get_SubSecondsShadow(void);
int32_t  Timebase_Timer_Get_SecondsShadow(void);
uint32_t Timebase_Timer_Get_SecondsUpTime(void);
uint16_t Timebase_Timer_Get_SubSeconds(void);
int32_t  Timebase_Timer_Get_Seconds(void);
void     Timebase_Timer_Set_SubSeconds(uint16_t value);
void     Timebase_Timer_Set_SubSecondsShadow(uint16_t value);
void     Timebase_Timer_Set_SubSecondsUpTime(uint32_t value);
void     Timebase_Timer_Set_Seconds(int32_t value);
uint16_t Timebase_Timer_Get_SubSecondsShadow_Securely(void);
uint32_t Timebase_Timer_Get_SubSecondsUpTime_Securely(void);
int32_t  Timebase_Timer_Get_SecondsShadow_Securely(void);
void     Timebase_Timer_Sync_With_Shadow_Variables(void);
void     Timebase_Timer_Delay_SubSeconds(uint16_t value);
void     Timebase_Timer_Await_SubSeconds(uint16_t value);
void     Timebase_Timer_Delay_Seconds(uint16_t value);


//Low Power Timer Functions
uint16_t Timebase_LPTimer_Get_SubSeconds(void);
int32_t  Timebase_LPTimer_Get_Seconds(void);
void     Timebase_LPTimer_Set_SubSeconds(uint16_t value);
void     Timebase_LPTimer_Set_Seconds(int32_t value);
void     Timebase_LPTimer_Delay_Seconds(uint16_t value);


//Window Calculation
void     Timebase_Window_Timer_Reset(void);
void     Timebase_Window_Timer_Start(void);
int32_t  Timebase_Window_Timer_Get_Interval(void);
int32_t  Timebase_Window_Timer_Get_Interval_Reset(void);


//UpCounter SS Functions
uint8_t  Timebase_UpCounter_SS_Get_Status(uint8_t window);
void     Timebase_UpCounter_SS_Set_Status(uint8_t window, uint8_t value);
int32_t  Timebase_UpCounter_SS_Get_Value(uint8_t window);
void     Timebase_UpCounter_SS_Set_Value(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_SS_Get_EndValueSec(uint8_t window);
int32_t  Timebase_UpCounter_SS_Get_EndValueSubSec(uint8_t window);
void     Timebase_UpCounter_SS_Set_EndValueSec(uint8_t window, int32_t value);
void     Timebase_UpCounter_SS_Set_EndValueSubSec(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_SS_Get_TargetValue(uint8_t window);
void     Timebase_UpCounter_SS_Set_TargetValue(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_SS_Get_TemporaryValue(uint8_t window);
void     Timebase_UpCounter_SS_Set_TemporaryValue(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_SS_Get_PeriodValue(uint8_t window);
void     Timebase_UpCounter_SS_Set_PeriodValue(uint8_t window, int32_t value);
uint8_t  Timebase_UpCounter_SS_Get_Period_Flag(uint8_t window);
void     Timebase_UpCounter_SS_Set_Period_Flag(uint8_t window);
int32_t  Timebase_UpCounter_SS_Get_ReloadValue(uint8_t window);
void     Timebase_UpCounter_SS_Set_ReloadValue(uint8_t window, int32_t value);
void     Timebase_UpCounter_SS_Clear_Period_Flag(uint8_t window);
void     Timebase_UpCounter_SS_Reset(uint8_t window);
void     Timebase_UpCounter_SS_Clear_All_Flags(uint8_t window);
void     Timebase_UpCounter_SS_Start(uint8_t window);
void     Timebase_UpCounter_SS_Stop(uint8_t window);
void     Timebase_UpCounter_SS_Set_Securely(uint8_t window, int32_t value);
void     Timebase_UpCounter_SS_Set_Forcefully(uint8_t window, int32_t value);
void     Timebase_UpCounter_SS_Update(uint8_t window);
uint8_t  Timebase_UpCounter_SS_Expired(uint8_t window);
uint8_t  Timebase_UpCounter_SS_Expired_Event(uint8_t window);
uint8_t  Timebase_UpCounter_SS_Oneshot_Expired_Event(uint8_t window);
uint8_t  Timebase_UpCounter_SS_Continuous_Expired_Event(uint8_t window);
void     Timebase_UpCounter_SS_Set_Period_Value_Securely(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_SS_Get_Remaining_Period_Value(uint8_t window);
uint8_t  Timebase_UpCounter_SS_Period_Value_Expired(uint8_t window);
uint8_t  Timebase_UpCounter_SS_Period_Value_Expired_Event(uint8_t window);
void     Timebase_UpCounter_SS_Update_All(void);
void     Timebase_UpCounter_SS_Reset_All(void);


//UpCounter Functions
uint8_t  Timebase_UpCounter_Get_Status(uint8_t window);
void     Timebase_UpCounter_Set_Status(uint8_t window, uint8_t value);
int32_t  Timebase_UpCounter_Get_Value(uint8_t window);
void     Timebase_UpCounter_Set_Value(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_Get_EndValueSec(uint8_t window);
int32_t  Timebase_UpCounter_Get_EndValueSubSec(uint8_t window);
void     Timebase_UpCounter_Set_EndValueSec(uint8_t window, int32_t value);
void     Timebase_UpCounter_Set_EndValueSubSec(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_Get_TargetValue(uint8_t window);
void     Timebase_UpCounter_Set_TargetValue(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_Get_TemporaryValue(uint8_t window);
void     Timebase_UpCounter_Set_TemporaryValue(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_Get_PeriodValue(uint8_t window);
void     Timebase_UpCounter_Set_PeriodValue(uint8_t window, int32_t value);
uint8_t  Timebase_UpCounter_Get_Period_Flag(uint8_t window);
void     Timebase_UpCounter_Set_Period_Flag(uint8_t window);
void     Timebase_UpCounter_Clear_Period_Flag(uint8_t window);
int32_t  Timebase_UpCounter_Get_ReloadValue(uint8_t window);
void     Timebase_UpCounter_Set_ReloadValue(uint8_t window, int32_t value);
void     Timebase_UpCounter_Reset(uint8_t window);
void     Timebase_UpCounter_Clear_All_Flags(uint8_t window);
void     Timebase_UpCounter_Start(uint8_t window);
void     Timebase_UpCounter_Stop(uint8_t window);
void     Timebase_UpCounter_Set_Securely(uint8_t window, int32_t value);
void     Timebase_UpCounter_Set_Forcefully(uint8_t window, int32_t value);
void     Timebase_UpCounter_Update(uint8_t window);
uint8_t  Timebase_UpCounter_Expired(uint8_t window);
uint8_t  Timebase_UpCounter_Expired_Event(uint8_t window);
uint8_t  Timebase_UpCounter_Oneshot_Expired_Event(uint8_t window);
uint8_t  Timebase_UpCounter_Continuous_Expired_Event(uint8_t window);
void     Timebase_UpCounter_Set_Period_Value_Securely(uint8_t window, int32_t value);
int32_t  Timebase_UpCounter_Get_Remaining_Period_Value(uint8_t window);
uint8_t  Timebase_UpCounter_Period_Value_Expired(uint8_t window);
uint8_t  Timebase_UpCounter_Period_Value_Expired_Event(uint8_t window);
void     Timebase_UpCounter_Update_All(void);
void     Timebase_UpCounter_Reset_All(void);


//DownCounter SubSeconds Functions
uint8_t  Timebase_DownCounter_SS_Get_Status(uint8_t window);
void     Timebase_DownCounter_SS_Set_Status(uint8_t window, uint8_t value);
int32_t  Timebase_DownCounter_SS_Get_Value(uint8_t window);
void     Timebase_DownCounter_SS_Set_Value(uint8_t window, int32_t value);
int32_t  Timebase_DownCounter_SS_Get_EndValueSec(uint8_t window);
int32_t  Timebase_DownCounter_SS_Get_EndValueSubSec(uint8_t window);
void     Timebase_DownCounter_SS_Set_EndValueSec(uint8_t window, int32_t value);
void     Timebase_DownCounter_SS_Set_EndValueSubSec(uint8_t window, int32_t value);
int32_t  Timebase_DownCounter_SS_Get_PeriodValue(uint8_t window);
void     Timebase_DownCounter_SS_Set_PeriodValue(uint8_t window, int32_t value);
uint8_t  Timebase_DownCounter_SS_Get_Period_Flag(uint8_t window);
void     Timebase_DownCounter_SS_Set_Period_Flag(uint8_t window);
void     Timebase_DownCounter_SS_Clear_Period_Flag(uint8_t window);
int32_t  Timebase_DownCounter_SS_Get_ReloadValue(uint8_t window);
void     Timebase_DownCounter_SS_Set_ReloadValue(uint8_t window, int32_t value);
void     Timebase_DownCounter_SS_Reset(uint8_t window);
void     Timebase_DownCounter_SS_Clear_All_Flags(uint8_t window);
void     Timebase_DownCounter_SS_Start(uint8_t window);
void     Timebase_DownCounter_SS_Stop(uint8_t window);
void     Timebase_DownCounter_SS_Set_Securely(uint8_t window, int32_t value);
void     Timebase_DownCounter_SS_Set_Forcefully(uint8_t window, int32_t value);
void     Timebase_DownCounter_SS_Update(uint8_t window);
uint8_t  Timebase_DownCounter_SS_Expired(uint8_t window);
uint8_t  Timebase_DownCounter_SS_Expired_Event(uint8_t window);
uint8_t  Timebase_DownCounter_SS_Oneshot_Expired_Event(uint8_t window);
uint8_t  Timebase_DownCounter_SS_Continuous_Expired_Event(uint8_t window);
void     Timebase_DownCounter_SS_Set_Period_Value_Securely(uint8_t window, int32_t value);
int32_t  Timebase_DownCounter_SS_Get_Remaining_Period_Value(uint8_t window);
uint8_t  Timebase_DownCounter_SS_Period_Value_Expired(uint8_t window);
uint8_t  Timebase_DownCounter_SS_Period_Value_Expired_Event(uint8_t window);
void     Timebase_DownCounter_SS_Update_All(void);
void     Timebase_DownCounter_SS_Reset_All(void);


//DownCounter Functions
uint8_t  Timebase_DownCounter_Get_Status(uint8_t window);
void     Timebase_DownCounter_Set_Status(uint8_t window, uint8_t value);
int32_t  Timebase_DownCounter_Get_Value(uint8_t window);
void     Timebase_DownCounter_Set_Value(uint8_t window, int32_t value);
int32_t  Timebase_DownCounter_Get_EndValue(uint8_t window);
void     Timebase_DownCounter_Set_EndValue(uint8_t window, int32_t value);
int32_t  Timebase_DownCounter_Get_PeriodValue(uint8_t window);
void     Timebase_DownCounter_Set_PeriodValue(uint8_t window, int32_t value);
uint8_t  Timebase_DownCounter_Get_Period_Flag(uint8_t window);
void     Timebase_DownCounter_Set_Period_Flag(uint8_t window);
void     Timebase_DownCounter_Clear_Period_Flag(uint8_t window);
int32_t  Timebase_DownCounter_Get_ReloadValue(uint8_t window);
void     Timebase_DownCounter_Set_ReloadValue(uint8_t window, int32_t value);
void     Timebase_DownCounter_Reset(uint8_t window);
void     Timebase_DownCounter_Clear_All_Flags(uint8_t window);
void     Timebase_DownCounter_Start(uint8_t window);
void     Timebase_DownCounter_Stop(uint8_t window);
void     Timebase_DownCounter_Set_Securely(uint8_t window, int32_t value);
void     Timebase_DownCounter_Set_Forcefully(uint8_t window, int32_t value);
void     Timebase_DownCounter_Update(uint8_t window);
uint8_t  Timebase_DownCounter_Expired(uint8_t window);
uint8_t  Timebase_DownCounter_Expired_Event(uint8_t window);
uint8_t  Timebase_DownCounter_Oneshot_Expired_Event(uint8_t window);
uint8_t  Timebase_DownCounter_Continuous_Expired_Event(uint8_t window);
void     Timebase_DownCounter_Set_Period_Value_Securely(uint8_t window, int32_t value);
int32_t  Timebase_DownCounter_Get_Remaining_Period_Value(uint8_t window);
uint8_t  Timebase_DownCounter_Period_Value_Expired(uint8_t window);
uint8_t  Timebase_DownCounter_Period_Value_Expired_Event(uint8_t window);
void     Timebase_DownCounter_Update_All(void);
void     Timebase_DownCounter_Reset_All(void);



//Low Power DownCounter Functions
uint8_t  Timebase_LPDownCounter_Get_Status(uint8_t window);
void     Timebase_LPDownCounter_Set_Status(uint8_t window, uint8_t value);
int32_t  Timebase_LPDownCounter_Get_Value(uint8_t window);
void     Timebase_LPDownCounter_Set_Value(uint8_t window, int32_t value);
int32_t  Timebase_LPDownCounter_Get_EndValue(uint8_t window);
void     Timebase_LPDownCounter_Set_EndValue(uint8_t window, int32_t value);
int32_t  Timebase_LPDownCounter_Get_PeriodValue(uint8_t window);
void     Timebase_LPDownCounter_Set_PeriodValue(uint8_t window, int32_t value);
uint8_t  Timebase_LPDownCounter_Get_Period_Flag(uint8_t window);
void     Timebase_LPDownCounter_Set_Period_Flag(uint8_t window);
void     Timebase_LPDownCounter_Clear_Period_Flag(uint8_t window);
int32_t  Timebase_LPDownCounter_Get_ReloadValue(uint8_t window);
void     Timebase_LPDownCounter_Set_ReloadValue(uint8_t window, int32_t value);
void     Timebase_LPDownCounter_Reset(uint8_t window);
void     Timebase_LPDownCounter_Clear_All_Flags(uint8_t window);
void     Timebase_LPDownCounter_Start(uint8_t window);
void     Timebase_LPDownCounter_Stop(uint8_t window);
void     Timebase_LPDownCounter_Set_Securely(uint8_t window, int32_t value);
void     Timebase_LPDownCounter_Set_Forcefully(uint8_t window, int32_t value);
void     Timebase_LPDownCounter_Update(uint8_t window);
uint8_t  Timebase_LPDownCounter_Expired(uint8_t window);
uint8_t  Timebase_LPDownCounter_Expired_Event(uint8_t window);
uint8_t  Timebase_LPDownCounter_Oneshot_Expired_Event(uint8_t window);
uint8_t  Timebase_LPDownCounter_Continuous_Expired_Event(uint8_t window);
void     Timebase_LPDownCounter_Set_Period_Value_Securely(uint8_t window, int32_t value);
int32_t  Timebase_LPDownCounter_Get_Remaining_Period_Value(uint8_t window);
uint8_t  Timebase_LPDownCounter_Period_Value_Expired(uint8_t window);
uint8_t  Timebase_LPDownCounter_Period_Value_Expired_Event(uint8_t window);
void     Timebase_LPDownCounter_Update_All(void);
void     Timebase_LPDownCounter_Reset_All(void);


//Common Functions
uint8_t  Timebase_Get_Update_Req_Sts(void);
void     Timebase_Reset(void);
void     Timebase_LP_Reset(void);
void     Timebase_Init(uint16_t UpdateRateHz);
void     Timebase_Main_Loop_Executables(void);
void     Timebase_ISR_Executables(void);
void     Timebase_LP_ISR_Executables(void);
#endif
