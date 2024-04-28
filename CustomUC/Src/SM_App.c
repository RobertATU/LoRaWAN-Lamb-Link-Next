/**
  ******************************************************************************
  * @file    SM_App.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Application user file for SM Module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup SM_MODULE_App
  * @{
  */

/* Private includes ----------------------------------------------------------*/
#include "SM_App.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32_seq.h"

/** @defgroup SM_MODULE_App_Private_const private constants
  * @brief <i>Private constants</i>
  * @{
  */
#define NUM_STATES countof(SM_Config_States)            /*!< number of states      */
#define NUM_EVENTS countof(SM_Config_Events)            /*!< number of events      */
#define NUM_TRANSITIONS countof(SM_Config_Transitions)  /*!< number of transitions */
#define NUM_MODULES countof(SM_Config_Modules)          /*!< number of modules     */
/**
  * @} SM_MODULE_App_Private_const
  */

/** @defgroup SM_MODULE_App_Private_macro private macro
  * @brief <i>Private macro</i>
  * @{
  */
#define countof(_A_) (sizeof(_A_) / sizeof(*(_A_)))     /*!< counting define        */
/**
  * @} SM_MODULE_App_Private_macro
  */

/** @defgroup SM_MODULE_App_Private_conf configuration
  * @brief <i>Configuration</i>
  * @{
  */
/** @brief example states definition
  */
const SM_StateItem_t SM_Config_States[] = {
  SM_ADD_STATE_TO("StartRun"  , APP_ST_START, SM_ENTRY_MODE_NM, SM_ST_EntryFunc, StartExitFunc, SM_TO_MODE_FIXED, 1,     APP_EV_EP),
  SM_ADD_STATE_TO("Run"       , APP_ST_RUN  , SM_ENTRY_MODE_NM, RunEntryFn     , SM_ST_ExitFunc, SM_TO_MODE_OK_ENTRY, 10,    APP_EV_ER),
  SM_ADD_STATE_TO("Low-power" , APP_ST_LP   , SM_ENTRY_MODE_RB, LpEntryFn      , SM_ST_ExitFunc, SM_TO_MODE_OK_ENTRY, 10,    APP_EV_ER),
  SM_ADD_STATE   ("End"       , APP_ST_END  , SM_ENTRY_MODE_NM, ShutdownFn     , SM_ST_ExitFunc),
  SM_ADD_STATE_TO("WAIT"      , APP_ST_WAIT , SM_ENTRY_MODE_NM, NULL           , BleErrorFn, SM_TO_MODE_OK_ENTRY, 10,    APP_EV_ER),
  SM_ADD_STATE   ("ERR"       , APP_ST_ERR  , SM_ENTRY_MODE_NM, ResetFn        , SM_ST_ExitFunc),
};

/** @brief example events definition
  */
const SM_EventItem_t SM_Config_Events[] = {
  SM_ADD_EVENT("EP", APP_EV_EP),
  SM_ADD_EVENT("BP", APP_EV_BP),
  SM_ADD_EVENT("LP", APP_EV_LP),
  SM_ADD_EVENT("RN", APP_EV_RN),
  SM_ADD_EVENT("SD", APP_EV_SD),
  SM_ADD_EVENT("ER", APP_EV_ER),
  SM_ADD_EVENT("GO", APP_EV_GO),
//  SM_ADD_EVENT("XX", APP_EV_XX),
//  SM_ADD_EVENT("XY", APP_EV_XY),
};

/** @brief example transitions definition
  */
const SM_TransitionItem_t SM_Config_Transitions[] = {
  SM_ADD_TRANSITION(APP_ST_START, APP_EV_EP,  APP_ST_WAIT ),
  SM_ADD_TRANSITION(APP_ST_RUN,   APP_EV_BP,  APP_ST_LP ),
  SM_ADD_TRANSITION(APP_ST_LP,    APP_EV_BP,  APP_ST_RUN ),
  SM_ADD_TRANSITION(APP_ST_RUN,   APP_EV_SD,  APP_ST_END ),
  SM_ADD_TRANSITION(APP_ST_LP,    APP_EV_SD,  APP_ST_END ),
  SM_ADD_TRANSITION(SM_ST_ANY,    APP_EV_LP,  APP_ST_LP  ),
  SM_ADD_TRANSITION(SM_ST_ANY,    APP_EV_RN,  APP_ST_RUN ),
  SM_ADD_TRANSITION(SM_ST_ANY,    APP_EV_ER,  APP_ST_ERR ),
  SM_ADD_TRANSITION(APP_ST_WAIT,  APP_EV_GO,  APP_ST_RUN ),
//  SM_ADD_TRANSITION(SM_ST_ANY,    APP_EV_XX,  APP_ST_LP  ),
//  SM_ADD_TRANSITION(APP_ST_END,   APP_EV_XY,  APP_ST_RUN ),
};

