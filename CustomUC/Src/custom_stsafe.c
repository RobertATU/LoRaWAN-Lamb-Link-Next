/**
  ******************************************************************************
  * @file    custom_stsafe.c
  * @author  SRA Team
  * @brief   Astra STSAFE BSP
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup STSAFEA STSAFEA
  * @{
  */

/** @addtogroup STSAFEA_CUSTOM STSAFEA customization
  * @{
  */

/* Includes ------------------------------------------------------------------*/

#include "astra_conf.h"

#if USE_STSAFE
#include "custom_stsafe.h"

#include "custom_stsafea_crc.h"
#include "stsafea_service.h"
#include "stsafea_interface_conf.h"

/* Private typedef -----------------------------------------------------------*/

/** @addtogroup CUSTOM_STFASE_Private_Defines Custom STSAFE - Private defines
  * @brief Private Defines
  * @{
 */

/* Private defines -----------------------------------------------------------*/
#define STSAFEA_DEVICE_ADDRESS                    0x0020

/**
* @} CUSTOM_STFASE_Private_Defines
*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** @addtogroup CUSTOM_STFASE_Private_FunctionPrototypes Custom STSAFE - Private function prototypes
  * @brief Private function prototypes
  * @{
 */

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  STSAFE hardware IO initialization
  * @param  none
  * @retval service error codes
  */
static int32_t HW_IO_Init(void);

/**
  * @brief  StSafeA_HW_Probe
  *         Configure STSAFE IO and Bus operation functions to be implemented at User level
  * @param  Ctx the STSAFE IO context
  * @retval 0 in case of success, an error code otherwise
  */
int8_t StSafeA_HW_Probe(void *pCtx);

/**
* @} CUSTOM_STFASE_Private_FunctionPrototypes
*/

/** @addtogroup CUSTOM_STFASE_FunctionDefinitions Custom STSAFE - Function definitions
  * @brief Function definitions
  * @{
 */
/* Exported functions --------------------------------------------------------*/

/* USE_STSAFE */
void custom_stsafe_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* D0_1_2_CTRL */
  STSAFE_RST_CLK_ENABLE();

  HAL_GPIO_WritePin(STSAFE_RST_PORT, STSAFE_RST_PIN, GPIO_PIN_SET);

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = STSAFE_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STSAFE_RST_PORT, &GPIO_InitStruct);

  I2C_Init();
}

void custom_stsafe_reset(uint8_t level)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* D0_1_2_CTRL */
  STSAFE_RST_CLK_ENABLE();

  if(level)
  {
    HAL_GPIO_WritePin(STSAFE_RST_PORT, STSAFE_RST_PIN, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(STSAFE_RST_PORT, STSAFE_RST_PIN, GPIO_PIN_RESET);
  }

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = STSAFE_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STSAFE_RST_PORT, &GPIO_InitStruct);
}

static int32_t HW_IO_Init(void)
{
  return STSAFEA_BUS_OK;
}

int8_t StSafeA_HW_Probe(void *pCtx)
{
  STSAFEA_HW_t *HwCtx = (STSAFEA_HW_t *)pCtx;

  HwCtx->IOInit     = HW_IO_Init;
  HwCtx->BusInit    = I2C_Init;
  HwCtx->BusDeInit  = I2C_DeInit;
  HwCtx->BusSend    = I2C_Send;
  HwCtx->BusRecv    = I2C_Recv;
  HwCtx->CrcInit    = CRC16X25_Init;
  HwCtx->CrcCompute = CRC_Compute;
  HwCtx->TimeDelay  = HAL_Delay;
  HwCtx->DevAddr    = STSAFEA_DEVICE_ADDRESS;

  return STSAFEA_BUS_OK;
}

#endif //USE_STSAFE

/**
* @} CUSTOM_STFASE_FunctionDefinitions
*/

/**
* @} STSAFEA_CUSTOM
*/

/**
* @} STSAFEA
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
