/**
  ******************************************************************************
  * @file    BLE_Implementation.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.h.
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

/** @addtogroup ASTRA_ENGINE ASTRA ENGINE
  * @{
  */

/** @addtogroup BLE_MANAGER BLE manager
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_BLE_IMPLEMENTATION__H__
#define __APP_BLE_IMPLEMENTATION__H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32wbxx_hal.h"

#include "BLE_Inertial.h"
#include "BLE_Environmental.h"
#include "BLE_Battery.h"
#include "BLE_Gnss.h"
#include "BLE_Ota.h"
#include "BLE_PnPLike.h"
#include "BLE_Json.h"
#include "BLE_Led.h"


/** @addtogroup BLE_IMPLEMENTATION_Defines BLE implementation - Defines
  * @brief Defines
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

/* Define the node name MUST be 7 char long
 *
 * #define BLE_MANAGER_DEFAULT_NODE_NAME 'B','L','E','_','M','N','G'-*/
#define BLE_MANAGER_DEFAULT_NODE_NAME 'B','L','E','_','M','N','G'

/* STM32 Unique ID */
#define STM32_UUID      AstraGetMcuId()
#define BLE_STM32_UUID  STM32_UUID

/**
* @}
*/

/** @addtogroup BLE_MANAGER_EXTERNAL_VARIABLES BLE manager - External variables
  * @brief External variables
  * @{
  */

/* External variables --------------------------------------------------------*/

extern int32_t  NeedToClearSecureDB;

/**
* @}
*/

/** @addtogroup BLE_MANAGER_EXTERNAL_FUNCTIONS BLE manager - External functions
  * @brief External functions
  * @{
  */

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Custom Service Initialization.
 * @param  None
 * @retval None
 */
extern void BLE_InitCustomService(void);

/**
 * @brief  Set Custom Advertize Data.
 * @param  uint8_t *manuf_data: Advertize Data
 * @retval None
 */
extern void BLE_SetCustomAdvertiseData(uint8_t *manuf_data);

/** @brief Initialize the BLE stack and services
  * @param  None
  * @retval None
  */
extern void BluetoothInit(void);

/**
* @} BLE_IMPLEMENTATION_Defines
*/

#ifdef __cplusplus
}
#endif

#endif /* __BLE_IMPLEMENTATION__H__ */

/**
* @} BLE_MANAGER
*/

/**
* @} ASTRA_ENGINE
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
