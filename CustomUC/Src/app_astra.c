/**
  ******************************************************************************
  * @file    app_astra.c
  * @version V1.0.0
  * @author  SRA Team
  * @brief   This file contains the Astra demo
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup ASTRA_ENGINE ASTRA ENGINE
  * @{
  */

/** @addtogroup APP_ASTRA ASTRA application
  * @{
  */

/* Includes ------------------------------------------------------------------*/

#include "main.h"

#include "stm32_lpm.h"

#include "app_common.h"

#include "app_astra.h"

#include "astra_confmng.h"
#include "astra_datamng.h"
#include "astra_sysmng.h"

#if USE_BOOT_UPGRADE
  #include "astra_boot.h"
#endif //USE_BOOT_UPGRADE

#if USE_SEQUENCER
#include "stm32_seq.h"
#endif //USE_SEQUENCER

#if USE_USB
#include "usb_device.h"
#endif //USE_USB

#include "SM_App.h"

/** @addtogroup APP_ASTRA_PrivateDefines Private defines used in ASTRA application
  * @brief Private defines
  * @{
  */

/* Private define ------------------------------------------------------------*/

/**
  * @} APP_ASTRA_PrivateDefines
  */

/* Private variables ---------------------------------------------------------*/

#if USE_SEQUENCER
static uint8_t AstraMainLoop_Timer_Id = 0;
#endif //USE_SEQUENCER

#if !USE_BLE
/* Keep ble_var only to avoid compilation warning */
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ble_var;
#endif //USE_BLE

/** @addtogroup APP_ASTRA_PrivateFunctionPrototypes ASTRA application - Private function prototypes
  * @brief Private function prototypes
  * @{
 */

/* Private Function prototypes -----------------------------------------------*/

    extern void SystemClock_Config(void);
    extern void PeriphCommonClock_Config(void);

/**
  * @brief  ASTRA process
  * @param  none
  * @retval none
  */
void AstraProcess(void);

/**
  * @brief  ASTRA process trigger
  * @param  none
  * @retval none
  */
void AstraProcessTrigger(void);

/**
  * @brief  ASTRA read GNSS
  * @param  none
  * @retval none
  */
void AstraReadGnssExec(void);

/**
* @} APP_ASTRA_PrivateFunctionPrototypes
*/

/** @addtogroup APP_ASTRA_PrivateFunction ASTRA application - Private function
  * @brief Private function
  * @{
 */

/* Private function ----------------------------------------------------------*/

void AstraInitSystem(void)
{
  MX_IPCC_Init();

  PeriphClock_Config();

  custom_sob_power_init();
  custom_sob_v_reg1_out_high_value();
  custom_sob_led_init();
  AstraLedColor(ASTRA_LED_COLOR_GREEN);

  debug_init();

  PRINTF_INFO("SYSTEM initialization\r\n");

  //init debug again
  debug_init();

  AstraLedColor(ASTRA_LED_COLOR_BLUE);

  PrintSystemInfo();

#if USE_SEQUENCER

  UTIL_SEQ_RegTask( (uint32_t)1<< ASTRA_TASK_ID, UTIL_SEQ_RFU, AstraProcess );
  UTIL_SEQ_SetTask( (uint32_t)1<<ASTRA_TASK_ID, CFG_SCH_PRIO_0);

#if USE_GNSS
  UTIL_SEQ_RegTask( (uint32_t)1<< ASTRA_GNSS_TASK_ID, UTIL_SEQ_RFU, AstraReadGnssExec );
#endif //USE_GNSS

  /* Create timer to trigger the AstraMainLoop */
  HW_TS_Create(CFG_TIM_PROC_ID_ISR,
        &(AstraMainLoop_Timer_Id),
        hw_ts_Repeated,
        AstraProcessTrigger);
#endif //USE_SEQUENCER

#if !USE_BLE
/* Keep ble_var only to avoid compilation warning */
        ble_var = 1;
        UNUSED(ble_var);
#endif //USE_BLE
}

