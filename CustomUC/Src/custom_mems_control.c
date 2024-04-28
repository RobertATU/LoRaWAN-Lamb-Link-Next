/**
 ******************************************************************************
 * @file    custom_mems_control.c
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

/** @addtogroup ASTRA_BSP_MAIN ASTRA main board BSP
  * @{
  */

/* Includes ------------------------------------------------------------------*/

#include "RTE_Components.h"

#if (defined BSP_MOTION_SENSORS)

#include "custom_mems_conf.h"
#include "custom_mems_conf_app.h"
#include "custom_mems_control.h"
#include "custom_mems_control_ex.h"

/** @addtogroup CUSTOM_MEMS_CONTROL_FunctionDefines MEMs control - Function Defines
  * @brief Function definitions
  * @{
 */

/* Function Definitions ------------------------------------------------------*/

/**
  * @brief  Initializes accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_ACC_Init(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACCELEROMETER
  (void)CUSTOM_MOTION_SENSOR_Init(CUSTOM_ACCELEROMETER, MOTION_ACCELERO);
  #endif
#endif
}

/**
  * @brief  Initializes accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_LP_ACC_Init(void)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Init(CUSTOM_LP_ACCELEROMETER, HYBRID_ACCELERO | HYBRID_TEMPERATURE);
  #endif
#endif
}

/**
  * @brief  Initializes accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_LP_ACC_Interrupt_Config(void)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Init(CUSTOM_LP_ACCELEROMETER, HYBRID_ACCELERO); // to be completed
  #endif
#endif
}

/**
 * @brief  Get the status of all hardware events
 * @param  Instance the device instance
 * @param  Status the pointer to the status of all hardware events
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Get_Event_Status(uint32_t Instance, CUSTOM_MOTION_SENSOR_Event_Status_t *Status)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      if (LIS2DTW12_ACC_Get_Event_Status(HybridCompObj[Instance], (LIS2DTW12_Event_Status_t *)(void *)Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
  * @brief  Read event status
  * @param  CUSTOM_MOTION_SENSOR_Event_Status_t
  * @retval None
  */

void  BSP_SENSOR_LP_ACC_Get_Event_Status(CUSTOM_MOTION_SENSOR_Event_Status_t *Status)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Get_Event_Status(CUSTOM_LP_ACCELEROMETER, Status);
  #endif
#endif
}

/**
  * @brief  Read event status
  * @param  CUSTOM_MOTION_SENSOR_Event_Status_t
  * @retval None
  */
void BSP_SENSOR_LP_ACC_Clear_Wkp(void)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
   uint8_t wkup_src;
   (void) BSP_SENSOR_LP_ACC_Read_Register(LIS2DTW12_WAKE_UP_SRC, &wkup_src);
  #endif
#endif
}

/**
  * @brief  Initializes gyroscope
  * @param  None
  * @retval None
  */
void BSP_SENSOR_GYR_Init(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Init(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO);
  #endif
#endif
}

/**
  * @brief  Initializes magnetometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_MAG_Init(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_MAG_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Init(CUSTOM_MAG_INSTANCE_0, MOTION_MAGNETO);
  #endif
#endif
}

/**
  * @brief  Initializes pressure sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_PRESS_Init(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_PRESS_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_Init(CUSTOM_PRESS_INSTANCE_0, ENV_PRESSURE);
  #endif
#endif
}

/**
  * @brief  Initializes temperature sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_TEMP_Init(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_STTS22H_0
  (void)CUSTOM_ENV_SENSOR_Init(CUSTOM_STTS22H_0, ENV_TEMPERATURE);
  #endif
#endif
}

/**
  * @brief  Initializes humidity sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_HUM_Init(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_HUM_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_Init(CUSTOM_HUM_INSTANCE_0, ENV_HUMIDITY);
  #endif
#endif
}

/**
  * @brief  Enables accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_ACC_Enable(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACC_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Enable(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO);
  #endif
#endif
}

/**
  * @brief  Enables gyroscope
  * @param  None
  * @retval None
  */
