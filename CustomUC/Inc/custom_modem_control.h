/**
  ******************************************************************************
  * @file    custom_modem_control.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   LoRa Modem interface header file for custom board
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

/** @addtogroup ASTRA_BSP_MODEM
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_MODEM_CONTROL__H__
#define __CUSTOM_MODEM_CONTROL__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "custom_lora_modem.h"

/** @addtogroup CUSTOM_MODEM_CONTROL_FunctionPrototypes Custom modem control - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/**
  * @brief  LoRa modem initialization
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaModemInit(void);

/**
  * @brief  LoRa modem hardware reset
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaModemHwReset(void);

/**
  * @brief  Send bin packet
  * @param  Buffer: Pointer to 8bit unsigned integer
  * @param  DataSize: 32bit unsigned integer
  * @param  Port: 32bit unsigned integer
  * @param  Confirmed: 8bit unsigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSendBinPacket(uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed);

/**
  * @brief  Get the version
  * @param  wlvers: Pointer to CUSTOM_MODEM_versions_t
  * @retval BSP status
  */
int32_t BSP_LoRaGetVersion(CUSTOM_MODEM_versions_t*wlvers);

/**
  * @brief  Ping the modem
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaPingWL(void);

/**
  * @brief  Manage the URCs
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaManageURCs(void);

/**
  * @brief  Get device EUI
  * @param  pDevEui: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetDeviceEUI(uint8_t *pDevEui);

/**
  * @brief  Get application EUI
  * @param  pAppEui: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetAppEUI(uint8_t *pAppEui);

/**
  * @brief  Get application key
  * @param  pAppKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetAppKEY(uint8_t *pAppKey);

/**
  * @brief  Get network key
  * @param  pNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetNwkKEY(uint8_t *pNwkKey);

/**
  * @brief  Set new application EUI
  * @param  NewAppEui: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSetAppEUI(uint8_t *NewAppEui);

/**
  * @brief  Set new application key
  * @param  NewAppKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSetAppKEY(uint8_t *NewAppKey);

/**
  * @brief  Set new network key
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSetNwkKEY(uint8_t *NewNwkKey);

/**
  * @brief  Get Device Addres
  * @param  pDAddress: Pointer to 32bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetDADD(uint32_t *pDAddress);

/**
  * @brief  Get application Skey
  * @param  pAppSKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetAppSKEY(uint8_t *pAppSKey);

/**
  * @brief  Get network Skey
  * @param  pNwkSKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetNwkSKEY(uint8_t *pNwkSKey);

/**
  * @brief  Set new Device Addres
  * @param  NewDAddress: 32bit unsigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSetDADD(uint32_t NewDAddress);

/**
  * @brief  Set new application Skey
  * @param  NewAppSKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSetAppSKEY(uint8_t *NewAppSKey);

/**
  * @brief  Set new network Skey
  * @param  NewNwkSKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaSetNwkSKEY(uint8_t *NewNwkSKey);

/**
  * @brief  Get data rate
  * @param  pnLoraDr: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t BSP_LoRaGetDataRate(uint8_t *nLoraDr);

/**
  * @brief  Set region band ID
  * @param  region: Pointer to CUSTOM_MODEM_region_t
  * @retval BSP status
  */
int32_t BSP_LoRaSetRegionBandID(CUSTOM_MODEM_region_t *region);

/**
  * @brief  Join OTAA
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaJoin_OTAA(void);

/**
  * @brief  Join ABP
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaJoin_ABP(void);

/**
  * @brief  Set adaptive data rate
  * @param  adr: Adaptative data rate
  * @retval BSP status
  */
int32_t BSP_LoRaSetAdaptiveDataRate(uint8_t adr);

/**
  * @brief  Set data rate
  * @param  dr: Data rate
  * @retval BSP status
  */
int32_t BSP_LoRaSetDataRate(uint8_t dr);

/**
  * @brief  Modem low power
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaWL_LowPower(void);

/**
  * @brief  Modem reboot
  * @param  none
  * @retval BSP status
  */
int32_t BSP_LoRaWL_RebootDfu(void);

/**
* @} CUSTOM_MODEM_CONTROL_FunctionPrototypes
*/

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_MODEM_CONTROL__H__ */

/**
* @} ASTRA_BSP_MODEM
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
