/**
  ******************************************************************************
  * @file    custom_lora_modem.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   This file provides LoRa Modem BSP interface for custom boards
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
#include "custom_lora_modem.h"

extern void *ModemCompObj[CUSTOM_MODEM_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */

/** @addtogroup CUSTOM_LORA_MODEM_PRIVATE_VARIABLES Custom LoRa modem - Private variables
  * @brief Private variables
  * @{
 */

/* Private variables ---------------------------------------------------------*/

void *ModemCompObj[CUSTOM_MODEM_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] = {0, 0, 1, 1, 2};
static MODEM_FuncDrv_t *ModemFuncDrv[CUSTOM_MODEM_INSTANCES_NBR][CUSTOM_MODEM_FUNCTIONS_NBR];
#if USE_STM32WL
static MODEM_CommonDrv_t *ModemDrv[CUSTOM_MODEM_INSTANCES_NBR];
#endif //USE_STM32WL
static CUSTOM_MODEM_Ctx_t ModemCtx[CUSTOM_MODEM_INSTANCES_NBR];

/**
  * @} CUSTOM_LORA_MODEM_PRIVATE_VARIABLES
  */

/** @addtogroup CUSTOM_LORA_MODEM_PivateFunctionPrototypes Custom LoRa modem - Private function Prototypes
  * @brief Provate function prototypes
  * @{
 */

/* Function prototypes -----------------------------------------------*/

#if USE_STM32WL

/**
  * @brief  Modem probe
  * @param  Functions: Modem functions. It could be:
  *         - MODEM_LORA
  * @retval BSP status
  */
static int32_t STM32WL_MODEM_0_Probe(uint32_t Functions);
#endif //USE_STM32WL

/**
  * @} CUSTOM_LORA_MODEM_PivateFunctionPrototypes
  */

/** @addtogroup CUSTOM_LORA_MODEM_FunctionDefines Custom LoRa modem - Function Defines
  * @brief Function definitions
  * @{
 */

/* Function Definitions ------------------------------------------------------*/

