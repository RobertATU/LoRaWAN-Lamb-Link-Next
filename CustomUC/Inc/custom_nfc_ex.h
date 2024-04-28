/**
  ******************************************************************************
  * @file    custom_nfc_ex.h
  * @author  System Research & Applications Team - Catania & Agrate Lab.
  * @version 1.0.0
  * @brief   This file contains definitions for the custom_nfc_ex.c
  *          specific functions.
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_NFC_EX_H
#define __CUSTOM_NFC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "astra_confmng.h"

/* Defines -------------------------------------------------------------------*/
   
/* Exported Variables --------------------------------------------------------*/

/* Exported Prototypes -------------------------------------------------------*/
extern int32_t BSP_NFCTAG_ChangeI2CPassword(uint32_t MsbPasswd,uint32_t LsbPasswd);
extern int32_t BSP_NFCTAG_SetICPasswordProtectionZone1(uint32_t MsbPasswd,uint32_t LsbPasswd,ST25DV_PROTECTION_CONF ProtectionLevel);
extern int32_t BSP_NFCTAG_EnableRFWritingPasswordProtectionZone1(uint32_t MsbPasswd,uint32_t LsbPasswd);
extern int32_t BSP_NFCTAG_DisableRFWritingPasswordProtectionZone1(uint32_t MsbPasswd,uint32_t LsbPasswd);
extern int32_t BSP_NFCTAG_CheckChangeEHMODE(uint32_t MsbPasswd,uint32_t LsbPasswd,ST25DV_EH_MODE_STATUS NewEHMode);
extern int32_t BSP_NFCTAG_WriteConfigIT(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint16_t ITConfig);
extern int32_t BSP_NFCTAG_ChangeMBMode(uint32_t MsbPasswd,uint32_t LsbPasswd,const ST25DV_EN_STATUS MB_mode);
extern int32_t BSP_NFCTAG_ChangeMBWDG(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t WdgDelay);
extern int32_t BSP_NFCTAG_ChangeITPulse(uint32_t MsbPasswd,uint32_t LsbPasswd,const ST25DV_PULSE_DURATION ITtime);

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_NFC_EX_H */


