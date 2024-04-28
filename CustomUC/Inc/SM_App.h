/**
  ******************************************************************************
  * @file    SM_App.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Header file for SM_App.c file in SM Module
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

#ifndef __SM_APP_H
#define __SM_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SM_MODULE
  * @{
  */

/** @defgroup SM_MODULE_App App template
  * @brief Application template for state machine customization and configuration
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "SM_Conf.h"
#include "SM_Engine.h"

/** @defgroup SM_MODULE_App_Exported_const exported constants
  * @brief <i>Exported constants</i>
  * @{
  */

/* States definition */
/** @defgroup SM_MODULE_App_Exported_const_states states
  * @brief example of state definitions
  * @note the states have to be 0-index based and in a continuous manner
  * @{
  */
enum _states_enum
{
  APP_ST_START,                   /*!< 0:start state                      */
  APP_ST_RUN,                     /*!< 1:run state                        */
  APP_ST_LP,                      /*!< 2:low-power state                  */
  APP_ST_END,                     /*!< 3:end state                        */
  APP_ST_WAIT,                     /*!< 3:end state                        */
  APP_ST_ERR,                     /*!< 3:error state                      */
};
/**
  * @} SM_MODULE_App_Exported_const_states
  */

/* Events definition */
/** @defgroup SM_MODULE_App_Exported_const_events events
  * @brief example of event definitions
  * @note the events have to be 0-index based and in a continuous manner
  * @{
  */
enum _events_enum
{
  APP_EV_EP,                      /*!< 0:epsilon event                    */
  APP_EV_BP,                      /*!< 1:button-pressed event             */
  APP_EV_SD,                      /*!< 2:shut-down event                  */
  APP_EV_LP,                      /*!< 3:go in low power                  */
  APP_EV_RN,                      /*!< 4:go in run                        */
  APP_EV_ER,                      /*!< 5:error                            */
  APP_EV_GO,                      /*!< 6:go ahead                         */
};
/**
  * @} SM_MODULE_App_Exported_const_events
  */

/* Modules defintion */
/** @defgroup SM_MODULE_App_Exported_const_modules modules
  * @brief example of module definitions
  * @note the modules have to be 0-index based and in a continuous manner
  * @{
  */
enum _modules_enum
{
  APP_MD_GNSS,                    /*!< 0:GNSS module                      */
  APP_MD_MEM,                     /*!< 1:MEMS module                      */
  APP_MD_SENS,                    /*!< 2:MEMS module                      */
  APP_MD_POWER,                   /*!< 3:Power module                     */
  APP_MD_BLE,                     /*!< 4:BLE module                       */
  APP_MD_LORA,                    /*!< 5:LoRa module                      */
  APP_MD_SEC,                     /*!< 6:Security module                  */
  APP_MD_APP,                     /*!< 7:Application module               */
  APP_MD_USB,                     /*!< 8:USB module                       */
  APP_MD_BTN,                     /*!< 9:Button module                    */
  APP_MD_BUZ,                     /*!< 10:Buzzer module                   */
};
/**
  * @} SM_MODULE_App_Exported_const_modules
  */
/**
  * @} SM_MODULE_App_Exported_const
  */

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup SM_MODULE_App_Exported_functions exported functions
  * @brief <i>Exported functions</i>
  * @{
  */
void SM_App_Init(void);
void SM_App_Process(void);
SM_Error_t SM_MD_Func(SM_Module_t module, SM_State_t state);
SM_Error_t SM_ST_EntryFunc(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);
SM_Error_t SM_ST_ExitFunc(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);
SM_Error_t RunEntryFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);
SM_Error_t LpEntryFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);
SM_Error_t ShutdownFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);
SM_Error_t ResetFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);

SM_Error_t StartExitFunc(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);

SM_Error_t StartGnssFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartMemFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartSensFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartPowerFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartBleFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartLoraFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartSecurFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartUsbFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartButtonFn(SM_Module_t module, SM_State_t state);
SM_Error_t StartBuzzerFn(SM_Module_t module, SM_State_t state);

SM_Error_t RunGnssFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunMemFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunSensFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunPowerFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunBleFn(SM_Module_t module, SM_State_t state);
SM_Error_t LpBleFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunLoraFn(SM_Module_t module, SM_State_t state);
SM_Error_t LpLoraFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunSecurFn(SM_Module_t module, SM_State_t state);
SM_Error_t LpSecurFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunUsbFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunButtonFn(SM_Module_t module, SM_State_t state);
SM_Error_t RunBuzzerFn(SM_Module_t module, SM_State_t state);

SM_Error_t ApplicationFn(SM_Module_t module, SM_State_t state);
SM_Error_t WaitFn(SM_Module_t module, SM_State_t state);
SM_Error_t BleErrorFn(uint8_t mode, SM_State_t startState, SM_Event_t event, SM_State_t endState);

SM_State_t SM_App_LoadFirstState(void);

void SM_App_SaveFirstState(SM_State_t s);

void SM_App_Reboot(void);

/**
  * @} SM_MODULE_App_Exported_functions
  */

/**
  * @} SM_MODULE_App
  */

/**
  * @} SM_MODULE
  */

#ifdef __cplusplus
}
#endif

#endif /* __SM_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