/** @brief example modules definition
  */
const SM_ModuleItem_t SM_Config_Modules[] = {
                                        //APP_ST_START          //APP_ST_RUN            //APP_ST_LP     //APP_ST_END    //APP_ST_WAIT   //APP_ST_ERR
  //INPUT MODULES
#if USE_PWRMNG
  SM_ADD_MODULE("POWER" , APP_MD_POWER, StartPowerFn,           RunPowerFn,             NULL,           SM_MD_Func,     NULL,     NULL),
#endif //USE_PWRMNG
#if USE_USB
  SM_ADD_MODULE("USB"   , APP_MD_USB  , StartUsbFn,             RunUsbFn,               NULL,           NULL,           NULL,           NULL),
#endif //USE_USB
#if USE_ASTRA_MAINBOARD_BSP
  SM_ADD_MODULE("BTN"   , APP_MD_BTN  , StartButtonFn,          RunButtonFn,            NULL,           NULL,           NULL,           NULL),
#endif //USE_ASTRA_MAINBOARD_BSP
#if USE_MEMORY
  SM_ADD_MODULE("MEM"   , APP_MD_MEM  , StartMemFn,             RunMemFn,               NULL,           NULL,           NULL,           NULL),
#endif //USE_MEMORY
#if USE_SENSORS
  SM_ADD_MODULE("SENS"  , APP_MD_SENS , StartSensFn,            RunSensFn,              NULL,           NULL,           NULL,           NULL),
#endif //USE_SENSORS
#if USE_GNSS
  SM_ADD_MODULE("GNSS"  , APP_MD_GNSS , StartGnssFn,            RunGnssFn,              NULL,           NULL,           NULL,           NULL),
#endif //USE_GNSS

  //APPLICATION MODULES
  SM_ADD_MODULE("APP"   , APP_MD_APP  , ApplicationFn,          ApplicationFn,          LpBleFn,        ApplicationFn,  WaitFn,  ApplicationFn),

#if USE_BUZZER
  SM_ADD_MODULE("BUZZER", APP_MD_BUZ  , StartBuzzerFn,          RunBuzzerFn,            NULL,           NULL,           NULL,           NULL),
#endif //USE_BUZZER

  //OUTPUT/CONNECTIVITY MODULES
#if USE_BLE
  SM_ADD_MODULE("BLE"   , APP_MD_BLE  , StartBleFn,             RunBleFn,               NULL,           NULL,           NULL,           NULL),
#endif //USE_BLE
#if USE_STM32WL
  SM_ADD_MODULE("LORA"  , APP_MD_LORA , StartLoraFn,            RunLoraFn,              NULL,           NULL,           NULL,           NULL),
#endif //USE_STM32WL
#if USE_STSAFE
  SM_ADD_MODULE("SEC"   , APP_MD_SEC  , StartSecurFn,           RunSecurFn,             NULL,           NULL,           NULL,           NULL),
#endif //USE_STSAFE
};

/** @brief example state machine configuration
  */
const SM_Config_t SM_Config =
{
  .StartState      = APP_ST_START,
  .NumStates       = NUM_STATES,
  .NumEvents       = NUM_EVENTS,
  .NumTransitions  = NUM_TRANSITIONS,
  .NumModules      = NUM_MODULES,
  .States          = SM_Config_States,
  .Events          = SM_Config_Events,
  .Transitions     = SM_Config_Transitions,
  .Modules         = SM_Config_Modules,
};
/**
  * @} SM_MODULE_App_Private_conf
  */

/** @defgroup SM_MODULE_App_Private_variables private variables
  * @brief <i>Private variables</i>
  * @{
  */
/** @brief pointer to live variables
  */
static SM_Live_t * pSMAppLive = NULL;

/** @brief pointer to current state item
  */
static SM_StateItem_t const * pSMAppCurrStateItem = NULL;

/** @brief application counter
  */
static uint32_t SMAppCount = 0;
/**
  * @} SM_MODULE_App_Private_variables
  */

/* private prototypes */
/** @defgroup SM_MODULE_App_Private_functions private functions
  * @brief  <i>Private functions</i>
  * @{
  */
extern void HAL_Delay(uint32_t Delay);
void _LogTransition(SM_State_t startState, SM_Event_t event, SM_State_t endState);
/**
  * @} SM_MODULE_App_Private_functions
  */

