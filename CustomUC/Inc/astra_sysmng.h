/**
  ******************************************************************************
  * @file    astra_sysmng.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   This file contains the Astra demo
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

/** @addtogroup SYSTEM_MANAGEMENT System management
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ASTRA_SYSMNG__H__
#define __ASTRA_SYSMNG__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "astra_datamng.h"
#include "astra_confmng.h"
#include "astra_boot.h"
  

/** @addtogroup SYSTEM_MANAGEMENT_EXPORTED_TYPES ASTRA System management - Exported Types
  * @brief Exported Types
  * @{
 */

/* Exported types ------------------------------------------------------------*/
typedef void (*pBtnCb_t)(void);
typedef enum {
  BTN1_SHORT,
  BTN1_LONG,
  BTN2_SHORT,
  BTN2_LONG,
#if USE_BTN1_ANTI_TAMPER
  BTN1_TAMPER_GLITCH,
  BTN1_TAMPER_ACTIVATED,
  BTN1_TAMPER_RESTORED
#endif //USE_BTN1_ANTI_TAMPER
}Button_t;

/**
* @}
*/

/** @addtogroup SYSTEM_MANAGEMENT_FunctionPrototypes ASTRA System management - Function prototypes
  * @brief Function prototypes
  * @{
  */

/* Function prototypes -------------------------------------------------------*/

#if USE_BLE
/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  activation: 1 notification activation, 0 otherwise
  * @retval none
  */
void AstraCustomEnvNotifyActivate(uint8_t activation);

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  activation: 1 notification activation, 0 otherwise
  * @retval none
  */
void AstraCustomInertialNotifyActivate(uint8_t activation);

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  activation: 1 notification activation, 0 otherwise
  * @retval none
  */
void AstraCustomBatteryNotifyActivate(uint8_t activation);

/**
  * @brief  Environmental characteristic notify activation on/off
  * @param  activation: 1 notification activation, 0 otherwise
  * @retval none
  */
void AstraCustomGnssNotifyActivate(uint8_t activation);
#endif //USE_BLE

void AstraGetMacAddress(uint8_t *BleMacAddress);
void AstraGetSecurePin(uint32_t *securepin);


/**
  * @brief  Get BLE firmware and FUS version
  * @param  uCM0_FwVer: Pointer to 32bit unsigned integer
  * @param  uCM0_FusVer: Pointer to 32bit unsigned integer
  * @retval none
  */
void AstraBleGetCM0ver(uint32_t *uCM0_FwVer, uint32_t *uCM0_FusVer);

/**
  * @brief  Register button callback
  * @param  button: Button
  * @param  BtnCb: Button callback
  * @retval none
  */
void RegisterButtonCallback(Button_t button, pBtnCb_t BtnCb);

/**
  * @brief  ASTRA system initialization
  * @param  none
  * @retval none
  */
void AstraSysInit(void);

/**
  * @brief  ASTRA system is state running
  * @param  none
  * @retval Returned value: 1--> YES; 0 --> NO
  */
uint8_t AstraSysIsStateRunning(void);

/**
  * @brief  ASTRA system state machine manager
  * @param  none
  * @retval none
  */
void AstraSysSmManager(void);

/**
  * @brief  Print system info
  * @param  none
  * @retval none
  */
void PrintSystemInfo(void);

/**
  * @brief  ASTRA process command
  * @param  att_data:
  * @param  data_length: Data lenght
  * @retval none
  */
void AstraProcessCommand(uint8_t *att_data, uint32_t data_length);

/**
  * @brief  ASTRA tamper
  * @param  none
  * @retval none
  */
void AstraTamper(void);

/**
  * @brief  ASTRA nothing to do
  * @param  none
  * @retval none
  */
void AstraNothing(void);

/**
  * @brief  ASTRA shutdown command
  * @param  none
  * @retval none
  */
void AstraShutdownCommand(void);

/**
  * @brief  ASTRA go in run command
  * @param  none
  * @retval none
  */
void AstraPrepareHwRun(void);

/**
  * @brief  ASTRA go in low power command
  * @param  none
  * @retval none
  */
void AstraPrepareHwLp(void);

/**
  * @brief  ASTRA send data
  * @param  none
  * @retval none
  */
void AstraSendData(void);

/**
  * @brief  ASTRA board reboot
  * @param  none
  * @retval none
  */
void AstraBoardReboot(void);

/**
  * @brief  Board LED play
  * @param  nMillisec: Number of millisecond
  * @retval none
  */
void BoardLedPlay(uint32_t nMillisec);

/**
  * @brief  ASTRA get device name
  * @param  boardName: Board name
  * @retval none
  */
void Astra_GetDeviceName(char *boardName);

/**
  * @brief  ASTRA set device name
  * @param  newname: New name
  * @retval none
  */
void Astra_SetDeviceName(char*newname);

