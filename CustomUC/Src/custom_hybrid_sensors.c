/**
  ******************************************************************************
  * @file    custom_hybrid_sensors.c
  * @author  MEMS Application Team
  * @brief   This file provides BSP Hybrid Sensors interface for custom boards
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
#include "custom_hybrid_sensors.h"

void *HybridCompObj[CUSTOM_HYBRID_INSTANCES_NBR];

static HYBRID_ENV_SENSOR_FuncDrv_t        *HybridEnvFuncDrv[CUSTOM_HYBRID_INSTANCES_NBR][CUSTOM_HYBRID_ENV_FUNCTIONS_NBR];
static HYBRID_MOTION_SENSOR_FuncDrv_t     *HybridMotionFuncDrv[CUSTOM_HYBRID_INSTANCES_NBR][CUSTOM_HYBRID_MOTION_FUNCTIONS_NBR];
static HYBRID_SENSOR_CommonDrv_t          *HybridDrv[CUSTOM_HYBRID_INSTANCES_NBR];
static CUSTOM_HYBRID_ENV_SENSOR_Ctx_t      HybridEnvCtx[CUSTOM_HYBRID_INSTANCES_NBR];
static CUSTOM_HYBRID_MOTION_SENSOR_Ctx_t   HybridMotionCtx[CUSTOM_HYBRID_INSTANCES_NBR];

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
static int32_t LIS2DTW12_0_Probe(uint32_t Functions);
#endif

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
static int32_t CUSTOM_LIS2DTW12_0_Init(void);
static int32_t CUSTOM_LIS2DTW12_0_DeInit(void);
static int32_t CUSTOM_LIS2DTW12_0_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t CUSTOM_LIS2DTW12_0_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

/**
 * @brief  Initialize the hybrid sensor
 * @param  Instance Hybrid sensor instance
 * @param  Functions Hybrid sensor functions. Must be combination of motion function:
 *         - HYBRID_ACCELERO
 *         - HYBRID_GYRO
 *         - HYBRID_MAGNETO
 *
 *         and environmental function:
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 *
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  uint32_t i;
  uint32_t function = 0;

  switch (Instance)
  {
#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
    case CUSTOM_LIS2DTW12_0:
      if (LIS2DTW12_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }

      break;
#endif

    default:
      return BSP_ERROR_WRONG_PARAM;
  }

  for (i = 0; i < CUSTOM_HYBRID_ENV_FUNCTIONS_NBR; i++)
  {
    function = HYBRID_TEMPERATURE << i;
    if ((Functions & HybridEnvCtx[Instance].Functions & function) == function)
    {
      if (HybridEnvFuncDrv[Instance][i]->Enable(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  for (i = 0; i < CUSTOM_HYBRID_MOTION_FUNCTIONS_NBR; i++)
  {
    function = HYBRID_GYRO << i;
    if ((Functions & HybridMotionCtx[Instance].Functions & function) == function)
    {
      if (HybridMotionFuncDrv[Instance][i]->Enable(HybridCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Deinitialize hybrid sensor
 * @param  Instance Hybrid sensor instance
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_DeInit(uint32_t Instance)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridDrv[Instance]->DeInit(HybridCompObj[Instance]) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor instance capabilities
 * @param  Instance Hybrid sensor instance
 * @param  Capabilities pointer to hybrid sensor capabilities
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_GetCapabilities(uint32_t Instance, CUSTOM_HYBRID_SENSOR_Capabilities_t *Capabilities)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridDrv[Instance]->GetCapabilities(HybridCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_COMPONENT;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get WHOAMI value
 * @param  Instance Hybrid sensor instance
 * @param  Id WHOAMI value
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridDrv[Instance]->ReadID(HybridCompObj[Instance], Id) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_COMPONENT;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Enable Hybrid sensor - motion part
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->Enable(HybridCompObj[Instance]) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Disable Hybrid sensor - motion part
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->Disable(HybridCompObj[Instance]) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor axes data - motion part only
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Axes pointer to axes data structure
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, CUSTOM_HYBRID_MOTION_SENSOR_Axes_t *Axes)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->GetAxes(HybridCompObj[Instance], Axes) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor axes raw data - motion part only
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Axes pointer to axes raw data structure
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, CUSTOM_HYBRID_MOTION_SENSOR_AxesRaw_t *Axes)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->GetAxesRaw(HybridCompObj[Instance], Axes) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor sensitivity - motion part only
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Sensitivity pointer to sensitivity read value
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float *Sensitivity)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->GetSensitivity(HybridCompObj[Instance], Sensitivity) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor Output Data Rate - motion part
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Odr pointer to Output Data Rate read value
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->GetOutputDataRate(HybridCompObj[Instance], Odr) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Set hybrid sensor Output Data Rate - motion part
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Odr Output Data Rate value to be set
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->SetOutputDataRate(HybridCompObj[Instance], Odr) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor Full Scale - motion part only
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Fullscale pointer to Fullscale read value
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->GetFullScale(HybridCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Set hybrid sensor Full Scale - motion part only
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor motion function. Could be :
 *         - HYBRID_GYRO
 *         - HYBRID_ACCELERO
 *         - HYBRID_MAGNETO
 * @param  Fullscale Fullscale value to be set
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridMotionCtx[Instance].Functions & HYBRID_MOTION_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridMotionFuncDrv[Instance][HYBRID_MOTION_FUNC_ID(Function)]->SetFullScale(HybridCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Enable Hybrid sensor - environmental part
 * @param  Instance Hybrid sensor instance
 * @param  Function Hybrid sensor environmental function. Could be :
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridEnvCtx[Instance].Functions & HYBRID_ENV_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridEnvFuncDrv[Instance][HYBRID_ENV_FUNC_ID(Function)]->Enable(HybridCompObj[Instance]) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Disable Hybrid sensor - environmental part
 * @param  Instance Hybrid sensor instance
 * @param  Functions Hybrid sensor environmental functions. Could be :
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridEnvCtx[Instance].Functions & HYBRID_ENV_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridEnvFuncDrv[Instance][HYBRID_ENV_FUNC_ID(Function)]->Disable(HybridCompObj[Instance]) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor Output Data Rate - environmental part
 * @param  Instance Hybrid sensor instance
 * @param  Functions Hybrid sensor environmental functions. Could be :
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 * @param  Odr pointer to Output Data Rate read value
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridEnvCtx[Instance].Functions & HYBRID_ENV_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridEnvFuncDrv[Instance][HYBRID_ENV_FUNC_ID(Function)]->GetOutputDataRate(HybridCompObj[Instance], Odr) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Set hybrid sensor Output Data Rate - environmental part
 * @param  Instance Hybrid sensor instance
 * @param  Functions Hybrid sensor environmental functions. Could be :
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 * @param  Odr Output Data Rate value to be set
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridEnvCtx[Instance].Functions & HYBRID_ENV_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridEnvFuncDrv[Instance][HYBRID_ENV_FUNC_ID(Function)]->SetOutputDataRate(HybridCompObj[Instance], Odr) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get hybrid sensor value - environmental part only
 * @param  Instance hybrid sensor instance
 * @param  Function Hybrid sensor environmental function. Could be :
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 * @param  Value pointer to hybrid sensor value
 * @retval BSP status
 */
