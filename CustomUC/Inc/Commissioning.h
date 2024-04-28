/**
  ******************************************************************************
  * @file    Commissioning.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   LoraWAN commissioning file
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
#ifndef __COMMISSIONING__H__
#define __COMMISSIONING__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/*!
 ******************************************************************************
 ********************************** WARNING ***********************************
 ******************************************************************************
  The crypto-element implementation supports both 1.0.x and 1.1.x LoRaWAN
  versions of the specification.
  Thus it has been decided to use the 1.1.x keys and EUI name definitions.
  The below table shows the names equivalence between versions:
               +-------------------+-------------------------+
               |       1.0.x       |          1.1.x          |
               +===================+=========================+
               | LORAWAN_DEVICE_EUI| LORAWAN_DEVICE_EUI      |
               +-------------------+-------------------------+
               | LORAWAN_APP_EUI   | LORAWAN_JOIN_EUI        |
               +-------------------+-------------------------+
               | N/A               | LORAWAN_APP_KEY         |
               +-------------------+-------------------------+
               | LORAWAN_APP_KEY   | LORAWAN_NWK_KEY         |
               +-------------------+-------------------------+
               | LORAWAN_NWK_S_KEY | LORAWAN_F_NWK_S_INT_KEY |
               +-------------------+-------------------------+
               | LORAWAN_NWK_S_KEY | LORAWAN_S_NWK_S_INT_KEY |
               +-------------------+-------------------------+
               | LORAWAN_NWK_S_KEY | LORAWAN_NWK_S_ENC_KEY   |
               +-------------------+-------------------------+
               | LORAWAN_APP_S_KEY | LORAWAN_APP_S_KEY       |
               +-------------------+-------------------------+
 ******************************************************************************
 ******************************************************************************
 ******************************************************************************
 */

/** @addtogroup COMMISSIONING_Defines Commissioning - Defines
  * @brief Defines
  * @{
  */

#define USE_COMMISSIONING_H_KEYS        0

#define LORA_JOIN_ABP                   0

#define SET_APPEUI                      1

#define LORAWAN_APPLICATION_EUI {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}

#define SET_NWKKEY                      1

#define LORAWAN_NETWORK_KEY {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

#define SET_APPKEY                      1

#define LORAWAN_APPLICATION_KEY {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

/*!
 * Device Address
 */
#define LORAWAN_DEVICE_ADDRESS  (uint32_t)0x00000000;
/*!
 * Network session key
 */
#define LORAWAN_NWK_S_KEY {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
/*!
 * Application session key
 */
#define LORAWAN_APP_S_KEY {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

/**
* @} COMMISSIONING_Defines
*/

#ifdef __cplusplus
}
#endif

#endif /* __COMMISSIONING__H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
