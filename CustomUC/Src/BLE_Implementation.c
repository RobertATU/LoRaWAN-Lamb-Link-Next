/**
  ******************************************************************************
  * @file    BLE_Implementation.c
  * @author  SRA Team
  * @version V1.0.0
  * @date    11-Apr-2022
  * @brief   BLE Implementation template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.c.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/** @addtogroup BLE_MANAGER BLE manager
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "astra_sysmng.h"

#include "custom_env_sensors.h"

#if !USE_BLE
__WEAK void BLE_InitCustomService(void) {}
__WEAK void BLE_SetCustomAdvertiseData(uint8_t *manuf_data){}
#endif //USE_BLE

#if USE_BLE
#include "BLE_Manager.h"

/** @addtogroup BLE_IMPLEMENTATION_Private_Variables BLE impementation - Private variables
  * @brief Private variables
  * @{
 */

/* Private Defines -----------------------------------------------------------*/
#define BLOCK_ON_HCI_ERROR      0
#define ASTRA_BLE_SET_DATETIME  1

/* Private Variables ---------------------------------------------------------*/
int32_t  NeedToClearSecureDB=0;
static uint16_t CustomCommandPageLevel=0;

#if ASTRA_BLE_SET_DATETIME
static char NewTimeString[] = "Thh:mm:ss"; //first char set to 'T' to recognize default string
static char NewDateString[] = "Twd/dd/mm/yy"; //first char set to 'T' to recognize default string
#endif //ASTRA_BLE_SET_DATETIME

/**
* @} BLE_IMPLEMENTATION_Private_Variables
*/

/** @addtogroup BLE_IMPLEMENTATION_Private_FunctionPrototype BLE manager - Private function prototype
  * @brief Private function prototype
  * @{
 */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  Event BLE_NOTIFY_SUB / BLE_NOTIFY_UNSUB
  * @retval none
  */
static void AstraCustomNotifyEventEnv(BLE_NotifyEvent_t Event);

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  Event BLE_NOTIFY_SUB / BLE_NOTIFY_UNSUB
  * @retval none
  */
static void AstraCustomNotifyEventInertial(BLE_NotifyEvent_t Event);

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  Event BLE_NOTIFY_SUB / BLE_NOTIFY_UNSUB
  * @retval none
  */
static void AstraCustomNotifyEventBattery(BLE_NotifyEvent_t Event);

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  Event BLE_NOTIFY_SUB / BLE_NOTIFY_UNSUB
  * @retval none
  */
static void AstraCustomNotifyEventGnss(BLE_NotifyEvent_t Event);

/**
 * @brief  This function makes the parsing of the Debug Console Commands
 * @param  uint8_t *att_data attribute data
 * @param  uint8_t data_length length of the data
 * @retval uint32_t SendBackData true/false
 */
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length);

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None
 * @retval None
 */
static void DisconnectionCompletedFunction(void);

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  None
 * @retval None
 */
static void ConnectionCompletedFunction(uint16_t ConnectionHandle);

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  None
 * @retval None
 */
static void HardwareErrorEventHandlerFunction(uint8_t Hardware_Code);

/**********************************************************************************************
 * Callback functions prototypes to manage the extended configuration characteristic commands *
 **********************************************************************************************/

/**
 * @brief  Callback Function for managing the custom command
 * @param  BLE_CustomCommadResult_t *CustomCommand:
 * @param                            uint8_t *CommandName: Nome of the command
 * @param                            CustomCommand->CommandType: Type of the command
 * @param                            int32_t IntValue:    Integer or boolean parameter
 * @param                            uint8_t *StringValue: String parameter
 * @retval None
 */
static void ExtConfigCustomCommandCallback(BLE_CustomCommadResult_t *CustomCommand);

/**
 * @brief  Callback Function for managing the DFU command
 * @param  None
 * @retval None
 */
static void ExtConfigRebootOnDFUModeCommandCallback(void);

/**
 * @brief  Callback Function for managing the PowerOff command
 * @param  None
 * @retval None
 */
static void ExtConfigPowerOffCommandCallback(void);

/**
 * @brief  Callback Function for managing the SetName command
 * @param  uint8_t *NewName
 * @retval None
 */
static void ExtConfigSetNameCommandCallback(uint8_t *NewName);

/**
 * @brief  Custom commands definition
 * @param  JSON_Array *JSON_SensorArray
 * @retval None
 */
static void ExtConfigReadCustomCommandsCallback(JSON_Array *JSON_SensorArray);

/**
 * @brief  Callback Function for managing the SetTime command
 * @param  uint8_t *NewTime
 * @retval None
 */
static void ExtConfigSetTimeCommandCallback(uint8_t *NewTime);

/**
 * @brief  Callback Function for managing the SetDate command
 * @param  uint8_t *NewDate
 * @retval None
 */
static void ExtConfigSetDateCommandCallback(uint8_t *NewDate);

/**
 * @brief  Callback Function for managing the ReadCert command
 * @param  uint8_t *Certificate to register
 * @retval None
 */
static void ExtConfigReadCertCommandCallback(uint8_t *Certificate);

/**
 * @brief  Callback Function for answering to the UID command
 * @param  uint8_t **UID STM32 UID Return value
 * @retval None
 */
static void ExtExtConfigUidCommandCallback(uint8_t **UID);

/**
 * @brief  Callback Function for answering to Info command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigInfoCommandCallback(uint8_t *Answer);

/**
 * @brief  Callback Function for answering to Help command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigHelpCommandCallback(uint8_t *Answer);

/**
 * @brief  Callback Function for answering to PowerStatus command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigPowerStatusCommandCallback(uint8_t *Answer);

/**
 * @brief  Callback Function for answering to VersionFw command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigVersionFwCommandCallback(uint8_t *Answer);

#if USE_BLE_OTA
/**
 * @brief  This function is called when a Write operation is Request by the peer device.
 * @param  att_data     pointer to attribute data
 * @param  data_length  attribute data buffer lenght
 * @retval None
 */
static void WriteRequestOtaFunction(uint8_t *att_data, uint8_t data_length);

/**
 * @brief  This function is called to verify if the application supports the FUOTA
 * @retval uint8_t 1: OTA is supported 0: OTA is not supported
 */
static uint8_t CheckFwAppValidity( void );
#endif //USE_BLE_OTA

/**
* @} BLE_IMPLEMENTATION_Private_FunctionPrototype
*/

/** @addtogroup BLE_IMPLEMENTATION_Function_Definitions BLE manager - Function definitions
  * @brief Function definitions
  * @{
 */

