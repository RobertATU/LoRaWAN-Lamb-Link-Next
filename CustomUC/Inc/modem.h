/**
  ******************************************************************************
  * @file    modem.h
  * @author  SRA Team
  * @brief   Modem header file
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
#ifndef __MODEM__H__
#define __MODEM__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/** @addtogroup MODEM_Exported_Types Modem - Exported Types
  * @brief Exported Types
  * @{
 */
typedef struct
{
  int32_t (*Init)(void *);
  int32_t (*DeInit)(void *);
  int32_t (*GetCapabilities)(void *, void *);
} MODEM_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(void *);
  int32_t (*Disable)(void *);
  int32_t (*HwReset)(void *);
  int32_t (*Ping)(void *);
  int32_t (*ManageURCs)(void *);
  int32_t (*SendBinPacket)(void *, uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed);
  int32_t (*GetVersion)(void *, void * ver);
  int32_t (*GetDeviceEUI)(void *, uint8_t *pDevEui);
  int32_t (*GetAppEUI)(void *, uint8_t *pAppEui);
  int32_t (*GetAppKEY)(void *, uint8_t *pAppKey);
  int32_t (*GetNwkKEY)(void *, uint8_t *pNwkKey);
  int32_t (*SetAppEUI)(void *, uint8_t *NewAppEui);
  int32_t (*SetAppKEY)(void *, uint8_t *NewAppKey);
  int32_t (*SetNwkKEY)(void *, uint8_t *NewNwkKey);

  int32_t (*GetDADD)(void *, uint32_t *pDAddress);
  int32_t (*GetAppSKEY)(void *, uint8_t *pAppSKey);
  int32_t (*GetNwkSKEY)(void *, uint8_t *pNwkSKey);
  int32_t (*SetDADD)(void *, uint32_t NewDAddress);
  int32_t (*SetAppSKEY)(void *, uint8_t *NewAppSKey);
  int32_t (*SetNwkSKEY)(void *, uint8_t *NewNwkSKey);

  int32_t (*GetDataRate)(void *, uint8_t *pnLoraDr);
  int32_t (*SetRegionBandID)(void *, void *region);
  int32_t (*Join_OTAA)(void *);
  int32_t (*Join_ABP)(void *);
  int32_t (*SetAdaptiveDataRate)(void *, uint8_t adr);
  int32_t (*SetDataRate)(void *, uint8_t dr);
  int32_t (*WL_LowPower)(void *);
  int32_t (*WL_RebootDfu)(void *);
} MODEM_FuncDrv_t;

/**
  * @} CUSTOM_LORA_MODEM_Exported_Types
  */

#ifdef __cplusplus
}
#endif

#endif /* __MODEM__H__ */

/**
* @} MODEM_Exported_Types
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
