#include "main.h"
#include "timebase.h"
//#include "rcc.h"



#define  TIMEBASE_COUNT_ATOMIC_OPERATION
//#define  TIMEBASE_TOKEN_FUNCTIONS
#define  TIMEBASE_TIME_WINDOW_CALCULATION


#ifdef TIMEBASE_LP_DOWNCOUNTER
#define  TIMEBASE_LP_TIMER_ENABLE
#endif




/********************************Structure & Enumeration Start******************************/


typedef struct timebase_time_t{
  volatile uint16_t        OVFUpdateValue     ;
  volatile int32_t         LastSample         ;
  volatile uint16_t        SubSeconds         ;
  volatile int32_t         Seconds            ;
  volatile uint16_t        SubSecondsShadow   ;
	volatile uint32_t        SubSecondsUpTime   ;
  volatile int32_t         SecondsShadow      ;
  volatile uint8_t         VariablesSync      ;

  #ifdef TIMEBASE_TIME_WINDOW_CALCULATION
  int32_t                  StartTimeSeconds   ;
  int32_t                  StartTimeSubSeconds;
  uint8_t                  Status             ;
  #endif

  #ifdef TIMEBASE_LP_TIMER_ENABLE
  volatile uint16_t        LPTimerSubSeconds  ;
  volatile int32_t         LPTimerSeconds     ;
  #endif

}timebase_time_t;



typedef union {
  struct {
    uint8_t               PeriodFlag        :1;
    uint8_t               Value             :4;
  };
  volatile uint8_t        StatusByte          ;
} timebase_status_t;




#ifdef TIMEBASE_UPCOUNTER_SUBSECONDS
typedef struct timebase_upcounter_ss_t{
  timebase_status_t       Status             ;
  int32_t                 EndValueSec        ;
  int32_t                 EndValueSubSec     ;
  int32_t                 Target             ;
  int32_t                 Temporary          ;
  int32_t                 Value              ;
  int32_t                 PeriodValue        ;
  int32_t                 ReloadValue        ;
}timebase_upcounter_ss_t;
#endif




#ifdef TIMEBASE_UPCOUNTER
typedef struct timebase_upcounter_t{
  timebase_status_t       Status             ;
  int32_t                 EndValue           ;
  int32_t                 Target             ;
  int32_t                 Temporary          ;
  int32_t                 Value              ;
  int32_t                 PeriodValue        ;
  int32_t                 ReloadValue        ;
}timebase_upcounter_t;
#endif



#if defined(TIMEBASE_DOWNCOUNTER_SUBSECONDS) || defined(TIMEBASE_LP_DOWNCOUNTER_SUBSECONDS)
typedef struct timebase_downcounter_ss_t{
  timebase_status_t       Status             ;
  int32_t                 EndValueSec        ;
  int32_t                 EndValueSubSec     ;
  int32_t                 Value              ;
  int32_t                 PeriodValue        ;
  int32_t                 ReloadValue        ;
}timebase_downcounter_ss_t;
#endif




#if defined(TIMEBASE_DOWNCOUNTER) || defined(TIMEBASE_LP_DOWNCOUNTER)
typedef struct timebase_downcounter_t{
  timebase_status_t         Status           ;
  int32_t                   EndValue         ;
  int32_t                   Value            ;
  int32_t                   PeriodValue      ;
  int32_t                 ReloadValue        ;
}timebase_downcounter_t;
#endif



typedef struct timebase_config_t{
  volatile uint16_t       UpdateRate         ;
  #ifdef TIMEBASE_LP_TIMER_ENABLE
  volatile uint16_t       LPUpdateRate       ;
  #endif
}timebase_config_t;



typedef struct timebase_t{
  timebase_config_t      Config              ;
  timebase_time_t        Time                ;
  volatile uint8_t       UpdateRequest       ;

  #ifdef TIMEBASE_TOKEN_FUNCTIONS
  volatile uint8_t       ActiveTokens        ;
  #endif

  #ifdef TIMEBASE_UPCOUNTER_SUBSECONDS
  timebase_upcounter_ss_t UpCounterSS[TIMEBASE_UPCOUNTER_SUBSECONDS] ;
  #endif

  #ifdef TIMEBASE_UPCOUNTER
  timebase_upcounter_t   UpCounter[TIMEBASE_UPCOUNTER]      ;
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER_SUBSECONDS
  timebase_downcounter_ss_t DownCounterSS[TIMEBASE_DOWNCOUNTER_SUBSECONDS] ;
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER
  timebase_downcounter_t DownCounter[TIMEBASE_DOWNCOUNTER]  ;
  #endif

  #ifdef TIMEBASE_LP_DOWNCOUNTER
  timebase_downcounter_t LPDownCounter[TIMEBASE_LP_DOWNCOUNTER]  ;
  #endif

}timebase_t;



enum{
  COUNTER_STATE_RESET       = 0,
  COUNTER_STATE_START       = 1,
  COUNTER_STATE_STARTED     = 1,
  COUNTER_STATE_STOP        = 2,
  COUNTER_STATE_STOPPED     = 2,
  COUNTER_STATE_EXPIRED     = 4
};



enum{
  FLAG_STATE_RESET          = 0,
  FLAG_STATE_SET            = 1,
};



enum{
  TIMEBASE_FALSE            = 0,
  TIMEBASE_TRUE             = 1,
};



enum{
  UPCOUNTER_UPDATE_REQ      =  0x01,
  DOWNCOUNTER_UPDATE_REQ    =  0x02,
  UPCOUNTER_SS_UPDATE_REQ   =  0x04,
  DOWNCOUNTER_SS_UPDATE_REQ =  0x08,
  LPUPCOUNTER_UPDATE_REQ    =  0x10,
  LPDOWNCOUNTER_UPDATE_REQ  =  0x20
};



timebase_t Timebase_type;
timebase_t *Timebase;

/********************************Structure & Enumeration End******************************/









/************************************Basic Functions Start*******************************/

void Timebase_Struct_Init(void){
  Timebase=&Timebase_type;
  Timebase->Config.UpdateRate = 1;
  Timebase->Time.OVFUpdateValue=0;
  Timebase->Time.SubSecondsShadow = 0;
	Timebase->Time.SubSecondsUpTime = 0;
  Timebase->Time.SecondsShadow = 0;
  Timebase->Time.SubSeconds = 0;
  Timebase->Time.Seconds = 0;
  Timebase->Time.VariablesSync = 0;
  Timebase->Time.LastSample = 0;

  #ifdef TIMEBASE_TIME_WINDOW_CALCULATION
  Timebase->Time.StartTimeSeconds = 0;
  Timebase->Time.StartTimeSubSeconds = 0;
  Timebase->Time.Status = 0;
  #endif

  #ifdef TIMEBASE_LP_TIMER_ENABLE
  Timebase->Time.LPTimerSubSeconds = 0;
  Timebase->Time.LPTimerSeconds = 0;
  Timebase->Config.LPUpdateRate = 1;
  #endif

  Timebase->UpdateRequest = 0;

  #ifdef TIMEBASE_TOKEN_FUNCTIONS
  Timebase->ActiveTokens = 0;
  #endif

  #ifdef TIMEBASE_UPCOUNTER
  for(uint8_t i=0; i < TIMEBASE_UPCOUNTER; i++){
    Timebase->UpCounter[i].Status.StatusByte = 0;
    Timebase->UpCounter[i].EndValue = 0;
    Timebase->UpCounter[i].Target = 0;
    Timebase->UpCounter[i].Temporary = 0;
    Timebase->UpCounter[i].Value = 0;
    Timebase->UpCounter[i].PeriodValue = 0;
	Timebase->UpCounter[i].ReloadValue = 0;
  }
  #endif

  #ifdef TIMEBASE_UPCOUNTER_SUBSECONDS
  for(uint8_t i=0; i < TIMEBASE_UPCOUNTER_SUBSECONDS; i++){
    Timebase->UpCounterSS[i].Status.StatusByte = 0;
    Timebase->UpCounterSS[i].EndValueSec = 0;
	Timebase->UpCounterSS[i].EndValueSubSec = 0;
    Timebase->UpCounterSS[i].Target = 0;
    Timebase->UpCounterSS[i].Temporary = 0;
    Timebase->UpCounterSS[i].Value = 0;
    Timebase->UpCounterSS[i].PeriodValue = 0;
	Timebase->UpCounterSS[i].ReloadValue = 0;
  }
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER
  for(uint8_t i=0; i < TIMEBASE_DOWNCOUNTER; i++){
    Timebase->DownCounter[i].Status.StatusByte = 0;
    Timebase->DownCounter[i].EndValue = 0;
    Timebase->DownCounter[i].Value = 0;
    Timebase->DownCounter[i].PeriodValue = 0;
	  Timebase->DownCounter[i].ReloadValue = 0;
  }
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER_SUBSECONDS
  for(uint8_t i=0; i < TIMEBASE_DOWNCOUNTER_SUBSECONDS; i++){
    Timebase->DownCounterSS[i].Status.StatusByte = 0;
    Timebase->DownCounterSS[i].EndValueSec = 0;
	  Timebase->DownCounterSS[i].EndValueSubSec = 0;
    Timebase->DownCounterSS[i].Value = 0;
    Timebase->DownCounterSS[i].PeriodValue = 0;
	  Timebase->DownCounterSS[i].ReloadValue = 0;
  }
  #endif

  #ifdef TIMEBASE_LP_DOWNCOUNTER
  for(uint8_t i=0; i < TIMEBASE_LP_DOWNCOUNTER; i++){
    Timebase->LPDownCounter[i].Status.StatusByte = 0;
    Timebase->LPDownCounter[i].EndValue = 0;
    Timebase->LPDownCounter[i].Value = 0;
    Timebase->LPDownCounter[i].PeriodValue = 0;
	  Timebase->LPDownCounter[i].ReloadValue = 0;
  }
  #endif

}


