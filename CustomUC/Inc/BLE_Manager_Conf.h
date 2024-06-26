/**
  ******************************************************************************
  * @file    BLE_Manager_Conf.h
  * @author  SRA Team
  * @version V1.0.0
  * @date    11-Apr-2022
  * @brief   BLE Manager configuration template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Manager_Conf.h.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/** @addtogroup BLE_MANAGER BLE manager
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLE_MANAGER_CONF_H__
#define __BLE_MANAGER_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

  #include "astra_confmng.h"

/** @addtogroup BLE_MANAGER_CONF_Defines BLE manager configuration - Defines
  * @brief Defines
  * @{
  */

/* Select the used bluetooth core:
 *
 * BLUENRG_1_2     0x00
 * BLUENRG_MS      0x01
 * BLUENRG_LP      0x02
 * BLUE_WB         0x03
*/

/* Comment this define if do not use parson */
#define BLE_MANAGER_USE_PARSON

#define BLUE_CORE BLUE_WB
/* Select the used hardware platform
 *
 * STEVAL-WESU1                         --> BLE_MANAGER_STEVAL_WESU1_PLATFORM
 * STEVAL-STLKT01V1 (SensorTile)        --> BLE_MANAGER_SENSOR_TILE_PLATFORM
 * STEVAL-BCNKT01V1 (BlueCoin)          --> BLE_MANAGER_BLUE_COIN_PLATFORM
 * STEVAL-IDB008Vx                      --> BLE_MANAGER_STEVAL_IDB008VX_PLATFORM
 * STEVAL-BCN002V1B (BlueTile)          --> BLE_MANAGER_STEVAL_BCN002V1_PLATFORM
 * STEVAL-MKSBOX1V1 (SensorTile.box)    --> BLE_MANAGER_SENSOR_TILE_BOX_PLATFORM
 * DISCOVERY-IOT01A                     --> BLE_MANAGER_DISCOVERY_IOT01A_PLATFORM
 * STEVAL-STWINKT1                      --> BLE_MANAGER_STEVAL_STWINKIT1_PLATFORM
 * STM32NUCLEO Board                    --> BLE_MANAGER_NUCLEO_PLATFORM
 *
 * Undefined platform                   --> BLE_MANAGER_UNDEF_PLATFORM
*/

/* Identify the used hardware platform  */
#define BLE_MANAGER_USED_PLATFORM	BLE_MANAGER_STEVAL_ASTRA1_PLATFORM

#ifndef BLE_MANAGER_USE_PARSON
  #define BLE_MANAGER_NO_PARSON
#endif /* BLE_MANAGER_USE_PARSON */

/*---------- Out-Of-Band data -----------*/
#define OUT_OF_BAND_ENABLEDATA      0x00
/*---------- Defines the Max dimension of the Bluetooth config characteristic -----------*/
#define DEFAULT_MAX_CONFIG_CHAR_LEN      20
/*---------- Bluetooth address types -----------*/
#define ADDRESS_TYPE      1
/*---------- Enable High Power mode. High power mode should be enabled only to reach the maximum output power. -----------*/
#define ENABLE_HIGH_POWER_MODE      0x01
/*---------- Power amplifier output level - The allowed PA levels depends on the device (see user manual to know wich output power is expected at a given PA level) -----------*/
#define POWER_AMPLIFIER_OUTPUT_LEVEL      0x04
/*---------- Length for configuration values. -----------*/
#define CONFIG_VALUE_LENGTH      6
/*---------- GAP Roles -----------*/
#define GAP_ROLES      0x01
/*---------- Maximum number of allocable bluetooth characteristics -----------*/
#define BLE_MANAGER_MAX_ALLOCABLE_CHARS      32
/*---------- Configuration values -----------*/
#define CONFIG_VALUE_OFFSETS      0x00
/*---------- Defines the Max dimension of the Bluetooth std error characteristic -----------*/
#define DEFAULT_MAX_STDERR_CHAR_LEN      20
/*---------- Defines the Max dimension of the Bluetooth characteristics for each packet -----------*/
#define DEFAULT_MAX_CHAR_LEN      255
/*---------- MITM protection requirements -----------*/
#define MITM_PROTECTION_REQUIREMENTS      0x01
/*---------- IO capabilities -----------*/
#define IO_CAPABILITIES      0x00
/*---------- Authentication requirements -----------*/
#define AUTHENTICATION_REQUIREMENTS      0x01
/*---------- Secure connection support option code -----------*/
#define SECURE_CONNECTION_SUPPORT_OPTION_CODE      0x01
/*---------- Secure connection key press notification option code -----------*/
#define SECURE_CONNECTION_KEYPRESS_NOTIFICATION      0x00
/*---------- Advertising policy for filtering (white list related) -----------*/
#define ADVERTISING_FILTER      0x00
/* USER CODE BEGIN 1 */

#define BLE_MANAGER_SDKV2

#define DEFAULT_MAX_STDOUT_CHAR_LEN     DEFAULT_MAX_CHAR_LEN
#define DEFAULT_MAX_EXTCONFIG_CHAR_LEN  DEFAULT_MAX_CHAR_LEN

/* For enabling the capability to handle BlueNRG Congestion */
//#define ACC_BLUENRG_CONGESTION

/* USER CODE END 1 */

/* Define the Delay function to use inside the BLE Manager (HAL_Delay/osDelay) */
#define BLE_MANAGER_DELAY HAL_Delay

/****************** Memory managment functions **************************/
#define BLE_MallocFunction      malloc
#define BLE_FreeFunction        free
#define BLE_MemCpy              memcpy

/*---------- Print messages from BLE Manager files at middleware level -----------*/

/* USER CODE BEGIN 2 */

/* Uncomment/Comment the following define for  disabling/enabling print messages from BLE Manager files */
#define BLE_MANAGER_DEBUG

#define BLE_DEBUG_LEVEL 1

#ifdef BLE_MANAGER_DEBUG
  /**
  * User can change here printf with a custom implementation.
  * For example:
  * #include "STBOX1_config.h"
  * #include "main.h"
  * #define BLE_MANAGER_PRINTF	STBOX1_PRINTF
  */

  #include <stdio.h>
 #define BLE_MANAGER_PRINTF(...)	PRINTF_VERBOSE(__VA_ARGS__)
#else
  #define BLE_MANAGER_PRINTF(...)
#endif

/* USER CODE END 2 */

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MANAGER_CONF_H__*/

