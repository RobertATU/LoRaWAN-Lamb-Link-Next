/**
 ******************************************************************************
 * @file    custom_mems_control.h
 * @author  MEMS Application Team
 * @brief   This file contains the MEMS sensors interface for custom board
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
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
#ifndef CUSTOM_MEMS_CONTROL_H
#define CUSTOM_MEMS_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "RTE_Components.h"

#if (defined BSP_MOTION_SENSORS)
#include "custom_motion_sensors.h"
#include "custom_motion_sensors_ex.h"
#endif

#if (defined BSP_ENV_SENSORS)
#include "custom_env_sensors.h"
#include "custom_env_sensors_ex.h"
#endif

#if (defined BSP_HYBRID_SENSORS)
#include "custom_hybrid_sensors.h"
#include "custom_hybrid_sensors_ex.h"
#endif

/** @addtogroup CUSTOM_MEMS_CONTROL_DEFINES MEMs control - Defines
  * @brief Defines
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

#if (defined BSP_MOTION_SENSORS)
#define MOTION_SENSOR_Axes_t CUSTOM_MOTION_SENSOR_Axes_t
#define MOTION_SENSOR_Event_Status_t  CUSTOM_MOTION_SENSOR_Event_Status_t
#else //(defined BSP_MOTION_SENSORS)

#define MOTION_SENSOR_Axes_t SENSOR_Axes_t
#define MOTION_SENSOR_Event_Status_t  uint8_t
#endif //(defined BSP_MOTION_SENSORS)

//#define LP_ACC_WAKEUP_DETECTION 1
//#define LP_ACC_INACTIVITY_DETECTION 2

/**
* @} CUSTOM_MEMS_CONTROL_DEFINES
*/

/** @addtogroup CUSTOM_MEMS_CONTROL_EXPORTED_TYPES MEMs control - Exported Types
  * @brief Exported Types
  * @{
 */

/* Exported types ------------------------------------------------------------*/

typedef struct SENSOR_Axes_s
{
  uint32_t x;
  uint32_t y;
  uint32_t z;
}SENSOR_Axes_t;

/**
  * @} CUSTOM_MEMS_CONTROL_EXPORTED_TYPES
  */

