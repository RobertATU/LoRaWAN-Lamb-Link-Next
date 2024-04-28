/**
******************************************************************************
* @file    astra_app_nfc.c
* @author  System Research & Applications Team - Catania & Agrate Lab.
* @version 1.0.0
* @brief   Application Process
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

#include "astra_confmng.h"

#if USE_MEMORY
#include "astra_app_nfc.h"
#include "astra_datamng.h"
#include "astra_sysmng.h"

/** @addtogroup ASTRA_NFC_Private_Macros ASTRA NFC - Private macros
  * @brief Private macros
  * @{
 */

/* Private Macros ------------------------------------------------------------*/

/* SAMPLE_TO_CODED macros*/
#define BATT_PERCENTAGE_SAMPLE_TO_CODED(Value)  ((uint16_t)((Value)*10))
#define BATT_VOLTAGE_SAMPLE_TO_CODED(Value)     ((uint16_t)(Value)/10)
#define STTS22H_SAMPLE_TO_CODED(Value)          ((uint16_t)(((Value)-(-10))*5))
#define LPS22HH_SAMPLE_TO_CODED(Value)          ((uint16_t)(((Value)-(260))*2))
#define HTS221_SAMPLE_TO_CODED(Value)           ((uint16_t)((Value)-(20)))

/* CODED_TO_SAMPLE macros */
#define BATT_PERCENTAGE_CODED_TO_SAMPLE(Value)  ((float)((Value)/10))
#define BATT_VOLTAGE_CODED_TO_SAMPLE(Value)     ((uint16_t)(Value)*10)
#define STTS22H_CODED_TO_SAMPLE(Value)          ((((float)(Value))/5)-10)
#define LPS22HH_CODED_TO_SAMPLE(Value)          ((((float)(Value))/2)+260)
#define HTS221_CODED_TO_SAMPLE(Value)           ((float)((Value)+(20)))

/**
* @} ASTRA_NFC_Private_Macros
*/

/** @addtogroup ASTRA_NFC_PRIVATE_TYPES ASTRA NFC - Private types
  * @brief Private Types
  * @{
 */

/* Private typedef -----------------------------------------------------------*/

/**
  * @brief  Valid Configuration Type
  */
typedef enum
{
  STNFC_VALID_CONFIG = 0,
  STNFC_NOT_VALID_CONFIG,
  STNFC_NOT_CHANGED_CONFIG,
  STNFC_ERROR_READING_CONFIG
} SNFC_ValidConfiguration_t;

/**
* @brief NFC Status Enumerative Type
*/
typedef enum
{
  NFC_STATUS_OFF = 0,
  NFC_STATUS_ON
} FNCStatusEnum_t;

/**
* @} ASTRA_NFC_PRIVATE_TYPES
*/

/** @addtogroup ASTRA_NFC_PRIVATE_VARIABLES ASTRA NFC - Private variables
  * @brief Private variables
  * @{
 */

/* Private variables ---------------------------------------------------------*/
static volatile FNCStatusEnum_t NFCStatus = NFC_STATUS_OFF;
static uint8_t LogMode = SMARTNFC_LOGMODE_INACTIVE;

//ST Smart NFC Protocol
SNFC_CodeDefinition_t SmartNFCCodeHeader;
SNFC_LogDefinition_t LogDefinition;
SNFC_VirtualSensor_t AllVirtualSensorsArray[SMARTNFC_VIRTUAL_SENSORS_NUM];
SNFC_VirtualSensor_t *ConfiguratedVirtualSensorsArray[SMARTNFC_VIRTUAL_SENSORS_NUM];

extern RTC_HandleTypeDef hrtc;

/**
* @} ASTRA_NFC_PRIVATE_VARIABLES
*/

/** @addtogroup ASTRA_NFC_PRIVATE_FUNCTION_PROTOTYPES ASTRA NFC - Private function prototypes
  * @brief Private function prototype
  * @{
 */

/* Private function prototypes -----------------------------------------------*/
static void SetNFCBehavior(void);
static int32_t AccNormVectorApproxEvaluator(CUSTOM_MOTION_SENSOR_Axes_t Value_XYZ_mg, uint16_t AccValueMin, uint16_t AccValueMax);

//Manage the Log configuration
static void CheckIfNewConfiguration(void);
static void ReadConfiguration(SNFC_LogDefinition_t *LogDefinition,uint32_t CurrentStartDateTime,int32_t OnlyChecks);
static void SetAllAvailableVirtualSensors(void);
static void SaveDefaultConfiguration(void);
static void ResetMaxMinValuesAllVirtualSensors(void);
static void SaveMaxMinValuesForVirtualSensors(void);
static void SaveVirtualSensorsConfiguration(void);
static void UpdateLastSamplePointerAndSampleCounter(SNFC_LogDefinition_t *LogDefinition);

/**
* @} ASTRA_NFC_PRIVATE_FUNCTION_PROTOTYPES
*/


/** @addtogroup ASTRA_NFC_FUNCTIONS_DEFINITION ASTRA NFC - Function definitions
  * @brief Function definitions
  * @{
 */

/* Function ------------------------------------------------------------------*/


/**
 * @brief  Factory Reset NFC
 * @param  None
 * @retval None.
 */
void APP_NFC_FactoryReset(void) 
{
  uint32_t DataBuf32 =0xFFFFFFFF;
  uint8_t *DataBufPointer = (uint8_t *) &DataBuf32;
  int32_t Counter;
  uint8_t SkipCounter = 0;
  
  
  PRINTF_VERBOSE("Erasing NFC...\r\n");
  AstraLedColor(ASTRA_LED_OFF);
  for(Counter=0;Counter<STSMART_NFC_MAX_SIZE;Counter+=4) {
    PRINTF_VERBOSE("Remaining %04d\r",STSMART_NFC_MAX_SIZE-Counter);
    if (SkipCounter == 10)
    {
      AstraLedToggle(ASTRA_LED_COLOR_MAGENTA);
      SkipCounter = 0;
    }
    else
    {
      SkipCounter++;
    }
    if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, DataBufPointer, Counter, 4)!=NFCTAG_OK){
      STNFC_Error_Handler(STNFC_WRITING_ERROR);
    }
  }
  PRINTF_VERBOSE("\r\nNFC Content Erased\r\n");
  AstraLedColor(ASTRA_LED_OFF);
}


/**
 * @brief  Force NFC initialization
 * @param  None
 * @retval None.
 */
void APP_NFC_ForceMemInit(void)
{
  uint32_t DataBuf32;
  uint8_t *DataBuf8 = (uint8_t *)&DataBuf32;
  
  PRINTF_VERBOSE("NFC initialization is forced\r\n");
  
  DataBuf8[0] = 0;
  DataBuf8[1] = 0;
  
  /* Set SampleTime to 0 */
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, SMARTNFC_START_ADDR_OFFSET+6, 2)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }  
}


void APP_NFC_ForceDataInit(uint32_t NewEpochStartTime)
{
  uint32_t DataBuf32;
  uint8_t *DataBuf8 = (uint8_t *)&DataBuf32;
  
  PRINTF_VERBOSE("NFC data initialization is forced\r\n");
  
  DataBuf8[3] = ((NewEpochStartTime >> 24) & 0xFF);
  DataBuf8[2] = ((NewEpochStartTime >> 16) & 0xFF);
  DataBuf8[1] = ((NewEpochStartTime >> 8) & 0xFF);
  DataBuf8[0] = (NewEpochStartTime & 0xFF);
  
  /* Set new epoch start time to 0 */
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, SMARTNFC_START_ADDR_OFFSET+8, 4)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }  
  
  CheckIfNewConfiguration();  
}

void SmarTagAppStart(void)
{
  NFCStatus = NFC_STATUS_ON;
  
  /* Reset the Structures used for Controlling the Log */
  memset(&SmartNFCCodeHeader,0,sizeof(SNFC_CodeDefinition_t));
  memset(&LogDefinition,0,sizeof(SNFC_LogDefinition_t));
  memset(AllVirtualSensorsArray,0,SMARTNFC_VIRTUAL_SENSORS_NUM*sizeof(SNFC_VirtualSensor_t));
  memset(ConfiguratedVirtualSensorsArray,0,SMARTNFC_VIRTUAL_SENSORS_NUM*sizeof(SNFC_VirtualSensor_t *));
  
  SetAllAvailableVirtualSensors();
  
  /* Control if there is a valid Configuration saved on Tag */
  PRINTF_VERBOSE("\r\nControl if there is a Valid Configuration");
  ReadConfiguration(&LogDefinition,0,0);
  
  /* Check if there a valid configuration saved on the TAG */
  if(LogDefinition.StartDateTime==0) {
    PRINTF_VERBOSE("\r\nConfiguration NOT Present use default One");
    /* Set the Default Configuration */
    SaveDefaultConfiguration();
    
    /* Set the Date&Time */
    if(STNFC_SetDateTime(LogDefinition.StartDateTime,&hrtc,&LogDefinition)==0) {
      PRINTF_VERBOSE("Error: Setting RTC\r\n");
    } else {
       PRINTF_VERBOSE("Set RTC Date&Time\r\n");
    }
    
  } else {
    
    /* There is a valid Configuration
    * Initializes the log section */
    PRINTF_VERBOSE("Configuration Present on NFC\r\n");
    
    /* Set the Date&Time */
    if(STNFC_SetDateTime(LogDefinition.StartDateTime,&hrtc,&LogDefinition)==0) {
      PRINTF_VERBOSE("Error: Setting RTC\r\n");
    } else {
       PRINTF_VERBOSE("Set RTC Date&Time\r\n");
    }
  
    /* Reset the Max/Min For each Sensor */
    ResetMaxMinValuesAllVirtualSensors();
    
    /* Save the Max/Min for each Sensor */
    SaveMaxMinValuesForVirtualSensors();
    
    /* Write Sample Counter and Last Sample Pointer*/
    LogDefinition.SampleCounterAddress = LogDefinition.LastSamplePointer;
    LogDefinition.SampleCounter=0;
    UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
    LogDefinition.LastSamplePointer+=8;
    
  }
  
  NfcType5_SetInitialNDEFPayLoadLengthValue(LogDefinition.LastSamplePointer);
  
  NfcType5_ComputeNDEFPayLoadSize(LogDefinition.LastSamplePointer,LogDefinition.SampleCounter);
  
  /* we go in INACTIVE Mode */
  LogMode = SMARTNFC_LOGMODE_INACTIVE;
  
  /* Set the NFC behavior */
  PRINTF_VERBOSE("Set NFC Behavior\r\n");
  SetNFCBehavior();
  NFCStatus = NFC_STATUS_OFF;
}