void BSP_SENSOR_GYR_Enable(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Enable(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO);
  #endif
#endif
}

/**
  * @brief  Enables magnetometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_MAG_Enable(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_MAG_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Enable(CUSTOM_MAG_INSTANCE_0, MOTION_MAGNETO);
  #endif
#endif
}

/**
  * @brief  Enables pressure sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_PRESS_Enable(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_PRESS_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_Enable(CUSTOM_PRESS_INSTANCE_0, ENV_PRESSURE);
  #endif
#endif
}

/**
  * @brief  Enables temperature sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_TEMP_Enable(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_STTS22H_0
  (void)CUSTOM_ENV_SENSOR_Enable(CUSTOM_STTS22H_0, ENV_TEMPERATURE);
  #endif
#endif
}

/**
  * @brief  Enables humidity sensors
  * @param  None
  * @retval None
  */
void BSP_SENSOR_HUM_Enable(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_HUM_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_Enable(CUSTOM_HUM_INSTANCE_0, ENV_HUMIDITY);
  #endif
#endif
}

/**
  * @brief  Disables accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_ACC_Disable(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACC_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Disable(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO);
  #endif
#endif
}

/**
  * @brief  Disables LP accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_LP_ACC_Disable(void)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Disable(CUSTOM_LP_ACCELEROMETER, HYBRID_ACCELERO);
  (void)CUSTOM_HYBRID_ENV_SENSOR_Disable(CUSTOM_LP_ACCELEROMETER, HYBRID_TEMPERATURE);
  #endif
#endif
}

/**
  * @brief  Disables gyroscope
  * @param  None
  * @retval None
  */
void BSP_SENSOR_GYR_Disable(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Disable(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO);
  #endif
#endif
}

/**
  * @brief  Disables magnetometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_MAG_Disable(void)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_MAG_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_Disable(CUSTOM_MAG_INSTANCE_0, MOTION_MAGNETO);
  #endif
#endif
}

/**
  * @brief  Disables pressure sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_PRESS_Disable(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_PRESS_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_Disable(CUSTOM_PRESS_INSTANCE_0, ENV_PRESSURE);
  #endif
#endif
}

/**
  * @brief  Disables temperature sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_TEMP_Disable(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_STTS22H_0
  (void)CUSTOM_ENV_SENSOR_Disable(CUSTOM_STTS22H_0, ENV_TEMPERATURE);
  #endif
#endif
}

/**
  * @brief  Disables humidity sensor
  * @param  None
  * @retval None
  */
void BSP_SENSOR_HUM_Disable(void)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_HUM_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_Disable(CUSTOM_HUM_INSTANCE_0, ENV_HUMIDITY);
  #endif
#endif
}

/**
  * @brief  Get accelerometer data
  * @param  Axes pointer to axes data structure
  * @retval None
  */
void BSP_SENSOR_ACC_GetAxes(CUSTOM_MOTION_SENSOR_Axes_t *Axes)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACCELEROMETER
  (void)CUSTOM_MOTION_SENSOR_GetAxes(CUSTOM_ACCELEROMETER, MOTION_ACCELERO, Axes);
  #else
  Axes->x = 0;
  Axes->y = 0;
  Axes->z = 0;
  #endif
#endif
}

/**
  * @brief  Get accelerometer data
  * @param  Axes pointer to axes data structure
  * @retval None
  */
void BSP_SENSOR_LP_ACC_GetAxes(CUSTOM_MOTION_SENSOR_Axes_t *Axes)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_GetAxes(CUSTOM_LP_ACCELEROMETER, HYBRID_ACCELERO, (CUSTOM_HYBRID_MOTION_SENSOR_Axes_t*)Axes);
  #else
  Axes->x = 0;
  Axes->y = 0;
  Axes->z = 0;
  #endif