/* Functions Definition ------------------------------------------------------*/
static SVCCTL_EvtAckStatus_t Custom_STM_Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blue_aci *blue_evt;
  aci_gatt_attribute_modified_event_rp0    * attribute_modified;
  aci_gatt_read_permit_req_event_rp0 *read_permit_req;

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch(event_pckt->evt)
  {
  case EVT_VENDOR:
    {
      blue_evt = (evt_blue_aci*)event_pckt->data;
      switch(blue_evt->ecode)
      {
        /* Handle Read request from GATT Client */
      case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          read_permit_req = (aci_gatt_read_permit_req_event_rp0*)blue_evt->data;

          aci_gatt_read_permit_req_event(read_permit_req->Connection_Handle,
                                         read_permit_req->Attribute_Handle,
                                         read_permit_req->Offset);
          break;
        }

        /* Handle Write request or Notification enabling from GATT Client */
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          attribute_modified = (aci_gatt_attribute_modified_event_rp0*)blue_evt->data;

          return_value = SVCCTL_EvtAckFlowEnable;

          aci_gatt_attribute_modified_event_BLE(attribute_modified->Connection_Handle,
                                                attribute_modified->Attr_Handle,
                                                attribute_modified->Offset,
                                                attribute_modified->Attr_Data_Length,
                                                attribute_modified->Attr_Data);
          break;
        }

      default:
        break;
      }
      break; /* HCI_EVT_VENDOR_SPECIFIC */
    }

  default:
    break;
  }

  return(return_value);
} /* end Custom_STM_Event_Handler */

void SVCCTL_InitCustomSvc(void)
{

  //Char_UUID_t  uuid;
/* USER CODE BEGIN SVCCTL_InitCustomSvc_1 */

/* USER CODE END SVCCTL_InitCustomSvc_1 */

  /**
   *	Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(Custom_STM_Event_Handler);

  BluetoothInit();

/* USER CODE BEGIN SVCCTL_InitCustomSvc_2 */

/* USER CODE END SVCCTL_InitCustomSvc_2 */

  return;
}

void BluetoothInit(void)
{
  /* BLE stack setting */
  BLE_StackValue.ConfigValueOffsets                   = CONFIG_DATA_PUBADDR_OFFSET;
  BLE_StackValue.ConfigValuelength                    = CONFIG_DATA_PUBADDR_LEN;
  BLE_StackValue.GAP_Roles                            = GAP_PERIPHERAL_ROLE;
  BLE_StackValue.IO_capabilities                      = IO_CAP_DISPLAY_ONLY;
  BLE_StackValue.AuthenticationRequirements           = BONDING;
  BLE_StackValue.MITM_ProtectionRequirements          = MITM_PROTECTION_REQUIRED;

  BLE_StackValue.AdvIntervalMin= SystemConfParams.BLE_ADV_INTV_MIN;
  BLE_StackValue.AdvIntervalMax= SystemConfParams.BLE_ADV_INTV_MAX;

  BLE_StackValue.OwnAddressType = PUBLIC_ADDR;

  /* To set the TX power level of the bluetooth device ( -2,1 dBm )*/
  BLE_StackValue.EnableHighPowerMode= 1; /*  High Power */

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  BLE_StackValue.PowerAmplifierOutputLevel =4;

  /* BLE services setting */
  BLE_StackValue.EnableConfig    = 1;
  BLE_StackValue.EnableConsole   = 1;
  BLE_StackValue.EnableExtConfig = 1;

  /* For Enabling the Secure Connection */
  BLE_StackValue.EnableSecureConnection=0;
  /* Default Secure PIN */
  BLE_StackValue.SecurePIN=123456;

  BLE_StackValue.EnableRandomSecurePIN = 0;

  BLE_StackValue.AdvertisingFilter    = NO_WHITE_LIST_USE;

  if(BLE_StackValue.EnableSecureConnection) {
    /* Using the Secure Connection, the Rescan should be done by BLE chip */
    BLE_StackValue.ForceRescan =0;
  } else {
    BLE_StackValue.ForceRescan =1;
  }

  InitBleManager();
}

void BLE_InitCustomService(void) {
  /* Define Custom Function for Debug Console Command parsing */
  CustomDebugConsoleParsingCallback = &DebugConsoleCommandParsing;

  /* Define Custom Function for Connection Completed */
  CustomConnectionCompleted = &ConnectionCompletedFunction;

  /* Define Custom Function for Disconnection Completed */
  CustomDisconnectionCompleted = &DisconnectionCompletedFunction;

  CustomHardwareErrorEventHandler = &HardwareErrorEventHandlerFunction;

  /***********************************************************************************
   * Callback functions to manage the extended configuration characteristic commands *
   ***********************************************************************************/
  CustomExtConfigCustomCommandCallback = &ExtConfigCustomCommandCallback;
  CustomExtConfigUidCommandCallback = ExtExtConfigUidCommandCallback;
  CustomExtConfigRebootOnDFUModeCommandCallback = &ExtConfigRebootOnDFUModeCommandCallback;
  CustomExtConfigPowerOffCommandCallback = &ExtConfigPowerOffCommandCallback;
  CustomExtConfigSetNameCommandCallback = &ExtConfigSetNameCommandCallback;
  CustomExtConfigReadCustomCommandsCallback = &ExtConfigReadCustomCommandsCallback;
  CustomExtConfigSetTimeCommandCallback = &ExtConfigSetTimeCommandCallback;
  CustomExtConfigSetDateCommandCallback = &ExtConfigSetDateCommandCallback;
  CustomExtConfigReadCertCommandCallback = &ExtConfigReadCertCommandCallback;
  CustomExtConfigInfoCommandCallback = &ExtConfigInfoCommandCallback;
  CustomExtConfigHelpCommandCallback = &ExtConfigHelpCommandCallback;
  CustomExtConfigPowerStatusCommandCallback = &ExtConfigPowerStatusCommandCallback;
  CustomExtConfigVersionFwCommandCallback = &ExtConfigVersionFwCommandCallback;

  /**
  * For each features, user can assign here the pointer at the function for the read request data.
  * For example for the environmental features:
  *
  * CustomReadRequestEnv = &ReadRequestEnvFunction;
  *
  * User can define and insert in the BLE_Implementation.c source code the functions for the read request data
  * ReadRequestEnvFunction function is already defined.
  *
  */

  /**
  * User can added here the custom service initialization for the selected BLE features.
  * For example for the environmental features */

  /* Service initialization and adding for the environmental features */

  CustomNotifyEventEnv = AstraCustomNotifyEventEnv;
  /* BLE_InitEnvService(PressEnable,HumEnable,NumTempEnabled[0,1,2]) */
  BleManagerAddChar(BLE_InitEnvService(1,1,1));

  /* Service initialization and adding  for the inertial features */
  CustomNotifyEventInertial = AstraCustomNotifyEventInertial;
  /* BLE_InitInertialService(AccEnable,GyroEnable,MagEnabled) */
  BleManagerAddChar(BLE_InitInertialService(1,1,0));

  /* Service initialization and adding for the battery features */
  CustomNotifyEventBattery = AstraCustomNotifyEventBattery;
  BleManagerAddChar(BLE_InitBatteryService());

  /* Service initialization and adding for the GNSS features */
  CustomNotifyEventGnss = AstraCustomNotifyEventGnss;
  BleManagerAddChar(BLE_InitGnssService());
#if USE_BLE_OTA
  if(CheckFwAppValidity())
  {
    CustomWriteRequestOta = &WriteRequestOtaFunction;
    
    /* Service initialization and adding for the OTA features */
    BleManagerAddChar(BLE_InitOtaService());
  }
#endif //USE_BLE_OTA
}