void SmarTagAppProcess(void)
{
  if(PlatformStatus.b.NFC_RF_ACTIVITY_STATUS==FIELD_FALLING)
  {
    if(PlatformStatus.b.NFC_DATA_TRIGGERED) 
    {
      if(NFCStatus == NFC_STATUS_OFF)
      { 
        NFCStatus = NFC_STATUS_ON;
        ST25_RETRY(BSP_NFCTAG_SetRFSleep_Dyn(BSP_NFCTAG_INSTANCE));
        PRINTF_VERBOSE("\t-->NFC Sleep RF\r\n");
      }
      APP_NFC_SaveData();
      PlatformStatus.b.NFC_DATA_TRIGGERED=0;
      
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_COLOR_MAGENTA);
      HAL_Delay(100);
#endif // USE_NFC_LED
      
      if (LogMode == SMARTNFC_LOGMODE_INACTIVE) {
        /* Do Nothing */
        goto SMARTNFC_SLEEP;
      }
      
    SMARTNFC_SLEEP:
      
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(500);
#endif // USE_NFC_LED
      
      if(NFCStatus == NFC_STATUS_ON) {
        ST25_RETRY(BSP_NFCTAG_ResetRFSleep_Dyn(BSP_NFCTAG_INSTANCE));
        PRINTF_VERBOSE("\t-->NFC WakeUp RF\r\n");
        NFCStatus = NFC_STATUS_OFF;
      }
    }
  }
}