#endif
}

/**
  * @brief  Get temperature data
  * @param  Value pointer to temperature value
  * @retval None
  */
void BSP_SENSOR_LP_ACC_GetTemp(float *Value)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_ENV_SENSOR_GetValue(CUSTOM_LP_ACCELEROMETER, HYBRID_TEMPERATURE, Value);
  #else
  *Value = 0.0f;
  #endif
#endif
}

/**
  * @brief  Get gyroscope data
  * @param  Axes pointer to axes data structure
  * @retval None
  */
void BSP_SENSOR_GYR_GetAxes(CUSTOM_MOTION_SENSOR_Axes_t *Axes)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetAxes(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Axes);
  #else
  Axes->x = 0;
  Axes->y = 0;
  Axes->z = 0;
  #endif
#endif
}

/**
  * @brief  Get magnetometer data
  * @param  Axes pointer to axes data structure
  * @retval None
  */
void BSP_SENSOR_MAG_GetAxes(CUSTOM_MOTION_SENSOR_Axes_t *Axes)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_MAG_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetAxes(CUSTOM_MAG_INSTANCE_0, MOTION_MAGNETO, Axes);
  #else
  Axes->x = 0;
  Axes->y = 0;
  Axes->z = 0;
  #endif
#endif
}

/**
  * @brief  Get pressure sensor data
  * @param  Value pointer to pressure value
  * @retval None
  */
void BSP_SENSOR_PRESS_GetValue(float *Value)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_PRESS_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_GetValue(CUSTOM_PRESS_INSTANCE_0, ENV_PRESSURE, Value);
  #else
  *Value = 0.0f;
  #endif
#endif
}

/**
  * @brief  Get temperature sensor data
  * @param  Value pointer to temperature value
  * @retval None
  */
void BSP_SENSOR_TEMP_GetValue(float *Value)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_STTS22H_0
  (void)CUSTOM_ENV_SENSOR_GetValue(CUSTOM_STTS22H_0, ENV_TEMPERATURE, Value);
  #else
  *Value = 0.0f;
  #endif
#endif
}

/**
  * @brief  Get humidity sensor data
  * @param  Value pointer to humidity value
  * @retval None
  */
void BSP_SENSOR_HUM_GetValue(float *Value)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_HUM_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_GetValue(CUSTOM_HUM_INSTANCE_0, ENV_HUMIDITY, Value);
  #else
  *Value = 0.0f;
  #endif
#endif
}