void MX_Astra_Init(void)
{
  custom_astra_init();

  HandleBoot();

  SM_App_Init();

#if USE_SEQUENCER
  UTIL_SEQ_RegTask( (uint32_t)1<< ASTRA_TASK_ID, UTIL_SEQ_RFU, AstraProcess );
  UTIL_SEQ_SetTask( (uint32_t)1<<ASTRA_TASK_ID, CFG_SCH_PRIO_0);
#endif //USE_SEQUENCER
}

void AstraProcess(void)
{
  SM_App_Process();

#if USE_SEQUENCER
  static uint8_t runonceHW_TS_Start = 1;
  if(runonceHW_TS_Start)
  {
    runonceHW_TS_Start = 0;

    RCC_PeriphCLKInitTypeDef  PeriphCLKInit;
    HAL_RCCEx_GetPeriphCLKConfig(&PeriphCLKInit);

//    if(PlatformStatus.s.SYS_PROC_INTERVAL == 0) PlatformStatus.s.SYS_PROC_INTERVAL = 100;

    if(PeriphCLKInit.RTCClockSelection == RCC_RTCCLKSOURCE_HSE_DIV32)
    {
      HW_TS_Start(AstraMainLoop_Timer_Id, 32*PlatformStatus.s.SYS_PROC_INTERVAL*1000/CFG_TS_TICK_VAL );
    }
    if(PeriphCLKInit.RTCClockSelection == RCC_RTCCLKSOURCE_LSE)
    {
      HW_TS_Start(AstraMainLoop_Timer_Id, PlatformStatus.s.SYS_PROC_INTERVAL*1000/CFG_TS_TICK_VAL );
    }
  }
#else //USE_SEQUENCER
  static uint32_t nNextTicksEx = 0;
  if(HAL_GetTick() <= nNextTicksEx + PlatformStatus.s.SYS_PROC_INTERVAL)
  {
    nNextTicksEx = (HAL_GetTick() + PlatformStatus.s.SYS_PROC_INTERVAL) - ((HAL_GetTick() + PlatformStatus.s.SYS_PROC_INTERVAL) % PlatformStatus.s.SYS_PROC_INTERVAL);
  }
  else
  {
    nNextTicksEx = HAL_GetTick();
  }
  while(HAL_GetTick() < nNextTicksEx) {}
#endif //USE_SEQUENCER
}

void MX_Astra_Process(void)
{
#if USE_SEQUENCER
  UTIL_SEQ_Run( UTIL_SEQ_DEFAULT );
#else //USE_SEQUENCER
  AstraProcess();
#endif //USE_SEQUENCER

 if(UTIL_LPM_GetMode() != UTIL_LPM_SLEEPMODE)
  {
    SystemClock_Config();
    PeriphCommonClock_Config();
  // PeriphClock_Config();
  }
}

#if USE_SEQUENCER
void AstraProcessTrigger(void)
{
  UTIL_SEQ_SetTask( (uint32_t)1<<ASTRA_TASK_ID, CFG_SCH_PRIO_0);
}

#if USE_GNSS
void AstraReadGnssExec(void)
{
  uint32_t nOldPrintfLevel = SystemConfParams.PrintLevelActive;
  uint8_t nOldBleTrace = PlatformStatus.b.BLE_TRACE;
  PlatformStatus.b.BLE_TRACE = 0;
//  SystemConfParams.PrintLevelActive = LEVEL_TERMINAL;
  ReadGnss(GNSS_READ_TIMEOUT, GNSS_WAIT_FIX);
  SystemConfParams.PrintLevelActive = nOldPrintfLevel;
  PlatformStatus.b.BLE_TRACE = nOldBleTrace;
}
#endif //USE_GNSS
#endif //USE_SEQUENCER

/**
  * @} APP_ASTRA_PrivateFunction
  */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
    BTN1_SWITCH_CASE

    BTN2_SWITCH_CASE

    SENS_INT1_SWITCH_CASE

    SENS_INT2_SWITCH_CASE

    POWER_GOOD_SWITCH_CASE

    ST25DV_INT_SWITCH_CASE

  default:
    break;
  }
}

/**
  * @} APP_ASTRA
  */

/**
  * @} ASTRA_ENGINE ASTRA ENGINE
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