void BLE_SetCustomAdvertiseData(uint8_t *manuf_data)
{
  /**
  * User can add here the custom advertize data setting  for the selected BLE features.
  */

#ifndef BLE_MANAGER_SDKV2
  /* Custom advertize data setting for the environmental features */
  BLE_SetEnvAdvertizeData(manuf_data);
  /* Custom advertize data setting for the inertial features */
  BLE_SetInertialAdvertizeData(manuf_data);
  /* Custom advertize data setting for the battery features */
  BLE_SetBatteryAdvertizeData(manuf_data);
#else /* BLE_MANAGER_SDKV2 */

  manuf_data[BLE_MANAGER_CUSTOM_FIELD1]=BLE_VERSION_FW_ID; /* "ble_fw_id": "0x02", "fw_name": "FP-ATR-ASTRA1", */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2]=AstraGetBatteryLevelPerc()/10; /* 98->98% battery Level */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3]=AstraGetUseCase(); /* "format": "enum_string", 0->FLEET_MNG, 1->LIVESTOCK_MON, 2->GOODS_MONITORING, 3->LOGISTICS, 4-> CUSTOM*/
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4]=AstraGetEnumSysStatusIcon(); /* "format": "enum_icon", 1 LoRa joined, 2 LoRa NOT joined, 3 GPS fix OK, 4 GPS no fix, 5 GPS disabled, 6 System in low power, 7 NFC enabled */

#endif /* BLE_MANAGER_SDKV2 */
}

static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  uint32_t SendBackData = 1;

  /* Help Command */
  if(!strncmp("help",(char *)(att_data),4)) {
    /* Print Legend */
    SendBackData=0;

    BytesToWrite =sprintf((char *)BufferToWrite,"Command:\r\n"
       "info -> System Info\r\n"
       "versionFw  -> FW Version\r\n"
         );
    Term_Update(BufferToWrite,BytesToWrite);

    } else if(!strncmp("versionFw",(char *)(att_data),9)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"%s,%s_%s_%d.%d.%d\r\n",
                            BLE_STM32_MICRO,
                            BLE_STM32_BOARD,
                            BLE_FW_PACKAGENAME,
                            BLE_VERSION_FW_MAJOR,
                            BLE_VERSION_FW_MINOR,
                            BLE_VERSION_FW_PATCH);

      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0;

    } else if(!strncmp("info",(char *)(att_data),4)) {
      SendBackData=0;

      BytesToWrite =sprintf((char *)BufferToWrite,"\r\nSTMicroelectronics %s:\r\n"
          "\tVersion %c.%c.%c\r\n"
          "\t%s\r\n",
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,BLE_VERSION_FW_MINOR,BLE_VERSION_FW_PATCH,
          BLE_STM32_BOARD);
      Term_Update(BufferToWrite,BytesToWrite);

      BytesToWrite =sprintf((char *)BufferToWrite,"\t(HAL %ld.%ld.%ld_%ld)\r\n"
        "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n",
#elif defined (__CC_ARM)
        " (KEIL)\r\n",
#elif defined (__GNUC__)
        " (STM32CubeIDE)\r\n",
#endif
          HAL_GetHalVersion() >>24,
          (HAL_GetHalVersion() >>16)&0xFF,
          (HAL_GetHalVersion() >> 8)&0xFF,
           HAL_GetHalVersion()      &0xFF,
           __DATE__,__TIME__);

      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0;
    }
    else
    {
      AstraProcessCommand(att_data, data_length);
    }

  return SendBackData;
}

#if USE_BLE_OTA
static uint8_t CheckFwAppValidity( void )
{
  uint8_t status;
  uint32_t magic_keyword_address;
  uint32_t last_user_flash_address;

  magic_keyword_address = *(uint32_t*)(FLASH_BASE + (CFG_APP_START_SECTOR_INDEX * 0x1000 + 0x140));
  last_user_flash_address = (((READ_BIT(FLASH->SFR, FLASH_SFR_SFSA) >> FLASH_SFR_SFSA_Pos) << 12) + FLASH_BASE) - 4;
  if( (magic_keyword_address < FLASH_BASE) || (magic_keyword_address > last_user_flash_address) )
  {
    /**
     * The address is not valid
     */
    status = 0;
  }
  else
  {
    if( (*(uint32_t*)magic_keyword_address) != 0x94448A29  )
    {
      /**
       * A firmware update procedure did not complete
       */
      status = 0;
    }
    else
    {
      /**
       * The firmware application is available
       */
      status = 1;
    }
  }

  return status;
}

static void WriteRequestOtaFunction(uint8_t *att_data, uint8_t data_length)
{
  BLE_MANAGER_PRINTF("-- OTA SERVICE COMMAND: REBOOT REQUESTED\r\n");

  *(uint32_t*)SRAM1_BASE = *(uint32_t*)att_data;
  HAL_NVIC_SystemReset();
}
#endif //USE_BLE_OTA

void HardwareErrorEventHandlerFunction(uint8_t Hardware_Code)
{
  char *HWCodeString[] = {
    /* 0x00 */ "NaN",
    /* 0x01 */ "Radio state error",
    /* 0x02 */ "Timer overrun error",
    /* 0x03 */ "Internal queue overflow error"
  };

  BLE_MANAGER_PRINTF("\r\n-------->hci_hardware_error_event Hardware_Code=\r\n\t%s<--------\r\n",HWCodeString[Hardware_Code]);
  BLE_MANAGER_DELAY(1000);
  if(BLOCK_ON_HCI_ERROR)
  {
    while(1)
    {
      BoardLedPlay(2000);
    }
  }
  HAL_NVIC_SystemReset();
}

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None
 * @retval None
 */
