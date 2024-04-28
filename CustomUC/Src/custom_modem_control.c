/**
  ******************************************************************************
  * @file    custom_modem_control.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   This file contains the LoRa Modem interface for custom board
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

/* Includes ------------------------------------------------------------------*/
#include "custom_modem_control.h"

/** @addtogroup CUSTOM_MODEM_CONTROL_FunctionDefinitions Custom modem control - Function definitions
  * @brief Function definitions
  * @{
 */

/* Function Definitions ------------------------------------------------------*/

#if USE_STM32WL
int32_t BSP_LoRaModemInit(void)
{
  return CUSTOM_LORA_MODEM_Init(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaModemHwReset(void)
{
  return CUSTOM_LORA_MODEM_HwReset(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaPingWL(void)
{
  return CUSTOM_LORA_MODEM_Ping(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaManageURCs(void)
{
  return CUSTOM_LORA_MODEM_ManageURCs(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaSendBinPacket(uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed)
{
  return CUSTOM_LORA_MODEM_SendBinPacket(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, Buffer, DataSize, Port, Confirmed);
}

int32_t BSP_LoRaGetVersion(CUSTOM_MODEM_versions_t*wlvers)
{
  return CUSTOM_LORA_MODEM_GetVersion(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, wlvers);
}

int32_t BSP_LoRaGetDeviceEUI(uint8_t *pDevEui)
{
  return CUSTOM_LORA_MODEM_GetDeviceEUI(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pDevEui);
}

int32_t BSP_LoRaGetAppEUI(uint8_t *pAppEui)
{
  return CUSTOM_LORA_MODEM_GetAppEUI(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pAppEui);
}

int32_t BSP_LoRaGetAppKEY(uint8_t *pAppKey)
{
  return CUSTOM_LORA_MODEM_GetAppKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pAppKey);
}

int32_t BSP_LoRaGetNwkKEY(uint8_t *pNwkKey)
{
  return CUSTOM_LORA_MODEM_GetNwkKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pNwkKey);
}

int32_t BSP_LoRaSetAppEUI(uint8_t *NewAppEui)
{
  return CUSTOM_LORA_MODEM_SetAppEUI(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, NewAppEui);
}

int32_t BSP_LoRaSetAppKEY(uint8_t *NewAppKey)
{
  return CUSTOM_LORA_MODEM_SetAppKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, NewAppKey);
}

int32_t BSP_LoRaSetNwkKEY(uint8_t *NewNwkKey)
{
  return CUSTOM_LORA_MODEM_SetNwkKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, NewNwkKey);
}



int32_t BSP_LoRaGetDADD(uint32_t *pDAddress)
{
  return CUSTOM_LORA_MODEM_GetDADD(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pDAddress);
}

int32_t BSP_LoRaGetAppSKEY(uint8_t *pAppSKey)
{
  return CUSTOM_LORA_MODEM_GetAppSKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pAppSKey);
}

int32_t BSP_LoRaGetNwkSKEY(uint8_t *pNwkSKey)
{
  return CUSTOM_LORA_MODEM_GetNwkSKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pNwkSKey);
}

int32_t BSP_LoRaSetDADD(uint32_t NewDAddress)
{
  return CUSTOM_LORA_MODEM_SetDADD(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, NewDAddress);
}

int32_t BSP_LoRaSetAppSKEY(uint8_t *NewAppSKey)
{
  return CUSTOM_LORA_MODEM_SetAppSKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, NewAppSKey);
}

int32_t BSP_LoRaSetNwkSKEY(uint8_t *NewNwkSKey)
{
  return CUSTOM_LORA_MODEM_SetNwkSKEY(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, NewNwkSKey);
}




int32_t BSP_LoRaGetDataRate(uint8_t *pnLoraDr)
{
  return CUSTOM_LORA_MODEM_GetDataRate(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, pnLoraDr);
}

int32_t BSP_LoRaSetRegionBandID(CUSTOM_MODEM_region_t *region)
{
  return CUSTOM_LORA_MODEM_SetRegionBandID(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, region);
}

int32_t BSP_LoRaJoin_OTAA(void)
{
  return CUSTOM_LORA_MODEM_Join_OTAA(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaJoin_ABP(void)
{
  return CUSTOM_LORA_MODEM_Join_ABP(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaSetAdaptiveDataRate(uint8_t adr)
{
  return CUSTOM_LORA_MODEM_SetAdaptiveDataRate(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, adr);
}

int32_t BSP_LoRaSetDataRate(uint8_t dr)
{
  return CUSTOM_LORA_MODEM_SetDataRate(CUSTOM_STM32WL_MODEM_0, MODEM_LORA, dr);
}

int32_t BSP_LoRaWL_LowPower(void)
{
  return CUSTOM_LORA_MODEM_WL_LowPower(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

int32_t BSP_LoRaWL_RebootDfu(void)
{
  return CUSTOM_LORA_MODEM_WL_RebootDfu(CUSTOM_STM32WL_MODEM_0, MODEM_LORA);
}

#endif //USE_STM32WL

/**
  * @} CUSTOM_MODEM_CONTROL_FunctionDefinitions
  */

/**
  * @} ASTRA_BSP_MODEM
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
