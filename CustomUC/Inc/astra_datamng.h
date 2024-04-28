/**
  ******************************************************************************
  * @file    astra_datamng.h
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

/** @addtogroup DATA_MANAGEMENT Data management
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ASTRA_DATAMNG__H__
#define __ASTRA_DATAMNG__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "astra_confmng.h"

/** @addtogroup DATA_MANAGEMENT_DEFINES ASTRA Data management - Defines
  * @brief Defines
  * @{
  */

/* Defines -------------------------------------------------------------------*/
#define STD_OUT_ARRAY_LENGHT                            1024


#define NEW_DATA_SET(N)                   (N.newDataPeripheral = 0xFF)
#define NEW_DATA_CLEAR(N,P)               (N.bfp.P = 0)
#define NEW_DATA_IS_SET(N,P)              (N.bfp.P != 0)

typedef struct bitFieldPeripheral_s
{
  uint8_t ble:1;
  uint8_t lora:1;
  uint8_t nfc:1;
  uint8_t algo:1;
  uint8_t usb:1;
  uint8_t user2:1;
  uint8_t user3:1;
  uint8_t user4:1;
}bfp_t;

typedef union
{
  bfp_t bfp;
  uint8_t newDataPeripheral;
}newDataPeripheral_t;


/**
* @} DATA_MANAGEMENT_DEFINES
*/

/** @addtogroup DATA_MANAGEMENT_EXPORTED_TYPES ASTRA Data management - Exported types
  * @brief External types
  * @{
  */

/* Exported types ------------------------------------------------------------*/

typedef struct
{
//  struct
//  {
//    // uint8_t bDebuggerEnabled:1;
//    // uint8_t GNSS_NEW_DATA:1;
//    // uint8_t ENVIRONMENTAL_NEW_DATA:1;
//    // uint8_t POW_MAN_NEW_DATA:1;
//    // uint8_t MEMS_EVENT_NEW_DATA:1;
//    // uint8_t INERTIAL_NEW_DATA:1;
//    // uint8_t bReserved7:1;
//    // uint8_t bReserved8:1;
//    // uint8_t bReserved8_16;
//    // uint8_t bReserved116_24;
//    // uint8_t bReserved124_32;
//  }c;

  struct
  {
    newDataPeripheral_t GNSS_NEW_DATA;
    newDataPeripheral_t ENVIRONMENTAL_NEW_DATA;
    newDataPeripheral_t POW_MAN_NEW_DATA;
    newDataPeripheral_t MEMS_EVENT_NEW_DATA;
    newDataPeripheral_t INERTIAL_NEW_DATA;
  }f;

  struct
  {
    /* gnss */
    float gnss_latitude;
    float gnss_longitude;
    float gnss_altitude;
    int32_t gnss_sats;
    uint8_t gnss_fix_validity;

    /* sensors */
    MOTION_SENSOR_Axes_t AccValue;
    MOTION_SENSOR_Axes_t LPAccValue;
    MOTION_SENSOR_Axes_t GyrValue;

    float LPTempValue;

    float PressValue;
    float TempValue;
    float HumValue;

    MOTION_SENSOR_Event_Status_t LPAccInt;
    MOTION_SENSOR_Event_Status_t AccInt;

    /* ADC */
    uint16_t batteryVoltage;
    uint16_t USBVoltage;

  }d;

  uint32_t nFwVersion;
} AstraEngData_t; //Astra Engine data

/**
* @} DATA_MANAGEMENT_EXPORTED_TYPES
*/

/** @addtogroup DATA_MANAGEMENT_EXTERNAL_VARIABLES ASTRA Data management - External variables
  * @brief External variables
  * @{
  */

/* External variables --------------------------------------------------------*/
extern AstraEngData_t AstraEngData;

/**
* @}
*/

/** @addtogroup DATA_MANAGEMENT_FunctionPrototypes ASTRA Data management - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/** @brief DBG_PRINTF_LOG implementation
  * @param L message priority level (0 = max, see Message priority levels)
  * @param sFormatString: Pointer to the string
  * @retval Returned values: 0 -> OK, 1 -> FAIL (or trace disabled)
  */
uint8_t DBG_PRINTF_LOG(uint8_t Q, uint16_t L, const char* sFormatString, ...);

/** @brief Disable BLE log messages
  * @param none
  * @retval none
  */
void AstraDisablePrintBle(void);

/** @brief Enable BLE log messages
  * @param none
  * @retval none
  */
void AstraEnablePrintBle(void);

/** @brief BLE log configuration
  * @param none
  * @retval none
  */
uint8_t AstraBlePrintConfig(void);

/** @brief Enable USB log messages
  * @param none
  * @retval none
  */
void AstraEnablePrintUsb(void);

/** @brief Disable BLE log messages
  * @param none
  * @retval none
  */
void AstraDisablePrintUsb(void);

/** @brief Read ADC values
  * @param none
  * @retval none
  */
void Astra_ReadADC(void);

/** @brief Init Astra power
  * @param none
  * @retval none
  */
void Astra_PwrInit(void);

/** @brief Get battery voltage
  * @param none
  * @retval uint16_t Battery voltage expressed in mV
  */
uint16_t Astra_GetBatteryVoltage_mV(void);

/** @brief Get USB voltage
  * @param none
  * @retval uint16_t USB voltage expressed in mV
  */