/**********************************Basic Timer Functions********************************/



void Timebase_Timer_Enable(uint16_t UpdateRateHz){
    __HAL_RCC_TIM17_CLK_ENABLE();  // Enable TIM17 clock

    htim17.Instance = TIM17;
    htim17.Init.Prescaler = (HAL_RCC_GetPCLK2Freq() / 1000000) - 1; // 1 MHz timer clock
    htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim17.Init.Period = (1000000 / UpdateRateHz) - 1;               // Auto-reload for desired update rate
    htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim17.Init.RepetitionCounter = 0;
    htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if(HAL_TIM_Base_Init(&htim17) != HAL_OK){
        Error_Handler();
    }

    // Start timer with interrupt
    HAL_TIM_Base_Start_IT(&htim17);

    // NVIC configuration
    HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);

    //Timebase.UpdateRate = UpdateRateHz;  // user-defined struct
}

void Timebase_Timer_Disable(void){
    HAL_TIM_Base_Stop_IT(&htim17);
    HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
    __HAL_RCC_TIM17_CLK_DISABLE();
}

#ifdef TIMEBASE_LP_TIMER_ENABLE
void Timebase_LPTimer_Enable(uint16_t UpdateRateHz){
    /*
        Low-power timer configuration placeholder
        E.g., use LPTIM1 for STM32G4
    */
}

void Timebase_LPTimer_Disable(void){
    /*
        Disable LP timer
    */
}
#endif

/********************************Atomic Functions********************************/

void Timebase_Atomic_Operation_Start(void){
    __disable_irq();
}

void Timebase_Atomic_Operation_End(void){
    __enable_irq();
}

/********************************Timer ISR********************************/

void TIM1_TRG_COM_TIM17_IRQHandler(void){
    HAL_TIM_IRQHandler(&htim17);  // HAL will call callback
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM17){
        Timebase_ISR_Executables(); // user-defined ISR function
    }
}









/**********************************LPTimer ISR Start*********************************/

#ifdef TIMEBASE_LP_TIMER_ENABLE

  /*
  Add LPTimer ISR
  */

#endif

/***********************************LPTimer ISR End**********************************/









/*********************************Token Functions Start*******************************/

#ifdef TIMEBASE_TOKEN_FUNCTIONS
uint8_t Timebase_Token_Executing(void){
  return Timebase->ActiveTokens;
}


void Timebase_Token_Add(void){
  if( Timebase_Token_Executing() < 63){
    Timebase->ActiveTokens += 1;
  }
}


void Timebase_Token_Remove(void){
  if( Timebase_Token_Executing() > 0){
    Timebase->ActiveTokens -= 1;
  }
}


void Timebase_Token_Remove_All(void){
  Timebase->ActiveTokens = 0;
}
#endif

/*********************************Token Functions End********************************/









/*****************************Base Timer Functions Start*****************************/

uint16_t Timebase_Timer_Get_SubSecondsShadow(void){
  return Timebase->Time.SubSecondsShadow;
}

uint32_t Timebase_Timer_Get_SecondsUpTime(void){
  return Timebase->Time.SubSecondsUpTime;
}

int32_t Timebase_Timer_Get_SecondsShadow(void){
  return Timebase->Time.SecondsShadow;
}


uint16_t Timebase_Timer_Get_SubSeconds(void){
  return Timebase->Time.SubSeconds;
}


int32_t Timebase_Timer_Get_Seconds(void){
  return Timebase->Time.Seconds;
}


void Timebase_Timer_Set_SubSeconds(uint16_t value){
  Timebase->Time.SubSeconds = value;
}

void Timebase_Timer_Set_SubSecondsShadow(uint16_t value){
	#ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
	Timebase_Atomic_Operation_Start();
	#endif
  Timebase->Time.SubSecondsShadow = value;
	#ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
	Timebase_Atomic_Operation_End();
	#endif
}

void Timebase_Timer_Set_SubSecondsUpTime(uint32_t value){
	#ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
	Timebase_Atomic_Operation_Start();
	#endif
  Timebase->Time.SubSecondsUpTime = value;
	#ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
	Timebase_Atomic_Operation_End();
	#endif
}

void Timebase_Timer_Set_Seconds(int32_t value){
  Timebase->Time.Seconds = value;
}


uint16_t Timebase_Timer_Get_SubSecondsShadow_Securely(void){
  uint16_t tmp0 , tmp1 ;
  while( 1 ){
    tmp0 = Timebase->Time.SubSecondsShadow;
    tmp1 = Timebase->Time.SubSecondsShadow;
	  if(tmp0 == tmp1){
	    break;
	  }
  }
  return tmp0;
}

uint32_t Timebase_Timer_Get_SubSecondsUpTime_Securely(void){
  uint32_t tmp0 , tmp1 ;
  while( 1 ){
    tmp0 = Timebase->Time.SubSecondsUpTime;
    tmp1 = Timebase->Time.SubSecondsUpTime;
	  if(tmp0 == tmp1){
	    break;
	  }
  }
  return tmp0;
}

int32_t Timebase_Timer_Get_SecondsShadow_Securely(void){
  int32_t tmp0, tmp1;
  while( 1 ){
    tmp0 = Timebase->Time.SecondsShadow;
    tmp1 = Timebase->Time.SecondsShadow;
	  if(tmp0 == tmp1){
	    break;
	  }
  }
  return tmp0;
}

void Timebase_Timer_Sync_With_Shadow_Variables(void){
  if(Timebase->Time.VariablesSync == TIMEBASE_FALSE){

	  #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
    Timebase_Atomic_Operation_Start();
    Timebase->Time.SubSeconds = Timebase->Time.SubSecondsShadow;
    Timebase->Time.Seconds    = Timebase->Time.SecondsShadow;
    Timebase_Atomic_Operation_End();
    #else
    #warning Shadow Variables Atomic Sync Turned Off
    Timebase->Time.SubSeconds = Timebase_Timer_Get_SubSecondsShadow_Securely();
    Timebase->Time.Seconds    = Timebase_Timer_Get_SecondsShadow_Securely();
    #endif

	  Timebase->Time.VariablesSync = TIMEBASE_TRUE;
  }
}


void Timebase_Timer_Delay_SubSeconds(uint16_t value){

  int32_t smpl_val = 0, smpl_ss = 0, smpl_s = 0;
  int32_t curr_val = 0, curr_ss = 0, curr_s = 0;

  #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
  Timebase_Atomic_Operation_Start();
  smpl_ss  = Timebase_Timer_Get_SubSecondsShadow();
  smpl_s   = Timebase_Timer_Get_SecondsShadow();
  Timebase_Atomic_Operation_End();
  #else
  smpl_ss  = Timebase_Timer_Get_SubSecondsShadow_Securely();
  smpl_s   = Timebase_Timer_Get_SecondsShadow_Securely();
  #endif

  smpl_val  = smpl_s;
  smpl_val *= Timebase->Config.UpdateRate;
  smpl_val += smpl_ss;
  smpl_val += value;

  while(curr_val<smpl_val){
    #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
    Timebase_Atomic_Operation_Start();
    curr_ss   = Timebase_Timer_Get_SubSecondsShadow();
	  curr_s    = Timebase_Timer_Get_SecondsShadow();
	  Timebase_Atomic_Operation_End();
	  #else
	  curr_ss  = Timebase_Timer_Get_SubSecondsShadow_Securely();
    curr_s   = Timebase_Timer_Get_SecondsShadow_Securely();
    #endif

    curr_val  = curr_s;
	  curr_val *= Timebase->Config.UpdateRate;
	  curr_val += curr_ss;
  }

}


void Timebase_Timer_Await_SubSeconds(uint16_t value){
  uint16_t temp=0;

  #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
  Timebase_Atomic_Operation_Start();
  temp = Timebase_Timer_Get_SubSecondsShadow();
  Timebase_Atomic_Operation_End();
  #else
  temp = Timebase_Timer_Get_SubSecondsShadow_Securely();
  #endif


  while(temp != Timebase->Time.LastSample){
    #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
    Timebase_Atomic_Operation_Start();
    temp = Timebase_Timer_Get_SubSecondsShadow();
    Timebase_Atomic_Operation_End();
	  #else
    temp = Timebase_Timer_Get_SubSecondsShadow_Securely();
    #endif
  }

  #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
  Timebase_Atomic_Operation_Start();
  temp = Timebase_Timer_Get_SubSecondsShadow();
  Timebase_Atomic_Operation_End();
  #else
  temp = Timebase_Timer_Get_SubSecondsShadow_Securely();
  #endif

  Timebase->Time.LastSample = temp;
  Timebase->Time.LastSample += value;
  if(Timebase->Time.LastSample >= Timebase->Config.UpdateRate){
    Timebase->Time.LastSample -= Timebase->Config.UpdateRate;
  }
}


