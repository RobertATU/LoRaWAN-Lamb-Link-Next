/**
  ******************************************************************************
  * @file    custom_hybrid_sensors_ex.c
  * @author  MEMS Application Team
  * @brief   This file provides BSP Hybrid Sensors Extended interface for custom boards
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

/* Includes ------------------------------------------------------------------*/
#include "custom_hybrid_sensors_ex.h"

extern void *HybridCompObj[];

/**
 * @brief  Get the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data pointer where the value is written to
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data)
{
  switch (Instance)
  {
#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_Read_Reg(HybridCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }

      break;
#endif

    default:
      return BSP_ERROR_WRONG_PARAM;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Set the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data value to be written
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data)
{
  switch (Instance)
  {
#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_Write_Reg(HybridCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }

      break;
#endif

    default:
      return BSP_ERROR_WRONG_PARAM;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get the status of data ready bit
 * @param  Instance the device instance
 * @param  Function Hybrid sensor function
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status)
{
  int32_t ret;

  switch (Instance)
  {
#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if ((Function & HYBRID_ACCELERO) == HYBRID_ACCELERO || (Function & HYBRID_TEMPERATURE) == HYBRID_TEMPERATURE)
      {
        if (LIS2DTW12_ACC_Get_DRDY_Status(HybridCompObj[Instance], Status) != BSP_ERROR_NONE)
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
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