uint16_t Astra_GetUSBVoltage_mV(void);

/** @brief Print battery voltage
  * @param BatteryVoltage_mV Battery voltage expressed in mV
  * @retval none
  */
void Astra_PrintBatteryVoltage (uint16_t BatteryVoltage_mV);

/** @brief Print USB voltage
  * @param USBVoltage_mV USB voltage expressed in mV
  * @retval none
  */
void Astra_PrintUSBVoltage (uint16_t USBVoltage_mV);

/** @brief Read sensors
  * @param none
  * @retval none
  */
void ReadSensors(void);

/** @brief Manage sensors event
  * @param none
  * @retval none
  */
void ManageSensorsEvents(void);

#if USE_GNSS

/** @brief Read GNSS
  * @param nTimeout Timeout
  * @param bWaitFix Wait for fix
  * @retval none
  */
void ReadGnss(uint32_t nTimeout, uint8_t bWaitFix);

/** @brief Manage GNSS
  * @param none
  * @retval none
  */
void ManageGnss(void);

/** @brief Request GNSS version
  * @param none
  * @retval none
  */
void GnssRequestVersion(void);

/** @brief Set NMEA configuration (RMC and GGA sentences only)
  * @param none
  * @retval none
  */
void GnssSetNmeaConfigRMC_GGA(void);

/** @brief Set Teseo-LIV3F nmea mask configuration to default (GGA, GSA, RMC, GSV, GLL, PSTMCPU)
  * @param none
  * @retval none
  */
void GnssSetNmeaConfigDefault(void);

/** @brief Get GNSS version
  * @param none
  * @retval char pointer to gnss version string
  */
char *GnssGetVersion(void);

/** @brief GNSS data initialization
  * @param none
  * @retval none
  */
void GNSS_DataInit(void);

#endif //USE_GNSS

/** @brief STM32WL process
  * @param none
  * @retval none
  */
void stm32wl_process(void);

/** @brief STM32WL low power
  * @param none
  * @retval none
  */
void STM32WL_LowPower(void);

/** @brief STM32WL reboot in dfu mode
  * @param none
  * @retval none
  */
void STM32WL_RebootDfu(void);

/** @brief Save LoRa Application Eui
  * @param PtrAppEui Pointer to uint8_t
  * @retval memory enum status
  */
int32_t SaveLoraAppEui(uint8_t *PtrAppEui);

/** @brief Save LoRa Network Key
  * @param PtrNwkKEY Pointer to uint8_t
  * @retval memory enum status
  */
int32_t SaveLoraNwkKey(uint8_t *PtrNwkKEY);

/** @brief Save LoRa Application Key
  * @param PtrAppKey Pointer to uint8_t
  * @retval memory enum status
  */
int32_t SaveLoraAppKey(uint8_t *PtrAppKey);

/** @brief Get LoRa validation flag
  * @param nMask Validation mask
  * @retval uint8_t Validation flag
  */
uint8_t GetLoraValidationFlag(uint8_t nMask);

/** @brief Get LoRa validation flag
  * @param none
  * @retval Returned values: 0 -> OK, 1 -> FAIL
  */
uint8_t GetWlLoraKeys(void);

/** @brief Get LoRa SKey validation flag
  * @param none
  * @retval Returned values: 0 -> OK, 1 -> FAIL
  */
uint8_t GetWlLoraSKeys(void);

/** @brief Get STM32WL firmware version
  * @param none
  * @retval Returned values: 0 -> OK, 1 -> FAIL
  */
uint8_t GetWlFwVersion(void);

/** @brief Load LoRa Application Eui
  * @param pTmpkey Pointer to uint8_t
  * @retval Returned values: 0 -> OK, 1 -> FAIL
  */
int32_t LoadLoraAppEui(uint8_t *pTmpkey);

/** @brief Load LoRa Network Key
  * @param pTmpkey Pointer to uint8_t
  * @retval Returned values: 0 -> OK, 1 -> FAIL
  */
int32_t LoadLoraNwkKey(uint8_t *pTmpkey);

/** @brief Load LoRa Application Key
  * @param pTmpkey Pointer to uint8_t
  * @retval Returned values: 0 -> OK, 1 -> FAIL
  */
int32_t LoadLoraAppKey(uint8_t *pTmpkey);

/** @brief Save board name
  * @param boardName Pointer to char
  * @retval none
  */
void SaveBoardName(char *boardName);

/** @brief Get board name
  * @param boardName Pointer to char
  * @retval none
  */
void GetBoardName(char *boardName);

/** @brief Manage memory
  * @param none
  * @retval none
  */
void ManageMemory(void);

/** @brief Buzzer process
  * @param none
  * @retval none
  */
void buzzer_process(void);

#if USE_STM32WL
/**
  * @brief  Load LoRa keys
  * @param  none
  * @retval LoRa keys
  */
int32_t LoadLoraKeys(void);

/**
  * @brief  Save LoRa keys
  * @param  none
  * @retval LoRa keys
  */
int32_t SaveLoraKeys(void);
#endif //USE_STM32WL

#ifdef __cplusplus
}
#endif

#endif /* __ASTRA_DATAMNG__H__ */

/**
* @} DATA_MANAGEMENT_FunctionPrototypes
*/

/**
* @} DATA_MANAGEMENT
*/

/**
* @} ASTRA_ENGINE
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