void Timebase_Timer_Delay_Seconds(uint16_t value){
  int32_t curr_s = 0, target_s = 0;

  #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
  Timebase_Atomic_Operation_Start();
  target_s = Timebase_Timer_Get_SecondsShadow();
  Timebase_Atomic_Operation_End();
  #else
  target_s = Timebase_Timer_Get_SecondsShadow_Securely();
  #endif

  target_s += value;
  while(target_s > curr_s){

	  #ifdef TIMEBASE_COUNT_ATOMIC_OPERATION
    Timebase_Atomic_Operation_Start();
    curr_s = Timebase_Timer_Get_SecondsShadow();
	  Timebase_Atomic_Operation_End();
	  #else
	  curr_s = Timebase_Timer_Get_SecondsShadow_Securely();
    #endif
  }
}

/******************************Base Timer Functions End******************************/









/******************************LP Timer Functions Start******************************/

#ifdef TIMEBASE_LP_TIMER_ENABLE

uint16_t Timebase_LPTimer_Get_SubSeconds(void){
  uint16_t temp0, temp1;
  while(1){
    temp0 = Timebase->Time.LPTimerSubSeconds;
    temp1 = Timebase->Time.LPTimerSubSeconds;
    if(temp0 == temp1){
	  break;
    }
  }
  return temp0;
}


int32_t Timebase_LPTimer_Get_Seconds(void){
  int32_t temp0, temp1;
  while(1){
    temp0 = Timebase->Time.LPTimerSeconds;
    temp1 = Timebase->Time.LPTimerSeconds;
    if(temp0 == temp1){
	  break;
    }
  }
  return temp0;
}


void Timebase_LPTimer_Set_SubSeconds(uint16_t value){
  Timebase->Time.LPTimerSubSeconds = value;
}


void Timebase_LPTimer_Set_Seconds(int32_t value){
  Timebase->Time.LPTimerSeconds = value;
}

void Timebase_LPTimer_Delay_Seconds(uint16_t value){
  int32_t curr_s = 0, target_s = 0;
  Timebase_Atomic_Operation_Start();
  target_s = Timebase_LPTimer_Get_Seconds();
  Timebase_Atomic_Operation_End();
  target_s += value;
  while(target_s > curr_s){
    Timebase_Atomic_Operation_Start();
    curr_s = Timebase_LPTimer_Get_Seconds();
	Timebase_Atomic_Operation_End();
  }
}

#endif

/*******************************LP Timer Functions End*******************************/









/****************************Time Window Functions Start*****************************/

#ifdef TIMEBASE_TIME_WINDOW_CALCULATION
void Timebase_Window_Timer_Reset(void){
  Timebase->Time.StartTimeSeconds = 0;
  Timebase->Time.StartTimeSubSeconds = 0;
  Timebase->Time.Status = COUNTER_STATE_RESET;
}


void Timebase_Window_Timer_Start(void){
  if(Timebase->Time.Status == COUNTER_STATE_RESET){
    Timebase_Atomic_Operation_Start();
    Timebase->Time.StartTimeSeconds = Timebase_Timer_Get_SecondsShadow();
    Timebase->Time.StartTimeSubSeconds = Timebase_Timer_Get_SubSecondsShadow();
	  Timebase_Atomic_Operation_End();
    Timebase->Time.Status = COUNTER_STATE_STARTED;
  }
}


int32_t Timebase_Window_Timer_Get_Interval(void){
  int32_t curr_ss = 0, curr_s = 0;
  if(Timebase->Time.Status == COUNTER_STATE_STARTED){
    Timebase_Atomic_Operation_Start();
    curr_s = Timebase_Timer_Get_SecondsShadow();
    curr_ss = Timebase_Timer_Get_SubSecondsShadow();
	  Timebase_Atomic_Operation_End();
	  curr_s -= Timebase->Time.StartTimeSeconds;
	  curr_ss -= Timebase->Time.StartTimeSubSeconds;
	  curr_s *= Timebase->Config.UpdateRate;
	  curr_s += curr_ss;
	  return curr_s;
  }else{
    return 0;
  }
}


int32_t Timebase_Window_Timer_Get_Interval_Reset(void){
  int32_t val=Timebase_Window_Timer_Get_Interval();
  if(Timebase->Time.Status == COUNTER_STATE_STARTED){
    Timebase_Window_Timer_Reset();
  }
  return val;
}

#endif

/*****************************Time Window Functions End******************************/









/****************************UpCounter SS Functions Start****************************/

#ifdef TIMEBASE_UPCOUNTER_SUBSECONDS
uint8_t Timebase_UpCounter_SS_Get_Status(uint8_t window){
  return Timebase->UpCounterSS[window].Status.Value;
}


void Timebase_UpCounter_SS_Set_Status(uint8_t window, uint8_t value){
  Timebase->UpCounterSS[window].Status.Value = value;
}


int32_t Timebase_UpCounter_SS_Get_Value(uint8_t window){
  return Timebase->UpCounterSS[window].Value;
}


void Timebase_UpCounter_SS_Set_Value(uint8_t window, int32_t value){
  if(value < 0){
    value = 0;
  }
  Timebase->UpCounterSS[window].Value = value;
}


int32_t Timebase_UpCounter_SS_Get_EndValueSec(uint8_t window){
  return Timebase->UpCounterSS[window].EndValueSec;
}


int32_t Timebase_UpCounter_SS_Get_EndValueSubSec(uint8_t window){
  return Timebase->UpCounterSS[window].EndValueSubSec;
}


void Timebase_UpCounter_SS_Set_EndValueSec(uint8_t window, int32_t value){
  Timebase->UpCounterSS[window].EndValueSec = value;
}


void Timebase_UpCounter_SS_Set_EndValueSubSec(uint8_t window, int32_t value){
  Timebase->UpCounterSS[window].EndValueSubSec = value;
}


int32_t Timebase_UpCounter_SS_Get_TargetValue(uint8_t window){
  return Timebase->UpCounterSS[window].Target;
}


void Timebase_UpCounter_SS_Set_TargetValue(uint8_t window, int32_t value){
  Timebase->UpCounterSS[window].Target = value;
}


int32_t Timebase_UpCounter_SS_Get_TemporaryValue(uint8_t window){
  return Timebase->UpCounterSS[window].Temporary;
}


void Timebase_UpCounter_SS_Set_TemporaryValue(uint8_t window, int32_t value){
  if(value<0){
    value = 0;
  }
  Timebase->UpCounterSS[window].Temporary = value;
}


int32_t Timebase_UpCounter_SS_Get_PeriodValue(uint8_t window){
  return Timebase->UpCounterSS[window].PeriodValue;
}


void Timebase_UpCounter_SS_Set_PeriodValue(uint8_t window, int32_t value){
  Timebase->UpCounterSS[window].PeriodValue = value;
}


uint8_t Timebase_UpCounter_SS_Get_Period_Flag(uint8_t window){
  return Timebase->UpCounterSS[window].Status.PeriodFlag;
}


void Timebase_UpCounter_SS_Set_Period_Flag(uint8_t window){
  Timebase->UpCounterSS[window].Status.PeriodFlag = FLAG_STATE_SET;
}


void Timebase_UpCounter_SS_Clear_Period_Flag(uint8_t window){
  Timebase->UpCounterSS[window].Status.PeriodFlag = FLAG_STATE_RESET;
}

int32_t Timebase_UpCounter_SS_Get_ReloadValue(uint8_t window){
  return Timebase->UpCounterSS[window].ReloadValue;
}

void Timebase_UpCounter_SS_Set_ReloadValue(uint8_t window, int32_t value){
  Timebase->UpCounterSS[window].ReloadValue = value;
}

void Timebase_UpCounter_SS_Reset(uint8_t window){
  Timebase_UpCounter_SS_Set_Status(window, COUNTER_STATE_RESET);
  Timebase_UpCounter_SS_Set_Value(window, 0);
  Timebase_UpCounter_SS_Set_EndValueSec(window, 0);
  Timebase_UpCounter_SS_Set_EndValueSubSec(window, 0);
  Timebase_UpCounter_SS_Set_TargetValue(window, 0);
  Timebase_UpCounter_SS_Set_TemporaryValue(window, 0);
  Timebase_UpCounter_SS_Set_PeriodValue(window, 0);
  Timebase_UpCounter_SS_Clear_Period_Flag(window);
  Timebase_UpCounter_SS_Set_ReloadValue(window, 0);
}


void Timebase_UpCounter_SS_Clear_All_Flags(uint8_t window){
  Timebase_UpCounter_SS_Reset( window );
}


void Timebase_UpCounter_SS_Start(uint8_t window){
  if(Timebase_UpCounter_SS_Get_Status(window) != COUNTER_STATE_STARTED){
    Timebase_UpCounter_SS_Set_Status(window , COUNTER_STATE_START);
  }
}


