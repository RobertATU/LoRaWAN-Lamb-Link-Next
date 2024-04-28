/**
  ******************************************************************************
  * @file    SmartNFC.h
  * @author  System Research & Applications Team - Catania & Agrate Lab.
  * @version 1.0.0
  * @brief   Smart NFC protocol 
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

/** @addtogroup ASTRA_ENGINE ASTRA ENGINE
  * @{
  */

/** @addtogroup NFC_APPLICATION NFC application
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMARTNFC_H
#define __SMARTNFC_H

/* Includes ------------------------------------------------------------------*/
#include "SmartNFCType.h"
#include "SmartNFC_config.h"

#ifdef __cplusplus
 extern "C" {
#endif

   
/** @addtogroup ASTRA_NFC_Defines ASTRA NFC - Defines
  * @brief Defines
  * @{
 */  
   
/* Exported Defines ----------------------------------------------------------*/

/* Dimension of the CC file in bytes */
#define ST25DV_CC_SIZE          8

/* Number of bytes for TL in the NDEF message */
#define ST25DV_TL_SIZE         0x04
   
/* Dimension of the NDEF records in bytes */
#define ST25DV_NDEF1_HEADER_SIZE        0x04
#define ST25DV_NDEF1_PAYLOAD_SIZE       0x40
#define ST25DV_NDEF1_SIZE               (ST25DV_NDEF1_HEADER_SIZE + ST25DV_NDEF1_PAYLOAD_SIZE)
#define ST25DV_NDEF2_HEADER_SIZE        0x0F
#define ST25DV_NDEF2_PAYLOAD_SIZE       0xD9
#define ST25DV_NDEF2_SIZE               (ST25DV_NDEF2_HEADER_SIZE + ST25DV_NDEF2_PAYLOAD_SIZE)
#define SMARTNFC_UID_EXTRA_LENGHT       0x0F
#define ST25DV_NDEF3_HEADER_SIZE        (0x15 + SMARTNFC_UID_EXTRA_LENGHT)
#define ST25DV_NDEF3_PAYLOAD_SIZE       0xF0

/* Dimension of the NDEF message payload */
#define ST25DV_TLV_PAYLOAD_SIZE         (ST25DV_NDEF1_SIZE + ST25DV_NDEF2_SIZE + ST25DV_NDEF3_HEADER_SIZE + ST25DV_NDEF3_PAYLOAD_SIZE)  /* It must be between 0x00FF and 0xFFFE */          

/* Address due to extended CC file + NDEFs and last NDEF header before payload */
#define SMARTNFC_MAC_ADDR_OFFSET        (ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_NDEF1_HEADER_SIZE)
#define ST25DV_ASTRA_CONF_MEMORY_OFFSET (ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_NDEF1_SIZE + ST25DV_NDEF2_HEADER_SIZE + 0x01)
#define SMARTNFC_START_ADDR_OFFSET      (ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_NDEF1_SIZE + ST25DV_NDEF2_SIZE + ST25DV_NDEF3_HEADER_SIZE )
#define SMARTNFC_SAVING_TIME_OFFSET     (ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_NDEF1_SIZE + ST25DV_NDEF2_SIZE + ST25DV_NDEF3_HEADER_SIZE + 6)

#define STNFC_Error_Handler(ErrorCode) STNFC_Error(ErrorCode,__FILE__, __LINE__)

/**
* @} ASTRA_NFC_Defines
*/

/** @addtogroup ASTRA_NFC_Macros ASTRA NFC - Macros
  * @brief Macros
  * @{
 */
   
/* Exported Macros -----------------------------------------------------------*/
#define MCR_STNFC_CompareWithLimits(Type,VirtualSensor,value)\
{\
  /* Limit the Value */\
  if(value>=VirtualSensor.MaxLimit.Type##Value) {\
    value=VirtualSensor.MaxLimit.Type##Value;\
  }\
  \
  if(value<=VirtualSensor.MinLimit.Type##Value) {\
    value=VirtualSensor.MinLimit.Type##Value;\
  }\
  /* Save the Value */\
  VirtualSensor.Sample.Type##Value = value;\
}

/**
* @} ASTRA_NFC_Macros
*/


/** @addtogroup ASTRA_NFC_VARIABLES ASTRA NFC - Variables
  * @brief Variables
  * @{
 */

/* Exported Variables --------------------------------------------------------*/
extern char *ThresholdsUsageName[4];

/**
* @} ASTRA_NFC_VARIABLES
*/


/** @addtogroup ASTRA_NFC_FUNCTION_PROTOTYPES ASTRA NFC - Function prototypes
  * @brief Function prototype
  * @{
 */  

/* Exported Prototypes -------------------------------------------------------*/

/**
  * @brief  Error Handler for Reading/Writing function for NFC.
  *         User may add here some code to deal with this error.
  * @param  SNFC_ErrorCode_t ErroCode Error Code Flag: Reading/Writing/Configuration
  * @retval None
  */
extern void STNFC_Error(SNFC_ErrorCode_t ErroCode, char *file, int32_t line);


/**
  * @brief  Initialize the ST-Smart NFC
  * @param  None
  * @retval None
  */
extern void InitSTSmartNFC(void);


/**
  * @brief  Set the data and time value.
  * @param  uint32_t DateTime Compressed Date&Time 
  * @param  RTC_HandleTypeDef *hrtc RTC handler pointer 
  * @param SNFC_LogDefinition_t *LogDef Pointer to Log definition
  * @retval Returns 1 in case of success, otherwise 0 on error..
  */
extern int32_t  STNFC_SetDateTime(uint32_t DateTime,RTC_HandleTypeDef *hrtc,SNFC_LogDefinition_t *LogDef);

/**
  * @brief  Get the Delta epoch time.
  * @param  RTC_HandleTypeDef *hrtc RTC handler pointer 
  * @param SNFC_LogDefinition_t *LogDef Pointer to Log definition
  * @retval Returns Delta epoch time in case of success, otherwise 0 on error.
  */
extern time_t STNFC_GetDeltaDateTime(RTC_HandleTypeDef *hrtc,SNFC_LogDefinition_t *LogDef);

/**
  * @brief  From Delta epoch time to Short Delta epoch time (without Seconds)
  * @param  time_t DeltaDateTimeStamp Delta epoch time stamp
  * @retval Returns Short Delta epoch time stamp
  */
extern uint32_t STNFC_ToShortDeltaDateTime(time_t DeltaDateTimeStamp);

/**
  * @brief  Set sample time (without Seconds)
  * @param  time_t SampleTime Sample time
  * @retval Returns ok
  */
extern int32_t STNFC_SetSampleTime(uint16_t SampleTime);

/**
  * @brief  Compare current value to thresholdes upgrading max min values
  * @param  VirtualSensor Pointer to virtual sensor struct 
  * @param  LogDefinition Pointer to log definition struct 
  * @param  hrtc Pointer to RTC struct 
  * @retval none
  */
extern void STNFC_ComputeMaxMinCompareTHsUi8t(SNFC_VirtualSensor_t *VirtualSensor,SNFC_LogDefinition_t *LogDefinition,RTC_HandleTypeDef *hrtc);

/**
  * @brief  Compare current value to thresholdes upgrading max min values
  * @param  VirtualSensor Pointer to virtual sensor struct 
  * @param  LogDefinition Pointer to log definition struct 
  * @param  hrtc Pointer to RTC struct 
  * @retval none
  */
extern void STNFC_ComputeMaxMinCompareTHsUi16t(SNFC_VirtualSensor_t *VirtualSensor,SNFC_LogDefinition_t *LogDefinition,RTC_HandleTypeDef *hrtc);

/**
  * @brief  Compare current value to thresholdes upgrading max min values
  * @param  VirtualSensor Pointer to virtual sensor struct 
  * @param  LogDefinition Pointer to log definition struct 
  * @param  hrtc Pointer to RTC struct 
  * @retval none
  */
extern void STNFC_ComputeMaxMinCompareTHsUi32t(SNFC_VirtualSensor_t *VirtualSensor,SNFC_LogDefinition_t *LogDefinition,RTC_HandleTypeDef *hrtc);

/**
* @} ASTRA_NFC_FUNCTION_PROTOTYPES
*/


#ifdef __cplusplus
}
#endif

#endif /* __SMARTNFC_H */


/**
* @} NFC_APPLICATION
*/

/**
* @} ASTRA_ENGINE
*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