void SmarTagAppDetectRFActivity(void)
{
  uint8_t ITStatus;
  static ST25DV_FIELD_STATUS RFfield = ST25DV_FIELD_OFF;
  uint8_t FieldOn=0;
  uint8_t FieldOff=0;
  
  /* Read the IT status register */
  ST25_RETRY(BSP_NFCTAG_ReadITSTStatus_Dyn(BSP_NFCTAG_INSTANCE, &ITStatus));
  
  //Check the Falling Bit
  if((ITStatus & ST25DV_ITSTS_DYN_FIELDFALLING_MASK) == ST25DV_ITSTS_DYN_FIELDFALLING_MASK){
    FieldOff = 1;
  }
  
  //Check the Rising Bit
  if((ITStatus & ST25DV_ITSTS_DYN_FIELDRISING_MASK) == ST25DV_ITSTS_DYN_FIELDRISING_MASK) {
    FieldOn = 1;
  }
  
  //Make the decision
  if(((FieldOff == 1) & (FieldOn == 1)) | 
     ((FieldOff == 0) & (FieldOn == 0) & (RFfield==ST25DV_FIELD_OFF)))
  {
    // can't decide, need to read the register to get actual state 
    static ST25DV_FIELD_STATUS field = ST25DV_FIELD_OFF;
    int32_t status = BSP_NFCTAG_GetRFField_Dyn(BSP_NFCTAG_INSTANCE,&field);
    if((field == ST25DV_FIELD_ON) || (status == NFCTAG_NACK)) {
      if(FieldOn || FieldOff) {
        // Off->On
        PRINTF_VERBOSE("\r\nDetected NFC FIELD Off->On\r\n");
        PlatformStatus.b.NFC_RF_ACTIVITY_STATUS = FIELD_RISING;
        RFfield = ST25DV_FIELD_ON;
      }
    } else {
      if(FieldOn || FieldOff) {
        //On->Off
        PRINTF_VERBOSE("\r\nDetected NFC FIELD On->Off\r\n");
        RFfield = ST25DV_FIELD_OFF;
        PlatformStatus.b.NFC_RF_ACTIVITY_STATUS = FIELD_FALLING;
      }
    }
  } else {
    if((FieldOff == 0) & (FieldOn == 1)) {
      // On
      PRINTF_VERBOSE("\r\nDetected NFC FIELD On\r\n");
      PlatformStatus.b.NFC_RF_ACTIVITY_STATUS = FIELD_RISING;
    } else if((FieldOff == 1) & (FieldOn == 0)) {
      //Off
      PRINTF_VERBOSE("\r\nDetected NFC FIELD Off\r\n");
      PlatformStatus.b.NFC_RF_ACTIVITY_STATUS = FIELD_FALLING;
    }
  }
  
  
  //When the Phone is no more under the Tag
  if(PlatformStatus.b.NFC_RF_ACTIVITY_STATUS == FIELD_FALLING)
  {
    /* Control if there is a new configuration */
    ST25_RETRY(BSP_NFCTAG_SetRFSleep_Dyn(BSP_NFCTAG_INSTANCE));
#ifdef SMARTNFC_VERBOSE_PRINTF
    PRINTF_VERBOSE("\t-->NFC Sleep RF\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */   
    CheckIfNewConfiguration();
    ST25_RETRY(BSP_NFCTAG_ResetRFSleep_Dyn(BSP_NFCTAG_INSTANCE));
#ifdef SMARTNFC_VERBOSE_PRINTF
    PRINTF_VERBOSE("\t-->NFC WakeUp RF\r\n");
#endif /* SMARTNFC_VERBOSE_PRINTF */     
    
  }
}


/**
* @brief  Set the NFC Security Level, GPO behavior and Energy harvesting mode
* @param  None
* @retval None
*/
static void SetNFCBehavior(void)
{
  /* Setting the New Password for I2C protection */
  if(BSP_NFCTAG_ChangeI2CPassword(0x90ABCDEF,0x12345678)!=NFCTAG_OK ) {
    STNFC_Error_Handler(STNFC_CONFIG_ERROR);
  }
  
  /* GPO sensible to RF Field change */
  if(BSP_NFCTAG_WriteConfigIT(0x90ABCDEF,0x12345678,ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_FIELDCHANGE_MASK)!=NFCTAG_OK ) {
    STNFC_Error_Handler(STNFC_CONFIG_ERROR);
  }
  
  /* Setting the Energy harvesting mode */
  if(BSP_NFCTAG_CheckChangeEHMODE(0x90ABCDEF,0x12345678,ST25DV_EH_ON_DEMAND)!=NFCTAG_OK ) {
    STNFC_Error_Handler(STNFC_CONFIG_ERROR);
  }
}


/**
* @brief  Checks if there is a new configuration.
* @param  None
* @retval None
*/
static void CheckIfNewConfiguration(void)
{
  SNFC_LogDefinition_t LocalLogDefinition;
  
  PRINTF_VERBOSE("Check if there is a new Configuration\r\n");
  ReadConfiguration(&LocalLogDefinition,LogDefinition.StartDateTime,1);
  
  if(LocalLogDefinition.StartDateTime!=0) {
    int32_t SensorNum;
    PRINTF_VERBOSE("Restart the Log\r\n");
    /* Disable all the Previous Enabled Virtual Sensors */
    for(SensorNum=0;SensorNum<LogDefinition.VirtualSensorsNum;SensorNum++) {
      ConfiguratedVirtualSensorsArray[SensorNum]->Enable=0;
      ConfiguratedVirtualSensorsArray[SensorNum]->SampleDeltaDateTime=0;
    }

    /* Read again and update the configuration restarting the log */
    ReadConfiguration(&LogDefinition,LogDefinition.StartDateTime,0);
    
    /* Set the Date&Time */
    if(STNFC_SetDateTime(LogDefinition.StartDateTime,&hrtc,&LogDefinition)==0) {
      PRINTF_VERBOSE("Error: Setting RTC\r\n");
    } else {
       PRINTF_VERBOSE("Set RTC Date&Time\r\n");
    }
    
    /* Set the NFC sample time */
    if(STNFC_SetSampleTime(LogDefinition.SampleTime)==0) {
      PRINTF_VERBOSE("Error: Setting Sample time\r\n");
    } else {
       PRINTF_VERBOSE("Set sample time\r\n");
    }
    
    /* Reset the Max/Min For each Sensor */
    ResetMaxMinValuesAllVirtualSensors();
    
    /* Save the Max/Min for each Sensor */
    SaveMaxMinValuesForVirtualSensors();
    
    /* Write Sample Counter and Last Sample Pointer*/
    LogDefinition.SampleCounterAddress = LogDefinition.LastSamplePointer;
    LogDefinition.SampleCounter=0;
    UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
    LogDefinition.LastSamplePointer+=8;
    
    NfcType5_SetInitialNDEFPayLoadLengthValue(LogDefinition.LastSamplePointer);
    
    NfcType5_ComputeNDEFPayLoadSize(LogDefinition.LastSamplePointer,LogDefinition.SampleCounter);
    
    /* we go in ACTIVE Mode */
    LogMode = SMARTNFC_LOGMODE_ACTIVE;
  }
}

/**
* @brief  Reads the active log configuration
* @param  SNFC_LogDefinition_t *LogDefinition
* @param  uint32_t CurrentStartDateTime  Actual Start Date&Time Value
* @param  int32_t OnlyChecks if ==1 it controls only if the configuration is valid
* @retval None
*/ 
static void ReadConfiguration(SNFC_LogDefinition_t *LogDefinition,uint32_t CurrentStartDateTime,int32_t OnlyChecks)
{
  uint32_t DataBuf32;
  uint8_t *DataBuf8 = (uint8_t *)&DataBuf32;
  uint16_t *DataBuf16 = (uint16_t *)&DataBuf32;
  SNFC_ValidConfiguration_t ValidConf=STNFC_VALID_CONFIG;
  SNFC_CodeDefinition_t *LocalSmartNFCCodeHeader = (SNFC_CodeDefinition_t *)&DataBuf32;
  
  LogDefinition->LastSamplePointer = SMARTNFC_START_ADDR_OFFSET;
  if(BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, DataBuf8, LogDefinition->LastSamplePointer, 4 )!=NFCTAG_OK) {
    STNFC_Error_Handler(STNFC_READING_ERROR);
    ValidConf=STNFC_ERROR_READING_CONFIG;
  }
  
  if(ValidConf==STNFC_VALID_CONFIG) {
    LogDefinition->LastSamplePointer+=4;
    
    /* Check the protocol header */
    if((LocalSmartNFCCodeHeader->ProtVersion  != SMARTNFC_RECORD_VERSION) ||
       (LocalSmartNFCCodeHeader->ProtRevision != SMARTNFC_RECORD_REVISION) ||
       (LocalSmartNFCCodeHeader->BoardId      != SMARTNFC_BOARD_ID) ||
       (LocalSmartNFCCodeHeader->FirmwareId   != SMARTNFC_FIRMWARE_ID)) {
             PRINTF_VERBOSE("Error: Protocol Header not valid\r\n");
             ValidConf=STNFC_NOT_VALID_CONFIG;
           } else if(OnlyChecks==0){
             //If we are not making only the check... update the configuration
             memcpy(&SmartNFCCodeHeader,LocalSmartNFCCodeHeader,sizeof(SNFC_CodeDefinition_t));
           }
  }
  
  if(ValidConf==STNFC_VALID_CONFIG) {
    if(BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, DataBuf8, LogDefinition->LastSamplePointer, 4 )!=NFCTAG_OK) {
      STNFC_Error_Handler(STNFC_READING_ERROR);
      ValidConf=STNFC_ERROR_READING_CONFIG;
    }
    if(ValidConf==STNFC_VALID_CONFIG) {
      LogDefinition->LastSamplePointer+=4;
      
      LogDefinition->RFU = DataBuf8[0];
      LogDefinition->VirtualSensorsNum = DataBuf8[1];
      LogDefinition->SampleTime = DataBuf16[1];
      
      PRINTF_VERBOSE("\tVn=%d SampleTime=%d\r\n",LogDefinition->VirtualSensorsNum,LogDefinition->SampleTime);
      
      if(LogDefinition->VirtualSensorsNum==0) {
        PRINTF_VERBOSE("\tError: VirtualSensorsNum==0\r\n");
        ValidConf=STNFC_NOT_VALID_CONFIG;
      }
      
      if(LogDefinition->VirtualSensorsNum>SMARTNFC_VIRTUAL_SENSORS_NUM) {
        PRINTF_VERBOSE("\tError: VirtualSensorsNum >%d\r\n",SMARTNFC_VIRTUAL_SENSORS_NUM);
        ValidConf=STNFC_NOT_VALID_CONFIG;
      }
      
      if((LogDefinition->SampleTime<1) | (LogDefinition->SampleTime> (UINT16_MAX-1))) {
        PRINTF_VERBOSE("\tError: SampleTime =%d Not Valid\r\n",LogDefinition->SampleTime);
        ValidConf=STNFC_NOT_VALID_CONFIG;
      }
    }
  }
  
  if(ValidConf==STNFC_VALID_CONFIG) {
    if(BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, DataBuf8, LogDefinition->LastSamplePointer, 4 )!=NFCTAG_OK) {
      STNFC_Error_Handler(STNFC_READING_ERROR);
      ValidConf=STNFC_ERROR_READING_CONFIG;
    }
    if(ValidConf==STNFC_VALID_CONFIG) {
      LogDefinition->LastSamplePointer+=4;
      
      LogDefinition->StartDateTime=DataBuf32;
      //If we have a valid not null Start Date&Time and different respect the Current one used...-> New Configuration
      if(LogDefinition->StartDateTime!=0) {
        if(LogDefinition->StartDateTime ==CurrentStartDateTime) {
          PRINTF_VERBOSE("Start Time not Changed\r\n");
          ValidConf=STNFC_NOT_CHANGED_CONFIG;
        }
      } else {
        PRINTF_VERBOSE("Start Time ==0\r\n");
        ValidConf=STNFC_NOT_VALID_CONFIG;
      }     
    }
  }
  
  if(ValidConf==STNFC_VALID_CONFIG) {
    /* We need to read the Virtual Sensor Configuration */
    int32_t SensorNum;
    for(SensorNum=0;((SensorNum<LogDefinition->VirtualSensorsNum)&(ValidConf==STNFC_VALID_CONFIG));SensorNum++) {
      if(BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, DataBuf8, LogDefinition->LastSamplePointer, 4 )!=NFCTAG_OK) {
        STNFC_Error_Handler(STNFC_READING_ERROR);
        ValidConf=STNFC_ERROR_READING_CONFIG;
      }
      if(ValidConf==STNFC_VALID_CONFIG) {
        LogDefinition->LastSamplePointer+=4;
        switch(DataBuf32&BITS_MASK_VSID)
        {
        case BATT_PERCENTAGE_VS_ID:
          PRINTF_VERBOSE("\tFound BATT_PERCENTAGE_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_10;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
            if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
              ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BITS_LENGTH_10))&BITS_MASK_10;
              PRINTF_VERBOSE("\tTh2.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value);
            }  
          }
          break;
          
        case BATT_VOLTAGE_VS_ID:
          PRINTF_VERBOSE("\tFound BATT_VOLTAGE_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_10;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
            if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
              ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BITS_LENGTH_10))&BITS_MASK_10;
              PRINTF_VERBOSE("\tTh2.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value);
            }    
          }
          break;


         case STTS22H_VS_ID:
          PRINTF_VERBOSE("\tFound STTS22H_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[STTS22H_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_9;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui16Value=%f\r\n",STTS22H_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value));
            if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
              ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BITS_LENGTH_9))&BITS_MASK_9;
              PRINTF_VERBOSE("\tTh2.Ui16Value=%f\r\n",STTS22H_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value));
            }
          }
          break;         

        case LPS22HH_VS_ID:
          PRINTF_VERBOSE("\tFound LPS22HH_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[LPS22HH_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_11;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
            if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
              ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BITS_LENGTH_11))&BITS_MASK_11;
              PRINTF_VERBOSE("\tTh2.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value);
            }    
          }
          break;           

        case HTS221_VS_ID:
          PRINTF_VERBOSE("\tFound HTS221_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[HTS221_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui8Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_7;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui8Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui8Value);
            if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
              ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui8Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BITS_LENGTH_7))&BITS_MASK_7;
              PRINTF_VERBOSE("\tTh2.Ui8Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui8Value);
            }   
          }
          break;  

        case LIS2DTW12_VS_ID:
          PRINTF_VERBOSE("\tFound LIS2DTW12_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[LIS2DTW12_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_15;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
          }
          break;           
          
   
        case LSM6DSOX32_VS_ID:
          PRINTF_VERBOSE("\tFound LSM6DSOX32_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[LSM6DSOX32_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_15;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui16Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
          }
          break;
          
#if ENABLE_ACC_EVENTS_VS_ID
        case ACC_EVENTS_VS_ID:
          PRINTF_VERBOSE("\tFound ACC_EVENTS_VS_ID:\r\n");
          if(OnlyChecks==0) {
            ConfiguratedVirtualSensorsArray[SensorNum] = &AllVirtualSensorsArray[ACC_EVENTS_VS_ID];
            ConfiguratedVirtualSensorsArray[SensorNum]->Enable=1;
            ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType = (SNFC_ThresholdsUsage_t)((DataBuf32>>BITS_LENGTH_VSID)&BITS_MASK_TH_MOD);
            ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui8Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD))&BITS_MASK_8;
            PRINTF_VERBOSE("\tThsUsageType=%s\r\n",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
            PRINTF_VERBOSE("\tTh1.Ui8Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui8Value);
            if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
              ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui8Value = (DataBuf32>>(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BITS_LENGTH_8))&BITS_MASK_8;
              PRINTF_VERBOSE("\tTh2.Ui8Value=%d\r\n",ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui8Value);
            }
          }
          break;