void Timebase_UpCounter_SS_Stop(uint8_t window){
  if(Timebase_UpCounter_SS_Get_Status(window) != COUNTER_STATE_STOPPED){
    Timebase_UpCounter_SS_Set_Status(window , COUNTER_STATE_STOP);
  }
}


void Timebase_UpCounter_SS_Set_Securely(uint8_t window, int32_t value){
  int32_t curr_s, curr_ss, subsec_val, sec_val;
  if( Timebase_UpCounter_SS_Get_Status( window ) == COUNTER_STATE_RESET ){
    Timebase_UpCounter_SS_Set_Value(window, 0);
    Timebase_UpCounter_SS_Set_TemporaryValue(window, 0);
    Timebase_UpCounter_SS_Set_TargetValue(window, value);
	  curr_ss = Timebase_Timer_Get_SubSeconds();
	  curr_s  = Timebase_Timer_Get_Seconds();
	  subsec_val = value % Timebase->Config.UpdateRate;
	  sec_val    = value / Timebase->Config.UpdateRate;
	  subsec_val += curr_ss;
	  if(subsec_val >= Timebase->Config.UpdateRate){
	    sec_val += 1;
	    subsec_val %= Timebase->Config.UpdateRate;
	  }
	  sec_val += curr_s;
    Timebase_UpCounter_SS_Set_EndValueSec(window, sec_val);
	  Timebase_UpCounter_SS_Set_EndValueSubSec(window, subsec_val);
	  Timebase_UpCounter_SS_Set_ReloadValue(window, value);
    Timebase_UpCounter_SS_Start(window);
  }
}


void Timebase_UpCounter_SS_Set_Forcefully(uint8_t window, int32_t value){
  Timebase_UpCounter_SS_Reset( window );
  Timebase_UpCounter_SS_Set_Securely( window, value );
  Timebase_UpCounter_SS_Set_ReloadValue(window, value);
}


void Timebase_UpCounter_SS_Update(uint8_t window){
  int32_t curr_s, curr_ss, value, subsec_val, sec_val;
  if( Timebase_UpCounter_SS_Get_Status( window ) == COUNTER_STATE_STARTED ){
	curr_s  = Timebase_Timer_Get_Seconds();
	curr_ss = Timebase_Timer_Get_SubSeconds();
	curr_s  = Timebase_UpCounter_SS_Get_EndValueSec(window) - curr_s;
	curr_s *= Timebase->Config.UpdateRate;
	curr_ss = Timebase_UpCounter_SS_Get_EndValueSubSec(window) - curr_ss;
	curr_ss += curr_s;
    Timebase_UpCounter_SS_Set_TemporaryValue(window, curr_ss );
    Timebase_UpCounter_SS_Set_Value(window, Timebase_UpCounter_SS_Get_TargetValue(window) - Timebase_UpCounter_SS_Get_TemporaryValue(window) );
    if(Timebase_UpCounter_SS_Get_TemporaryValue(window) <= 0){
      Timebase_UpCounter_SS_Set_EndValueSec(window, 0);
	    Timebase_UpCounter_SS_Set_EndValueSubSec(window, 0);
      Timebase_UpCounter_SS_Set_TemporaryValue(window, 0);
      Timebase_UpCounter_SS_Set_Value(window, Timebase_UpCounter_SS_Get_TargetValue(window));
	    int32_t temp = Timebase_UpCounter_SS_Get_ReloadValue(window);
	    //reset
	    Timebase_UpCounter_SS_Set_ReloadValue(window, temp);
      Timebase_UpCounter_SS_Set_Status(window, COUNTER_STATE_EXPIRED);
    }
  } else if (Timebase_UpCounter_SS_Get_Status( window ) == COUNTER_STATE_STOPPED){
    curr_s  = Timebase_Timer_Get_Seconds();
	  curr_ss = Timebase_Timer_Get_SubSeconds();
	  value      = Timebase_UpCounter_SS_Get_TemporaryValue(window);
	  subsec_val = value % Timebase->Config.UpdateRate;
	  sec_val    = value / Timebase->Config.UpdateRate;
	  subsec_val += curr_ss;
	  if(subsec_val >= Timebase->Config.UpdateRate){
	    sec_val += 1;
	    subsec_val %= Timebase->Config.UpdateRate;
	  }
	  sec_val += curr_s;
	  Timebase_UpCounter_SS_Set_EndValueSec(window, sec_val);
	  Timebase_UpCounter_SS_Set_EndValueSubSec(window, subsec_val);
    Timebase_UpCounter_SS_Set_Value(window, Timebase_UpCounter_SS_Get_TargetValue(window) - Timebase_UpCounter_SS_Get_TemporaryValue(window));
  }
}