/**
  * @brief  Set output data rate for accelerometer
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_ACC_SetOutputDataRate(float Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACCELEROMETER
  (void)CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_ACCELEROMETER, MOTION_ACCELERO, Odr);
  #endif
#endif
}

/**
  * @brief  Set output data rate for accelerometer
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_LP_ACC_SetOutputDataRate(float Odr)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_SetOutputDataRate(CUSTOM_LP_ACCELEROMETER, HYBRID_ACCELERO, Odr);
  (void)CUSTOM_HYBRID_ENV_SENSOR_SetOutputDataRate(CUSTOM_LP_ACCELEROMETER, HYBRID_TEMPERATURE, Odr);
  #endif
#endif
}

/**
  * @brief  Set output data rate for gyroscope
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_GYR_SetOutputDataRate(float Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Odr);
  #endif
#endif
}

/**
  * @brief  Set output data rate for magnetometer
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_MAG_SetOutputDataRate(float Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_MAG_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_MAG_INSTANCE_0, MOTION_MAGNETO, Odr);
  #endif
#endif
}

/**
  * @brief  Set output data rate for pressure sensor
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_PRESS_SetOutputDataRate(float Odr)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_PRESS_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_SetOutputDataRate(CUSTOM_PRESS_INSTANCE_0, ENV_PRESSURE, Odr);
  #endif
#endif
}

/**
  * @brief  Set output data rate for temperature sensors
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_TEMP_SetOutputDataRate(float Odr)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_STTS22H_0
  (void)CUSTOM_ENV_SENSOR_SetOutputDataRate(CUSTOM_STTS22H_0, ENV_TEMPERATURE, Odr);
  #endif
#endif
}

/**
  * @brief  Set output data rate for humidity sensor
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_HUM_SetOutputDataRate(float Odr)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_HUM_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_SetOutputDataRate(CUSTOM_HUM_INSTANCE_0, ENV_HUMIDITY, Odr);
  #endif
#endif
}

/**
  * @brief  Get output data rate for accelerometer
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_ACC_GetOutputDataRate(float *Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACC_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetOutputDataRate(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, Odr);
  #endif
#endif
}

/**
  * @brief  Get output data rate for gyroscope
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_GYR_GetOutputDataRate(float *Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetOutputDataRate(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Odr);
  #endif
#endif
}

/**
  * @brief  Get output data rate for magnetometer
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_MAG_GetOutputDataRate(float *Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_MAG_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetOutputDataRate(CUSTOM_MAG_INSTANCE_0, MOTION_MAGNETO, Odr);
  #endif
#endif
}

/**
  * @brief  Get output data rate for pressure sensor
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_PRESS_GetOutputDataRate(float *Odr)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_PRESS_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_GetOutputDataRate(CUSTOM_PRESS_INSTANCE_0, ENV_PRESSURE, Odr);
  #endif
#endif
}

/**
  * @brief  Get output data rate for temperature sensors
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_TEMP_GetOutputDataRate(float *Odr)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_STTS22H_0
  (void)CUSTOM_ENV_SENSOR_GetOutputDataRate(CUSTOM_STTS22H_0, ENV_TEMPERATURE, Odr);
  #endif
#endif
}

/**
  * @brief  Get output data rate for humidity sensor
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_HUM_GetOutputDataRate(float *Odr)
{
#if (defined BSP_ENV_SENSORS)
  #ifdef CUSTOM_HUM_INSTANCE_0
  (void)CUSTOM_ENV_SENSOR_GetOutputDataRate(CUSTOM_HUM_INSTANCE_0, ENV_HUMIDITY, Odr);
  #endif
#endif
}

/**
  * @brief  Set full scale for acclerometer
  * @param  Fullscale Fullscale value to be set
  * @retval None
  */
void BSP_SENSOR_ACC_SetFullScale(int32_t Fullscale)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACCELEROMETER
  (void)CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_ACCELEROMETER, MOTION_ACCELERO, Fullscale);
  #endif
#endif
}

/**
 * @brief  Enable the wake up detection
 * @param
 * @param
 * @retval
 */
void BSP_SENSOR_ACC_Enable_Wake_Up_Detection()
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACCELEROMETER
  (void)CUSTOM_MOTION_SENSOR_Enable_Wake_Up_Detection(CUSTOM_ACCELEROMETER, CUSTOM_MOTION_SENSOR_INT2_PIN);
  #endif
#endif
}

/**
  * @brief  Read event status
  * @param  CUSTOM_MOTION_SENSOR_Event_Status_t
  * @retval None
  */
void BSP_SENSOR_ACC_Get_Event_Status(CUSTOM_MOTION_SENSOR_Event_Status_t *Status)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACCELEROMETER
  (void)CUSTOM_MOTION_SENSOR_Get_Event_Status(CUSTOM_ACCELEROMETER, Status);
  #endif
#endif
}

/**
  * @brief  Set full scale for acclerometer
  * @param  Fullscale Fullscale value to be set
  * @retval None
  */
void BSP_SENSOR_LP_ACC_SetFullScale(int32_t Fullscale)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER

  (void)CUSTOM_HYBRID_MOTION_SENSOR_SetFullScale(CUSTOM_LP_ACCELEROMETER, HYBRID_ACCELERO, Fullscale);
  #endif
#endif
}

