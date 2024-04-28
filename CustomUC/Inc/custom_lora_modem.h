/**
  ******************************************************************************
  * @file    custom_lora_modem.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   LoRa Modem BSP header file
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
#ifndef __CUSTOM_LORA_MODEM__H__
#define __CUSTOM_LORA_MODEM__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "custom_modem_conf.h"
#include "modem.h"

#if (USE_CUSTOM_LORA_MODEM_STM32WL_MODEM_0 == 1)
#include "stm32wl_modem.h"
#endif

/** @addtogroup CUSTOM_LORA_MODEM_DEFINES Custom LoRa modem - Defines
  * @brief Defines
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

#if (USE_CUSTOM_LORA_MODEM_STM32WL_MODEM_0 == 1)
#define CUSTOM_STM32WL_MODEM_0 (0)
#endif

#ifndef MODEM_LORA
#define MODEM_LORA         1U
#endif

#ifndef MODEM_LOW_POWER
#define MODEM_LOW_POWER    (1U<<1)
#endif

#define CUSTOM_MODEM_FUNCTIONS_NBR    1U
#define CUSTOM_MODEM_INSTANCES_NBR    (USE_CUSTOM_LORA_MODEM_STM32WL_MODEM_0)

/**
  * @} CUSTOM_LORA_MODEM_DEFINES
  */

/** @addtogroup CUSTOM_LORA_MODEM_Exported_Types Custom LoRa modem - Exported Types
  * @brief Exported Types
  * @{
 */

/* Exported types ------------------------------------------------------------*/

/* Modem instance Info */
typedef struct
{
  uint8_t  LoRa;
  uint8_t  LowPower;
} CUSTOM_MODEM_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} CUSTOM_MODEM_Ctx_t;

typedef union
{
  struct customver_s
  {
    uint16_t rev;
    uint8_t minor;
    uint8_t major;
  }s;
  uint32_t w;
}customver_t;

typedef struct CUSTOM_MODEM_versions_s
{
  customver_t m4app;
  customver_t m0plusapp;
  customver_t mwlora;
  customver_t mwradio;
}CUSTOM_MODEM_versions_t;

/*!
 * LoRaMAC region enumeration
 */
typedef enum eCUSTOM_MODEM_region
{
    /*!
     * AS band on 923MHz
     */
    AS923,
    /*!
     * Australian band on 915MHz
     */
    AU915,
    /*!
     * Chinese band on 470MHz
     */
    CN470,
    /*!
     * Chinese band on 779MHz
     */
    CN779,
    /*!
     * European band on 433MHz
     */
    EU433,
    /*!
     * European band on 868MHz
     */
    EU868,
    /*!
     * South korean band on 920MHz
     */
    KR920,
    /*!
     * India band on 865MHz
     */
    IN865,
    /*!
     * North american band on 915MHz
     */
    US915,
    /*!
     * Russia band on 864MHz
     */
    RU864,
}CUSTOM_MODEM_region_t;

/**
  * @} CUSTOM_LORA_MODEM_Exported_Types
  */

/** @addtogroup CUSTOM_LORA_MODEM_FunctionPrototypes Custom LoRA modem - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize the modem
  * @param  Instance Modem instance
  * @param  Functions Modem functions. Could be :
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_Init(uint32_t Instance, uint32_t Functions);

/**
  * @brief  Reset the modem
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_HwReset(uint32_t Instance, uint32_t Function);

/**
  * @brief  Ping the modem
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_Ping(uint32_t Instance, uint32_t Function);

/**
  * @brief  Manage the URCs
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_ManageURCs(uint32_t Instance, uint32_t Function);

/**
  * @brief  Send bin packet
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  Buffer: Pointer to 8bit unsigned integer
  * @param  DataSize: 32bit unsigned integer
  * @param  Port: 32bit unsigned integer
  * @param  Confirmed: 8bit unsigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SendBinPacket(uint32_t Instance, uint32_t Function, uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed);

/**
  * @brief  Get the version
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  ver: Pointer to CUSTOM_MODEM_versions_t
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetVersion(uint32_t Instance, uint32_t Function, CUSTOM_MODEM_versions_t * ver);

/**
  * @brief  Get device EUI
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  pDevEui: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetDeviceEUI(uint32_t Instance, uint32_t Function, uint8_t *pDevEui);

/**
  * @brief  Get application EUI
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  pAppEui: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetAppEUI(uint32_t Instance, uint32_t Function, uint8_t *pAppEui);

/**
  * @brief  Get application key
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  pAppKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetAppKEY(uint32_t Instance, uint32_t Function, uint8_t *pAppKey);

/**
  * @brief  Get network key
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  pNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetNwkKEY(uint32_t Instance, uint32_t Function, uint8_t *pNwkKey);

/**
  * @brief  Set new application EUI
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewAppEui: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetAppEUI(uint32_t Instance, uint32_t Function, uint8_t *NewAppEui);

/**
  * @brief  Set new application key
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewAppKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetAppKEY(uint32_t Instance, uint32_t Function, uint8_t *NewAppKey);

/**
  * @brief  Set new network key
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetNwkKEY(uint32_t Instance, uint32_t Function, uint8_t *NewNwkKey);



/**
  * @brief  Get new device addres
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: Pointer to 32bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetDADD(uint32_t Instance, uint32_t Function, uint32_t *pDAddress);
/**
  * @brief  Get new application Skey
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetAppSKEY(uint32_t Instance, uint32_t Function, uint8_t *pAppSKey);
/**
  * @brief  Get new network Skey
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetNwkSKEY(uint32_t Instance, uint32_t Function, uint8_t *pNwkSKey);
/**
  * @brief  Set new device addres
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: 32bit unsigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetDADD(uint32_t Instance, uint32_t Function, uint32_t NewDAddress);
/**
  * @brief  Set new application Skey
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetAppSKEY(uint32_t Instance, uint32_t Function, uint8_t *NewAppSKey);
/**
  * @brief  Set new network Skey
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetNwkSKEY(uint32_t Instance, uint32_t Function, uint8_t *NewNwkSKey);




/**
  * @brief  Get data rate
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  pnLoraDr: Pointer to 8bit usigned integer
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_GetDataRate(uint32_t Instance, uint32_t Function, uint8_t *pnLoraDr);

/**
  * @brief  Set region band ID
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  region: Pointer to CUSTOM_MODEM_region_t
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetRegionBandID(uint32_t Instance, uint32_t Function, CUSTOM_MODEM_region_t *region);

/**
  * @brief  Join OTAA
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_Join_OTAA(uint32_t Instance, uint32_t Function);

/**
  * @brief  Join ABP
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_Join_ABP(uint32_t Instance, uint32_t Function);

/**
  * @brief  Set adaptive data rate
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  adr: Adaptative data rate
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetAdaptiveDataRate(uint32_t Instance, uint32_t Function, uint8_t adr);

/**
  * @brief  Set data rate
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @param  dr: Data rate
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_SetDataRate(uint32_t Instance, uint32_t Function, uint8_t dr);

/**
  * @brief  Modem low power
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_WL_LowPower(uint32_t Instance, uint32_t Function);

/**
  * @brief  Modem reboot
  * @param  Instance: Modem instance
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
int32_t CUSTOM_LORA_MODEM_WL_RebootDfu(uint32_t Instance, uint32_t Function);

/**
  * @} CUSTOM_LORA_MODEM_FunctionPrototypes
  */

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_LORA_MODEM__H__ */

/**
* @} ASTRA_BSP_MODEM
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