uint8_t Timebase_UpCounter_SS_Expired(uint8_t window){
  if(Timebase_UpCounter_SS_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_UpCounter_SS_Expired_Event(uint8_t window){
  if(Timebase_UpCounter_SS_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    Timebase_UpCounter_SS_Clear_All_Flags( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}



uint8_t Timebase_UpCounter_SS_Oneshot_Expired_Event(uint8_t window){
  return Timebase_UpCounter_SS_Expired_Event(window);
}


uint8_t Timebase_UpCounter_SS_Continuous_Expired_Event(uint8_t window){
  if(Timebase_UpCounter_SS_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    int32_t temp = Timebase_UpCounter_SS_Get_ReloadValue(window);
    Timebase_UpCounter_SS_Clear_All_Flags( window );
	  Timebase_UpCounter_SS_Set_Securely(window, temp);
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_UpCounter_SS_Set_Period_Value_Securely(uint8_t window, int32_t value){
  if(Timebase_UpCounter_SS_Get_Period_Flag( window ) == FLAG_STATE_RESET){
    Timebase_UpCounter_SS_Set_PeriodValue( window, Timebase_UpCounter_SS_Get_Value( window ) + value);
    Timebase_UpCounter_SS_Set_Period_Flag( window);
  }
}


int32_t Timebase_UpCounter_SS_Get_Remaining_Period_Value(uint8_t window){
  int32_t temp = Timebase_UpCounter_SS_Get_PeriodValue( window );
  temp -= Timebase_UpCounter_SS_Get_Value( window );
  if(temp < 0){
    temp = 0;
  }
  return temp;
}


uint8_t Timebase_UpCounter_SS_Period_Value_Expired(uint8_t window){
  if( (Timebase_UpCounter_SS_Get_Remaining_Period_Value( window ) == 0) && (Timebase_UpCounter_SS_Get_Period_Flag( window ) == FLAG_STATE_SET) ){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_UpCounter_SS_Period_Value_Expired_Event(uint8_t window){
  if(Timebase_UpCounter_SS_Period_Value_Expired( window ) == TIMEBASE_TRUE){
    Timebase_UpCounter_SS_Clear_Period_Flag( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_UpCounter_SS_Update_All(void){
  for(uint8_t i=0; i<TIMEBASE_UPCOUNTER_SUBSECONDS; i++){
    Timebase_UpCounter_SS_Update(i);
  }
}


void Timebase_UpCounter_SS_Reset_All(void){
  for(uint8_t i=0; i<TIMEBASE_UPCOUNTER_SUBSECONDS; i++){
    Timebase_UpCounter_SS_Reset(i);
  }
}
#endif

/*****************************UpCounter SS Functions End*****************************/









/******************************UpCounter Functions Start*****************************/

#ifdef TIMEBASE_UPCOUNTER
uint8_t Timebase_UpCounter_Get_Status(uint8_t window){
  return Timebase->UpCounter[window].Status.Value;
}


void Timebase_UpCounter_Set_Status(uint8_t window, uint8_t value){
  Timebase->UpCounter[window].Status.Value = value;
}


int32_t Timebase_UpCounter_Get_Value(uint8_t window){
  return Timebase->UpCounter[window].Value;
}


void Timebase_UpCounter_Set_Value(uint8_t window, int32_t value){
  if(value < 0){
    value = 0;
  }
  Timebase->UpCounter[window].Value = value;
}


int32_t Timebase_UpCounter_Get_EndValueSec(uint8_t window){
  return Timebase->UpCounter[window].EndValue;
}


int32_t Timebase_UpCounter_Get_EndValueSubSec(uint8_t window){
  return Timebase->UpCounter[window].EndValue;
}


void Timebase_UpCounter_Set_EndValueSec(uint8_t window, int32_t value){
  Timebase->UpCounter[window].EndValue = value;
}


void Timebase_UpCounter_Set_EndValueSubSec(uint8_t window, int32_t value){
  Timebase->UpCounter[window].EndValue = value;
}


int32_t Timebase_UpCounter_Get_TargetValue(uint8_t window){
  return Timebase->UpCounter[window].Target;
}


void Timebase_UpCounter_Set_TargetValue(uint8_t window, int32_t value){
  Timebase->UpCounter[window].Target = value;
}


int32_t Timebase_UpCounter_Get_TemporaryValue(uint8_t window){
  return Timebase->UpCounter[window].Temporary;
}


void Timebase_UpCounter_Set_TemporaryValue(uint8_t window, int32_t value){
  if(value < 0){
    value = 0;
  }
  Timebase->UpCounter[window].Temporary = value;
}


int32_t Timebase_UpCounter_Get_PeriodValue(uint8_t window){
  return Timebase->UpCounter[window].PeriodValue;
}


void Timebase_UpCounter_Set_PeriodValue(uint8_t window, int32_t value){
  Timebase->UpCounter[window].PeriodValue = value;
}


uint8_t Timebase_UpCounter_Get_Period_Flag(uint8_t window){
  return Timebase->UpCounter[window].Status.PeriodFlag;
}


void Timebase_UpCounter_Set_Period_Flag(uint8_t window){
  Timebase->UpCounter[window].Status.PeriodFlag = FLAG_STATE_SET;
}


void Timebase_UpCounter_Clear_Period_Flag(uint8_t window){
  Timebase->UpCounter[window].Status.PeriodFlag = FLAG_STATE_RESET;
}

int32_t Timebase_UpCounter_Get_ReloadValue(uint8_t window){
  return Timebase->UpCounter[window].ReloadValue;
}

void Timebase_UpCounter_Set_ReloadValue(uint8_t window, int32_t value){
  Timebase->UpCounter[window].ReloadValue = value;
}


void Timebase_UpCounter_Reset(uint8_t window){
  Timebase_UpCounter_Set_Status(window, COUNTER_STATE_RESET);
  Timebase_UpCounter_Set_Value(window, 0);
  Timebase_UpCounter_Set_EndValueSec(window, 0);
  Timebase_UpCounter_Set_EndValueSubSec(window, 0);
  Timebase_UpCounter_Set_TargetValue(window, 0);
  Timebase_UpCounter_Set_TemporaryValue(window, 0);
  Timebase_UpCounter_Set_PeriodValue(window, 0);
  Timebase_UpCounter_Clear_Period_Flag(window);
  Timebase_UpCounter_Set_ReloadValue(window, 0);
}


void Timebase_UpCounter_Clear_All_Flags(uint8_t window){
  Timebase_UpCounter_Reset( window );
}


void Timebase_UpCounter_Start(uint8_t window){
  if(Timebase_UpCounter_Get_Status(window) != COUNTER_STATE_STARTED){
    Timebase_UpCounter_Set_Status(window , COUNTER_STATE_START);
  }
}


void Timebase_UpCounter_Stop(uint8_t window){
  if(Timebase_UpCounter_Get_Status(window) != COUNTER_STATE_STOPPED){
    Timebase_UpCounter_Set_Status(window , COUNTER_STATE_STOP);
  }
}


void Timebase_UpCounter_Set_Securely(uint8_t window, int32_t value){
  int32_t curr_s;
  if( Timebase_UpCounter_Get_Status( window ) == COUNTER_STATE_RESET ){
    Timebase_UpCounter_Set_Value(window, 0);
    Timebase_UpCounter_Set_TemporaryValue(window, 0);
    Timebase_UpCounter_Set_TargetValue(window, value);
	  curr_s = Timebase_Timer_Get_Seconds();
    Timebase_UpCounter_Set_EndValueSec(window, curr_s + value);
	  Timebase_UpCounter_Set_ReloadValue(window, value);
    Timebase_UpCounter_Start(window);
  }
}


void Timebase_UpCounter_Set_Forcefully(uint8_t window, int32_t value){
  Timebase_UpCounter_Reset( window );
  Timebase_UpCounter_Set_Securely( window, value );
  Timebase_UpCounter_Set_ReloadValue(window, value);
}


void Timebase_UpCounter_Update(uint8_t window){
  int32_t curr_s = 0;
  if( Timebase_UpCounter_Get_Status( window ) == COUNTER_STATE_STARTED ){
	curr_s = Timebase_Timer_Get_Seconds();
    Timebase_UpCounter_Set_TemporaryValue(window, Timebase_UpCounter_Get_EndValueSec(window) - curr_s );
    Timebase_UpCounter_Set_Value(window, Timebase_UpCounter_Get_TargetValue(window) - Timebase_UpCounter_Get_TemporaryValue(window) );
    if(Timebase_UpCounter_Get_TemporaryValue(window) <= 0){
      Timebase_UpCounter_Set_EndValueSec(window, 0);
      Timebase_UpCounter_Set_TemporaryValue(window, 0);
      Timebase_UpCounter_Set_Value(window, Timebase_UpCounter_Get_TargetValue(window));
	    int32_t temp = Timebase_UpCounter_Get_ReloadValue(window);
	    //reset
	    Timebase_UpCounter_Set_ReloadValue(window, temp);
      Timebase_UpCounter_Set_Status(window, COUNTER_STATE_EXPIRED);
    }
  } else if (Timebase_UpCounter_Get_Status( window ) == COUNTER_STATE_STOPPED){
    curr_s = Timebase_Timer_Get_Seconds();
    Timebase_UpCounter_Set_EndValueSec(window, Timebase_UpCounter_Get_TemporaryValue(window) + curr_s);
    Timebase_UpCounter_Set_Value(window, Timebase_UpCounter_Get_TargetValue(window) - Timebase_UpCounter_Get_TemporaryValue(window));
  }
}


uint8_t Timebase_UpCounter_Expired(uint8_t window){
  if(Timebase_UpCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_UpCounter_Expired_Event(uint8_t window){
  if(Timebase_UpCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    Timebase_UpCounter_Clear_All_Flags( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_UpCounter_Oneshot_Expired_Event(uint8_t window){
  return Timebase_UpCounter_Expired_Event(window);
}


uint8_t Timebase_UpCounter_Continuous_Expired_Event(uint8_t window){
  if(Timebase_UpCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    int32_t temp = Timebase_UpCounter_Get_ReloadValue(window);
    Timebase_UpCounter_Clear_All_Flags( window );
	  Timebase_UpCounter_Set_Securely(window, temp);
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_UpCounter_Set_Period_Value_Securely(uint8_t window, int32_t value){
  if(Timebase_UpCounter_Get_Period_Flag( window ) == FLAG_STATE_RESET){
    Timebase_UpCounter_Set_PeriodValue( window, Timebase_UpCounter_Get_Value( window ) + value);
    Timebase_UpCounter_Set_Period_Flag( window);
  }
}


int32_t Timebase_UpCounter_Get_Remaining_Period_Value(uint8_t window){
  int32_t temp = Timebase_UpCounter_Get_PeriodValue( window ) - Timebase_UpCounter_Get_Value( window );
  if(temp < 0){
    temp = 0;
  }
  return temp;
}


uint8_t Timebase_UpCounter_Period_Value_Expired(uint8_t window){
  if( (Timebase_UpCounter_Get_Remaining_Period_Value( window ) == 0) && (Timebase_UpCounter_Get_Period_Flag( window ) == FLAG_STATE_SET) ){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_UpCounter_Period_Value_Expired_Event(uint8_t window){
  if(Timebase_UpCounter_Period_Value_Expired( window ) == TIMEBASE_TRUE){
    Timebase_UpCounter_Clear_Period_Flag( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_UpCounter_Update_All(void){
  for(uint8_t i=0; i<TIMEBASE_UPCOUNTER; i++){
    Timebase_UpCounter_Update(i);
  }
}


void Timebase_UpCounter_Reset_All(void){
  for(uint8_t i=0; i<TIMEBASE_UPCOUNTER; i++){
    Timebase_UpCounter_Reset(i);
  }
}
#endif

/*******************************UpCounter Functions End******************************/









/***************************DownCounter SS Functions Start**************************/

#ifdef TIMEBASE_DOWNCOUNTER_SUBSECONDS
uint8_t Timebase_DownCounter_SS_Get_Status(uint8_t window){
  return Timebase->DownCounterSS[window].Status.Value;
}


void Timebase_DownCounter_SS_Set_Status(uint8_t window, uint8_t value){
  Timebase->DownCounterSS[window].Status.Value = value;
}


int32_t Timebase_DownCounter_SS_Get_Value(uint8_t window){
  return Timebase->DownCounterSS[window].Value;
}


void Timebase_DownCounter_SS_Set_Value(uint8_t window, int32_t value){
  Timebase->DownCounterSS[window].Value = value;
}


int32_t Timebase_DownCounter_SS_Get_EndValueSec(uint8_t window){
  return Timebase->DownCounterSS[window].EndValueSec;
}


int32_t Timebase_DownCounter_SS_Get_EndValueSubSec(uint8_t window){
  return Timebase->DownCounterSS[window].EndValueSubSec;
}


void Timebase_DownCounter_SS_Set_EndValueSec(uint8_t window, int32_t value){
  Timebase->DownCounterSS[window].EndValueSec = value;
}


void Timebase_DownCounter_SS_Set_EndValueSubSec(uint8_t window, int32_t value){
  Timebase->DownCounterSS[window].EndValueSubSec = value;
}


int32_t Timebase_DownCounter_SS_Get_PeriodValue(uint8_t window){
  return Timebase->DownCounterSS[window].PeriodValue;
}


void Timebase_DownCounter_SS_Set_PeriodValue(uint8_t window, int32_t value){
  if(value < 0){
    Timebase->DownCounterSS[window].PeriodValue = 0;
  }else{
    Timebase->DownCounterSS[window].PeriodValue = value;
  }
}


uint8_t Timebase_DownCounter_SS_Get_Period_Flag(uint8_t window){
  return Timebase->DownCounterSS[window].Status.PeriodFlag;
}


void Timebase_DownCounter_SS_Set_Period_Flag(uint8_t window){
  Timebase->DownCounterSS[window].Status.PeriodFlag = FLAG_STATE_SET;
}


void Timebase_DownCounter_SS_Clear_Period_Flag(uint8_t window){
  Timebase->DownCounterSS[window].Status.PeriodFlag = FLAG_STATE_RESET;
}

int32_t Timebase_DownCounter_SS_Get_ReloadValue(uint8_t window){
  return Timebase->DownCounterSS[window].ReloadValue;
}

void Timebase_DownCounter_SS_Set_ReloadValue(uint8_t window, int32_t value){
  Timebase->DownCounterSS[window].ReloadValue = value;
}


void Timebase_DownCounter_SS_Reset(uint8_t window){
  Timebase_DownCounter_SS_Set_EndValueSubSec(window, 0);
  Timebase_DownCounter_SS_Set_EndValueSec(window, 0);
  Timebase_DownCounter_SS_Set_Value(window, 0);
  Timebase_DownCounter_SS_Set_Status(window, COUNTER_STATE_RESET);
  Timebase_DownCounter_SS_Clear_Period_Flag(window);
  Timebase_DownCounter_SS_Set_ReloadValue(window, 0);
}


void Timebase_DownCounter_SS_Clear_All_Flags(uint8_t window){
  Timebase_DownCounter_SS_Reset( window );
}


void Timebase_DownCounter_SS_Start(uint8_t window){
  if(Timebase_DownCounter_SS_Get_Status(window) != COUNTER_STATE_STARTED){
    Timebase_DownCounter_SS_Set_Status(window, COUNTER_STATE_START);
  }
}


void Timebase_DownCounter_SS_Stop(uint8_t window){
  if(Timebase_DownCounter_SS_Get_Status(window) != COUNTER_STATE_STOPPED){
    Timebase_DownCounter_SS_Set_Status(window, COUNTER_STATE_STOP);
  }
}


void Timebase_DownCounter_SS_Set_Securely(uint8_t window, int32_t value){
  int32_t temp_ss, temp_s, subsec_val, sec_val;
  if( Timebase_DownCounter_SS_Get_Status( window ) == COUNTER_STATE_RESET ){
    Timebase_DownCounter_SS_Set_Value(window, value);
	  temp_ss     = Timebase_Timer_Get_SubSeconds();
	  temp_s      = Timebase_Timer_Get_Seconds();
	  subsec_val  = value % Timebase->Config.UpdateRate;
	  sec_val     = value / Timebase->Config.UpdateRate;
	  subsec_val += temp_ss;
	  if(subsec_val >= Timebase->Config.UpdateRate){
	    sec_val += 1;
	    subsec_val = subsec_val % Timebase->Config.UpdateRate;
	  }
	  sec_val += temp_s;
    Timebase_DownCounter_SS_Set_EndValueSec(window, sec_val);
	  Timebase_DownCounter_SS_Set_EndValueSubSec(window, subsec_val);
	  Timebase_DownCounter_SS_Set_ReloadValue(window, value);
    Timebase_DownCounter_SS_Start(window);
  }
}


void Timebase_DownCounter_SS_Set_Forcefully(uint8_t window, int32_t value){
  Timebase_DownCounter_SS_Reset( window );
  Timebase_DownCounter_SS_Set_Securely( window, value );
  Timebase_DownCounter_SS_Set_ReloadValue(window, value);
}


void Timebase_DownCounter_SS_Update(uint8_t window){
  int32_t temp_s, temp_ss;
  int32_t curr_val, curr_s, curr_ss;
  if( Timebase_DownCounter_SS_Get_Status( window ) == COUNTER_STATE_STARTED ){
    temp_ss = Timebase_Timer_Get_SubSeconds();
	  temp_s  = Timebase_Timer_Get_Seconds();
	  temp_ss = Timebase_DownCounter_SS_Get_EndValueSubSec(window) - temp_ss;
	  temp_s  = Timebase_DownCounter_SS_Get_EndValueSec(window) - temp_s;
	  temp_s *= Timebase->Config.UpdateRate;
	  temp_s += temp_ss;
	  Timebase_DownCounter_SS_Set_Value(window, temp_s);
    if(Timebase_DownCounter_SS_Get_Value(window) <= 0){
	    int32_t temp = Timebase_DownCounter_SS_Get_ReloadValue(window);
      Timebase_DownCounter_SS_Reset(window);
	    Timebase_DownCounter_SS_Set_ReloadValue(window, temp);
      Timebase_DownCounter_SS_Set_Status(window, COUNTER_STATE_EXPIRED);
    }
  } else if (Timebase_DownCounter_SS_Get_Status( window ) == COUNTER_STATE_STOPPED){
    temp_ss  = Timebase_Timer_Get_SubSeconds();
	  temp_s   = Timebase_Timer_Get_Seconds();
	  curr_val = Timebase_DownCounter_SS_Get_Value(window);
	  curr_s   = curr_val / Timebase->Config.UpdateRate;
	  curr_ss  = curr_val % Timebase->Config.UpdateRate;
	  Timebase_DownCounter_SS_Set_EndValueSec(window, curr_s + temp_s );
	  Timebase_DownCounter_SS_Set_EndValueSubSec(window, curr_ss + temp_ss);
  }
}


uint8_t Timebase_DownCounter_SS_Expired(uint8_t window){
  if(Timebase_DownCounter_SS_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_DownCounter_SS_Expired_Event(uint8_t window){
  if(Timebase_DownCounter_SS_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    Timebase_DownCounter_SS_Clear_All_Flags( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_DownCounter_SS_Oneshot_Expired_Event(uint8_t window){
  return Timebase_DownCounter_SS_Expired_Event(window);
}


uint8_t Timebase_DownCounter_SS_Continuous_Expired_Event(uint8_t window){
  if(Timebase_DownCounter_SS_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    int32_t temp = Timebase_DownCounter_SS_Get_ReloadValue(window);
    Timebase_DownCounter_SS_Clear_All_Flags( window );
	  Timebase_DownCounter_SS_Set_Securely(window, temp);
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_DownCounter_SS_Set_Period_Value_Securely(uint8_t window, int32_t value){
  if(Timebase_DownCounter_SS_Get_Period_Flag( window ) == FLAG_STATE_RESET){
    Timebase_DownCounter_SS_Set_PeriodValue(window, Timebase_DownCounter_SS_Get_Value(window) - value);
    Timebase_DownCounter_SS_Set_Period_Flag(window);
  }
}


int32_t Timebase_DownCounter_SS_Get_Remaining_Period_Value(uint8_t window){
  int32_t temp = Timebase_DownCounter_SS_Get_Value(window) - Timebase_DownCounter_SS_Get_PeriodValue(window) ;
  if(temp < 0){
    temp = 0;
  }
  return temp;
}


uint8_t Timebase_DownCounter_SS_Period_Value_Expired(uint8_t window){
  if( (Timebase_DownCounter_SS_Get_Remaining_Period_Value( window ) == 0) && (Timebase_DownCounter_SS_Get_Period_Flag( window ) == FLAG_STATE_SET) ){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_DownCounter_SS_Period_Value_Expired_Event(uint8_t window){
  if(Timebase_DownCounter_SS_Period_Value_Expired( window ) == TIMEBASE_TRUE){
    Timebase_DownCounter_SS_Clear_Period_Flag( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_DownCounter_SS_Update_All(void){
  for(uint8_t i=0; i<TIMEBASE_DOWNCOUNTER_SUBSECONDS; i++){
    Timebase_DownCounter_SS_Update(i);
  }
}


void Timebase_DownCounter_SS_Reset_All(void){
  for(uint8_t i=0; i<TIMEBASE_DOWNCOUNTER_SUBSECONDS; i++){
    Timebase_DownCounter_SS_Reset(i);
  }
}
#endif

/****************************DownCounter SS Functions End***************************/









/*****************************DownCounter Functions Start****************************/

#ifdef TIMEBASE_DOWNCOUNTER
uint8_t Timebase_DownCounter_Get_Status(uint8_t window){
  return Timebase->DownCounter[window].Status.Value;
}


void Timebase_DownCounter_Set_Status(uint8_t window, uint8_t value){
  Timebase->DownCounter[window].Status.Value = value;
}


int32_t Timebase_DownCounter_Get_Value(uint8_t window){
  return Timebase->DownCounter[window].Value;
}


void Timebase_DownCounter_Set_Value(uint8_t window, int32_t value){
  Timebase->DownCounter[window].Value = value;
}


int32_t Timebase_DownCounter_Get_EndValue(uint8_t window){
  return Timebase->DownCounter[window].EndValue;
}


void Timebase_DownCounter_Set_EndValue(uint8_t window, int32_t value){
  Timebase->DownCounter[window].EndValue = value;
}


int32_t Timebase_DownCounter_Get_PeriodValue(uint8_t window){
  return Timebase->DownCounter[window].PeriodValue;
}


void Timebase_DownCounter_Set_PeriodValue(uint8_t window, int32_t value){
  if(value < 0){
    Timebase->DownCounter[window].PeriodValue = 0;
  }else{
    Timebase->DownCounter[window].PeriodValue = value;
  }
}


uint8_t Timebase_DownCounter_Get_Period_Flag(uint8_t window){
  return Timebase->DownCounter[window].Status.PeriodFlag;
}


void Timebase_DownCounter_Set_Period_Flag(uint8_t window){
  Timebase->DownCounter[window].Status.PeriodFlag = FLAG_STATE_SET;
}


void Timebase_DownCounter_Clear_Period_Flag(uint8_t window){
  Timebase->DownCounter[window].Status.PeriodFlag = FLAG_STATE_RESET;
}

int32_t Timebase_DownCounter_Get_ReloadValue(uint8_t window){
  return Timebase->DownCounter[window].ReloadValue;
}

void Timebase_DownCounter_Set_ReloadValue(uint8_t window, int32_t value){
  Timebase->DownCounter[window].ReloadValue = value;
}


void Timebase_DownCounter_Reset(uint8_t window){
  Timebase_DownCounter_Set_EndValue(window, 0);
  Timebase_DownCounter_Set_Value(window, 0);
  Timebase_DownCounter_Set_Status(window, COUNTER_STATE_RESET);
  Timebase_DownCounter_Clear_Period_Flag(window);
  Timebase_DownCounter_Set_ReloadValue(window, 0);
}


void Timebase_DownCounter_Clear_All_Flags(uint8_t window){
  Timebase_DownCounter_Reset( window );
}


void Timebase_DownCounter_Start(uint8_t window){
  if(Timebase_DownCounter_Get_Status(window) != COUNTER_STATE_STARTED){
    Timebase_DownCounter_Set_Status(window, COUNTER_STATE_START);
  }
}


void Timebase_DownCounter_Stop(uint8_t window){
  if(Timebase_DownCounter_Get_Status(window) != COUNTER_STATE_STOPPED){
    Timebase_DownCounter_Set_Status(window, COUNTER_STATE_STOP);
  }
}


void Timebase_DownCounter_Set_Securely(uint8_t window, int32_t value){
  if( Timebase_DownCounter_Get_Status( window ) == COUNTER_STATE_RESET ){
    Timebase_DownCounter_Set_Value(window, value);
    Timebase_DownCounter_Set_EndValue(window, Timebase_Timer_Get_Seconds() + value);
	  Timebase_DownCounter_Set_ReloadValue(window, value);
    Timebase_DownCounter_Start(window);
  }
}


void Timebase_DownCounter_Set_Forcefully(uint8_t window, int32_t value){
  Timebase_DownCounter_Reset( window );
  Timebase_DownCounter_Set_Securely( window, value );
  Timebase_DownCounter_Set_ReloadValue(window, value);
}


void Timebase_DownCounter_Update(uint8_t window){
  if( Timebase_DownCounter_Get_Status( window ) == COUNTER_STATE_STARTED ){
    Timebase_DownCounter_Set_Value(window, Timebase_DownCounter_Get_EndValue(window) - Timebase_Timer_Get_Seconds());
    if(Timebase_DownCounter_Get_Value(window) <= 0){
	    int32_t temp = Timebase_DownCounter_Get_ReloadValue(window);
      Timebase_DownCounter_Reset(window);
	    Timebase_DownCounter_Set_ReloadValue(window, temp);
      Timebase_DownCounter_Set_Status(window, COUNTER_STATE_EXPIRED);
    }
  } else if (Timebase_DownCounter_Get_Status( window ) == COUNTER_STATE_STOPPED){
    Timebase_DownCounter_Set_EndValue(window, Timebase_DownCounter_Get_Value(window) + Timebase_Timer_Get_Seconds());
  }
}


uint8_t Timebase_DownCounter_Expired(uint8_t window){
  if(Timebase_DownCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_DownCounter_Expired_Event(uint8_t window){
  if(Timebase_DownCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    Timebase_DownCounter_Clear_All_Flags( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_DownCounter_Set_Period_Value_Securely(uint8_t window, int32_t value){
  if(Timebase_DownCounter_Get_Period_Flag( window ) == FLAG_STATE_RESET){
    Timebase_DownCounter_Set_PeriodValue(window, Timebase_DownCounter_Get_Value(window) - value);
    Timebase_DownCounter_Set_Period_Flag(window);
  }
}


int32_t Timebase_DownCounter_Get_Remaining_Period_Value(uint8_t window){
  int32_t temp = Timebase_DownCounter_Get_Value(window) - Timebase_DownCounter_Get_PeriodValue(window) ;
  if(temp < 0){
    temp = 0;
  }
  return temp;
}


uint8_t Timebase_DownCounter_Period_Value_Expired(uint8_t window){
  if( (Timebase_DownCounter_Get_Remaining_Period_Value( window ) == 0) && (Timebase_DownCounter_Get_Period_Flag( window ) == FLAG_STATE_SET) ){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_DownCounter_Period_Value_Expired_Event(uint8_t window){
  if(Timebase_DownCounter_Period_Value_Expired( window ) == TIMEBASE_TRUE){
    Timebase_DownCounter_Clear_Period_Flag( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_DownCounter_Oneshot_Expired_Event(uint8_t window){
  return Timebase_DownCounter_Expired_Event(window);
}


uint8_t Timebase_DownCounter_Continuous_Expired_Event(uint8_t window){
  if(Timebase_DownCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    int32_t temp = Timebase_DownCounter_Get_ReloadValue(window);
    Timebase_DownCounter_Clear_All_Flags( window );
	  Timebase_DownCounter_Set_Securely(window, temp);
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_DownCounter_Update_All(void){
  for(uint8_t i=0; i<TIMEBASE_DOWNCOUNTER; i++){
    Timebase_DownCounter_Update(i);
  }
}


void Timebase_DownCounter_Reset_All(void){
  for(uint8_t i=0; i<TIMEBASE_DOWNCOUNTER; i++){
    Timebase_DownCounter_Reset(i);
  }
}
#endif

/******************************DownCounter Functions End*****************************/









/****************************LPDownCounter Functions Start***************************/

#ifdef TIMEBASE_LP_DOWNCOUNTER
uint8_t Timebase_LPDownCounter_Get_Status(uint8_t window){
  return Timebase->LPDownCounter[window].Status.Value;
}


void Timebase_LPDownCounter_Set_Status(uint8_t window, uint8_t value){
  Timebase->LPDownCounter[window].Status.Value = value;
}


int32_t Timebase_LPDownCounter_Get_Value(uint8_t window){
  return Timebase->LPDownCounter[window].Value;
}


void Timebase_LPDownCounter_Set_Value(uint8_t window, int32_t value){
  Timebase->LPDownCounter[window].Value = value;
}


int32_t Timebase_LPDownCounter_Get_EndValue(uint8_t window){
  return Timebase->LPDownCounter[window].EndValue;
}


void Timebase_LPDownCounter_Set_EndValue(uint8_t window, int32_t value){
  Timebase->LPDownCounter[window].EndValue = value;
}


int32_t Timebase_LPDownCounter_Get_PeriodValue(uint8_t window){
  return Timebase->LPDownCounter[window].PeriodValue;
}


void Timebase_LPDownCounter_Set_PeriodValue(uint8_t window, int32_t value){
  if(value < 0){
    Timebase->LPDownCounter[window].PeriodValue = 0;
  }else{
    Timebase->LPDownCounter[window].PeriodValue = value;
  }
}


uint8_t Timebase_LPDownCounter_Get_Period_Flag(uint8_t window){
  return Timebase->LPDownCounter[window].Status.PeriodFlag;
}


void Timebase_LPDownCounter_Set_Period_Flag(uint8_t window){
  Timebase->LPDownCounter[window].Status.PeriodFlag = FLAG_STATE_SET;
}


void Timebase_LPDownCounter_Clear_Period_Flag(uint8_t window){
  Timebase->LPDownCounter[window].Status.PeriodFlag = FLAG_STATE_RESET;
}

int32_t Timebase_LPDownCounter_Get_ReloadValue(uint8_t window){
  return Timebase->LPDownCounter[window].ReloadValue;
}

void Timebase_LPDownCounter_Set_ReloadValue(uint8_t window, int32_t value){
  Timebase->LPDownCounter[window].ReloadValue = value;
}

void Timebase_LPDownCounter_Reset(uint8_t window){
  Timebase_LPDownCounter_Set_EndValue(window, 0);
  Timebase_LPDownCounter_Set_Value(window, 0);
  Timebase_LPDownCounter_Set_Status(window, COUNTER_STATE_RESET);
  Timebase_LPDownCounter_Clear_Period_Flag(window);
  Timebase_LPDownCounter_Set_ReloadValue(window, 0);
}


void Timebase_LPDownCounter_Clear_All_Flags(uint8_t window){
  Timebase_LPDownCounter_Reset( window );
}


void Timebase_LPDownCounter_Start(uint8_t window){
  if(Timebase_LPDownCounter_Get_Status(window) != COUNTER_STATE_STARTED){
    Timebase_LPDownCounter_Set_Status(window, COUNTER_STATE_START);
  }
}


void Timebase_LPDownCounter_Stop(uint8_t window){
  if(Timebase_LPDownCounter_Get_Status(window) != COUNTER_STATE_STOPPED){
    Timebase_LPDownCounter_Set_Status(window, COUNTER_STATE_STOP);
  }
}


void Timebase_LPDownCounter_Set_Securely(uint8_t window, int32_t value){
  if( Timebase_LPDownCounter_Get_Status( window ) == COUNTER_STATE_RESET ){
    Timebase_LPDownCounter_Set_Value(window, value);
    Timebase_LPDownCounter_Set_EndValue(window, Timebase_LPTimer_Get_Seconds() + value);
	  Timebase_LPDownCounter_Set_ReloadValue(window, value);
    Timebase_LPDownCounter_Start(window);
  }
}


void Timebase_LPDownCounter_Set_Forcefully(uint8_t window, int32_t value){
  Timebase_LPDownCounter_Reset( window );
  Timebase_LPDownCounter_Set_Securely( window, value );
  Timebase_LPDownCounter_Set_ReloadValue(window, value);
}


void Timebase_LPDownCounter_Update(uint8_t window){
  if( Timebase_LPDownCounter_Get_Status( window ) == COUNTER_STATE_STARTED ){
    Timebase_LPDownCounter_Set_Value(window, Timebase_LPDownCounter_Get_EndValue(window) - Timebase_LPTimer_Get_Seconds());
    if(Timebase_LPDownCounter_Get_Value(window) <= 0){
	    int32_t temp = Timebase_LPDownCounter_Get_ReloadValue(window);
      Timebase_LPDownCounter_Reset(window);
	    Timebase_LPDownCounter_Set_ReloadValue(window, temp);
      Timebase_LPDownCounter_Set_Status(window, COUNTER_STATE_EXPIRED);
    }
  } else if (Timebase_LPDownCounter_Get_Status( window ) == COUNTER_STATE_STOPPED){
    Timebase_LPDownCounter_Set_EndValue(window, Timebase_LPDownCounter_Get_Value(window) + Timebase_LPTimer_Get_Seconds());
  }
}


uint8_t Timebase_LPDownCounter_Expired(uint8_t window){
  if(Timebase_LPDownCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_LPDownCounter_Expired_Event(uint8_t window){
  if(Timebase_LPDownCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    Timebase_LPDownCounter_Clear_All_Flags( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}

uint8_t Timebase_LPDownCounter_Oneshot_Expired_Event(uint8_t window){
  return Timebase_LPDownCounter_Expired_Event(window);
}


uint8_t Timebase_LPDownCounter_Continuous_Expired_Event(uint8_t window){
  if(Timebase_LPDownCounter_Get_Status( window ) == COUNTER_STATE_EXPIRED){
    int32_t temp = Timebase_LPDownCounter_Get_ReloadValue(window);
    Timebase_LPDownCounter_Clear_All_Flags( window );
	  Timebase_LPDownCounter_Set_Securely(window, temp);
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_LPDownCounter_Set_Period_Value_Securely(uint8_t window, int32_t value){
  if(Timebase_LPDownCounter_Get_Period_Flag( window ) == FLAG_STATE_RESET){
    Timebase_LPDownCounter_Set_PeriodValue(window, Timebase_LPDownCounter_Get_Value(window) - value);
    Timebase_LPDownCounter_Set_Period_Flag(window);
  }
}


int32_t Timebase_LPDownCounter_Get_Remaining_Period_Value(uint8_t window){
  int32_t temp = Timebase_LPDownCounter_Get_Value(window) - Timebase_LPDownCounter_Get_PeriodValue(window) ;
  if(temp < 0){
    temp = 0;
  }
  return temp;
}


uint8_t Timebase_LPDownCounter_Period_Value_Expired(uint8_t window){
  if( (Timebase_LPDownCounter_Get_Remaining_Period_Value( window ) == 0) && (Timebase_LPDownCounter_Get_Period_Flag( window ) == FLAG_STATE_SET) ){
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


uint8_t Timebase_LPDownCounter_Period_Value_Expired_Event(uint8_t window){
  if(Timebase_LPDownCounter_Period_Value_Expired( window ) == TIMEBASE_TRUE){
    Timebase_LPDownCounter_Clear_Period_Flag( window );
    return TIMEBASE_TRUE;
  }else{
    return TIMEBASE_FALSE;
  }
}


void Timebase_LPDownCounter_Update_All(void){
  for(uint8_t i=0; i<TIMEBASE_LP_DOWNCOUNTER; i++){
    Timebase_LPDownCounter_Update(i);
  }
}


void Timebase_LPDownCounter_Reset_All(void){
  for(uint8_t i=0; i<TIMEBASE_LP_DOWNCOUNTER; i++){
    Timebase_LPDownCounter_Reset(i);
  }
}
#endif

/******************************DownCounter Functions End*****************************/









/*******************************Common Functions Start******************************/

uint8_t  Timebase_Get_Update_Req_Sts(void){
  return Timebase->UpdateRequest;
}


void Timebase_Reset(void){
  #ifdef TIMEBASE_UPCOUNTER
  Timebase_UpCounter_Reset_All();
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER_SUBSECONDS
  Timebase_DownCounter_SS_Reset_All();
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER
  Timebase_DownCounter_Reset_All();
  #endif
}


void Timebase_LP_Reset(void){
  #ifdef TIMEBASE_LP_DOWNCOUNTER
  Timebase_LPDownCounter_Reset_All();
  #endif
}


void Timebase_Init(uint16_t UpdateRateHz){
  Timebase_Struct_Init();
  Timebase_Timer_Enable(UpdateRateHz);
  Timebase_Reset();

  #ifdef TIMEBASE_LP_TIMER_ENABLE
  #warning LPTimer default Freq 1Hz
  Timebase_LPTimer_Enable(1);
  Timebase_LP_Reset();
  #endif

}


void Timebase_Main_Loop_Executables(void){

  #ifdef TIMEBASE_UPCOUNTER_SUBSECONDS
  if(Timebase->UpdateRequest & UPCOUNTER_SS_UPDATE_REQ){
    Timebase_Timer_Sync_With_Shadow_Variables();
    Timebase_UpCounter_SS_Update_All();
	  Timebase->UpdateRequest &=~ UPCOUNTER_SS_UPDATE_REQ;
  }
  #endif

  #ifdef TIMEBASE_UPCOUNTER
  if(Timebase->UpdateRequest & UPCOUNTER_UPDATE_REQ){
    Timebase_Timer_Sync_With_Shadow_Variables();
    Timebase_UpCounter_Update_All();
	  Timebase->UpdateRequest &=~ UPCOUNTER_UPDATE_REQ;
  }
  #endif



  #ifdef TIMEBASE_DOWNCOUNTER_SUBSECONDS
  if(Timebase->UpdateRequest & DOWNCOUNTER_SS_UPDATE_REQ){
    Timebase_Timer_Sync_With_Shadow_Variables();
    Timebase_DownCounter_SS_Update_All();
	  Timebase->UpdateRequest &=~ DOWNCOUNTER_SS_UPDATE_REQ;
  }
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER
  if(Timebase->UpdateRequest & DOWNCOUNTER_UPDATE_REQ){
    Timebase_Timer_Sync_With_Shadow_Variables();
    Timebase_DownCounter_Update_All();
	  Timebase->UpdateRequest &=~ DOWNCOUNTER_UPDATE_REQ;
  }
  #endif
  Timebase->Time.VariablesSync = TIMEBASE_FALSE;

  #ifdef TIMEBASE_LP_DOWNCOUNTER
  if(Timebase->UpdateRequest & LPDOWNCOUNTER_UPDATE_REQ){
    Timebase_LPDownCounter_Update_All();
	  Timebase->UpdateRequest &=~ LPDOWNCOUNTER_UPDATE_REQ;
  }
  #endif
}


void Timebase_ISR_Executables(void){
  Timebase->Time.SubSecondsShadow++;
  Timebase->Time.SubSecondsUpTime++;

  #ifdef TIMEBASE_UPCOUNTER_SUBSECONDS
  Timebase->UpdateRequest |= UPCOUNTER_SS_UPDATE_REQ;
  #endif

  #ifdef TIMEBASE_DOWNCOUNTER_SUBSECONDS
  Timebase->UpdateRequest |= DOWNCOUNTER_SS_UPDATE_REQ;
  #endif

  if(Timebase->Time.SubSecondsShadow >= Timebase->Config.UpdateRate){
    Timebase->Time.SubSecondsShadow = 0;
    Timebase->Time.SecondsShadow++;

	#ifdef TIMEBASE_UPCOUNTER
    Timebase->UpdateRequest |= UPCOUNTER_UPDATE_REQ;
    #endif

	#ifdef TIMEBASE_DOWNCOUNTER
    Timebase->UpdateRequest |= DOWNCOUNTER_UPDATE_REQ;
    #endif
  }
}


void Timebase_LP_ISR_Executables(void){
  #ifdef TIMEBASE_LP_TIMER_ENABLE
  Timebase->Time.LPTimerSubSeconds++;
  if(Timebase->Time.LPTimerSubSeconds >= Timebase->Config.LPUpdateRate){
    Timebase->Time.LPTimerSubSeconds = 0;
    Timebase->Time.LPTimerSeconds++;
  }
  Timebase->UpdateRequest |= LPDOWNCOUNTER_UPDATE_REQ;
  #endif
}

/********************************Common Functions End*******************************/
