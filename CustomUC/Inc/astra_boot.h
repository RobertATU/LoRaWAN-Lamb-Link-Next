/**
  ******************************************************************************
  * @file    astra_boot.h
  * @version V1.0.0
  * @author  SRA Team
  * @brief   This file contains the Astra boot upgrade
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

/** @addtogroup ASTRA_ENGINE ASTRA engine
  * @{
  */

/** @addtogroup BOOT_APPLICATION BOOT application
  * @{
  */    
    
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ASTRA_BOOT__H__
#define __ASTRA_BOOT__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "astra_sysmng.h"
  
/** @addtogroup ASTRA_BOOT_PRIVATE_TYPES ASTRA BOOT - Private Types
  * @brief Private Types
  * @{
 */

/* Private typedef -----------------------------------------------------------*/

typedef struct _s_bootVersion
{
  uint32_t  currentVersion;
  uint32_t  availableVersion;
}bootVersionFw_t;

/**
* @} ASTRA_BOOT_PRIVATE_TYPES
*/

/** @addtogroup ASTRA_BOOT_VARIABLES ASTRA BOOT - Variables
  * @brief Variables
  * @{
 */

/* Variables -----------------------------------------------------------------*/

extern bootVersionFw_t bootVersionFw;

/**
* @} ASTRA_BOOT_VARIABLES
*/


/** @addtogroup ASTRA_BOOT_Defines ASTRA BOOT - Defines
  * @brief Defines
  * @{
 */

/* Private defines -----------------------------------------------------------*/


#define ASTRA_BOOT_PRINTF(...)	PRINTF_VERBOSE(__VA_ARGS__)

/* BOOT REASON REGISTERS DEFINES */   
#define RTC_BKP_CHECK_0                 RTC_BKP_DR0
#define RTC_BKP_CHECK_1                 RTC_BKP_DR1
#define RTC_BKP_REBOOT_REASON           RTC_BKP_DR2
#define RTC_BKP_WAKE_REASON             RTC_BKP_DR3
#define RTC_BKP_BLE_HEALTH              RTC_BKP_DR10
#define RTC_BKP_SM_STATE                RTC_BKP_DR11

/* BOOT REASON VALUE DEFINES */ 
#define BOOT_REASON_CHECK_0             0xA6
#define BOOT_REASON_CHECK_1             0xA7

#define BOOT_REASON_NONE                0x01
#define BOOT_REASON_NORMAL              0x00
#define BOOT_REASON_DFU                 0x02
   
#define BOOT_REASON_OPT1                0x03
#define BOOT_REASON_OPT2                0x04
#define BOOT_REASON_OPT3                0x05
#define BOOT_REASON_BOOT_UPDATE         0x06

#define DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING 8
#define BOOT_SECTOR_START               0
#define BOOT_SECTOR_NUMBERS             7
#define BLE_NUMBER_OF_FAILURES          3

/**
* @} ASTRA_BLE_Private_Defines
*/


/** @addtogroup ASTRA_BOOT_FUNCTION_PROTOTYPES ASTRA BOOT - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Functions prototypes ------------------------------------------------------*/

/**
  * @brief  Function to perform jump to system memory boot from user application
  * @note Call this function when you want to jump to system memory
  * @param  none
  * @retval none
  */
void SystemMemoryBoot(void);

#if USE_BOOT_UPGRADE
  /**
    * @brief  ASTRA reboot for OTA FW update
    * @param  none
    * @retval none
    */
  void AstraRebootBOOTUpdate(void);
#endif //USE_BOOT_UPGRADE

/**
  * @brief  Save wake reason
  * @param  Value to pass at BCKP RTC wake Reason
  * @retval none
  */
void AstraSaveWakeReason(uint8_t reason);

/**
  * @brief  Get wake reason
  * @param  none 
  * @retval wake reason
  */
uint32_t AstraGetWakeReason(void);

/**
  * @brief  Board reboot with reason
  * @param  Value to pass at BCKP RTC Reboot Reason
  * @retval none
  */
void BoardRebootWithReason(uint8_t reason);

#if USE_BOOT_UPGRADE
  /**
    * @brief  Board reboot for OTA FW update
    * @param  none
    * @retval none
    */
  void BoardRebootBOOTUpdate(void);
#endif //USE_BOOT_UPGRADE

/**
  * @brief  Handle boot
  * @param  none
  * @retval none
  */
void HandleBoot(void);


/**
  * @brief  It verifies via OTA if a new FW is available also checking if the current revison is lower than the newer 
  * @param  none
  * @retval boolean
  */
bool AstraHasNewBootFwAvailable(void);


/**
  * @brief  Bootloader upgrade
  * @param  none
  * @retval none
  */
void AstraBootUpgrade(void);


/**
  * @brief  Reset bootloader failure number
  * @param  none
  * @retval none
  */
void Astra_BLE_ResetBootFailureNumber(void);


/**
  * @brief  Get bootloader failure number
  * @param  none
  * @retval Failure number
  */
uint32_t Astra_BLE_GetBootFailureNumber(void);


/**
  * @brief  Increase bootloader failure number
  * @param  none
  * @retval none
  */
void Astra_BLE_IncreaseBootFailureNumber(void);


/**
  * @brief  Get bootloader state 
  * @param  none
  * @retval Bootloader state
  */
uint32_t AstraBootGetSmState(void);


/**
  * @brief  Set bootloader state 
  * @param  Bootloader state
  * @retval none
  */
void AstraBootSetSmState(uint16_t s);


/**
* @} ASTRA_NFC_FUNCTION_PROTOTYPES
*/

#ifdef __cplusplus
}
#endif

#endif /* __ASTRA_BOOT__H__ */

/**
* @} BOOT_APPLICATION
*/

/**
* @} ASTRA_ENGINE
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