/**
 * @brief  Enable the wake up detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Enable_Wake_Up_Detection(uint32_t Instance, CUSTOM_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      /* For LIS2DW12 the Wake Up event can be routed only on INT1 pin */
      if (LIS2DTW12_ACC_Enable_Wake_Up_Detection(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the wake up detection
 * @param
 * @param
 * @retval
 */
void BSP_SENSOR_LP_ACC_Enable_Wake_Up_Detection()
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Enable_Wake_Up_Detection(CUSTOM_LP_ACCELEROMETER, CUSTOM_MOTION_SENSOR_INT1_PIN);
  #endif
#endif
}

/**
 * @brief  Enable the inactivity detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Enable_Inactivity_Detection(uint32_t Instance, CUSTOM_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      /* For LIS2DW12 the Inactivity event can be routed only on INT2 pin */
      if (LIS2DTW12_ACC_Enable_Inactivity_Detection(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the inactivity detection
 * @param
 * @param
 * @retval
 */
void BSP_SENSOR_LP_ACC_Enable_Inactivity_Detection()
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Enable_Inactivity_Detection(CUSTOM_LP_ACCELEROMETER, CUSTOM_MOTION_SENSOR_INT1_PIN);
  #endif
#endif
}


uint32_t CUSTOM_HYBRID_SENSOR_Set_Sleep_Duration(uint32_t Instance,  uint8_t Duration)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_ACC_Set_Sleep_Duration(HybridCompObj[Instance], Duration) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }
  return ret;
}

/**
 * @brief  Set sleep duration
 * @param  sleep durtion
 * @retval
 */
void BSP_SENSOR_LP_ACC_Set_Sleep_Duration( uint8_t Duration)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Set_Sleep_Duration(CUSTOM_LP_ACCELEROMETER,  Duration);
  #endif
#endif
}


/**
  * @brief  Set all int on int1
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t LIS2DTW12_ACC_Enable_All_On_INT1(LIS2DTW12_Object_t *pObj)
{
  /* Set all int on int1 */
  if (lis2dtw12_all_on_int1_set(&(pObj->Ctx), 1) != LIS2DTW12_OK)
  {
    return LIS2DTW12_ERROR;
  }
  return LIS2DTW12_OK;
}

uint32_t CUSTOM_HYBRID_SENSOR_Enable_All_On_INT1(uint32_t Instance)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_ACC_Enable_All_On_INT1(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

void BSP_SENSOR_LP_ACC_Enable_All_On_INT1()
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Enable_All_On_INT1(CUSTOM_LP_ACCELEROMETER);
  #endif
#endif
}


/**
  * @brief  Set all int on int1
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t LIS2DTW12_ACC_Disable_All_On_INT1(LIS2DTW12_Object_t *pObj)
{
  /* Set all int on int1 */
  if (lis2dtw12_all_on_int1_set(&(pObj->Ctx), 0) != LIS2DTW12_OK)
  {
    return LIS2DTW12_ERROR;
  }
  return LIS2DTW12_OK;
}

uint32_t CUSTOM_HYBRID_SENSOR_Disable_All_On_INT1(uint32_t Instance)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_ACC_Disable_All_On_INT1(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

void BSP_SENSOR_LP_ACC_Disable_All_On_INT1()
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Disable_All_On_INT1(CUSTOM_LP_ACCELEROMETER);
  #endif
#endif
}


