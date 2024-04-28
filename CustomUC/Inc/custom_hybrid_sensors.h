/**
  ******************************************************************************
  * @file    custom_hybrid_sensors.h
  * @author  MEMS Application Team
  * @brief   This file contains definitions for the BSP Hybrid Sensors interface for custom boards
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_HYBRID_SENSORS_H__
#define __CUSTOM_HYBRID_SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "custom_mems_conf.h"
#include "hybrid_sensor.h"

#ifndef USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0
#define USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0  1
#endif

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
#include "lis2dtw12.h"
#endif

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
#define CUSTOM_LIS2DTW12_0 (0)
#endif

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} CUSTOM_HYBRID_MOTION_SENSOR_Axes_t;

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} CUSTOM_HYBRID_MOTION_SENSOR_AxesRaw_t;

/* Hybrid Sensor instance Info */
typedef struct
{
  uint8_t  Acc;
  uint8_t  Gyro;
  uint8_t  Magneto;
  uint8_t  Temperature;
  uint8_t  Pressure;
  uint8_t  Humidity;
  uint8_t  LowPower;
  uint32_t GyroMaxFS;
  uint32_t AccMaxFS;
  uint32_t MagMaxFS;
  float    GyroMaxOdr;
  float    AccMaxOdr;
  float    MagMaxOdr;
  float    HumMaxOdr;
  float    TempMaxOdr;
  float    PressMaxOdr;
} CUSTOM_HYBRID_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} CUSTOM_HYBRID_MOTION_SENSOR_Ctx_t;

typedef struct
{
  uint32_t Functions;
} CUSTOM_HYBRID_ENV_SENSOR_Ctx_t;

#define CUSTOM_HYBRID_INSTANCES_NBR    (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0)

#ifndef HYBRID_GYRO
#define HYBRID_GYRO             1U
#endif
#ifndef HYBRID_ACCELERO
#define HYBRID_ACCELERO         2U
#endif
#ifndef HYBRID_MAGNETO
#define HYBRID_MAGNETO          4U
#endif
#ifndef HYBRID_MOTION_FUNC
#define HYBRID_MOTION_FUNC  (HYBRID_GYRO | HYBRID_ACCELERO | HYBRID_MAGNETO)
#endif

#define HYBRID_MOTION_FUNC_ID(f) \
       ((f) & HYBRID_GYRO)       ? 0 \
  :    ((f) & HYBRID_ACCELERO)   ? 1 \
  : /* ((f) & HYBRID_MAGNETO) */   2

#define CUSTOM_HYBRID_MOTION_FUNCTIONS_NBR    3U

#ifndef HYBRID_TEMPERATURE
#define HYBRID_TEMPERATURE  (uint32_t)0x10U
#endif
#ifndef HYBRID_PRESSURE
#define HYBRID_PRESSURE     (uint32_t)0x20U
#endif
#ifndef HYBRID_HUMIDITY
#define HYBRID_HUMIDITY     (uint32_t)0x40U
#endif
#ifndef HYBRID_ENV_FUNC
#define HYBRID_ENV_FUNC     (HYBRID_TEMPERATURE | HYBRID_PRESSURE | HYBRID_HUMIDITY)
#endif

#define HYBRID_ENV_FUNC_ID(f) \
       ((f) & HYBRID_TEMPERATURE) ? 0 \
  :    ((f) & HYBRID_PRESSURE)    ? 1 \
  : /* ((f) & HYBRID_HUMIDITY) */   2

#define CUSTOM_HYBRID_ENV_FUNCTIONS_NBR  3U

int32_t CUSTOM_HYBRID_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t CUSTOM_HYBRID_SENSOR_DeInit(uint32_t Instance);
int32_t CUSTOM_HYBRID_SENSOR_GetCapabilities(uint32_t Instance, CUSTOM_HYBRID_SENSOR_Capabilities_t *Capabilities);
int32_t CUSTOM_HYBRID_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, CUSTOM_HYBRID_MOTION_SENSOR_Axes_t *Axes);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, CUSTOM_HYBRID_MOTION_SENSOR_AxesRaw_t *Axes);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float *Sensitivity);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale);
int32_t CUSTOM_HYBRID_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale);
int32_t CUSTOM_HYBRID_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t CUSTOM_HYBRID_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t CUSTOM_HYBRID_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t CUSTOM_HYBRID_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t CUSTOM_HYBRID_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value);

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_HYBRID_SENSORS_H__ */