/**
  * @brief  The application entry point.
  * @retval int
  */
void SM_App_Init(void)
{
  SM_Handlers_t handlers = {
    .GetSysTickFunc = NULL,
    .LoadFirstStateFunc = SM_App_LoadFirstState,
    .SaveFirstStateFunc = SM_App_SaveFirstState,
    .RebootFunc = SM_App_Reboot,
#if SM_USE_LOG
    .LogTransitionFunc = _LogTransition,
#endif /* SM_USE_LOG */
  };

  SM_Init("First-example", &SM_Config, &handlers);
  SM_Start();
  pSMAppLive = SM_GetLive();
  pSMAppCurrStateItem = NULL;
  SMAppCount = 0;
  extern SM_Handle_t SM_Handle ;
  SM_Handle.CurrentState = SM_App_LoadFirstState();
}

/**
  * @brief  The application process in main loop.
  * @retval int
  */
void SM_App_Process(void)
{
  SM_Cycle();
  pSMAppCurrStateItem = SM_GetCurrentStateItem();

  SMAppCount++;
  if (SM_IsRunning())
  {
    if (pSMAppCurrStateItem)
    {
#if SM_USE_NAME
      SM_PRINTF_0("%8d:%s (%d) (Tick:%d ModuleErr:%d TOTick:%d)\n", SMAppCount,  pSMAppCurrStateItem->Name, pSMAppCurrStateItem->State, pSMAppLive->Tick, pSMAppLive->ModuleErrorCount, pSMAppLive->Timeout );
#else
      SM_PRINTF_0("%8d:State %d (Tick:%d ModuleErr:%d TOTick:%d)\n", SMAppCount,  pSMAppCurrStateItem->State, pSMAppLive->Tick, pSMAppLive->ModuleErrorCount, pSMAppLive->Timeout);
#endif
    }
    else
    {
      SM_PRINTF("%8d:NO STATE (Tick:%d ModuleErr:%d TOTick:%d)\n", SMAppCount, pSMAppLive->Tick, pSMAppLive->ModuleErrorCount, pSMAppLive->Timeout);
    }
  }
  else
  {
    SM_PRINTF("%8d:STOPPED or PAUSED\n", SMAppCount);
  }
}

SM_State_t SM_App_LoadFirstState(void)
{
  return (SM_State_t)AstraBootGetSmState();
}

void SM_App_SaveFirstState(SM_State_t s)
{
  AstraBootSetSmState(s);
}

void SM_App_Reboot(void)
{
  AstraBoardReboot();
}

/**
  * @brief handler of module
  * @param module module
  * @param state state
  * @retval error condition
  */
SM_Error_t SM_MD_Func(SM_Module_t module, SM_State_t state)
{
#if 1
#if SM_USE_NAME
  SM_ModuleItem_t const * pmodule = SM_GetModuleItem(module);
  SM_StateItem_t  const * pstate  = SM_GetStateItem(state);

  SM_PRINTF("         Module: %s State:%s\n",
         pmodule ? pmodule->Name : "NULL",
         pstate  ? pstate->Name  : "NULL");
#else
  SM_PRINTF("         Module: %d State:%d\n",  module, state);
#endif
#endif
  return SM_OK;
}

SM_Error_t StartGnssFn(SM_Module_t module, SM_State_t state)
{
#if USE_GNSS
  gnss_init();
#endif //USE_GNSS
  return SM_OK;
}

SM_Error_t RunGnssFn(SM_Module_t module, SM_State_t state)
{
#if USE_GNSS
  ManageGnss();
#endif //USE_GNSS
  return SM_OK;
}

SM_Error_t StartMemFn(SM_Module_t module, SM_State_t state)
{
#if USE_MEMORY
  memory_init();
#endif //USE_MEMORY
  return SM_OK;
}

SM_Error_t RunMemFn(SM_Module_t module, SM_State_t state)
{
#if USE_MEMORY
  ManageMemory();
#endif //USE_MEMORY
  return SM_OK;
}

SM_Error_t StartSensFn(SM_Module_t module, SM_State_t state)
{
#if USE_SENSORS
  sensor_init();
#endif //USE_SENSORS
  return SM_OK;
}

SM_Error_t RunSensFn(SM_Module_t module, SM_State_t state)
{
#if USE_SENSORS
  ReadSensors();
#endif //USE_SENSORS
  return SM_OK;
}

SM_Error_t StartPowerFn(SM_Module_t module, SM_State_t state)
{
  Astra_PwrInit();
  return SM_OK;
}