static void DisconnectionCompletedFunction(void)
{
  BLE_MANAGER_PRINTF("Call to DisconnectionCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);
  DisconnectionComplete();
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  None
 * @retval None
 */
static void ConnectionCompletedFunction(uint16_t ConnectionHandle)
{
  BLE_MANAGER_PRINTF("Call to ConnectionCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);
  ConnectionComplete();
  CustomCommandPageLevel=0;
}

/***********************************************************************************
 * Callback functions to manage the extended configuration characteristic commands *
 ***********************************************************************************/

static void  ExtConfigCustomCommandCallback(BLE_CustomCommadResult_t *CustomCommand)
{
  BLE_MANAGER_PRINTF("Received Custom Command:\r\n");
  BLE_MANAGER_PRINTF("\tCommand Name: <%s>\r\n", CustomCommand->CommandName);
  BLE_MANAGER_PRINTF("\tCommand Type: <%d>\r\n", CustomCommand->CommandType);

  switch(CustomCommand->CommandType) {
  case BLE_CUSTOM_COMMAND_VOID:
    if(0==strcmp("System commands", (char *)CustomCommand->CommandName)) //submenu
    {
      CustomCommandPageLevel=1;
      SendNewCustomCommandList();
      CustomCommandPageLevel=0;
    }
    if(0==strcmp("System Status", (char *)CustomCommand->CommandName)) //submenu
    {
      char sysStatusString[] = "Sys status: Running. \nSys timer: 4000000000ms\nGNSS: DISABLED\nSENSORS: DISABLED\nLoRa fw: DISABLED";

      if(AstraGetEnumSysStatus() == 0)
      {
        //State_LowPower
        sprintf(sysStatusString, "Sys status: Low power");
      }
      else
      {
        //State_Run
        sprintf(sysStatusString, "Sys status: Running. \nSys timer: %ldms\nGNSS: %s\nSENSORS: %s\nLoRa fw: %s",
                PlatformStatus.s.SYS_PROC_INTERVAL,
                (PlatformStatus.b.GNSS_ENABLED)?"RUN":"DISABLED",
                (PlatformStatus.b.SENSORS_ENABLED)?"RUN":"DISABLED",
                (PlatformStatus.b.APP_LORA_ENABLED)?"RUN":"DISABLED"
                  );
      }

      SendInfo(sysStatusString);
    }
    if(0==strcmp("LED blink", (char *)CustomCommand->CommandName)) //submenu
    {
      char sysStatusString[] = "Led blink 2 seconds to identify the board";

      SendInfo(sysStatusString);

      BoardLedPlay(2000);
    }
    if(0==strcmp("Astra Settings", (char *)CustomCommand->CommandName)) //submenu
    {
      char sysStatusString[] = "Astra Engine: exampleGOODS_MONITORING\nLoRa interval: 1000000\nBLE interval: 1000000\nIndoor: 0/1";

      sprintf(sysStatusString, "Astra Engine: %s\nLoRa interval: %lds\nBLE interval: %ldms\nIndoor: %c",
              AstraGetUseCaseString(),
              PlatformStatus.s.LORA_SEND_INTERVAL/1000,
              PlatformStatus.s.BLE_SEND_INTERVAL,
              (PlatformStatus.b.GNSS_ENABLED)?'N':'Y');

      SendInfo(sysStatusString);
    }
    if(0==strcmp("LoRa conf", (char *)CustomCommand->CommandName)) //submenu
    {
      CustomCommandPageLevel=2;
      SendNewCustomCommandList();
      CustomCommandPageLevel=0;
    }
    if(0==strcmp("Board Reboot", (char*)CustomCommand->CommandName))
    {
      SendInfo("Rebooting...\nPlease wait reconnection");
      AstraBoardReboot();
    }
    if(0==strcmp("STM32WB CM0+", (char*)CustomCommand->CommandName))
    {
      char wbinfostring[] = "STM32WB CM0+ FW version: V%ld.%ld.%ld\nFUS version: V%ld.%ld.%ld";

      sprintf((char *)wbinfostring,"STM32WB CM0+ FW version: V%ld.%ld.%ld\nFUS version: V%ld.%ld.%ld",
              MAJOR_VERSION(PlatformStatus.s.STM32WB_CM0_VER), MINOR_VERSION(PlatformStatus.s.STM32WB_CM0_VER), REV_VERSION(PlatformStatus.s.STM32WB_CM0_VER),
              MAJOR_VERSION(PlatformStatus.s.STM32WB_CM0_FUS), MINOR_VERSION(PlatformStatus.s.STM32WB_CM0_FUS), REV_VERSION(PlatformStatus.s.STM32WB_CM0_FUS));
      SendInfo(wbinfostring);
    }
    if(0==strcmp("STM32WL info", (char*)CustomCommand->CommandName))
    {
      char wbinfostring[] = "STM32WL FW\nCM4: V%ld.%ld.%ld\nCM0+: V%ld.%ld.%ld\nMWLORA: V%ld.%ld.%ld\nMWRADIO: V%ld.%ld.%ld";

      sprintf((char *)wbinfostring,"STM32WL FW\nCM4: V%ld.%ld.%ld\nCM0+: V%ld.%ld.%ld\nMWLORA: V%ld.%ld.%ld\nMWRADIO: V%ld.%ld.%ld",
              MAJOR_VERSION(PlatformStatus.s.STM32WL_CM4_VER), MINOR_VERSION(PlatformStatus.s.STM32WL_CM4_VER), REV_VERSION(PlatformStatus.s.STM32WL_CM4_VER),
              MAJOR_VERSION(PlatformStatus.s.STM32WL_CM0_VER), MINOR_VERSION(PlatformStatus.s.STM32WL_CM0_VER), REV_VERSION(PlatformStatus.s.STM32WL_CM0_VER),
              MAJOR_VERSION(PlatformStatus.s.STM32WL_MWLORA_VER), MINOR_VERSION(PlatformStatus.s.STM32WL_MWLORA_VER), REV_VERSION(PlatformStatus.s.STM32WL_MWLORA_VER),
              MAJOR_VERSION(PlatformStatus.s.STM32WL_MWRADIO_VER), MINOR_VERSION(PlatformStatus.s.STM32WL_MWRADIO_VER), REV_VERSION(PlatformStatus.s.STM32WL_MWRADIO_VER));
      SendInfo(wbinfostring);
    }
    if(0==strcmp("STSAFE status", (char*)CustomCommand->CommandName))
    {
      if(!StSafeCheck())
      {
        SendInfo("Host Key is set : STSAFEA_BUS_OK");
      }
      else
      {
        SendInfo("Host key is not set, personalization of STSAFE-A has never been done before!");
      }
    }
    if(0==strcmp("<- Back to main menu", (char *)CustomCommand->CommandName))
    {
      CustomCommandPageLevel=0;
      SendNewCustomCommandList();
    }
    if(0==strcmp("GetStm32wlKeys", (char*)CustomCommand->CommandName))
    {
      char lorakey[] = "00000000000000000000000000000000";
      char stm32wlParams[] =  "STM32WLKEYS\nDevEUI:0000000000000000\n"\
        "AppEUI:0000000000000000\n"\
          "NwkKey:00000000000000000000000000000000\n"\
            "AppKey:00000000000000000000000000000000";
      AstraGetStm32wlDevEuiString(lorakey);
      memcpy(&stm32wlParams[19], lorakey, 16);
      AstraGetStm32wlAppEuiString(lorakey);
      memcpy(&stm32wlParams[43], lorakey, 16);
      AstraGetStm32wlNwkKeyString(lorakey);
      memcpy(&stm32wlParams[67], lorakey, 32);
      AstraGetStm32wlAppKeyString(lorakey);
      memcpy(&stm32wlParams[107], lorakey, 32);
      SendInfo(stm32wlParams);
    }
    if(0==strcmp("GetLoRaKeys", (char*)CustomCommand->CommandName))
    {
      char lorakey[] = "00000000000000000000000000000000";
      char LoraKeys[] =  "EEPROM_KEYS\nDevEUI:0000000000000000\n"\
        "AppEUI:0000000000000000\n"\
          "NwkKey:00000000000000000000000000000000\n"\
            "AppKey:00000000000000000000000000000000";
      AstraGetStm32wlDevEuiString(lorakey);
      memcpy(&LoraKeys[19], lorakey, 16);
      AstraGetLoRaAppEuiString(lorakey);
      memcpy(&LoraKeys[43], lorakey, 16);
      AstraGetLoRaNwkKeyString(lorakey);
      memcpy(&LoraKeys[67], lorakey, 32);
      AstraGetLoRaAppKeyString(lorakey);
      memcpy(&LoraKeys[107], lorakey, 32);
      SendInfo(LoraKeys);
    }
    if(0==strcmp("LoRa Status", (char*)CustomCommand->CommandName))
    {
      char tempstring[] = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\r\n";

      sprintf(tempstring, "LoRa status\nProcessing:%d\nJoined:%d\nADR:%s - DR:%d - REGION:%s",
              AstraGetLoraProcessing(),
              AstraGetLoraJoinStatus(),
              SystemConfParams.LORA_ADR?"ON":"OFF",
              SystemConfParams.LORA_DR,
              GET_REGION_STRING(SystemConfParams.LORA_REGION));

      SendInfo(tempstring);
    }
    #if USE_BOOT_UPGRADE
      if(0==strcmp("Bootloader Upgrade", (char*)CustomCommand->CommandName))
      {
        //AstraSetLoraAck(CustomCommand->IntValue);
        SendInfo("Bootloader is already up to date");
      }
    #endif //USE_BOOT_UPGRADE
    break;
  case BLE_CUSTOM_COMMAND_INTEGER:
    BLE_MANAGER_PRINTF("\tInt    Value: <%d>\r\n", CustomCommand->IntValue);
    if(0==strcmp("LoRa Send intv", (char*)CustomCommand->CommandName))
    {
      AstraSetLoraInterval(CustomCommand->IntValue);
      SendInfo("New settings saved. Please restart the board");
    }
    if(0==strcmp("Set LoRa DR", (char*)CustomCommand->CommandName))
    {
      AstraSetLoraDr(CustomCommand->IntValue);
      SendInfo("New settings saved. Please restart the board");
    }
    break;
  case BLE_CUSTOM_COMMAND_ENUM_INTEGER:
    BLE_MANAGER_PRINTF("\tInt     Enum: <%d>\r\n", CustomCommand->IntValue);
    break;
  case BLE_CUSTOM_COMMAND_BOOLEAN:
    BLE_MANAGER_PRINTF("\tInt    Value: <%d>\r\n", CustomCommand->IntValue);
    if(0==strcmp("LoRa config", (char*)CustomCommand->CommandName))
    {
      AstraSetLoraProcessing(CustomCommand->IntValue);
    }
    if(0==strcmp("GNSS config", (char*)CustomCommand->CommandName))
    {
      AstraSetGnssConfig(CustomCommand->IntValue);
    }
    if(0==strcmp("Sensors config", (char*)CustomCommand->CommandName))
    {
      AstraSetSensorsConfig(CustomCommand->IntValue);
    }
    if(0==strcmp("Set LoRa ADR", (char*)CustomCommand->CommandName))
    {
      AstraSetLoraAdr(CustomCommand->IntValue);
      SendInfo("New settings saved. Please restart the board");
    }
    if(0==strcmp("Set LoRa ACK", (char*)CustomCommand->CommandName))
    {
      AstraSetLoraAck(CustomCommand->IntValue);
      SendInfo("New settings saved. Please restart the board");
    }
    if(0==strcmp("Inactivity Ultra Low Power", (char*)CustomCommand->CommandName))
    {
      SystemConfParams.b.CONF_MEMS_REBOOT = (CustomCommand->IntValue);
      if (!CustomCommand->IntValue)
      {
        ClearScheduleAstraSmChangeState();
      }
      AstraSaveDefaultParams();
      SendInfo("New settings saved.");
    }
#if USE_BOOT_UPGRADE
    if(0==strcmp("Bootloader Upgrade", (char*)CustomCommand->CommandName))
    {
      if(CustomCommand->IntValue)
      {
        SendInfo("The Bootloader will be updated...");
        
        AstraRebootBOOTUpdate();
      }
    }
#endif //USE_BOOT_UPGRADE
    if(0==strcmp("SmartNFC reset", (char*)CustomCommand->CommandName))
    {
      if(CustomCommand->IntValue)
      {
        SendInfo("The NFC memory is formatting ...");
        
        AstraNfcDataInit();
      }
    }
    break;
  case  BLE_CUSTOM_COMMAND_STRING:
    BLE_MANAGER_PRINTF("\tString Value: <%s>\r\n", CustomCommand->StringValue);
    if(0 == strcmp("STSAFE personalization", (char*)CustomCommand->CommandName))
    {
      if(0 == strcmp("pers", (char*)CustomCommand->StringValue))
      {
        SendInfo("Personalization scheduled. Issue a 'STSAFE status' cmd.");
        PlatformStatus.s.STSAFE_PERS = 'p' | 'e'<<8 | 'r'<<16 | 's'<<24;
      }
      else
      {
        SendError("Wrong confirmation string. No action performed. Type 'pers'");
      }
    }
    if(0==strcmp("SetLoRaAppEui", (char*)CustomCommand->CommandName))
    {
      if(0 == AstraSetLoRaAppEui((char*)CustomCommand->StringValue))
      {
        SendInfo("SetLoRaAppEui command OK");
      }
      else
      {
        SendError("Error on\nSetLoRaAppEui command");
      }
    }
    if(0==strcmp("SetLoRaAppKey", (char*)CustomCommand->CommandName))
    {
      if(0 == AstraSetLoRaAppKey((char*)CustomCommand->StringValue))
      {
        SendInfo("SetLoRaAppKey command OK");
      }
      else
      {
        SendError("Error on\nSetLoRaAppKey command");
      }
    }
    if(0==strcmp("SetLoRaNwkKey", (char*)CustomCommand->CommandName))
    {
      if(0 == AstraSetLoRaNwkKey((char*)CustomCommand->StringValue))
      {
        SendInfo("SetLoRaNwkKey command OK");
      }
      else
      {
        SendError("Error on\nSetLoRaNwkKey command");
      }
    }
    break;
  case  BLE_CUSTOM_COMMAND_ENUM_STRING:
    BLE_MANAGER_PRINTF("\tString  Enum: <%s>\r\n", CustomCommand->StringValue);
    if(0==strcmp("ASTRA ENGINE", (char*)CustomCommand->CommandName))
    {
      AstraSetEngine((char*)CustomCommand->StringValue);
    }
    if(0==strcmp("Dbg Console Enum", (char*)CustomCommand->CommandName))
    {
      AstraSetVerbLevel((char*)CustomCommand->StringValue);
    }
    if(0==strcmp("Set LoRa region", (char*)CustomCommand->CommandName))
    {
      AstraSetLoraRegion((char*)CustomCommand->StringValue);
      SendInfo("New settings saved. Please restart the board");
    }
    if(0==strcmp("Ultra Low Power State", (char*)CustomCommand->CommandName))
    { 
      BLE_MANAGER_PRINTF("\t String selected: <%s>\r\n", CustomCommand->StringValue);      
      AstraULP_Schedule((char*)CustomCommand->StringValue);
      SendInfo("The device will be restarted...");
    }
    break;
  }

  /* Insert here the code for managing the received command */

  /* ToBeChanged*/

  /* Understand if it's a valid Command or not */
}

static void ExtConfigReadCustomCommandsCallback(JSON_Array *JSON_SensorArray)
{
  /* Clear the previous Costom Command List */
  ClearCustomCommandsList();
  
  if(CustomCommandPageLevel==0)
  {
#if USE_BOOT_UPGRADE
    BLE_CustomCommandTypes_t typeCmd;
    bool hasNewVersion = false;
    char strUpdaterBoot[] = "Current BOOT FW version: V%ld.%ld.%ld\nAvailable BOOT FW version: V%ld.%ld.%ld/\ndo you want to update?";
    
    hasNewVersion = AstraHasNewBootFwAvailable(); 
    
    // force enable for downgrade option 
    hasNewVersion = true;
    
    if(hasNewVersion)
    {
      typeCmd = BLE_CUSTOM_COMMAND_BOOLEAN;
      sprintf((char *)strUpdaterBoot,"Current BOOT FW version: V%ld.%ld.%ld\nAvailable BOOT FW version: V%ld.%ld.%ld\ndo you want to update?",
              MAJOR_VERSION(bootVersionFw.currentVersion), MINOR_VERSION(bootVersionFw.currentVersion), REV_VERSION(bootVersionFw.currentVersion),
              MAJOR_VERSION(bootVersionFw.availableVersion), MINOR_VERSION(bootVersionFw.availableVersion), REV_VERSION(bootVersionFw.availableVersion));
    }
    else
    {
      typeCmd = BLE_CUSTOM_COMMAND_VOID;
      sprintf((char *)strUpdaterBoot,"Current BOOT FW version: V%ld.%ld.%ld", MAJOR_VERSION(bootVersionFw.currentVersion), MINOR_VERSION(bootVersionFw.currentVersion), REV_VERSION(bootVersionFw.currentVersion));
    }
    
    if(AddCustomCommand("Bootloader Upgrade", //Name
                        typeCmd, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        strUpdaterBoot, //Description
                        JSON_SensorArray)) 
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Bootloader Upgrade");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Bootloader Upgrade");
      return;
    }
#endif //USE_BOOT_UPGRADE
    
    if(AddCustomCommand("SmartNFC reset", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "SmartNFC protocol will be initialised", //Description
                        JSON_SensorArray)) 
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","SmartNFC reset");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","SmartNFC reset");
      return;
    }
    

    char const **OptNames = AstraGetULPwakeNames();
    
    if(AddCustomCommand("Ultra Low Power State", //Name
                        BLE_CUSTOM_COMMAND_ENUM_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        (void *)OptNames, //Enum String
                        "Select ULP wake type", //Description
                        JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Ultra Low Power State");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Ultra Low Power State");
      return;
    }

    if(AddCustomCommand("Inactivity Ultra Low Power", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        SystemConfParams.b.CONF_MEMS_REBOOT, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Ultra Low Power and wakeup triggered by MEMS events", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Inactivity Ultra Low Power");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Inactivity Ultra Low Power");
       return;
    }

    if(AddCustomCommand("System Status", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Get system status", //Description
                        JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","System Status");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","System Status");
      return;
    }

    if(AddCustomCommand("Astra Settings", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Get Astra Settings", //Description
                        JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Get Astra Settings");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Get Astra Settings");
      return;
    }

    //The Last value should be NULL
    char const **EngineUcNames = AstraGetEngineNames();

    if(AddCustomCommand("ASTRA ENGINE", //Name
                        BLE_CUSTOM_COMMAND_ENUM_STRING, //Type
                        AstraGetEngineEnum(), //Default Value
                        // AstraGetUseCase(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        (void *)EngineUcNames, //Enum String
                        "ASTRA ENGINE use case selection", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","ASTRA ENGINE");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","ASTRA ENGINE");
      return;
    }

    if(AddCustomCommand("LoRa conf", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Click for submenu", //Description
                        JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","LoRa conf");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","LoRa conf");
      return;
    }

    if(AddCustomCommand("LoRa Send intv", //Name
                        BLE_CUSTOM_COMMAND_INTEGER, //Type
                        AstraGetLoraInterval(), //Default Value
                        20, //MIN
                        24*3600,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Send interval on LoRa interface; just for this session", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","LoRa Send intv");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","LoRa Send intv");
       return;
    }

    if(AddCustomCommand("LED blink", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "LED blink to identify the board", //Description
                        JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","LED blink");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","LED blink");
      return;
    }

    if(AddCustomCommand("System commands", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Click for submenu", //Description
                        JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","System commands");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","System commands");
      return;
    }

  }
  else if(CustomCommandPageLevel==1)
  {
     if(AddCustomCommand("<- Back to main menu", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Come back to previous Menu", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","<- Back to main menu");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","<- Back to main menu");
       return;
    }

    if(AddCustomCommand("GNSS config", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        AstraGetGnssConfig(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "GNSS management (see status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","GNSS config");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","GNSS config");
       return;
    }

    if(AddCustomCommand("Sensors config", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        AstraGetSensorsConfig(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "SENSORS management (see status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Sensors config");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Sensors config");
       return;
    }

    //The Last value should be NULL
    char const **VerbLevelNames = AstraGetVerbLevelNames();

    if(AddCustomCommand("Dbg Console Enum", //Name
                        BLE_CUSTOM_COMMAND_ENUM_STRING, //Type
                        AstraGetVerbLevelEnum(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        (void *)VerbLevelNames, //Enum String
                        "Set debug console verbosity level", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Dbg Console Enum");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Dbg Console Enum");
      return;
    }

    if(AddCustomCommand("Board Reboot", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Reboot the board", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Board Reboot");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Board Reboot");
       return;
    }

    if(AddCustomCommand("STSAFE status", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "STSAFE-A status", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "STSAFE status");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "STSAFE status");
       return;
    }

    if(AddCustomCommand("STSAFE personalization", //Name
                        BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        4, //MIN "pers"
                        4,  //MAX "pers"
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Type 'pers' to confirm. STSAFE-A personalization, a one time operation that is successfull only if STSAFE-A's host key slot are virgin", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "STSAFE personalization");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "STSAFE personalization");
       return;
    }

    if(AddCustomCommand("STM32WB CM0+", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "STM32WB CM0+ fw info", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "STM32WB CM0+");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "STM32WB CM0+");
       return;
    }

    if(AddCustomCommand("STM32WL info", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "STM32WL fw info", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "STM32WL info");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "STM32WL info");
       return;
    }

  }
  else if(CustomCommandPageLevel==2)
  {
     if(AddCustomCommand("<- Back to main menu", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Come back to previous Menu", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","<- Back to main menu");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","<- Back to main menu");
       return;
    }

    if(AddCustomCommand("LoRa Status", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Get LoRa status", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","LoRa Status");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","LoRa Status");
       return;
    }

    if(AddCustomCommand("LoRa config", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        AstraGetLoraProcessing(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "LoRa fw module management (see LoRa status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","LoRa config");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","LoRa config");
       return;
    }

    if(AddCustomCommand("Set LoRa ADR", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        AstraGetLoraAdr(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "LoRa ADR management (see LoRa status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Set LoRa ADR");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Set LoRa ADR");
       return;
    }

    if(AddCustomCommand("Set LoRa DR", //Name
                        BLE_CUSTOM_COMMAND_INTEGER, //Type
                        AstraGetLoraDr(), //Default Value
                        0, //MIN
                        15,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "LoRa data rate management (see LoRa status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Set LoRa DR");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Set LoRa DR");
       return;
    }

    if(AddCustomCommand("Set LoRa ACK", //Name
                        BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        AstraGetLoraAck(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "LoRa ACK management (see LoRa status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Set LoRa ACK");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Set LoRa ACK");
       return;
    }

    //The Last value should be NULL
    char const **LoRaRegionNames = AstraGetLoRaRegionNames();
    
    if(AddCustomCommand("Set LoRa region", //Name
                        BLE_CUSTOM_COMMAND_ENUM_STRING, //Type
                        AstraGetLoRaRegionEnum(), //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        (void *)LoRaRegionNames, //Enum String
                        "Set LoRa region (see LoRa status first)", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Set LoRa region");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Set LoRa region");
      return;
    }

    if(AddCustomCommand("GetStm32wlKeys", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Get the STM32WL internal keys", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","GetStm32wlKeys");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","GetStm32wlKeys");
       return;
    }

    if(AddCustomCommand("GetLoRaKeys", //Name
                        BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Get the configured LoRa keys", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","GetLoRaKeys");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","GetLoRaKeys");
       return;
    }

    if(AddCustomCommand("SetLoRaAppEui", //Name
                        BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        16, //MIN
                        16,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Insert your Application EUI, 8bytes -> 16hex chars", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","SetLoRaAppEui");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","SetLoRaAppEui");
       return;
    }

    if(AddCustomCommand("SetLoRaNwkKey", //Name
                        BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        32, //MIN
                        32,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Insert your Network key, 16bytes -> 32hex chars", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","SetLoRaNwkKey");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","SetLoRaNwkKey");
       return;
    }

    if(AddCustomCommand("SetLoRaAppKey", //Name
                        BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        32, //MIN
                        32,  //MAX
                        NULL, //Enum Int
                        NULL, //Enum String
                        "Insert your Application key, 16bytes -> 32hex chars", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","SetLoRaAppKey");
    } else {
       BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","SetLoRaAppKey");
       return;
    }

  }
  return;

#if 0 //exempli code for custom commands

  /* Add all the custom Commands */
  if(AddCustomCommand("IntValue1", //Name
                      BLE_CUSTOM_COMMAND_INTEGER, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      -100, //MIN
                      200,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      NULL, //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","IntValue1");
  } else {
     BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","IntValue1");
     return;
  }

  if(AddCustomCommand("IntValue2", //Name
                      BLE_CUSTOM_COMMAND_INTEGER, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      10, //MIN
                      3000,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      NULL, //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","IntValue2");
  } else {
     BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","IntValue2");
     return;
  }

  if(AddCustomCommand("BleManagerReset", //Name
                      BLE_CUSTOM_COMMAND_VOID, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                      BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      "Reset the Bluetooth", //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","Command1");
  } else {
     BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","Command1");
     return;
  }

  if(AddCustomCommand("StringValue1", //Name
                      BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                      20,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      NULL, //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","StringValue1");
  } else {
     BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","StringValue1");
     return;
  }

  if(AddCustomCommand("BooleanValue", //Name
                      BLE_CUSTOM_COMMAND_BOOLEAN, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                      BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      "Example for Boolean", //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","BooleanValue");
  } else {
     BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","BooleanValue");
     return;
  }

  if(AddCustomCommand("StringValue2", //Name
                      BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      4, //MIN
                      10,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      "It's possible to add a  very very very very very very long description", //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","StringValue2");
  } else {
    BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","StringValue2");
    return;
  }

  //Example of Enum String Custom Command
  {
    //The Last value should be NULL
    char *EngineUcNames[]={"Ciao", "Buona","Giornata",NULL};
    if(AddCustomCommand("StringEnum", //Name
                        BLE_CUSTOM_COMMAND_ENUM_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        NULL, //Enum Int
                        (void *)EngineUcNames, //Enum String
                        "Example of Enum String", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","StringEnum");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","StringEnum");
      return;
    }
  }

  //Example of Enum Int Custom Command
  {
    //The Last value should be BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN
    int32_t ValidIntValues[]={-1,12,123,321,BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN};
    if(AddCustomCommand("IntEnum", //Name
                        BLE_CUSTOM_COMMAND_ENUM_INTEGER, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //MIN
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  //MAX
                        (void *) ValidIntValues, //Enum Int
                        NULL, //Enum String
                        "Example of Enum Integer", //Description
                        JSON_SensorArray)) {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n","IntEnum");
    } else {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","IntEnum");
      return;
    }
  }

  //Just one Example of one Invalid Command
  if(AddCustomCommand("ReadCert", //Name
                      BLE_CUSTOM_COMMAND_STRING, //Type
                        BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, //Default Value
                      4, //MIN
                      10,  //MAX
                      NULL, //Enum Int
                      NULL, //Enum String
                      "Invalid Command...", //Description
                      JSON_SensorArray)) {
    BLE_MANAGER_PRINTF("Added Command <%s>\r\n","ReadCert");
  } else {
    BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n","ReadCert");
    return;//not mandatory... it's the last one
  }
#endif //0 //exempli code for custom commands
}

static void ExtConfigRebootOnDFUModeCommandCallback(void)
{
  BLE_MANAGER_PRINTF("RebootOnDFUModeCommandCallback\r\n");

  /* Insert here the code for managing the received command */
  AstraBoardUsbDfu();
}

static void ExtExtConfigUidCommandCallback(uint8_t **UID)
{
#ifdef STM32_UUID
  *UID = (uint8_t *)STM32_UUID;
#endif /* STM32_UUID */
}

static void ExtConfigInfoCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"STMicroelectronics\n%s:\n"
    "%s - Version %d.%d.%d\n"
    "(HAL %ld.%ld.%ld_%ld)\n"
    "STM32WL APP V%ld.%ld.%ld\n"
    "STM32WB CM0 V%ld.%ld.%ld\n"
    "Running %s\n"
    "Compiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
    " (IAR)",
#elif defined (__CC_ARM)
    " (KEIL)",
#elif defined (__GNUC__)
    " (STM32CubeIDE)",
#endif
    BLE_STM32_BOARD,
    BLE_FW_PACKAGENAME,
    BLE_VERSION_FW_MAJOR,
    BLE_VERSION_FW_MINOR,
    BLE_VERSION_FW_PATCH,
    HAL_GetHalVersion() >>24,
    (HAL_GetHalVersion() >>16)&0xFF,
    (HAL_GetHalVersion() >> 8)&0xFF,
     HAL_GetHalVersion()      &0xFF,
     MAJOR_VERSION(PlatformStatus.s.STM32WL_CM4_VER), MINOR_VERSION(PlatformStatus.s.STM32WL_CM4_VER), REV_VERSION(PlatformStatus.s.STM32WL_CM4_VER),
     MAJOR_VERSION(PlatformStatus.s.STM32WB_CM0_VER), MINOR_VERSION(PlatformStatus.s.STM32WB_CM0_VER), REV_VERSION(PlatformStatus.s.STM32WB_CM0_VER),
     AstraRunningBy(),
     __DATE__,__TIME__);
}

static void ExtConfigHelpCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"Version %d.%d.%d\n"
          "Limitations: Cannot use NFC data storage (settings only)",
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH);
}

static void ExtConfigPowerStatusCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"Board status: <%s>", Astra_GetDevicePowerStatus());
}

static void ExtConfigVersionFwCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"%s_%d.%d.%d",
      BLE_FW_PACKAGENAME,
      BLE_VERSION_FW_MAJOR,
      BLE_VERSION_FW_MINOR,
      BLE_VERSION_FW_PATCH);
}

static void ExtConfigPowerOffCommandCallback(void)
{
  BLE_MANAGER_PRINTF("ExtConfigPowerOffCommandCallback\r\n");

  /* Insert here the code for managing the received command */
  AstraShutdownCommand();
}

static void ExtConfigSetNameCommandCallback(uint8_t *NewName)
{
  BLE_MANAGER_PRINTF("New Board Name = <%s>\r\n", NewName);
  /* Change the Board Name */

  Astra_SetDeviceName((char*)NewName);
}

static void ExtConfigSetTimeCommandCallback(uint8_t *NewTime)
{
  BLE_MANAGER_PRINTF("Set new time = <%s>\r\n", NewTime);
  BLE_MANAGER_PRINTF("Not implemented\r\n");

  /* Insert here the code for changing the RTC time */
#if ASTRA_BLE_SET_DATETIME
  memcpy(NewTimeString, NewTime, 9);
  if(NewDateString[0] != 'T')
  {
    SendInfo("Setting date and time");
    AstraSetDateTime(NewDateString, NewTimeString);
  }
  else
  {
    SendInfo("Now set DATE");
  }
#endif //ASTRA_BLE_SET_DATETIME
}

static void ExtConfigSetDateCommandCallback(uint8_t *NewDate)
{
  BLE_MANAGER_PRINTF("Set new date = <%s>\r\n", NewDate);
  BLE_MANAGER_PRINTF("Not implemented\r\n");

  /* Insert here the code for changing the RTC Date */
#if ASTRA_BLE_SET_DATETIME
  memcpy(NewDateString, NewDate, 12);
  if(NewTimeString[0] != 'T')
  {
    SendInfo("Setting date and time");
    AstraSetDateTime(NewDateString, NewTimeString);
  }
  else
  {
    SendInfo("Now set TIME");
  }
#endif //ASTRA_BLE_SET_DATETIME
}

/**
 * @brief  Callback Function for managing the ReadCert command
 * @param  uint8_t *Certificate to register
 * @retval None
 */
static void ExtConfigReadCertCommandCallback(uint8_t *Certificate)
{
#if USE_STSAFE
  uint8_t ret = 0;
  ret = StSafeInit_HAL(&stsafea_handle);

  if(ret == 0)
  {
    uint16_t CertificateSize=0;
    ret = GetCertificateSize(&stsafea_handle, &CertificateSize);
    StSafeA_LVBuffer_t sts_read_cert;
#if (!STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
    uint8_t *data_sts_read_cert = (uint8_t *)calloc(CertificateSize,sizeof(uint8_t));
    sts_read_cert.Length = CertificateSize;
    sts_read_cert.Data = data_sts_read_cert;
#endif
    ret = ExtractCertificate(&stsafea_handle, CertificateSize, &sts_read_cert);

    sprintf((char *)Certificate,"%s","-----BEGIN DER CERTIFICATE-----\r\n");

    char tempstring[] = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\r\n";
    uint32_t index = 0;
    for(index = 0;index < sts_read_cert.Length/32;index++)
    {
      sprintf(tempstring, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X\r\n",
              sts_read_cert.Data[32*index], sts_read_cert.Data[32*index+1], sts_read_cert.Data[32*index+2], sts_read_cert.Data[32*index+3],
              sts_read_cert.Data[32*index+4], sts_read_cert.Data[32*index+5], sts_read_cert.Data[32*index+6], sts_read_cert.Data[32*index+7],
              sts_read_cert.Data[32*index+8], sts_read_cert.Data[32*index+9], sts_read_cert.Data[32*index+10], sts_read_cert.Data[32*index+11],
              sts_read_cert.Data[32*index+12], sts_read_cert.Data[32*index+13], sts_read_cert.Data[32*index+14], sts_read_cert.Data[32*index+15],
              sts_read_cert.Data[32*index+16], sts_read_cert.Data[32*index+17], sts_read_cert.Data[32*index+18], sts_read_cert.Data[32*index+19],
              sts_read_cert.Data[32*index+20], sts_read_cert.Data[32*index+21], sts_read_cert.Data[32*index+22], sts_read_cert.Data[32*index+23],
              sts_read_cert.Data[32*index+24], sts_read_cert.Data[32*index+25], sts_read_cert.Data[32*index+26], sts_read_cert.Data[32*index+27],
              sts_read_cert.Data[32*index+28], sts_read_cert.Data[32*index+29], sts_read_cert.Data[32*index+30], sts_read_cert.Data[32*index+31]);
      strcat((char *)Certificate, tempstring);
    }
    index *= 32;
    if(index<sts_read_cert.Length)
    {
      for( ;index<sts_read_cert.Length;index++)
      {
        sprintf(tempstring, "%.2X",sts_read_cert.Data[index]);
        strcat((char *)Certificate, tempstring);
      }
    }

#if (!STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
    free(data_sts_read_cert);
#endif

    strcat((char *)Certificate,"\r\n-----END DER CERTIFICATE-----\r\n");
  }
  else
  {
    strcpy((char *)Certificate,"-----NO DER CERTIFICATE-----\r\n" "-----END DER CERTIFICATE-----\r\n");
  }

#else //USE_STSAFE

  const char CertFromSTsafe[] = {
    "-----NO DER CERTIFICATE-----\r\n"
      "-----END DER CERTIFICATE-----\r\n"
  };

  sprintf((char *)Certificate,"%s",CertFromSTsafe);
#endif //USE_STSAFE
}

void AstraCustomNotifyEventEnv(BLE_NotifyEvent_t Event)
{
  if(Event == BLE_NOTIFY_SUB)
  {
    AstraCustomEnvNotifyActivate(1);
  }
  if(Event == BLE_NOTIFY_UNSUB)
  {
    AstraCustomEnvNotifyActivate(0);
  }
}

void AstraCustomNotifyEventInertial(BLE_NotifyEvent_t Event)
{
  if(Event == BLE_NOTIFY_SUB)
  {
    AstraCustomInertialNotifyActivate(1);
  }
  if(Event == BLE_NOTIFY_UNSUB)
  {
    AstraCustomInertialNotifyActivate(0);
  }
}

void AstraCustomNotifyEventBattery(BLE_NotifyEvent_t Event)
{
  if(Event == BLE_NOTIFY_SUB)
  {
    AstraCustomBatteryNotifyActivate(1);
  }
  if(Event == BLE_NOTIFY_UNSUB)
  {
    AstraCustomBatteryNotifyActivate(0);
  }
}

void AstraCustomNotifyEventGnss(BLE_NotifyEvent_t Event)
{
  if(Event == BLE_NOTIFY_SUB)
  {
    AstraCustomGnssNotifyActivate(1);
  }
  if(Event == BLE_NOTIFY_UNSUB)
  {
    AstraCustomGnssNotifyActivate(0);
  }
}

/**
* @} BLE_IMPLEMENTATION_Function_Definitions
*/

#endif //USE_BLE

/**
* @} BLE_MANAGER
*/

/**
* @} ASTRA_ENGINE
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