int32_t CUSTOM_HYBRID_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value)
{
  if (Instance >= CUSTOM_HYBRID_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((HybridEnvCtx[Instance].Functions & HYBRID_ENV_FUNC & Function) != Function)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (HybridEnvFuncDrv[Instance][HYBRID_ENV_FUNC_ID(Function)]->GetValue(HybridCompObj[Instance], Value) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

#if (USE_CUSTOM_HYBRID_SENSOR_LIS2DTW12_0 == 1)
/**
 * @brief  Register Bus IOs for LIS2DTW12 instance
 * @param  Functions Hybrid sensor functions. Must be combination of motion function:
 *         - HYBRID_ACCELERO
 *         - HYBRID_GYRO
 *         - HYBRID_MAGNETO
 *
 *         and environmental function:
 *         - HYBRID_HUMIDITY
 *         - HYBRID_PRESSURE
 *         - HYBRID_TEMPERATURE
 *
 * @retval BSP status
 */
static int32_t LIS2DTW12_0_Probe(uint32_t Functions)
{
  LIS2DTW12_IO_t            io_ctx;
  uint8_t                   id;
  static LIS2DTW12_Object_t lis2dtw12_obj_0;
  LIS2DTW12_Capabilities_t  cap;

  /* Configure the hybrid sensor driver */
  io_ctx.BusType  = LIS2DTW12_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address  = 0x00;
  io_ctx.Init     = CUSTOM_LIS2DTW12_0_Init;
  io_ctx.DeInit   = CUSTOM_LIS2DTW12_0_DeInit;
  io_ctx.ReadReg  = CUSTOM_LIS2DTW12_0_ReadReg;
  io_ctx.WriteReg = CUSTOM_LIS2DTW12_0_WriteReg;
  io_ctx.GetTick  = BSP_GetTick;

  if (LIS2DTW12_RegisterBusIO(&lis2dtw12_obj_0, &io_ctx) != LIS2DTW12_OK)
  {
    return BSP_ERROR_UNKNOWN_COMPONENT;
  }

  if (LIS2DTW12_ReadID(&lis2dtw12_obj_0, &id) != LIS2DTW12_OK)
  {
    return BSP_ERROR_UNKNOWN_COMPONENT;
  }

  if (id != LIS2DTW12_ID)
  {
    return BSP_ERROR_UNKNOWN_COMPONENT;
  }

  if (LIS2DTW12_GetCapabilities(&lis2dtw12_obj_0, &cap) != LIS2DTW12_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  HybridMotionCtx[CUSTOM_LIS2DTW12_0].Functions = ((cap.Acc     == 1U) ? HYBRID_ACCELERO : 0)
                                                | ((cap.Gyro    == 1U) ? HYBRID_GYRO     : 0)
                                                | ((cap.Magneto == 1U) ? HYBRID_MAGNETO  : 0);

  HybridEnvCtx[CUSTOM_LIS2DTW12_0].Functions = ((cap.Temperature == 1U) ? HYBRID_TEMPERATURE : 0)
                                             | ((cap.Humidity    == 1U) ? HYBRID_HUMIDITY    : 0)
                                             | ((cap.Pressure    == 1U) ? HYBRID_PRESSURE    : 0);

  HybridCompObj[CUSTOM_LIS2DTW12_0] = &lis2dtw12_obj_0;

  /* The second cast (void *) is added to bypass Misra R11.3 rule */
  HybridDrv[CUSTOM_LIS2DTW12_0] = (HYBRID_SENSOR_CommonDrv_t *)(void *)&LIS2DTW12_COMMON_Driver;

  if ((Functions & HybridMotionCtx[CUSTOM_LIS2DTW12_0].Functions & HYBRID_ACCELERO) == HYBRID_ACCELERO)
  {
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    HybridMotionFuncDrv[CUSTOM_LIS2DTW12_0][HYBRID_MOTION_FUNC_ID(HYBRID_ACCELERO)] = (HYBRID_MOTION_SENSOR_FuncDrv_t *)(void *)&LIS2DTW12_ACC_Driver;

    if (HybridDrv[CUSTOM_LIS2DTW12_0]->Init(HybridCompObj[CUSTOM_LIS2DTW12_0]) != LIS2DTW12_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  if ((Functions & HybridMotionCtx[CUSTOM_LIS2DTW12_0].Functions & HYBRID_GYRO) == HYBRID_GYRO)
  {
    /* Function not supported by the component */
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((Functions & HybridMotionCtx[CUSTOM_LIS2DTW12_0].Functions & HYBRID_MAGNETO) == HYBRID_MAGNETO)
  {
    /* Function not supported by the component */
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((Functions & HybridEnvCtx[CUSTOM_LIS2DTW12_0].Functions & HYBRID_TEMPERATURE) == HYBRID_TEMPERATURE)
  {
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    HybridEnvFuncDrv[CUSTOM_LIS2DTW12_0][HYBRID_ENV_FUNC_ID(HYBRID_TEMPERATURE)] = (HYBRID_ENV_SENSOR_FuncDrv_t *)(void *)&LIS2DTW12_TEMP_Driver;

    if (HybridDrv[CUSTOM_LIS2DTW12_0]->Init(HybridCompObj[CUSTOM_LIS2DTW12_0]) != LIS2DTW12_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  if ((Functions & HybridEnvCtx[CUSTOM_LIS2DTW12_0].Functions & HYBRID_HUMIDITY) == HYBRID_HUMIDITY)
  {
    /* Function not supported by the component */
    return BSP_ERROR_WRONG_PARAM;
  }

  if ((Functions & HybridEnvCtx[CUSTOM_LIS2DTW12_0].Functions & HYBRID_PRESSURE) == HYBRID_PRESSURE)
  {
    /* Function not supported by the component */
    return BSP_ERROR_WRONG_PARAM;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Initialize SPI bus for LIS2DTW12 instance
 * @retval BSP status
 */
static int32_t CUSTOM_LIS2DTW12_0_Init(void)
{
  if (CUSTOM_LIS2DTW12_0_SPI_Init() != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInitialize SPI bus for LIS2DTW12 instance
 * @retval BSP status
 */
static int32_t CUSTOM_LIS2DTW12_0_DeInit(void)
{
  if (CUSTOM_LIS2DTW12_0_SPI_DeInit() == BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Write register by SPI bus for LIS2DTW12 instance
 * @param  addr  not used, it is only for BSP compatibility
 * @param  reg   the starting register address to be written
 * @param  pdata the pointer to the data to be written
 * @param  len   the length of the data to be written
 * @retval BSP status
 */
static int32_t CUSTOM_LIS2DTW12_0_WriteReg(uint16_t addr, uint16_t reg, uint8_t *pdata, uint16_t len)
{
  uint8_t dataReg = (uint8_t)reg;

  /* CS Enable */
  HAL_GPIO_WritePin(CUSTOM_LIS2DTW12_0_CS_PORT, CUSTOM_LIS2DTW12_0_CS_PIN, GPIO_PIN_RESET);

  if (CUSTOM_LIS2DTW12_0_SPI_Send(&dataReg, 1) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_FAILURE;
  }

  if (CUSTOM_LIS2DTW12_0_SPI_Send(pdata, len) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* CS Disable */
  HAL_GPIO_WritePin(CUSTOM_LIS2DTW12_0_CS_PORT, CUSTOM_LIS2DTW12_0_CS_PIN, GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Read register by SPI bus for LIS2DTW12 instance
 * @param  addr  not used, it is only for BSP compatibility
 * @param  reg   the starting register address to be read
 * @param  pdata the pointer to the data to be read
 * @param  len   the length of the data to be read
 * @retval BSP status
 */
static int32_t CUSTOM_LIS2DTW12_0_ReadReg(uint16_t addr, uint16_t reg, uint8_t *pdata, uint16_t len)
{
  uint8_t dataReg = (uint8_t)reg;

  dataReg |= 0x80;

  /* CS Enable */
  HAL_GPIO_WritePin(CUSTOM_LIS2DTW12_0_CS_PORT, CUSTOM_LIS2DTW12_0_CS_PIN, GPIO_PIN_RESET);

  if (CUSTOM_LIS2DTW12_0_SPI_Send(&dataReg, 1) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_FAILURE;
  }

  if (CUSTOM_LIS2DTW12_0_SPI_Recv(pdata, len) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* CS Disable */
  HAL_GPIO_WritePin(CUSTOM_LIS2DTW12_0_CS_PORT, CUSTOM_LIS2DTW12_0_CS_PIN, GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}
#endif