#endif //ENABLE_ACC_EVENTS_VS_ID
        default:
          ValidConf=STNFC_NOT_VALID_CONFIG;
          PRINTF_VERBOSE("Error: Not recognized VirtualSensorID=%d\r\n",DataBuf32&0x7);
        }
      }
    }
  }
  
  switch(ValidConf) {
    case STNFC_NOT_VALID_CONFIG:
      PRINTF_VERBOSE("Not Valid Configuration present on NFC Skip it\r\n");
      LogDefinition->StartDateTime=0;
    break;
    case STNFC_ERROR_READING_CONFIG:
      PRINTF_VERBOSE("Error Reading Configuration present on NFC Skip it\r\nTry again to write the new configuration\r\n");
      LogDefinition->StartDateTime=0;
    break;
    case STNFC_VALID_CONFIG:
      if(OnlyChecks) {
        PRINTF_VERBOSE("Valid Configuration present on NFC\r\n");
      }
    break;
    case STNFC_NOT_CHANGED_CONFIG:
      if(OnlyChecks) {
        PRINTF_VERBOSE("Going on with the current Configuration\r\n");
      }
      LogDefinition->StartDateTime=0;
    break;
  }
}

/**
* @brief  Set all Avaialbles Virtual Sensor Configuration
* @param  None
* @retval None
*/ 
static void SetAllAvailableVirtualSensors(void)
{
  /* Virtual sensors initialization */
  
  //Number 0
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].VirtualSensorId = BATT_PERCENTAGE_VS_ID;
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Enable=0;
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].SensorType=VST_UI16;
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].ThsUsageType=TH_INT;                                                                    // Normal values are within thresholds
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Th1.Ui16Value = BATT_PERCENTAGE_SAMPLE_TO_CODED(BATT_PERCENTAGE_LOW_THRESHOLD);         // Lower threshold 
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Th2.Ui16Value = BATT_PERCENTAGE_SAMPLE_TO_CODED(BATT_PERCENTAGE_UPP_THRESHOLD);         // Upper threshold
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinLimit.Ui16Value = BATT_PERCENTAGE_SAMPLE_TO_CODED(BATT_PERCENTAGE_MIN_VALUE);        // Min value
  AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxLimit.Ui16Value = BATT_PERCENTAGE_SAMPLE_TO_CODED(BATT_PERCENTAGE_MAX_VALUE);        // Max value
  
  //Number 1
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].VirtualSensorId = BATT_VOLTAGE_VS_ID;
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Enable=0;
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].SensorType=VST_UI16;
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].ThsUsageType=TH_INT;                                                                       // Normal values are within thresholds
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Th1.Ui16Value = BATT_VOLTAGE_SAMPLE_TO_CODED(BATT_VOLTAGE_LOW_THRESHOLD);                  // Lower threshold     
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Th2.Ui16Value = BATT_VOLTAGE_SAMPLE_TO_CODED(BATT_VOLTAGE_UPP_THRESHOLD);                  // Upper threshold
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinLimit.Ui16Value = BATT_VOLTAGE_SAMPLE_TO_CODED(BATT_VOLTAGE_MIN_VALUE);                 // Min value
  AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxLimit.Ui16Value = BATT_VOLTAGE_SAMPLE_TO_CODED(BATT_VOLTAGE_MAX_VALUE);                 // Max value
  
  //Number 2
  AllVirtualSensorsArray[STTS22H_VS_ID].VirtualSensorId = STTS22H_VS_ID;
  AllVirtualSensorsArray[STTS22H_VS_ID].Enable=0;
  AllVirtualSensorsArray[STTS22H_VS_ID].SensorType=VST_UI16;    
  AllVirtualSensorsArray[STTS22H_VS_ID].ThsUsageType=TH_INT;                                                                            // Normal values are within thresholds
  AllVirtualSensorsArray[STTS22H_VS_ID].Th1.Ui16Value = STTS22H_SAMPLE_TO_CODED(STTS22H_LOW_THRESHOLD);                                 // Lower threshold
  AllVirtualSensorsArray[STTS22H_VS_ID].Th2.Ui16Value = STTS22H_SAMPLE_TO_CODED(STTS22H_UPP_THRESHOLD);                                 // Upper threshold
  AllVirtualSensorsArray[STTS22H_VS_ID].MinLimit.Ui16Value = STTS22H_SAMPLE_TO_CODED(STTS22H_MIN_VALUE);                                // Min value
  AllVirtualSensorsArray[STTS22H_VS_ID].MaxLimit.Ui16Value = STTS22H_SAMPLE_TO_CODED(STTS22H_MAX_VALUE);                                // Max value

  //Number 3
  AllVirtualSensorsArray[LPS22HH_VS_ID].VirtualSensorId = LPS22HH_VS_ID;
  AllVirtualSensorsArray[LPS22HH_VS_ID].Enable=0;
  AllVirtualSensorsArray[LPS22HH_VS_ID].SensorType=VST_UI16;
  AllVirtualSensorsArray[LPS22HH_VS_ID].ThsUsageType=TH_INT;                                                                            // Normal values are within thresholds
  AllVirtualSensorsArray[LPS22HH_VS_ID].Th1.Ui16Value = LPS22HH_SAMPLE_TO_CODED(LPS22HH_LOW_THRESHOLD);                                 // Lower threshold
  AllVirtualSensorsArray[LPS22HH_VS_ID].Th2.Ui16Value = LPS22HH_SAMPLE_TO_CODED(LPS22HH_UPP_THRESHOLD);                                 // Upper threshold
  AllVirtualSensorsArray[LPS22HH_VS_ID].MinLimit.Ui16Value = LPS22HH_SAMPLE_TO_CODED(LPS22HH_MIN_VALUE);                                // Min value
  AllVirtualSensorsArray[LPS22HH_VS_ID].MaxLimit.Ui16Value = LPS22HH_SAMPLE_TO_CODED(LPS22HH_MAX_VALUE);                                // Max value

  //Number 4
  AllVirtualSensorsArray[HTS221_VS_ID].VirtualSensorId = HTS221_VS_ID;
  AllVirtualSensorsArray[HTS221_VS_ID].Enable=0;
  AllVirtualSensorsArray[HTS221_VS_ID].SensorType=VST_UI8;
  AllVirtualSensorsArray[HTS221_VS_ID].ThsUsageType=TH_INT;                                                                             // Normal values are within thresholds
  AllVirtualSensorsArray[HTS221_VS_ID].Th1.Ui16Value = HTS221_SAMPLE_TO_CODED(HTS221_LOW_THRESHOLD);                                    // Lower threshold
  AllVirtualSensorsArray[HTS221_VS_ID].Th2.Ui16Value = HTS221_SAMPLE_TO_CODED(HTS221_UPP_THRESHOLD);                                    // Upper threshold
  AllVirtualSensorsArray[HTS221_VS_ID].MinLimit.Ui16Value = HTS221_SAMPLE_TO_CODED(HTS221_MIN_VALUE);                                   // Min value
  AllVirtualSensorsArray[HTS221_VS_ID].MaxLimit.Ui16Value = HTS221_SAMPLE_TO_CODED(HTS221_MAX_VALUE);                                   // Max value

  //Number 5
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].VirtualSensorId = LIS2DTW12_VS_ID;
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].Enable=0;
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].SensorType=VST_UI16;
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].ThsUsageType=TH_BIGGER;                                                                       // Normal values are below the threshold
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].Th1.Ui16Value = LIS2DTW12_UPP_THRESHOLD;                                                      // Upper threshold
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].MinLimit.Ui16Value = LIS2DTW12_MIN_VALUE;                                                     // Min value
  AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxLimit.Ui16Value = LIS2DTW12_MAX_VALUE;                                                     // Max value
  
  //Number 6
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].VirtualSensorId = LSM6DSOX32_VS_ID;
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Enable=0;
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].SensorType=VST_UI16;
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].ThsUsageType=TH_BIGGER;                                                                      // Normal values are below the threshold
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Th1.Ui16Value = LSM6DSOX32_UPP_THRESHOLD;                                                    // Upper threshold
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxLimit.Ui16Value = LSM6DSOX32_MAX_VALUE;                                                   // Min value
  AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MinLimit.Ui16Value = LSM6DSOX32_MIN_VALUE;                                                   // Max value
  
  //Number 7
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].VirtualSensorId = ACC_EVENTS_VS_ID;
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Enable=0;
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].SensorType=VST_UI8;
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].ThsUsageType=TH_INT;                                                                         // Normal values are within thresholds
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Th1.Ui16Value = ACC_EVENTS_LOW_THRESHOLD;                                                    // Lower threshold
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Th2.Ui16Value = ACC_EVENTS_UPP_THRESHOLD;                                                    // Upper threshold
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinLimit.Ui8Value = ACC_EVENTS_MIN_VALUE;                                                    // Min value
//  AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxLimit.Ui8Value = ACC_EVENTS_MAX_VALUE;                                                    // Max value
}