/**
  * @brief  ASTRA set date and time
  * @param  newdate: New date
  * @param  newtime: New time
  * @retval none
  */
void AstraSetDateTime(char*newdate, char*newtime);

/**
  * @brief  NFC data memory init
  * @param  none
  * @retval none
  */
void AstraNfcDataInit(void);

/**
  * @brief  Disconnection complete
  * @param  none
  * @retval none
  */
void DisconnectionComplete(void);

/**
  * @brief  Connection complete
  * @param  none
  * @retval none
  */
void ConnectionComplete(void);

/**
  * @brief  ASTRA set LoRa interval
  * @param  newInterval
  * @retval none
  */
void AstraSetLoraInterval(int32_t newInterval);

/**
  * @brief  ASTRA get LoRa interval
  * @retval uint32_t interval
  */
uint32_t AstraGetLoraInterval(void);

void AstraSetLoraDr(int32_t newDr);
uint32_t AstraGetLoraDr(void);
void AstraSetLoraAdr(int32_t newAdr);
uint32_t AstraGetLoraAdr(void);
void AstraSetLoraAck(int32_t newAck);
uint32_t AstraGetLoraAck(void);

char const **AstraGetLoRaRegionNames(void);
uint32_t AstraGetLoRaRegionEnum(void);
void AstraSetLoraRegion(const char*newLoraRegion);
char *AstraGetLoraRegion(void);

/**
  * @brief  ASTRA set debug console
  * @param  newInterval
  * @retval none
  */
void AstraSetDbgConsole(int32_t newInterval);

/**
  * @brief  ASTRA get LoRa Application Eui string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetLoRaAppEuiString(char*str);

/**
  * @brief  ASTRA get LoRa Network key string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetLoRaNwkKeyString(char*str);

/**
  * @brief  ASTRA get LoRa Application Key string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetLoRaAppKeyString(char*str);

/**
  * @brief  ASTRA get STM32WL Device Eui string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetStm32wlDevEuiString(char*str);

/**
  * @brief  ASTRA get STM32WL Application Eui string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetStm32wlAppEuiString(char*str);

/**
  * @brief  ASTRA get STM32WL Network Key string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetStm32wlNwkKeyString(char*str);

/**
  * @brief  ASTRA get STM32WL Application Key string
  * @param  str: Pointer to char
  * @retval none
  */
void AstraGetStm32wlAppKeyString(char*str);

/**
  * @brief  ASTRA set LoRa Application Eui
  * @param  strNewAppEui: Pointer to char
  * @retval Returned values: 0 --> OK; 1 --> Error
  */
uint8_t AstraSetLoRaAppEui(char*strNewAppEui);

/**
  * @brief  ASTRA set LoRa Network Key
  * @param  strNewNwkKey: Pointer to char
  * @retval Returned values: 0 --> OK; 1 --> Error
  */
uint8_t AstraSetLoRaNwkKey(char*strNewNwkKey);

/**
  * @brief  ASTRA set LoRa Application Key
  * @param  strNewAppKey: Pointer to char
  * @retval Returned values: 0 --> OK; 1 --> Error
  */
uint8_t AstraSetLoRaAppKey(char*strNewAppKey);

/**
  * @brief  ASTRA set Engine
  * @param  newEngine: Pointer to char
  * @retval none
  */
void AstraSetEngine(const char*newEngine);

/**
  * @brief  Save default Astra params to EEPROM
  * @param  none
  * @retval none
  */
void AstraSaveDefaultParams(void);

/**
  * @brief  Load params from EEPROM
  * @param  none
  * @retval none
  */
void AstraLoadParams(void);

/**
  * @brief  ASTRA get verbal level enum
  * @param  none
  * @retval uint32_t: Verbose level enum
  */
uint32_t AstraGetVerbLevelEnum(void);

/**
  * @brief  ASTRA get verbal level names
  * @param  none
  * @retval char const: Verbose level names
  */
char const **AstraGetVerbLevelNames(void);

/**
  * @brief  ASTRA get verbal level
  * @param  none
  * @retval char: Verbose level
  */
char *AstraGetVerbLevel(void);

/**
  * @brief  ASTRA set verbal level
  * @param  newVerbLevel: Pointer to char
  * @retval none
  */
void AstraSetVerbLevel(const char*newVerbLevel);

/**
  * @brief  ASTRA get MCU ID
  * @param  none
  * @retval uint8_t: MCU ID
  */
uint8_t *AstraGetMcuId(void);

/**
  * @brief  ASTRA running time
  * @param  none
  * @retval running time
  */
char *AstraRunningBy(void);

/**
  * @brief  ASTRA get device power status
  * @param  none
  * @retval device status
  */
char*Astra_GetDevicePowerStatus(void);

/**
  * @brief  ASTRA get battery level (percentage)
  * @param  none
  * @retval percentage
  */