/**
 * @brief  Disable the wake up detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Disable_Wake_Up_Detection(uint32_t Instance, CUSTOM_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      /* For LIS2DW12 the Wake Up event can be routed only on INT1 pin */
      if (LIS2DTW12_ACC_Disable_Wake_Up_Detection(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the wake up detection
 * @param
 * @param
 * @retval
 */
void BSP_SENSOR_LP_ACC_Disable_Wake_Up_Detection()
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Disable_Wake_Up_Detection(CUSTOM_LP_ACCELEROMETER, CUSTOM_MOTION_SENSOR_INT1_PIN);
  #endif
#endif
}


/**
 * @brief  Diable the inactivity detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Disable_Inactivity_Detection(uint32_t Instance, CUSTOM_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      /* For LIS2DW12 the Inactivity event can be routed only on INT2 pin */
      if (LIS2DTW12_ACC_Disable_Inactivity_Detection(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Diable the inactivity detection
 * @param
 * @param
 * @retval
 */
void BSP_SENSOR_LP_ACC_Disable_Inactivity_Detection()
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Disable_Inactivity_Detection(CUSTOM_LP_ACCELEROMETER, CUSTOM_MOTION_SENSOR_INT1_PIN);
  #endif
#endif
}

/**
 * @brief  Se  Set the wake up threshold
 * @param  Instance the device instance
 * @param  status latched or pulsed
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_Set_Wake_Up_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_ACC_Set_Wake_Up_Threshold(HybridCompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the wake up threshold
 * @param  Threshold for wakeup.1 LSB = FS_XL / 64.
 * @retval
 */
void BSP_SENSOR_LP_ACC_Set_Wake_Up_Threshold( uint8_t Threshold)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Set_Wake_Up_Threshold(CUSTOM_LP_ACCELEROMETER,  Threshold);
  #endif
#endif
}

/**
 * @brief  Set the wake up Duration
 * @param  Instance the device instance
 * @param  status latched or pulsed
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_Set_Wake_Up_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_ACC_Set_Wake_Up_Duration(HybridCompObj[Instance], Duration) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the wake up threshold
 * @param   Wake up duration event.1LSb = 1 / ODR.[set]
 * @retval
 */
void BSP_SENSOR_LP_ACC_Set_Wake_Up_Duration( uint8_t Duration)
{
#if (defined BSP_HYBRID_SENSORS)
  #ifdef CUSTOM_LP_ACCELEROMETER
  (void)CUSTOM_HYBRID_SENSOR_Set_Wake_Up_Duration(CUSTOM_LP_ACCELEROMETER,  Duration);
  #endif
#endif
}

/**
 * @brief  Set Interrupt Latched
 * @param  Instance the device instance
 * @param  status latched or pulsed
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Set_Interrupt_Latch(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  extern void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

  switch (Instance)
  {

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_Set_Interrupt_Latch(HybridCompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable the inactivity detection
 * @param
 * @param
 * @retval
 */
void BSP_SENSOR_LP_ACC_Enable_Interrupt_Latch()
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_LIS2DTW12_0
  (void)CUSTOM_HYBRID_MOTION_SENSOR_Set_Interrupt_Latch(CUSTOM_LIS2DTW12_0, LIS2DTW12_INT_LATCHED);
  uint8_t wkup_src;
  (void)BSP_SENSOR_LP_ACC_Read_Register(LIS2DTW12_WAKE_UP_SRC, &wkup_src);
  #endif
#endif
}

/**
  * @brief  Set full scale for gyroscope
  * @param  Fullscale Fullscale value to be set
  * @retval None
  */
void BSP_SENSOR_GYR_SetFullScale(int32_t Fullscale)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Fullscale);
  #endif
#endif
}

/**
  * @brief  Get full scale for acclerometer
  * @param  Fullscale Fullscale value
  * @retval None
  */
void BSP_SENSOR_ACC_GetFullScale(int32_t *Fullscale)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACC_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetFullScale(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, Fullscale);
  #endif
#endif
}

/**
  * @brief  Get full scale for gyroscope
  * @param  Fullscale Fullscale value
  * @retval None
  */
void BSP_SENSOR_GYR_GetFullScale(int32_t *Fullscale)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetFullScale(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Fullscale);
  #endif
#endif
}

#endif //(defined BSP_MOTION_SENSORS)

/**
  * @} CUSTOM_MEMS_CONTROL_FunctionDefines
  */

/**
  * @} ASTRA_BSP_MAIN
  */

/**
  * @} ASTRA_BSP
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