/**
* @brief  Set and Save the Default log configuration
* @param  None
* @retval None
*/ 
static void SaveDefaultConfiguration(void)
{
  uint8_t VirtualSensorIndex = 0;
  uint32_t DataBuf32;
  uint8_t *DataBuf8 = (uint8_t *)&DataBuf32;
  uint16_t *DataBuf16 = (uint16_t *)&DataBuf32;
  
  PRINTF_VERBOSE("SaveDefaultConfiguration\r\n");
  
  //Initialize the Protocol header
  SmartNFCCodeHeader.ProtVersion  = SMARTNFC_RECORD_VERSION;
  SmartNFCCodeHeader.ProtRevision = SMARTNFC_RECORD_REVISION;
  SmartNFCCodeHeader.BoardId      = SMARTNFC_BOARD_ID;
  SmartNFCCodeHeader.FirmwareId   = SMARTNFC_FIRMWARE_ID;

  DataBuf8[0] = SmartNFCCodeHeader.ProtVersion;
  DataBuf8[1] = SmartNFCCodeHeader.ProtRevision;
  DataBuf8[2] = SmartNFCCodeHeader.BoardId;
  DataBuf8[3] = SmartNFCCodeHeader.FirmwareId;
  
  /* Save Protocol/Board and Fw Id */
  LogDefinition.LastSamplePointer = SMARTNFC_START_ADDR_OFFSET;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }
  /* Update Last Sample Pointer (pointer to next Sample...) */
  LogDefinition.LastSamplePointer+=4;
  
  //Initialize the Log Header
  LogDefinition.ConfigSectionPositionPointer = LogDefinition.LastSamplePointer;
  LogDefinition.SampleTime=DATA_DEFAULT_SAMPLE_INT;                     // Default save interval in seconds
  LogDefinition.StartDateTime=SMARTNFC_DEFAULT_EPOCH_START_TIME;        // Default epoch time
  LogDefinition.RFU = 0;                                                // RFU is not Used
  LogDefinition.VirtualSensorsNum = VIRTUAL_SENSOR_NUM_EN;              // Number of used virtual sensors
  
  /* Write Virtual Sensor Number and Polling Sample Time */
  DataBuf8[0] = LogDefinition.RFU;
  DataBuf8[1] = LogDefinition.VirtualSensorsNum;
  DataBuf16[1] = LogDefinition.SampleTime;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }
  LogDefinition.LastSamplePointer+=4;
  
  /* Write TimeStamp==0 meaning Default Configuration */
  DataBuf32 = LogDefinition.StartDateTime;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }
  LogDefinition.LastSamplePointer+=4;
  
  // Initialization of the used virtual sensors  

  //Number 0
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID];
    VirtualSensorIndex++;
  }
  
  //Number 1
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID];
    VirtualSensorIndex++;
  }
  
  //Number 2
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[STTS22H_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[STTS22H_VS_ID];
    VirtualSensorIndex++;
  }

  //Number 3
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[LPS22HH_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[LPS22HH_VS_ID];
    VirtualSensorIndex++;
  }

  //Number 4
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[HTS221_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[HTS221_VS_ID];  
    VirtualSensorIndex++;
  }
  
  //Number 5
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[LIS2DTW12_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[LIS2DTW12_VS_ID];  
    VirtualSensorIndex++;
  }

  //Number 6
  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
  {
    AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Enable=1;
    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[LSM6DSOX32_VS_ID];  
    VirtualSensorIndex++;
  }  

  //Number 7
//  if (VirtualSensorIndex < VIRTUAL_SENSOR_NUM_EN)
//  {
//    AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Enable=1;
//    ConfiguratedVirtualSensorsArray[VirtualSensorIndex] = &AllVirtualSensorsArray[ACC_EVENTS_VS_ID];  
//    VirtualSensorIndex++;
//  }
  
  /* Write Virtual Sensors Configuration */
  SaveVirtualSensorsConfiguration();
  
  /* Reset the Max/Min For each Sensor */
  ResetMaxMinValuesAllVirtualSensors();
  
  /* Save the Max/Min for each Sensor */
  SaveMaxMinValuesForVirtualSensors();
  
  /* Write Sample Counter and Last Sample Pointer*/
  LogDefinition.SampleCounterAddress = LogDefinition.LastSamplePointer;
  LogDefinition.SampleCounter=0;
  UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
  LogDefinition.LastSamplePointer+=8; /* We use 4bytes for each one */
}


/**
* @brief  Save the Last Sample Pointer and Sample Counter
* @param  SNFC_LogDefinition_t *LogDefinition Pointer to Log definition structure
* @retval None
*/ 
static void UpdateLastSamplePointerAndSampleCounter(SNFC_LogDefinition_t *LogDefinition)
{
  uint32_t DataBuf32;
  
  /* Write Sample Counter */
  DataBuf32 = LogDefinition->SampleCounter;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition->SampleCounterAddress, 4)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }
  
  /*  Write LastSamplePointer */
  DataBuf32 = LogDefinition->LastSamplePointer;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition->SampleCounterAddress+4, 4)!=NFCTAG_OK){
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
  }
}

/**
* @brief  Reset the Max/Min Values
* @param  None
* @retval None
*/ 
static void ResetMaxMinValuesAllVirtualSensors(void)
{
  PRINTF_VERBOSE("ResetMaxMinValuesAllVirtualSensors\r\n");
  
  if(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Enable) {
    AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxValue.Ui16Value = BATT_PERCENTAGE_SAMPLE_TO_CODED(BATT_PERCENTAGE_MIN_VALUE);
    AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinValue.Ui16Value = BATT_PERCENTAGE_SAMPLE_TO_CODED(BATT_PERCENTAGE_MAX_VALUE);
    AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinDeltaDateTime = 0;  }
  
  if(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Enable) {
    AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxValue.Ui16Value = BATT_VOLTAGE_SAMPLE_TO_CODED(BATT_VOLTAGE_MIN_VALUE);
    AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinValue.Ui16Value = BATT_VOLTAGE_SAMPLE_TO_CODED(BATT_VOLTAGE_MAX_VALUE);
    AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinDeltaDateTime = 0;  }
  
  if(AllVirtualSensorsArray[STTS22H_VS_ID].Enable) {
    AllVirtualSensorsArray[STTS22H_VS_ID].MaxValue.Ui16Value = STTS22H_SAMPLE_TO_CODED(STTS22H_MIN_VALUE);
    AllVirtualSensorsArray[STTS22H_VS_ID].MinValue.Ui16Value = STTS22H_SAMPLE_TO_CODED(STTS22H_MAX_VALUE);
    AllVirtualSensorsArray[STTS22H_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[STTS22H_VS_ID].MinDeltaDateTime = 0;
  }
  
  if(AllVirtualSensorsArray[LPS22HH_VS_ID].Enable) {
    AllVirtualSensorsArray[LPS22HH_VS_ID].MaxValue.Ui16Value = LPS22HH_SAMPLE_TO_CODED(LPS22HH_MIN_VALUE);
    AllVirtualSensorsArray[LPS22HH_VS_ID].MinValue.Ui16Value = LPS22HH_SAMPLE_TO_CODED(LPS22HH_MAX_VALUE);
    AllVirtualSensorsArray[LPS22HH_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[LPS22HH_VS_ID].MinDeltaDateTime = 0;
  }
  
  if(AllVirtualSensorsArray[HTS221_VS_ID].Enable) {
    AllVirtualSensorsArray[HTS221_VS_ID].MaxValue.Ui8Value = HTS221_SAMPLE_TO_CODED(HTS221_MIN_VALUE);
    AllVirtualSensorsArray[HTS221_VS_ID].MinValue.Ui8Value = HTS221_SAMPLE_TO_CODED(HTS221_MAX_VALUE);  
    AllVirtualSensorsArray[HTS221_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[HTS221_VS_ID].MinDeltaDateTime = 0;
  }
  
  if(AllVirtualSensorsArray[LIS2DTW12_VS_ID].Enable) {
    AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxValue.Ui16Value = LIS2DTW12_MIN_VALUE;
    AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[LIS2DTW12_VS_ID].MinDeltaDateTime = 0;
  }
  
  if(AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Enable) {
    AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxValue.Ui16Value = LSM6DSOX32_MIN_VALUE;
    AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxDeltaDateTime = 0;
    AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MinDeltaDateTime = 0;
  }
  
//  if(AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Enable) {
//    AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxValue.Ui16Value = ACC_EVENTS_MIN_VALUE;
//    AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinValue.Ui16Value = ACC_EVENTS_MAX_VALUE;
//    AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxDeltaDateTime = LogDefinition.StartDateTime;
//    AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinDeltaDateTime = LogDefinition.StartDateTime;
//  }
}


/**
* @brief  Save the Max/Min Values
* @param  None
* @retval None
*/ 
static void SaveMaxMinValuesForVirtualSensors(void)
{
  int32_t SensorNum;
  uint32_t DataBuf32;
  
  PRINTF_VERBOSE("SaveMaxMinValuesForVirtualSensors\r\n");
  
  for(SensorNum=0;SensorNum<LogDefinition.VirtualSensorsNum;SensorNum++) {
    if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID]) {
      
      AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID]) {
      
      AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[STTS22H_VS_ID]) {
      
      AllVirtualSensorsArray[STTS22H_VS_ID].MinPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].MinValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      AllVirtualSensorsArray[STTS22H_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].MaxValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[LPS22HH_VS_ID]) {
      
      AllVirtualSensorsArray[LPS22HH_VS_ID].MinPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].MinValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      AllVirtualSensorsArray[LPS22HH_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].MaxValue.Ui16Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[HTS221_VS_ID]) {
      
      AllVirtualSensorsArray[HTS221_VS_ID].MinPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].MinValue.Ui8Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      AllVirtualSensorsArray[HTS221_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].MaxValue.Ui8Value)<<20);
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[LIS2DTW12_VS_ID]) {
      
      AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) ;
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      DataBuf32 = AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxValue.Ui16Value;
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;      
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[LSM6DSOX32_VS_ID]) {
      
      AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
      DataBuf32 = (0 /* ShortDeltaTime */) ;
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      DataBuf32 = AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxValue.Ui16Value;
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
//    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[ACC_EVENTS_VS_ID]) {
//
//      AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinPositionPointer = LogDefinition.LastSamplePointer;
//      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinValue.Ui8Value)<<20);
//      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
//        STNFC_Error_Handler(STNFC_WRITING_ERROR);
//      }
//      LogDefinition.LastSamplePointer+=4;
//      
//      AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxPositionPointer = LogDefinition.LastSamplePointer;
//      DataBuf32 = (0 /* ShortDeltaTime */) | (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxValue.Ui8Value)<<20);
//      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
//        STNFC_Error_Handler(STNFC_WRITING_ERROR);
//      }
//      LogDefinition.LastSamplePointer+=4;

    }
    
  }
}

