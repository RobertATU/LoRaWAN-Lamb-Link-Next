/**
  ******************************************************************************
  * @file    SM_Conf.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Configuration file in SM Module
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

#ifndef __SM_CONF_H
#define __SM_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SM_MODULE
  * @{
  */

/** @defgroup SM_MODULE_Conf Conf template
  * @brief Configuration file template: defines for states, events and modules
  * @{
  */

/* module configuration */
/** @defgroup SM_MODULE_Conf_misc misc
  * @brief SM module configuration
  * @{
  */
#define SM_USE_LOG  1                          /*!< enable/disable log         */
#define SM_USE_NAME 1                          /*!< enable/disable name (to reduce the footprint) */
/**
  * @} SM_MODULE_Conf_misc
  */

/* module macro definition */
/** @defgroup SM_MODULE_Conf_macro macro
  * @brief example of macro definitions
  * @{
  */
#include "astra_confmng.h"
#include "astra_datamng.h"
#include "astra_sysmng.h"

/* Low priority printf remap */
#define SM_PRINTF_0(...)  PRINTF_INFO(__VA_ARGS__)     /*!< printf define              */

/* printf remap */
#define SM_PRINTF(...)  PRINTF_VERBOSE(__VA_ARGS__)     /*!< printf define              */

/**
  * @} SM_MODULE_Conf_macro
  */

/**
  * @} SM_MODULE_Conf
  */

/**
  * @} SM_MODULE
  */

#ifdef __cplusplus
}
#endif

#endif /* __SM_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
