/**
  ******************************************************************************
  * @file    custom_modem_conf.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   LoRa Modem BSP configuration file for custom boards
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

/** @addtogroup ASTRA_BSP ASTRA board support package
  * @{
  */

/** @addtogroup ASTRA_BSP_SOB ASTRA SOB board BSP
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_MODEM_CONF__H__
#define __CUSTOM_MODEM_CONF__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"

#include "custom_sob.h"

#define USE_CUSTOM_LORA_MODEM_STM32WL_MODEM_0     1U

#define CUSTOM_STM32WL_MODEM_0_UART_Init        custom_sob_stm32wl_init
#define CUSTOM_STM32WL_MODEM_0_UART_DeInit      custom_sob_stm32wl_deinit
#define CUSTOM_STM32WL_MODEM_0_UART_Read        custom_sob_stm32wl_uartrecv_dma
#define CUSTOM_STM32WL_MODEM_0_UART_Write       custom_sob_stm32wl_uartsend
#define CUSTOM_STM32WL_MODEM_0_Reset            custom_sob_stm32wl_reset

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_MODEM_CONF__H__ */

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