/**
* @brief  Save the Virtual Sensors Configurations
* @param  None
* @retval None
*/ 
static void SaveVirtualSensorsConfiguration(void)
{
  int32_t SensorNum;
  uint32_t DataBuf32;
  
  PRINTF_VERBOSE("\r\nSave virtual sensors configuration");
  
  for(SensorNum=0;SensorNum<LogDefinition.VirtualSensorsNum;SensorNum++) {
    if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID]) {
      
      DataBuf32 = BATT_PERCENTAGE_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD)) |
            (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Th2.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BATT_PERCENTAGE_VS_TH_BITS_LENGHT));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave BATT_PERCENTAGE_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%3.1f",BATT_PERCENTAGE_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value));
      if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
        PRINTF_VERBOSE("\r\n\tTh2.Ui16Value=%3.1f",BATT_PERCENTAGE_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value));
      }
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID]) {
      
      DataBuf32 = BATT_VOLTAGE_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD)) |
            (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Th2.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+BATT_VOLTAGE_VS_TH_BITS_LENGHT));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave BATT_VOLTAGE_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%f",BATT_VOLTAGE_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value));
      if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
        PRINTF_VERBOSE("\r\n\tTh2.Ui16Value=%f",BATT_VOLTAGE_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value));
      } 
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[STTS22H_VS_ID]) {
      
      DataBuf32 = STTS22H_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD)) |
            (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].Th2.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+STTS22H_VS_TH_BITS_LENGHT));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave STTS22H_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%f",STTS22H_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value));
      if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
        PRINTF_VERBOSE("\r\n\tTh2.Ui16Value=%f",STTS22H_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value));
      }
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[LPS22HH_VS_ID]) {
      
      DataBuf32 = LPS22HH_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD)) |
            (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].Th2.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+LPS22HH_VS_TH_BITS_LENGHT));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave LPS22HH_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%f",LPS22HH_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value));
      if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
        PRINTF_VERBOSE("\r\n\tTh2.Ui16Value=%f",LPS22HH_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value));
      }
      
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[HTS221_VS_ID]) {
      
      DataBuf32 = HTS221_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD)) |
            (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].Th2.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+HTS221_VS_TH_BITS_LENGHT));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave HTS221_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%f",HTS221_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value));
      if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
        PRINTF_VERBOSE("\r\n\tTh2.Ui16Value=%f",HTS221_CODED_TO_SAMPLE(ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value));
      }      
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[LIS2DTW12_VS_ID]) {
      
      DataBuf32 = LIS2DTW12_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[LIS2DTW12_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[LIS2DTW12_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave LIS2DTW12_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%d",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
      
    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[LSM6DSOX32_VS_ID]) {
      
      DataBuf32 = LSM6DSOX32_VS_ID |
        (((uint32_t)AllVirtualSensorsArray[LSM6DSOX32_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
          (((uint32_t)AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Th1.Ui16Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD));
      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      LogDefinition.LastSamplePointer+=4;
      
      PRINTF_VERBOSE("\r\nSave LSM6DSOX32_VS_ID:");
      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
      PRINTF_VERBOSE("\r\n\tTh1.Ui16Value=%d",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
      
//    } else if(ConfiguratedVirtualSensorsArray[SensorNum] == &AllVirtualSensorsArray[ACC_EVENTS_VS_ID]) {
//      
//      DataBuf32 = ACC_EVENTS_VS_ID |
//        (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].ThsUsageType)<<BITS_LENGTH_VSID) |
//          (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Th1.Ui8Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD)) |
//            (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Th2.Ui8Value)<<(BITS_LENGTH_VSID+BITS_LENGTH_TH_MOD+ACC_EVENTS_VS_TH_BITS_LENGHT));
//      if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
//        STNFC_Error_Handler(STNFC_WRITING_ERROR);
//      }
//      LogDefinition.LastSamplePointer+=4;
//      
//      PRINTF_VERBOSE("\r\nSave ACC_EVENTS_VS_ID:");
//      PRINTF_VERBOSE("\r\n\tThsUsageType=%s",ThresholdsUsageName[ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType]);
//      PRINTF_VERBOSE("\r\n\tTh1.Ui8Value=%d",ConfiguratedVirtualSensorsArray[SensorNum]->Th1.Ui16Value);
//      if(ConfiguratedVirtualSensorsArray[SensorNum]->ThsUsageType<TH_LESS) {
//        PRINTF_VERBOSE("\r\n\tTh2.Ui8Value=%d",ConfiguratedVirtualSensorsArray[SensorNum]->Th2.Ui16Value);
//      }
    }
    PRINTF_VERBOSE("\r\n");
  }
}


/**
* @brief  Evaluates accelerometer normalization vector approximation in mg value
* @param  CUSTOM_MOTION_SENSOR_Axes_t Value_XYZ_mg
* @param  uint16_t AccValueMin
* @param  uint16_t AccValueMax
* @retval int32_t Normalization value
*/  
static int32_t AccNormVectorApproxEvaluator(CUSTOM_MOTION_SENSOR_Axes_t Value_XYZ_mg, uint16_t AccValueMin, uint16_t AccValueMax)
{
  int32_t Ax,Ay,Az,SumABS,MaxABS;
  int32_t AccNormVector;
  
  Ax =  Value_XYZ_mg.x;
  if (Ax<0) {
    Ax=-Ax;
  }
  MaxABS=Ax;
  
  Ay =  Value_XYZ_mg.y;
  if (Ay<0) {
    Ay=-Ay;
  }
  if(Ay>MaxABS) {
    MaxABS=Ay;
  }
  
  Az =  Value_XYZ_mg.z;
  if (Az<0) {
    Az=-Az;
  }
  if(Az>MaxABS) {
    MaxABS=Az;
  }
  
  SumABS = Ax+Ay+Az;
  
  /* Vector norm approximation in mg (error +/-7.5%) */
  AccNormVector = (45*SumABS + 77*MaxABS) >> 7;
  
  if (AccNormVector<(AccValueMin)) {
    AccNormVector = AccValueMin;
  }
  
  if (AccNormVector>(AccValueMax)) {
    AccNormVector = AccValueMax;
  }
  
  return AccNormVector;
}


int32_t BSP_GPO_Init( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Pin   = ST25DV_INT_PIN_GPO_EXTI_PIN; 
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init( ST25DV_INT_PIN_GPO_EXTI_PORT, &GPIO_InitStruct );

  (void)HAL_EXTI_GetHandle(&GPO_EXTI, ST25DV_INT_PIN_GPO_EXTI_LINE);
#if USE_ASTRA_MAINBOARD_BSP
  (void)HAL_EXTI_RegisterCallback(&GPO_EXTI,  HAL_EXTI_COMMON_CB_ID, ST25DV_INT_PIN_GPO_EXTI_CB);
#endif //USE_ASTRA_MAINBOARD_BSP

  /* Enable interruption */
  HAL_NVIC_SetPriority( ST25DV_INT_PIN_GPO_EXTI_IRQn, BSP_NFCTAG_GPO_PRIORITY, 0 );
  HAL_NVIC_EnableIRQ( ST25DV_INT_PIN_GPO_EXTI_IRQn );

  return BSP_ERROR_NONE;
}


int32_t BSP_GPO_Deinit( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Pin   = ST25DV_INT_PIN_GPO_EXTI_PIN; 
  GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init( ST25DV_INT_PIN_GPO_EXTI_PORT, &GPIO_InitStruct );

//   (void)HAL_EXTI_GetHandle(&GPO_EXTI, ST25DV_INT_PIN_GPO_EXTI_LINE);
// #if USE_ASTRA_MAINBOARD_BSP
//   (void)HAL_EXTI_RegisterCallback(&GPO_EXTI,  HAL_EXTI_COMMON_CB_ID, ST25DV_INT_PIN_GPO_EXTI_CB);
// #endif //USE_ASTRA_MAINBOARD_BSP

  /* Disable interruption */
  HAL_NVIC_DisableIRQ( ST25DV_INT_PIN_GPO_EXTI_IRQn );

  return BSP_ERROR_NONE;
}

void APP_NFC_CheckRFActivity(void)
{
  if(PlatformStatus.b.NFC_RF_ACTIVITY)
  {
    PlatformStatus.b.NFC_RF_ACTIVITY = 0;
    
    //Disable Interrupt from MEMS
    HAL_NVIC_DisableIRQ(EXTI4_IRQn);
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    
    //Detect RF Activity
    SmarTagAppDetectRFActivity();
    
    //Enable Interrupt from MEMS
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    
  }
}


void  STNFC_Error(SNFC_ErrorCode_t ErroCode, char *file, int32_t line)
{
  PRINTF_VERBOSE("%s@%d:",file,line);

  switch(ErroCode) {
   case STNFC_RUNTIME_ERROR:
      PRINTF_VERBOSE("STNFC_RUNTIME_ERROR\r\n");
      break;
    case STNFC_INIT_ERROR:
      PRINTF_VERBOSE("STNFC_INIT_ERROR\r\n");
      break;
    case STNFC_CONFIG_ERROR:
      PRINTF_VERBOSE("STNFC_CONFIG_ERROR\r\n");
      break;
    case STNFC_WRITING_ERROR:
      PRINTF_VERBOSE("STNFC_WRITING_ERROR\r\n");
      break;
    case STNFC_READING_ERROR:
      PRINTF_VERBOSE("STNFC_READING_ERROR\r\n");
      break;
  }
  
  switch(ErroCode) {
    case STNFC_RUNTIME_ERROR:
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_COLOR_MAGENTA);
      HAL_Delay(100);
      AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(500);
#endif //USE_NFC_LED
    case STNFC_INIT_ERROR:
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_COLOR_MAGENTA);
      HAL_Delay(100);
      AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(500);
#endif //USE_NFC_LED
    case STNFC_CONFIG_ERROR:
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_COLOR_MAGENTA);
      HAL_Delay(100);
      AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(500);
#endif //USE_NFC_LED
    case STNFC_WRITING_ERROR:
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_COLOR_MAGENTA);
      HAL_Delay(100);
      AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(500);
#endif //USE_NFC_LED
    case STNFC_READING_ERROR:
#if USE_NFC_LED
      AstraLedColor(ASTRA_LED_COLOR_MAGENTA);
      HAL_Delay(100);
      AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(500);
#endif //USE_NFC_LED
  }
}


