/**
  ******************************************************************************
  * @file    app_astra.h
  * @version V1.0.0
  * @author  SRA Team
  * @brief   Header file contains the Astra demo
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

/** @addtogroup ASTRA_ENGINE ASTRA ENGINE
  * @{
  */

/** @addtogroup APP_ASTRA ASTRA application
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ASTRA_H__
#define __APP_ASTRA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "app_conf.h"
#include "app_common.h"

/** @addtogroup APP_ASTRA_FunctionPrototypes ASTRA application - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/**
  * @brief  ASTRA Init. See \ref SM_Config_Modules structure, APP_ST_START state
  * @param  none
  * @retval none
  */
void MX_Astra_Init(void);

/**
  * @brief  ASTRA process. See \ref SM_Config_Modules structure, APP_ST_RUN and APP_ST_LP states
  * @param  none
  * @retval none
  */
void MX_Astra_Process(void);

#ifdef __cplusplus
}
#endif

#endif /*__APP_ASTRA_H__ */

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