SM_Error_t RunPowerFn(SM_Module_t module, SM_State_t state)
{
#if USE_ASTRA_MAINBOARD_BSP
  Astra_ReadADC();
#endif //USE_ASTRA_MAINBOARD_BSP
  return SM_OK;
}

SM_Error_t StartUsbFn(SM_Module_t module, SM_State_t state)
{
#if USE_USB
  custom_USB_Manager();
  custom_USB_Manager();
  custom_USB_Manager();
#endif //USE_USB
  return SM_OK;
}

SM_Error_t RunUsbFn(SM_Module_t module, SM_State_t state)
{
#if USE_USB
  custom_USB_Manager();
#endif //USE_USB
  return SM_OK;
}

SM_Error_t StartButtonFn(SM_Module_t module, SM_State_t state)
{
#if USE_ASTRA_MAINBOARD_BSP
  Astra_ButtonInit();
#endif //USE_ASTRA_MAINBOARD_BSP
  return SM_OK;
}

SM_Error_t RunButtonFn(SM_Module_t module, SM_State_t state)
{
#if USE_ASTRA_MAINBOARD_BSP
  Astra_ButtonsCheck();
#endif //USE_ASTRA_MAINBOARD_BSP
  return SM_OK;
}

SM_Error_t StartBuzzerFn(SM_Module_t module, SM_State_t state)
{
#if USE_BUZZER
  buzzer_init();
#endif //USE_BUZZER
  return SM_OK;
}

SM_Error_t RunBuzzerFn(SM_Module_t module, SM_State_t state)
{
#if USE_BUZZER
  buzzer_process();
#endif //USE_BUZZER
  return SM_OK;
}

SM_Error_t StartBleFn(SM_Module_t module, SM_State_t state)
{
#if USE_BLE
  astra_ble_init();
#endif //USE_BLE
  return SM_OK;
}

SM_Error_t RunBleFn(SM_Module_t module, SM_State_t state)
{

  return SM_OK;
}

SM_Error_t LpBleFn(SM_Module_t module, SM_State_t state)
{
#if USE_BLE
  uint32_t WakeReason;
#endif //USE_BLE
  while(1)
  {
    UTIL_SEQ_Run( UTIL_SEQ_DEFAULT );

#if USE_BLE
    static uint8_t bleDisableFlag = 1;
    if(bleDisableFlag)
    {
      if(!AstraCheckBleInit())
      {
        WakeReason = AstraGetWakeReason();
        if (WakeReason == SmChangeState_lpAccWake || WakeReason == SmChangeState_lpButtonWake)
        {
          AstraSetNonConnectable();
        }
        bleDisableFlag = 0;          
      }
    }
#endif //USE_BLE

    if(PlatformStatus.b.MEMS_INT1_EVENT == 1)
      break;
    if(PlatformStatus.b.BLE_CONNECTED)
      break;
    if(PlatformStatus.b.BTN1_PUSHED_EVENT)
      break;
    if(PlatformStatus.b.BTN1_PUSHED_LONG_EVENT)
      break;
    if(PlatformStatus.b.BTN2_PUSHED_EVENT)
      break;
    if(PlatformStatus.b.BTN2_PUSHED_LONG_EVENT)
      break;
  }
  return SM_OK;
}

SM_Error_t StartLoraFn(SM_Module_t module, SM_State_t state)
{
#if USE_STM32WL
  stm32wl_init();
#endif //USE_STM32WL
  return SM_OK;
}

SM_Error_t RunLoraFn(SM_Module_t module, SM_State_t state)
{
#if USE_STM32WL
  stm32wl_process();
#endif //USE_STM32WL
  return SM_OK;
}

SM_Error_t LpLoraFn(SM_Module_t module, SM_State_t state)
{

  return SM_OK;
}

SM_Error_t StartSecurFn(SM_Module_t module, SM_State_t state)
{
#if USE_STSAFE
  stsafe_init();
#endif //USE_STSAFE
  return SM_OK;
}

SM_Error_t RunSecurFn(SM_Module_t module, SM_State_t state)
{
#if USE_STSAFE
  stsafe_process();
#endif //USE_STSAFE
  return SM_OK;
}

SM_Error_t LpSecurFn(SM_Module_t module, SM_State_t state)
{

  return SM_OK;
}

SM_Error_t WaitFn(SM_Module_t module, SM_State_t state)
{
  if(!AstraCheckBleInit())
  {
    SM_NormalTrigger(APP_EV_GO);
    
    return SM_OK;
  }
  return SM_ERR_GENERIC;
}