void APP_NFC_SaveData(void)
{
  uint32_t DataBuf32;
  float TempFloatValue;
  uint16_t TempUnsignedIntValue;
  uint16_t ValueToCheck;
  
  if (NEW_DATA_IS_SET(AstraEngData.f.POW_MAN_NEW_DATA, nfc) || NEW_DATA_IS_SET(AstraEngData.f.ENVIRONMENTAL_NEW_DATA, nfc) || NEW_DATA_IS_SET(AstraEngData.f.INERTIAL_NEW_DATA, nfc)) {
    PRINTF_VERBOSE("There are new data to be saved in the NFC memory\r\n");
  }      
  
  
  if (NEW_DATA_IS_SET(AstraEngData.f.POW_MAN_NEW_DATA, nfc))
  {
    /* Save battery charge percentage values */
    if (AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Enable)
    {
      ValueToCheck = BATT_PERCENTAGE_SAMPLE_TO_CODED(AstraGetBatteryLevelPerc()/10);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui16,AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi16t(&AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].SampleDeltaDateTime!=0) {
        TempFloatValue = BATT_VOLTAGE_CODED_TO_SAMPLE(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Sample.Ui16Value);
        PRINTF_VERBOSE("\tSave battery charge percentage =%2.1f%% in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = BATT_PERCENTAGE_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].SampleDeltaDateTime)<<3);
        
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinDeltaDateTime!=0)  {
        TempFloatValue = BATT_VOLTAGE_CODED_TO_SAMPLE(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new min value for battery percentage = %2.1f %% in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MinDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxDeltaDateTime!=0)  {
        TempFloatValue = BATT_VOLTAGE_CODED_TO_SAMPLE(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new max value for battery percentage = %2.1f %% in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[BATT_PERCENTAGE_VS_ID].MaxDeltaDateTime=0;
      }
      
    }
    
    /* Save battery voltage values */
    if (AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Enable)
    {
      ValueToCheck = BATT_VOLTAGE_SAMPLE_TO_CODED(AstraEngData.d.batteryVoltage);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui16,AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi16t(&AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].SampleDeltaDateTime!=0) { 
        PRINTF_VERBOSE("\tSave battery voltage = %u mV in NFC memory\r\n", AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Sample.Ui16Value);
        
        DataBuf32 = BATT_VOLTAGE_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].SampleDeltaDateTime)<<BITS_LENGTH_VSID);
        
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinDeltaDateTime!=0)  {
        TempUnsignedIntValue = BATT_VOLTAGE_CODED_TO_SAMPLE(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new min value for battery voltage = %u mV in NFC memory\r\n", TempUnsignedIntValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MinDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxDeltaDateTime!=0)  {
        TempUnsignedIntValue = BATT_VOLTAGE_CODED_TO_SAMPLE(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new max value for battery voltage= %u mV in NFC memory\r\n", TempUnsignedIntValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[BATT_VOLTAGE_VS_ID].MaxDeltaDateTime=0;
      }
      
    }
    NEW_DATA_CLEAR(AstraEngData.f.POW_MAN_NEW_DATA, nfc);
  }
  
  
  if (NEW_DATA_IS_SET(AstraEngData.f.ENVIRONMENTAL_NEW_DATA, nfc))
  {
    /* Save STTS22H_VS values */
    if (AllVirtualSensorsArray[STTS22H_VS_ID].Enable)
    {
      ValueToCheck = STTS22H_SAMPLE_TO_CODED(AstraEngData.d.TempValue);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui16,AllVirtualSensorsArray[STTS22H_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi16t(&AllVirtualSensorsArray[STTS22H_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[STTS22H_VS_ID].SampleDeltaDateTime!=0) {
        TempFloatValue = STTS22H_CODED_TO_SAMPLE(AllVirtualSensorsArray[STTS22H_VS_ID].Sample.Ui16Value);
        PRINTF_VERBOSE("\tSave temperature (from STS22H) = %2.1f Celsius degrees in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = STTS22H_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].SampleDeltaDateTime)<<3);
        
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[STTS22H_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[STTS22H_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[STTS22H_VS_ID].MinDeltaDateTime!=0)  {
        TempFloatValue = STTS22H_CODED_TO_SAMPLE(AllVirtualSensorsArray[STTS22H_VS_ID].MinValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new min temperature value (from STS22H) = %2.1f Celsius degrees in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[STTS22H_VS_ID].MinDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].MinValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[STTS22H_VS_ID].MinPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[STTS22H_VS_ID].MinDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[STTS22H_VS_ID].MaxDeltaDateTime!=0)  {
        TempFloatValue = STTS22H_CODED_TO_SAMPLE(AllVirtualSensorsArray[STTS22H_VS_ID].MaxValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new max temperature value (from STS22H) = %2.1f Celsius degrees in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[STTS22H_VS_ID].MaxDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[STTS22H_VS_ID].MaxValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[STTS22H_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[STTS22H_VS_ID].MaxDeltaDateTime=0;
      }
      
    }
    
    
    /* Save LPS22HH_VS values */
    if (AllVirtualSensorsArray[LPS22HH_VS_ID].Enable) {
      ValueToCheck = LPS22HH_SAMPLE_TO_CODED(AstraEngData.d.PressValue);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui16,AllVirtualSensorsArray[LPS22HH_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi16t(&AllVirtualSensorsArray[LPS22HH_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[LPS22HH_VS_ID].SampleDeltaDateTime!=0) {
        TempFloatValue = LPS22HH_CODED_TO_SAMPLE(AllVirtualSensorsArray[LPS22HH_VS_ID].Sample.Ui16Value);
        PRINTF_VERBOSE("\tSave pressure (from LPS22HH) = %f hPa in NFC memory\r\n", TempFloatValue);        
        
        DataBuf32 = LPS22HH_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].SampleDeltaDateTime)<<3);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[LPS22HH_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[LPS22HH_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[LPS22HH_VS_ID].MinDeltaDateTime!=0)  {
        TempFloatValue = LPS22HH_CODED_TO_SAMPLE(AllVirtualSensorsArray[LPS22HH_VS_ID].MinValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new min pressure value (from LPS22HH) = %f hPa in NFC memory\r\n", TempFloatValue);
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[LPS22HH_VS_ID].MinDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].MinValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[LPS22HH_VS_ID].MinPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[LPS22HH_VS_ID].MinDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[LPS22HH_VS_ID].MaxDeltaDateTime!=0)  {
        TempFloatValue = LPS22HH_CODED_TO_SAMPLE(AllVirtualSensorsArray[LPS22HH_VS_ID].MaxValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new max pressure value (from LPS22HH) = %f hPa in NFC memory\r\n", TempFloatValue);        
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[LPS22HH_VS_ID].MaxDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[LPS22HH_VS_ID].MaxValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[LPS22HH_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[LPS22HH_VS_ID].MaxDeltaDateTime=0;
      }
      
    }
    
    /* Save HTS221_VS values */
    if (AllVirtualSensorsArray[HTS221_VS_ID].Enable) 
    {
      ValueToCheck = HTS221_SAMPLE_TO_CODED(AstraEngData.d.HumValue);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui8,AllVirtualSensorsArray[HTS221_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi8t(&AllVirtualSensorsArray[HTS221_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[HTS221_VS_ID].SampleDeltaDateTime!=0)
      {
        TempFloatValue = HTS221_CODED_TO_SAMPLE(AllVirtualSensorsArray[HTS221_VS_ID].Sample.Ui16Value);
        PRINTF_VERBOSE("\tSave humidity (from HTS221) = %f %% in NFC memory\r\n", TempFloatValue); 
        
        DataBuf32 = HTS221_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].SampleDeltaDateTime)<<3);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[HTS221_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[HTS221_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[HTS221_VS_ID].MinDeltaDateTime!=0)  {
        TempFloatValue = HTS221_CODED_TO_SAMPLE(AllVirtualSensorsArray[HTS221_VS_ID].MinValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new min humidity value (from HTS221) = %f %% in NFC memory\r\n", TempFloatValue);         
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[HTS221_VS_ID].MinDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].MinValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[HTS221_VS_ID].MinPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[HTS221_VS_ID].MinDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[HTS221_VS_ID].MaxDeltaDateTime!=0)  {
        TempFloatValue = HTS221_CODED_TO_SAMPLE(AllVirtualSensorsArray[HTS221_VS_ID].MaxValue.Ui16Value);
        PRINTF_VERBOSE("\tSave new max humidity value (from HTS221) = %f %% in NFC memory\r\n", TempFloatValue);   
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[HTS221_VS_ID].MaxDeltaDateTime) |
          (((uint32_t)AllVirtualSensorsArray[HTS221_VS_ID].MaxValue.Ui16Value)<<20);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[HTS221_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        AllVirtualSensorsArray[HTS221_VS_ID].MaxDeltaDateTime=0;
      }
      
    }
    NEW_DATA_CLEAR(AstraEngData.f.ENVIRONMENTAL_NEW_DATA, nfc);
  }
  
  if (NEW_DATA_IS_SET(AstraEngData.f.INERTIAL_NEW_DATA, nfc))
  {
    /* Save LIS2DTW12_VS_ID values */
    if (AllVirtualSensorsArray[LIS2DTW12_VS_ID].Enable) 
    {
      ValueToCheck = AccNormVectorApproxEvaluator(AstraEngData.d.LPAccValue, LIS2DTW12_MIN_VALUE, LIS2DTW12_MAX_VALUE);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui16,AllVirtualSensorsArray[LIS2DTW12_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi16t(&AllVirtualSensorsArray[LIS2DTW12_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[LIS2DTW12_VS_ID].SampleDeltaDateTime!=0) {
        TempUnsignedIntValue = AllVirtualSensorsArray[LIS2DTW12_VS_ID].Sample.Ui16Value;
        PRINTF_VERBOSE("\tSave acceleration (from LIS2DTW12) = %u mg in NFC memory\r\n", TempUnsignedIntValue); 
        
        DataBuf32 = LIS2DTW12_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[LIS2DTW12_VS_ID].SampleDeltaDateTime)<<3);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[LIS2DTW12_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[LIS2DTW12_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxDeltaDateTime!=0)  {
        TempUnsignedIntValue = AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxValue.Ui16Value;
        PRINTF_VERBOSE("\tSave new max acceleration value (from LIS2DTW12) = %u mg in NFC memory\r\n", TempUnsignedIntValue); 
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxDeltaDateTime);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        
        DataBuf32 = (uint32_t ) AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxValue.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxPositionPointer+4, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        
        AllVirtualSensorsArray[LIS2DTW12_VS_ID].MaxDeltaDateTime=0;
      }
    }
    
    /* Save LSM6DSOX32_VS_ID values */
    if (AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Enable) 
    {
      ValueToCheck = AccNormVectorApproxEvaluator(AstraEngData.d.AccValue, LSM6DSOX32_MIN_VALUE, LSM6DSOX32_MAX_VALUE);
      /* Check the Value respect Min and Max Limit Values*/
      MCR_STNFC_CompareWithLimits(Ui16,AllVirtualSensorsArray[LSM6DSOX32_VS_ID],ValueToCheck);
      /* Compare with Ths and Update the Max/Min Sample Value */
      STNFC_ComputeMaxMinCompareTHsUi16t(&AllVirtualSensorsArray[LSM6DSOX32_VS_ID],&LogDefinition,&hrtc);
      
      if(AllVirtualSensorsArray[LSM6DSOX32_VS_ID].SampleDeltaDateTime!=0) {
        TempUnsignedIntValue = AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Sample.Ui16Value;
        PRINTF_VERBOSE("\tSave acceleration (from LSM6DSOX32) = %u mg in NFC memory\r\n", TempUnsignedIntValue); 
        
        DataBuf32 = LSM6DSOX32_VS_ID |
          (((uint32_t)AllVirtualSensorsArray[LSM6DSOX32_VS_ID].SampleDeltaDateTime)<<3);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        DataBuf32 = AllVirtualSensorsArray[LSM6DSOX32_VS_ID].Sample.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
        LogDefinition.LastSamplePointer+=4;
        
        /* Update Sample Counter and Last Sample Pointer */
        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
        
        /* Increment the new Sample Counter until the end of the Tag */
        NfcType5_UpdateSampleCounter(&LogDefinition,8);
        
        AllVirtualSensorsArray[LSM6DSOX32_VS_ID].SampleDeltaDateTime=0;
      }
      
      if(AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxDeltaDateTime!=0)  {
        TempUnsignedIntValue = AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxValue.Ui16Value;
        PRINTF_VERBOSE("\tSave new max acceleration value (from LSM6DSOX32) = %u mg in NFC memory\r\n", TempUnsignedIntValue); 
        
        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxDeltaDateTime);
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        
        DataBuf32 = (uint32_t ) AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxValue.Ui16Value;
        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
                                AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxPositionPointer+4, 4)!=NFCTAG_OK){
                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
                                }
        
        AllVirtualSensorsArray[LSM6DSOX32_VS_ID].MaxDeltaDateTime=0;
      }
    }  
    
    /* Save ACC_EVENTS values */
//    if (AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Enable) 
//    {
//      ValueToCheck = (AstraEngData.d.LPAccInt.WakeUpStatus<<4)&ACC_EVENTS_WAKEUP_STATUS;
//      /* Check the Value respect Min and Max Limit Values*/
//      MCR_STNFC_CompareWithLimits(Ui8,AllVirtualSensorsArray[ACC_EVENTS_VS_ID],ValueToCheck);
//      /* Compare with Ths and Update the Max/Min Sample Value */
//      STNFC_ComputeMaxMinCompareTHsUi8t(&AllVirtualSensorsArray[ACC_EVENTS_VS_ID],&LogDefinition,&hrtc);
//      
//      if(AllVirtualSensorsArray[ACC_EVENTS_VS_ID].SampleDeltaDateTime!=0)
//      {
//        TempUnsignedIntValue = AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Sample.Ui8Value;
//        PRINTF_VERBOSE("\tSave accelerometer event = 0x%2x in NFC memory\r\n", TempUnsignedIntValue);
//        
//        DataBuf32 = ACC_EVENTS_VS_ID |
//          (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].SampleDeltaDateTime)<<3);
//        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
//          STNFC_Error_Handler(STNFC_WRITING_ERROR);
//        }
//        LogDefinition.LastSamplePointer+=4;
//        
//        DataBuf32 = AllVirtualSensorsArray[ACC_EVENTS_VS_ID].Sample.Ui8Value;
//        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32, LogDefinition.LastSamplePointer, 4)!=NFCTAG_OK){
//          STNFC_Error_Handler(STNFC_WRITING_ERROR);
//        }
//        LogDefinition.LastSamplePointer+=4;
//        
//        /* Update Sample Counter and Last Sample Pointer */
//        UpdateLastSamplePointerAndSampleCounter(&LogDefinition);
//        
//        /* Increment the new Sample Counter until the end of the Tag */
//        NfcType5_UpdateSampleCounter(&LogDefinition,8);
//        
//        AllVirtualSensorsArray[ACC_EVENTS_VS_ID].SampleDeltaDateTime=0;
//      }
//      
//      if(AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinDeltaDateTime!=0)  {
//        TempUnsignedIntValue = AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxValue.Ui8Value;
//        PRINTF_VERBOSE("\tSave new min accelerometer event = 0x%2x in NFC memory\r\n", TempUnsignedIntValue);         
//        
//        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinDeltaDateTime) |
//          (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinValue.Ui8Value)<<20);
//        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
//                                AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinPositionPointer, 4)!=NFCTAG_OK){
//                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
//                                }
//        AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MinDeltaDateTime=0;
//      }
//      
//      if(AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxDeltaDateTime!=0)  {
//        TempUnsignedIntValue = AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxValue.Ui8Value;
//        PRINTF_VERBOSE("\tSave new max accelerometer event = 0x%2x in NFC memory\r\n", TempUnsignedIntValue);          
//        
//        DataBuf32 = STNFC_ToShortDeltaDateTime(AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxDeltaDateTime) |
//          (((uint32_t)AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxValue.Ui8Value)<<20);
//        if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, (uint8_t *)&DataBuf32,
//                                AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxPositionPointer, 4)!=NFCTAG_OK){
//                                  STNFC_Error_Handler(STNFC_WRITING_ERROR);
//                                }
//        AllVirtualSensorsArray[ACC_EVENTS_VS_ID].MaxDeltaDateTime=0;
//      }
//    }
    NEW_DATA_CLEAR(AstraEngData.f.INERTIAL_NEW_DATA, nfc);
  }
}


int32_t STNFC_SetSampleTime(uint16_t SampleTime)
{
  PlatformStatus.s.NFC_SAVING_INTERVAL = (SampleTime * 1000);
  setNFCDataSavingTimer(PlatformStatus.s.NFC_SAVING_INTERVAL, 1);

  return 1;
  
}

#endif //USE_MEMORY


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
