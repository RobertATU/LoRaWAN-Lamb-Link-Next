/**
  ******************************************************************************
  * @file    custom_hybrid_sensors_ex.h
  * @author  MEMS Application Team
  * @brief   This file contains definitions for the BSP Hybrid Sensors Extended interface for custom boards
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
#ifndef __CUSTOM_HYBRID_SENSORS_EX_H__
#define __CUSTOM_HYBRID_SENSORS_EX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "custom_hybrid_sensors.h"

typedef enum
{
  CUSTOM_HYBRID_SENSOR_INT1_PIN = 0,
  CUSTOM_HYBRID_SENSOR_INT2_PIN
} CUSTOM_HYBRID_SENSOR_IntPin_t;

typedef struct
{
  unsigned int FreeFallStatus : 1;
  unsigned int TapStatus : 1;
  unsigned int DoubleTapStatus : 1;
  unsigned int WakeUpStatus : 1;
  unsigned int StepStatus : 1;
  unsigned int TiltStatus : 1;
  unsigned int D6DOrientationStatus : 1;
  unsigned int SleepStatus : 1;
} CUSTOM_HYBRID_SENSOR_Event_Status_t;

int32_t CUSTOM_HYBRID_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data);
int32_t CUSTOM_HYBRID_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data);
int32_t CUSTOM_HYBRID_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status);

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_HYBRID_SENSORS_EX_H__ */