uint16_t AstraGetBatteryLevelPerc(void);

/**
  * @brief  ASTRA get battery status
  * @param  none
  * @retval status
  */
uint8_t AstraGetBatteryStatus(void);

/**
  * @brief  ASTRA get Engine option names
  * @param  none
  * @retval char const: names
  */
char const **AstraGetEngineNames(void);

/**
  * @brief  ASTRA get Engine option enum
  * @param  none
  * @retval uint32_t: enum engine option
  */
uint32_t AstraGetEngineEnum(void);

/**
  * @brief  ASTRA get use case
  * @param  none
  * @retval case
  */
uint8_t AstraGetUseCase(void);

/**
  * @brief  ASTRA get use case string
  * @param  none
  * @retval case
  */
char *AstraGetUseCaseString(void);

/**
  * @brief  Update system status
  * @param  none
  * @retval none
  */
void UpdateSystemStatus(void);

/**
  * @brief  ASTRA get enumeration system status
  * @param  none
  * @retval enumerated status
  */
uint8_t AstraGetEnumSysStatus(void);

/**
  * @brief  ASTRA get enumeration system status icon
  * @param  none
  * @retval enumerated status icon
  */
uint8_t AstraGetEnumSysStatusIcon(void);

/**
  * @brief  ASTRA set LoRa processing
  * @param  newStatus
  * @retval none
  */
void AstraSetLoraProcessing(int32_t newStatus);

/**
  * @brief  ASTRA set GNSS configuration
  * @param  newStatus
  * @retval none
  */
void AstraSetGnssConfig(int32_t newStatus);

/**
  * @brief  ASTRA get GNSS configuration
  * @retval uint8_t Status
  */
uint8_t AstraGetGnssConfig(void);

/**
  * @brief  ASTRA set sensor configuration
  * @param  newStatus
  * @retval none
  */
void AstraSetSensorsConfig(int32_t newStatus);

/**
  * @brief  ASTRA get sensor configuration
  * @retval uint8_t Status
  */
uint8_t AstraGetSensorsConfig(void);


/**
  * @brief  ASTRA get LoRa processing
  * @param  none
  * @retval LoRa processing
  */
uint8_t AstraGetLoraProcessing(void);

/**
  * @brief  ASTRA get LoRa join status
  * @param  none
  * @retval LoRa join status
  */
uint8_t AstraGetLoraJoinStatus(void);

/**
  * @brief  Set LoRa join timer
  * @param  TimerIntv
  * @param  bImmediateTrigger
  * @retval none
  */
void setLoraJoinTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger);

/**
  * @brief  Unset LoRa join timer
  * @param  none
  * @retval none
  */
void unsetLoraJoinTimer(void);

/**
  * @brief  Delay function without systick interrupt
  * @param  none
  * @retval none
  */
void pause(uint32_t ms);

/**
  * @brief  ASTRA board USB DFU
  * @param  none
  * @retval none
  */
void AstraBoardUsbDfu(void);

/**
  * @brief  ASTRA get engine names
  * @param  none
  * @retval pointer to a char pointer
  */
char const **AstraGetEngineNames(void);

/**
  * @brief  ASTRA get reboot reasons names
  * @param  none
  * @retval pointer to a char pointer
  */
char const **AstraGetRebootReasonNames(void);

/**
  * @brief  ASTRA get ULP wake type names
  * @param  none
  * @retval pointer to a char pointer
  */
char const **AstraGetULPwakeNames(void);

/**
  * @brief  ASTRA reboot sistem on custom service
  * @param  reasonName: string
  * @retval none
  */
void AstraRebootWithReason(const char*reasonName);

/**
  * @brief  ASTRA schedule ULP state change
  * @param  wakeType: ULP wake type
  * @retval none
  */
void AstraULP_Schedule(const char*wakeType);

/**
  * @brief  ASTRA set BLE interval
  * @param  newInterval: 32bit unsigned integer
  * @retval none
  */
void AstraSetBleInterval(int32_t newInterval);

void Astra_Error_Handler(void);

void AstraSetNonConnectable(void);

uint8_t AstraCheckBleInit(void);

/**
  * @brief  Set status update timer
  * @param  TimerIntv: 32bit unsigned integer
  * @param  bImmediateTrigger: 8bit unsigned integer
  * @retval none
  */
void setNFCDataSavingTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger);

#if USE_BOOT_UPGRADE
/**
  * @brief  Schedule ASTRA board reboot
  * @param  reason
  * @retval none
  */

void ScheduleAstraSmChangeState(SmChangeState_t reason);

/**
  * @brief  Clear schedule ASTRA board reboot
  * @param  none
  * @retval none
  */
void ClearScheduleAstraSmChangeState(void);
#endif //USE_BOOT_UPGRADE

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __ASTRA_SYSMNG__H__ */

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
