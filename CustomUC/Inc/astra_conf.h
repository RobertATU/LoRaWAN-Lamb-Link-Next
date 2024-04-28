/**
  ******************************************************************************
  * @file    astra_conf.h
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

/** @addtogroup CONFIGURATION Configuration
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ASTRA_CONF__H__
#define __ASTRA_CONF__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/*
 name OK = astra_conf.h
*/

/** @addtogroup ASTRA_CONFIGURATION_DEFINES ASTRA Configuration management - Defines
  * @brief Defines
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

/* TEST ON SWIPDatas */

/* ASTRA ENGINE DEFINES */
#define FLEET_MNG_ENG                   0
#define LIVESTOCK_MON_ENG               1
#define GOODS_MONITORING_ENG            2
#define LOGISTICS_ENG                   3
#define CUSTOM_ENG                      4

#define ALGO_DISABLE                    0
#define ALGO_MOTION_DETECTION           1
#define ALGO_GEOFENCE                   2
#define ALGO_CUSTOM_PROC                3

/* ASTRA ENGINE SELECTION */
#define ASTRA_ENGINE                      CUSTOM_ENG

/* MODULES USAGE ENABLE */
#define USE_USB                      1
#define USE_MEMORY                      1
#define USE_BTN1_ANTI_TAMPER                      0
#define USE_SENSORS                      1
#define USE_STSAFE                      1
#define USE_GNSS                      1
#define USE_ASTRA_MAINBOARD_BSP                      1
#define USE_BLE                      1
#define USE_BUZZER                      0
#define USE_PWRMNG                      1
#define USE_STM32WL                      1

/* FLEET MANAGEMENT Settings */
#define FLEET_MNG_SENDFREQ                      40000
#define FLEET_MNG_BLE_SEND_INTV                      100
#define FLEET_MNG_NFCDATASAVING_INTV                      10000
#define FLEET_MNG_INDOORONLY                      0
#define FLEET_MNG_RUNALGO                      ALGO_DISABLE
#define FLEET_MNG_LOG                      1

/* LIVESTOCK MONITORING settings */
#define LIVESTOCK_MON_SENDFREQ                      20000
#define LIVESTOCK_BLE_SEND_INTV                      500
#define LIVESTOCK_MON_NFCDATASAVING_INTV                      10000
#define LIVESTOCK_MON_INDOORONLY                      0
#define LIVESTOCK_MON_RUNALGO                      ALGO_MOTION_DETECTION
#define LIVESTOCK_MON_LOG                      0

/* GOODS MONITORING Settings */
#define GOODS_MON_SENDFREQ                      3600000
#define GOODS_MON_BLE_SEND_INTV                      200
#define GOODS_MON_NFCDATASAVING_INTV                      10000
#define GOODS_MON_INDOORONLY                      1
#define GOODS_MON_RUNALGO                      ALGO_GEOFENCE
#define GOODS_MON_LOG                      1

/* LOGISTICS settings */
#define LOGISTICS_SENDFREQ                      60000
#define LOGISTICS_BLE_SEND_INTV                      1000
#define LOGISTICS_NFCDATASAVING_INTV                      10000
#define LOGISTICS_INDOORONLY                      0
#define LOGISTICS_RUNALGO                      ALGO_DISABLE
#define LOGISTICS_LOG                      1

/* CUSTOM ASSET TRACKING APPLICATION Settings */
#define CUSTOM_SENDFREQ                      60000
#define CUSTOM_BLE_SEND_INTV                      400
#define CUSTOM_NFCDATASAVING_INTV                      10000
#define CUSTOM_INDOORONLY                      0
#define CUSTOM_RUNALGO                      ALGO_MOTION_DETECTION
#define CUSTOM_LOG                      1

/* STM32WL Settings */
#define STM32WL_SUBGHZ_REGION                      EU868
#define STM32WL_LORA_CONFIRMED_MSG                      0
#define STM32WL_ADR                      0
#define STM32WL_DATA_RATE                      4

/* STM32WB Settings */
#define STM32WB_ADV_INTERVAL                      0x80

/* FIRMWARE LIB CONFIGURATIONS */
#define SW_CONF_PRINTF_LEVEL                      LEVEL_VERBOSE
#define SW_CONF_WAKE_SRC                      WAKE_SRC_BLE_ACC

/* TEST ON BspIpDatas */
 /* BspIpDatas not ok */

  /* group asset tracking configs here */

#ifndef USE_BLE
  #define USE_BLE   0
#endif //USE_BLE

#define USE_BLE_OTA             1
#define CFG_APP_START_SECTOR_INDEX      7

#ifndef USE_SEQUENCER
  #define USE_SEQUENCER   1
#endif //USE_SEQUENCER

#ifndef USE_STM32WL
  #define USE_STM32WL   0
#endif //USE_STM32WL

#ifndef USE_ASTRA_MAINBOARD_BSP
  #define USE_ASTRA_MAINBOARD_BSP   0
#endif //USE_ASTRA_MAINBOARD_BSP

#define USE_UART                    0

#define BLE_STM32_MICRO "STM32WB5MMG"
#define BLE_STM32_BOARD "STEVAL-ASTRA1B"
#define BLE_VERSION_FW_MAJOR    2
#define BLE_VERSION_FW_MINOR    0
#define BLE_VERSION_FW_PATCH    0
#define BLE_FW_PACKAGENAME      "FP-ATR-ASTRA1"
#define BLE_VERSION_FW_ID       0x03

#define CHECK_HARDWARE_VERSION          1

#define ENABLE_STSAFE_ECHO      1
#define USE_BOOT_UPGRADE        1

#define USE_NFC_LED             1

#endif /* __ASTRA_CONF__H__ */

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
