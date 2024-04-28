/**
  ******************************************************************************
  * @file    SmartNFC.c
  * @author  System Research & Applications Team - Catania & Agrate Lab.
  * @version 1.0.0
  * @brief   Minimal NDEF header APIs implementation
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

/* Includes ------------------------------------------------------------------*/
#include "SmartNFC.h"
#include "TagType5.h"

/* Define  -------------------------------------------------------------------*/

/* Uncomment the following define for saving the Max & Min
 * only if the measure in inside the Valid Range that we want to control
 * Otherwise save the Max and Min for any read sensor's value */
//#define STNFC_MAX_MIN_ONLY_ON_VALID_RANGE


/** @addtogroup ASTRA_NFC_Private_Macros ASTRA NFC - Private macros
  * @brief Private macros
  * @{
 */

/* Macros  -------------------------------------------------------------------*/

#ifdef STNFC_MAX_MIN_ONLY_ON_VALID_RANGE
// update the Max/Min only for value that are inside the log interval
#define MCR_STNFC_ComputeMaxMinCompareTHs(Type)\
void STNFC_ComputeMaxMinCompareTHs##Type##t(SNFC_VirtualSensor_t *VirtualSensor,SNFC_LogDefinition_t *LogDefinition,RTC_HandleTypeDef *hrtc)\
{\
  time_t sampletime = 0;\
  sampletime = STNFC_GetDeltaDateTime(hrtc,LogDefinition);\
  /* Compare with Ths */\
  switch(VirtualSensor->ThsUsageType) {\
    case TH_EXT:\
      /* External Range: -->TH1 TH2<-- */\
      if((VirtualSensor->Sample.Type##Value<=VirtualSensor->Th1.Type##Value) | \
         (VirtualSensor->Sample.Type##Value>=VirtualSensor->Th2.Type##Value)) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
    break;\
    case TH_INT:\
      /* Internal Range: TH1<---->TH2 */\
      if((VirtualSensor->Sample.Type##Value>=VirtualSensor->Th1.Type##Value) & \
          (VirtualSensor->Sample.Type##Value<=VirtualSensor->Th2.Type##Value)) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
    break;\
    case TH_LESS:\
      /* Less than     : -->TH1 */\
      if(VirtualSensor->Sample.Type##Value<=VirtualSensor->Th1.Type##Value) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
    break;\
   case TH_BIGGER:\
      /* Bigger than   : TH1<-- */\
      if(VirtualSensor->Sample.Type##Value>=VirtualSensor->Th1.Type##Value) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
  }\
  \
  /* Compare with Max Min */\
  if(VirtualSensor->SampleDeltaDateTime!=0) {\
    if(VirtualSensor->Sample.Type##Value>VirtualSensor->MaxValue.Type##Value) {\
      VirtualSensor->MaxValue.Type##Value = VirtualSensor->Sample.Type##Value;\
      VirtualSensor->MaxDeltaDateTime = sampletime;\
    }\
    \
    if(VirtualSensor->Sample.Type##Value<VirtualSensor->MinValue.Type##Value) {\
      VirtualSensor->MinValue.Type##Value = VirtualSensor->Sample.Type##Value;\
      VirtualSensor->MinDeltaDateTime = sampletime;\
    }\
  }\
}
#else /* STNFC_MAX_MIN_ONLY_ON_VALID_RANGE */
//update the Max/Min for any valid value
#define MCR_STNFC_ComputeMaxMinCompareTHs(Type)\
void STNFC_ComputeMaxMinCompareTHs##Type##t(SNFC_VirtualSensor_t *VirtualSensor,SNFC_LogDefinition_t *LogDefinition,RTC_HandleTypeDef *hrtc)\
{\
  time_t sampletime = 0;\
  sampletime = STNFC_GetDeltaDateTime(hrtc,LogDefinition);\
  \
  /* Compare with Ths */\
  switch(VirtualSensor->ThsUsageType) {\
    case TH_EXT:\
      /* External Range: -->TH1 TH2<-- */\
      if((VirtualSensor->Sample.Type##Value<=VirtualSensor->Th1.Type##Value) | \
         (VirtualSensor->Sample.Type##Value>=VirtualSensor->Th2.Type##Value)) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
    break;\
    case TH_INT:\
      /* Internal Range: TH1<---->TH2 */\
      if((VirtualSensor->Sample.Type##Value>=VirtualSensor->Th1.Type##Value) & \
          (VirtualSensor->Sample.Type##Value<=VirtualSensor->Th2.Type##Value)) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
    break;\
    case TH_LESS:\
      /* Less than     : -->TH1 */\
      if(VirtualSensor->Sample.Type##Value<=VirtualSensor->Th1.Type##Value) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
    break;\
   case TH_BIGGER:\
      /* Bigger than   : TH1<-- */\
      if(VirtualSensor->Sample.Type##Value>=VirtualSensor->Th1.Type##Value) {\
        VirtualSensor->SampleDeltaDateTime = sampletime;\
      }\
  }\
\
  /* Compare with Max Min */\
  if(VirtualSensor->Sample.Type##Value>VirtualSensor->MaxValue.Type##Value) {\
    VirtualSensor->MaxValue.Type##Value = VirtualSensor->Sample.Type##Value;\
    VirtualSensor->MaxDeltaDateTime = sampletime;\
  }\
\
  if(VirtualSensor->Sample.Type##Value<VirtualSensor->MinValue.Type##Value) {\
    VirtualSensor->MinValue.Type##Value = VirtualSensor->Sample.Type##Value;\
    VirtualSensor->MinDeltaDateTime = sampletime;\
  }\
}
#endif /* STNFC_MAX_MIN_ONLY_ON_VALID_RANGE */

/**
* @} ASTRA_NFC_Private_Macros
*/

/** @addtogroup ASTRA_NFC_VARIABLES ASTRA NFC - Variables
  * @brief Variables
  * @{
 */

/* Exported Variables -------------------------------------------------------- */
char *ThresholdsUsageName[4] = {
  "Ext",
  "Int",
  "Less",
  "Bigger"
};

/**
* @} ASTRA_NFC_VARIABLES
*/


/** @addtogroup ASTRA_NFC_FUNCTIONS_DEFINITION ASTRA NFC - Function definitions
  * @brief Function definitions
  * @{
 */

/* Functions Definition ------------------------------------------------------*/


//Functions For making the Comparison with the Thresholds
MCR_STNFC_ComputeMaxMinCompareTHs(Ui8)
MCR_STNFC_ComputeMaxMinCompareTHs(Ui16)
MCR_STNFC_ComputeMaxMinCompareTHs(Ui32)



void InitSTSmartNFC(void)
{
  NfcType5_NDEFInitHeader();
}


int32_t STNFC_SetDateTime(uint32_t DateTime,RTC_HandleTypeDef *hrtc,SNFC_LogDefinition_t *LogDef)
{
  RTC_TimeTypeDef StartTime;  /* Starting Time */
  RTC_DateTypeDef StartDate;  /* Starting Date */
  struct tm *currTime;
  
  LogDef->StartTimeStamp = DateTime;
  currTime = localtime(&LogDef->StartTimeStamp);
  
  StartDate.Year    = currTime->tm_year;
  StartDate.Date    = currTime->tm_mday;
  StartDate.Month   = currTime->tm_mon;
  StartDate.WeekDay = currTime->tm_wday;
  StartTime.Hours   = currTime->tm_hour;
  StartTime.Minutes = currTime->tm_min;
  StartTime.Seconds = currTime->tm_sec;

  StartTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  StartTime.StoreOperation = RTC_STOREOPERATION_RESET;
  
  return (AstraSetTimeStamp(StartTime, StartDate) == 0);
}


time_t STNFC_GetDeltaDateTime(RTC_HandleTypeDef *hrtc,SNFC_LogDefinition_t *LogDef)
{
  time_t DeltaDateTimeStamp;
 
  DeltaDateTimeStamp = AstraGetTimeStamp();
  
  /* Compute the Delta Epoch Time */
  DeltaDateTimeStamp -= LogDef->StartTimeStamp;

  PRINTF_VERBOSE("Delta TS=%u\r\n",(uint32_t) DeltaDateTimeStamp);
  return DeltaDateTimeStamp;
}


uint32_t STNFC_ToShortDeltaDateTime(time_t DeltaDateTimeStamp)
{
  uint32_t ShortDeltaTimeStamp;
  
  DeltaDateTimeStamp/=60;
  //For using like Maximum 20bits
  ShortDeltaTimeStamp = ((uint32_t) DeltaDateTimeStamp)&0xFFFFF;
  
#ifdef SMARTNFC_VERBOSE_PRINTF
  PRINTF_VERBOSE("Short Delta TS=%u\r\n",ShortDeltaTimeStamp);
#endif /* SMARTNFC_VERBOSE_PRINTF */
  return ShortDeltaTimeStamp;
}


/**
  * @brief  Error Handler for Reading/Writing function for NFC.
  *         User may add here some code to deal with this error.
  * @param  SNFC_ErrorCode_t ErroCode Error Code Flag
  * @retval None
  */
__weak void STNFC_Error(SNFC_ErrorCode_t ErroCode, char *file, int32_t line)
{
#ifdef SMARTNFC_VERBOSE_PRINTF
  PRINTF_VERBOSE("%s@%d:",file,line);
#endif /* SMARTNFC_VERBOSE_PRINTF */
  switch(ErroCode) {
    case STNFC_CONFIG_ERROR:
#ifdef SMARTNFC_VERBOSE_PRINTF
      PRINTF_VERBOSE("STNFC_CONFIG_ERROR\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */
      break;
    case STNFC_WRITING_ERROR:
#ifdef SMARTNFC_VERBOSE_PRINTF
      PRINTF_VERBOSE("NSTFC_WRITING_ERROR\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */
      break;
    case STNFC_READING_ERROR:
#ifdef SMARTNFC_VERBOSE_PRINTF
      PRINTF_VERBOSE("NFC_READING_ERROR\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */
      break;
     case STNFC_INIT_ERROR:
#ifdef SMARTNFC_VERBOSE_PRINTF
      PRINTF_VERBOSE("STNFC_INIT_ERROR\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */
      break;
     case STNFC_RUNTIME_ERROR:
#ifdef SMARTNFC_VERBOSE_PRINTF
      PRINTF_VERBOSE("STNFC_RUNTIME_ERROR\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */
      break;
  }
}


/**
* @} ASTRA_NFC_FUNCTIONS_DEFINITION
*/

/**
* @} NFC_APPLICATION
*/

/**
* @} ASTRA_ENGINE
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
