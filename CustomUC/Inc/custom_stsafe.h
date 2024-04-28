/**
  ******************************************************************************
  * @file    custom_stsafe.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Astra STSAFE BSP header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_STSAFE__H__
#define __CUSTOM_STSAFE__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "steval_astra1b_conf.h"
#include "steval_astra1b_bus.h"
#include "steval_astra1b_errno.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* STSAFE_RST */

/* STSAFE_I2C */

/** @addtogroup CUSTOM_STSAFE_EXPORTED_DEFINES Custom STSAFE - Exported defines
  * @brief Exported Defines
  * @{
 */

/* Exported defines -----------------------------------------------------------*/

/* STSAFE_RST */
#define STSAFE_RST_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define STSAFE_RST_PORT            GPIOC
#define STSAFE_RST_PIN             GPIO_PIN_12

/* STSAFE_I2C */
#define STSAFE_I2C_INST             I2C3

#define H_STSAFE_I2C                hi2c3

#define I2C_Init                                  BSP_I2C3_Init
#define I2C_DeInit                                BSP_I2C3_DeInit
#define I2C_Send                                  BSP_I2C3_Send
#define I2C_Recv                                  BSP_I2C3_Recv

/**
* @} CUSTOM_STSAFE_EXPORTED_DEFINES
*/

/* External variables --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

#if USE_STSAFE

/** @addtogroup CUSTOM_STSAFE_EXPORTED_FUNCTIONS_PROTOTYPES Custom STSAFE - Exported function prototypes
  * @brief Exported function prototypes
  * @{
 */

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  STSAFE initialization
  * @param  none
  * @retval none
  */
void custom_stsafe_init(void);

/**
  * @brief  STSAFE reset
  * @param  none
  * @retval none
  */
void custom_stsafe_reset(uint8_t level);

/**
* @} CUSTOM_STSAFE_EXPORTED_FUNCTIONS_PROTOTYPES
*/

#endif //USE_STSAFE

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_STSAFE__H__ */

/**
* @} STSAFEA_CUSTOM
*/

/**
* @} STSAFEA
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