/** @addtogroup CUSTOM_MEMS_CONTROL_FunctionPrototypes MEMs control - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_Init(void);
void BSP_SENSOR_LP_ACC_Init(void);
void BSP_SENSOR_GYR_Init(void);
void BSP_SENSOR_MAG_Init(void);
#endif //(defined BSP_MOTION_SENSORS)
#if (defined BSP_ENV_SENSORS)
void BSP_SENSOR_PRESS_Init(void);
void BSP_SENSOR_TEMP_Init(void);
void BSP_SENSOR_HUM_Init(void);
#endif //(defined BSP_ENV_SENSORS)

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_Enable(void);
void BSP_SENSOR_GYR_Enable(void);
void BSP_SENSOR_MAG_Enable(void);
#endif //(defined BSP_MOTION_SENSORS)
#if (defined BSP_ENV_SENSORS)
void BSP_SENSOR_PRESS_Enable(void);
void BSP_SENSOR_TEMP_Enable(void);
void BSP_SENSOR_HUM_Enable(void);
#endif //(defined BSP_ENV_SENSORS)

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_Disable(void);
void BSP_SENSOR_LP_ACC_Disable(void);
void BSP_SENSOR_GYR_Disable(void);
void BSP_SENSOR_MAG_Disable(void);
#endif //(defined BSP_MOTION_SENSORS)
#if (defined BSP_ENV_SENSORS)
void BSP_SENSOR_PRESS_Disable(void);
void BSP_SENSOR_TEMP_Disable(void);
void BSP_SENSOR_HUM_Disable(void);
#endif //(defined BSP_ENV_SENSORS)

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_GetAxes(MOTION_SENSOR_Axes_t *Axes);
void BSP_SENSOR_LP_ACC_GetAxes(MOTION_SENSOR_Axes_t *Axes);
void BSP_SENSOR_GYR_GetAxes(MOTION_SENSOR_Axes_t *Axes);
void BSP_SENSOR_MAG_GetAxes(MOTION_SENSOR_Axes_t *Axes);
void BSP_SENSOR_LP_ACC_GetTemp(float *Value);
#endif //(defined BSP_MOTION_SENSORS)
#if (defined BSP_ENV_SENSORS)
void BSP_SENSOR_PRESS_GetValue(float *Value);
void BSP_SENSOR_TEMP_GetValue(float *Value);
void BSP_SENSOR_HUM_GetValue(float *Value);
#endif //(defined BSP_ENV_SENSORS)

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_SetOutputDataRate(float Odr);
void BSP_SENSOR_LP_ACC_SetOutputDataRate(float Odr);
void BSP_SENSOR_GYR_SetOutputDataRate(float Odr);
void BSP_SENSOR_MAG_SetOutputDataRate(float Odr);
#endif //(defined BSP_MOTION_SENSORS)
#if (defined BSP_ENV_SENSORS)
void BSP_SENSOR_PRESS_SetOutputDataRate(float Odr);
void BSP_SENSOR_TEMP_SetOutputDataRate(float Odr);
void BSP_SENSOR_HUM_SetOutputDataRate(float Odr);
#endif //(defined BSP_ENV_SENSORS)

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_GetOutputDataRate(float *Odr);
void BSP_SENSOR_GYR_GetOutputDataRate(float *Odr);
void BSP_SENSOR_MAG_GetOutputDataRate(float *Odr);
#endif //(defined BSP_MOTION_SENSORS)
#if (defined BSP_ENV_SENSORS)
void BSP_SENSOR_PRESS_GetOutputDataRate(float *Odr);
void BSP_SENSOR_TEMP_GetOutputDataRate(float *Odr);
void BSP_SENSOR_HUM_GetOutputDataRate(float *Odr);
#endif //(defined BSP_ENV_SENSORS)

#if (defined BSP_MOTION_SENSORS)
void BSP_SENSOR_ACC_SetFullScale(int32_t Fullscale);
void BSP_SENSOR_LP_ACC_SetFullScale(int32_t Fullscale);
void BSP_SENSOR_LP_ACC_Enable_Wake_Up_Detection(void);
void BSP_SENSOR_LP_ACC_Enable_Inactivity_Detection(void);
void BSP_SENSOR_LP_ACC_Enable_Interrupt_Latch(void);
void BSP_SENSOR_LP_ACC_Set_Wake_Up_Threshold( uint8_t Threshold);
void BSP_SENSOR_LP_ACC_Set_Wake_Up_Duration( uint8_t Duration);
void BSP_SENSOR_LP_ACC_Get_Event_Status(MOTION_SENSOR_Event_Status_t *Status);
void BSP_SENSOR_LP_ACC_Clear_Wkp(void);
void BSP_SENSOR_LP_ACC_Set_Sleep_Duration(uint8_t Duration);
void BSP_SENSOR_LP_ACC_Enable_All_On_INT1(void);
void BSP_SENSOR_LP_ACC_Disable_All_On_INT1(void);
void BSP_SENSOR_LP_ACC_Disable_Wake_Up_Detection(void);
void BSP_SENSOR_LP_ACC_Disable_Inactivity_Detection(void);
void BSP_SENSOR_GYR_SetFullScale(int32_t Fullscale);
void BSP_SENSOR_MAG_SetFullScale(int32_t Fullscale);
void BSP_SENSOR_ACC_Enable_Wake_Up_Detection(void);
void BSP_SENSOR_ACC_Get_Event_Status(MOTION_SENSOR_Event_Status_t *Status);

void BSP_SENSOR_ACC_GetFullScale(int32_t *Fullscale);
void BSP_SENSOR_GYR_GetFullScale(int32_t *Fullscale);
void BSP_SENSOR_MAG_GetFullScale(int32_t *Fullscale);

/**
  * @} CUSTOM_MEMS_CONTROL_FunctionPrototypes
  */

#endif //(defined BSP_MOTION_SENSORS)

#endif /* CUSTOM_MEMS_CONTROL_H */

/**
  * @} ASTRA_BSP_SOB
  */

/**
  * @} ASTRA_BSP
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