SM_Error_t ApplicationFn(SM_Module_t module, SM_State_t state)
{
  switch (state)
  {
  case APP_ST_START:
    AstraSysInit();
    AstraSysSmManager();

    break;
  case APP_ST_RUN:
  case APP_ST_END:
  case APP_ST_ERR:
    AstraSysSmManager();
    break;
  default:
    break;
  }
  return SM_OK;
}

/**
  * @brief handler of entry state transition
  * @param typeAction type action
  * @param startState start state
  * @param event event
  * @param endState end state
  * @retval error condition
  */
SM_Error_t SM_ST_EntryFunc(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  extern void AstraInitSystem(void);
  AstraInitSystem();
#if SM_USE_NAME
  SM_StateItem_t  const * pstart_state  = SM_GetStateItem(startState);
  SM_EventItem_t  const * pevent        = SM_GetEventItem(event);
  SM_StateItem_t  const * pend_state    = SM_GetStateItem(endState);

  SM_PRINTF("         %s handler %s --(%s)--> %s\n",
            mode == SM_TA_EXIT ? "Exit" : "Entry",
            pstart_state  ? pstart_state->Name  : "NULL",
            pevent  ? pevent->Name  : "NULL",
            pend_state  ? pend_state->Name  : "NULL");
#else
  SM_PRINTF("         %s handler %d --(%d)--> %d\n",
            mode == SM_TA_EXIT ? "Exit" : "Entry",
            startState, event, endState);
#endif

  return SM_OK;
}

SM_Error_t RunEntryFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  AstraPrepareHwRun();
  return SM_OK;
}

SM_Error_t LpEntryFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  AstraPrepareHwLp();
  return SM_OK;
}

SM_Error_t ShutdownFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  AstraShutdownCommand();
  return SM_OK;
}

SM_Error_t ResetFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  AstraBoardReboot();
  return SM_OK;
}

/**
  * @brief handler of exit state transition
  * @param typeAction type action
  * @param startState start state
  * @param event event
  * @param endState end state
  * @retval error condition
  */
SM_Error_t SM_ST_ExitFunc(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
#if SM_USE_NAME
  SM_StateItem_t  const * pstart_state  = SM_GetStateItem(startState);
  SM_EventItem_t  const * pevent        = SM_GetEventItem(event);
  SM_StateItem_t  const * pend_state    = SM_GetStateItem(endState);

  SM_PRINTF("         %s handler %s --(%s)--> %s\n",
            mode == SM_TA_EXIT ? "Exit" : "Entry",
            pstart_state  ? pstart_state->Name  : "NULL",
            pevent  ? pevent->Name  : "NULL",
            pend_state  ? pend_state->Name  : "NULL");
#else
  SM_PRINTF("         %s handler %d --(%d)--> %d\n",
            mode == SM_TA_EXIT ? "Exit" : "Entry",
            startState, event, endState);
#endif

  return SM_OK;
}

SM_Error_t StartExitFunc(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  return SM_OK;
}

SM_Error_t BleErrorFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  if(AstraCheckBleInit())
  {
    SM_PRINTF("Ble configuration error, sys will reboot (%d)\r\n", Astra_BLE_GetBootFailureNumber());
    Astra_BLE_IncreaseBootFailureNumber();
  }
  return SM_ERR_GENERIC;
}

/**
  * @} SM_MODULE_App_Exported_functions
  */

/** @addtogroup SM_MODULE_App_Private_functions private functions
  * @{
  */
/** @brief log transition callback
  * @param startState start state
  * @param event event
  * @param endState end state
  */
void _LogTransition(SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
#if SM_USE_NAME
  SM_EventItem_t const * ev_item = SM_GetEventItem(event);
  SM_StateItem_t const * s0_item = SM_GetStateItem(startState);
  SM_StateItem_t const * s1_item = SM_GetStateItem(endState);

  /* get the event name */
  char const * ev_name = ev_item ? ev_item->Name : "Unknown";
  char const * s0_name = s0_item ? s0_item->Name : "X INVALID";
  char const * s1_name = s1_item ? s1_item->Name : "X INVALID";

  SM_PRINTF("         Transition: %s --(%s)--> %s\n", s0_name, ev_name, s1_name);
#else /* SM_USE_NAME */
  SM_PRINTF("         Transition: S%d --(E%d)--> S%d\n", startState, event, endState);
#endif /* SM_USE_NAME */
}
/**
  * @} SM_MODULE_App_Private_functions
  */

/**
  * @} SM_MODULE_App
  */