int32_t CUSTOM_LORA_MODEM_Ping(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->Ping(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_ManageURCs(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->ManageURCs(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_GetDeviceEUI(uint32_t Instance, uint32_t Function, uint8_t *pDevEui)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetDeviceEUI(ModemCompObj[Instance], pDevEui) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_GetAppEUI(uint32_t Instance, uint32_t Function, uint8_t *pAppEui)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetAppEUI(ModemCompObj[Instance], pAppEui) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_GetAppKEY(uint32_t Instance, uint32_t Function, uint8_t *pAppKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetAppKEY(ModemCompObj[Instance], pAppKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_GetNwkKEY(uint32_t Instance, uint32_t Function, uint8_t *pNwkKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetNwkKEY(ModemCompObj[Instance], pNwkKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetAppEUI(uint32_t Instance, uint32_t Function, uint8_t *NewAppEui)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetAppEUI(ModemCompObj[Instance], NewAppEui) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetAppKEY(uint32_t Instance, uint32_t Function, uint8_t *NewAppKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetAppKEY(ModemCompObj[Instance], NewAppKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetNwkKEY(uint32_t Instance, uint32_t Function, uint8_t *NewNwkKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetNwkKEY(ModemCompObj[Instance], NewNwkKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}






int32_t CUSTOM_LORA_MODEM_GetDADD(uint32_t Instance, uint32_t Function, uint32_t *pDAddress)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetDADD(ModemCompObj[Instance], pDAddress) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;

}

int32_t CUSTOM_LORA_MODEM_GetAppSKEY(uint32_t Instance, uint32_t Function, uint8_t *pAppSKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetAppSKEY(ModemCompObj[Instance], pAppSKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;

}

int32_t CUSTOM_LORA_MODEM_GetNwkSKEY(uint32_t Instance, uint32_t Function, uint8_t *pNwkSKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetNwkSKEY(ModemCompObj[Instance], pNwkSKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetDADD(uint32_t Instance, uint32_t Function, uint32_t NewDAddress)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetDADD(ModemCompObj[Instance], NewDAddress) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;

}

int32_t CUSTOM_LORA_MODEM_SetAppSKEY(uint32_t Instance, uint32_t Function, uint8_t *NewAppSKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetAppSKEY(ModemCompObj[Instance], NewAppSKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;

}

int32_t CUSTOM_LORA_MODEM_SetNwkSKEY(uint32_t Instance, uint32_t Function, uint8_t *NewNwkSKey)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetNwkSKEY(ModemCompObj[Instance], NewNwkSKey) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;

}






int32_t CUSTOM_LORA_MODEM_GetDataRate(uint32_t Instance, uint32_t Function, uint8_t *pnLoraDr)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetDataRate(ModemCompObj[Instance], pnLoraDr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetRegionBandID(uint32_t Instance, uint32_t Function, CUSTOM_MODEM_region_t *region)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetRegionBandID(ModemCompObj[Instance], region) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_Join_OTAA(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->Join_OTAA(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_Join_ABP(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->Join_ABP(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetAdaptiveDataRate(uint32_t Instance, uint32_t Function, uint8_t adr)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetAdaptiveDataRate(ModemCompObj[Instance], adr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SetDataRate(uint32_t Instance, uint32_t Function, uint8_t dr)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SetDataRate(ModemCompObj[Instance], dr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_WL_LowPower(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->WL_LowPower(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_WL_RebootDfu(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->WL_RebootDfu(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_GetVersion(uint32_t Instance, uint32_t Function, CUSTOM_MODEM_versions_t * ver)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->GetVersion(ModemCompObj[Instance], ver) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_SendBinPacket(uint32_t Instance, uint32_t Function, uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->SendBinPacket(ModemCompObj[Instance], Buffer, DataSize, Port, Confirmed) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_HwReset(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= CUSTOM_MODEM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((ModemCtx[Instance].Functions & Function) == Function)
    {
      if (ModemFuncDrv[Instance][FunctionIndex[Function]]->HwReset(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

int32_t CUSTOM_LORA_MODEM_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = MODEM_LORA;
  uint32_t i;
  uint32_t component_functions = 0;
  CUSTOM_MODEM_Capabilities_t cap;

  switch (Instance)
  {
#if USE_STM32WL
    case CUSTOM_STM32WL_MODEM_0:
      if (STM32WL_MODEM_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (ModemDrv[Instance]->GetCapabilities(ModemCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.LoRa == 1U)
      {
        component_functions |= MODEM_LORA;
      }
      break;
#endif //USE_STM32WL
    default:
      UNUSED(cap);
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if (ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for (i = 0; i < CUSTOM_MODEM_FUNCTIONS_NBR; i++)
  {
    if (((Functions & function) == function) && ((component_functions & function) == function))
    {
      if (ModemFuncDrv[Instance][FunctionIndex[function]]->Enable(ModemCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

#if USE_STM32WL
static int32_t STM32WL_MODEM_0_Probe(uint32_t Functions)
{
  STM32WL_MODEM_IO_t            io_ctx;
  static STM32WL_MODEM_Object_t stm32wl_modem_obj_0;
  STM32WL_MODEM_Capabilities_t  cap;
  int32_t                       ret = BSP_ERROR_NONE;

  /* Configure the driver */
  io_ctx.BusType     = STM32WL_MODEM_UART_BUS; /* UART */
  io_ctx.Address     = 0; /* reserved */
  io_ctx.Init        = CUSTOM_STM32WL_MODEM_0_UART_Init;
  io_ctx.DeInit      = CUSTOM_STM32WL_MODEM_0_UART_DeInit;
  io_ctx.Read        = CUSTOM_STM32WL_MODEM_0_UART_Read;
  io_ctx.Write       = CUSTOM_STM32WL_MODEM_0_UART_Write;
  io_ctx.Reset       = CUSTOM_STM32WL_MODEM_0_Reset;
  io_ctx.GetTick     = BSP_GetTick;

  if (STM32WL_MODEM_RegisterBusIO(&stm32wl_modem_obj_0, &io_ctx) != STM32WL_MODEM_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)STM32WL_MODEM_GetCapabilities(&stm32wl_modem_obj_0, &cap);
    ModemCtx[CUSTOM_STM32WL_MODEM_0].Functions = ((uint32_t)cap.LoRa);

    ModemCompObj[CUSTOM_STM32WL_MODEM_0] = &stm32wl_modem_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    ModemDrv[CUSTOM_STM32WL_MODEM_0] = (MODEM_CommonDrv_t *)(void *)&STM32WL_MODEM_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MODEM_LORA) == MODEM_LORA) && (cap.LoRa == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      ModemFuncDrv[CUSTOM_STM32WL_MODEM_0][FunctionIndex[MODEM_LORA]] = (MODEM_FuncDrv_t *)(void *)&STM32WL_MODEM_LORA_Driver;

      if (ModemDrv[CUSTOM_STM32WL_MODEM_0]->Init(ModemCompObj[CUSTOM_STM32WL_MODEM_0]) != STM32WL_MODEM_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }
  return ret;
}
#endif //USE_STM32WL

/**
  * @} CUSTOM_LORA_MODEM_FunctionDefines
  */

/**
  * @} ASTRA_BSP_MODEM
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
