/**
  ******************************************************************************
  * @file    astra_datamng.c
  * @version V1.0.1
  * @author  SRA Team
  * @brief   This file contains the Astra data management
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "astra_datamng.h"
#include "astra_confmng.h"
#include "astra_sysmng.h"
#if USE_USB
#include "usbd_cdc_if.h"
#endif //USE_USB

#include "Commissioning.h"

#if USE_STM32WL
#include "custom_modem_control.h"
#endif //USE_STM32WL

#if USE_BLE
#include "BLE_Manager.h"
#endif //USE_BLE

#if USE_SEQUENCER
#include "stm32_seq.h"
#endif //USE_SEQUENCER

#if USE_GNSS
#include "teseo_liv3f_conf.h"
#include "custom_gnss.h"
#undef TRUE
#undef FALSE
#include "gnss_data.h"
#endif //USE_GNSS


#if USE_MEMORY
#include "astra_app_nfc.h"
#endif //USE_SEQUENCER

/** @addtogroup DATA_MANAGEMENT_Private_Defines ASTRA Data management - Private defines
  * @brief Private defines
  * @{
 */

/* Private defines -----------------------------------------------------------*/

#define LORAWAN_APP_DATA_BUFF_SIZE                      (64 + 4 + 2)  /*  Added bytes are: epochtime (4 bytes) + channel and datatype (2 bytes) */

#define LPP_DATATYPE_DIGITAL_INPUT  0x0
#define LPP_DATATYPE_DIGITAL_OUTPUT 0x1
#define LPP_DATATYPE_ANALOG_INPUT   0x2
#define LPP_DATATYPE_HUMIDITY       0x68
#define LPP_DATATYPE_TEMPERATURE    0x67
#define LPP_DATATYPE_BAROMETER      0x73
#define LPP_DATATYPE_GPSLOCATION    0x88
#define LPP_DATATYPE_ACCELEROMETER  0x71
#define LPP_DATATYPE_GPSTIME        0x85
#define LPP_APP_PORT 99

/**
* @}
*/

/** @addtogroup DATA_MANAGEMENT_Private_Variables ASTRA Data management - Private variables
  * @brief Private variables
  * @{
 */

/* Private variables ---------------------------------------------------------*/

static uint16_t aADCxConvertedData[ASTRA_ADC_CHANNELS];

#if USE_GNSS
/* GNSS variables */
GNSSParser_Data_t GNSSParser_Data;
uint32_t NMEA_PARSED_MASK = 0;
#endif //USE_GNSS

uint8_t pDevEui[8] = {0};
uint8_t pAppEui[8] = {0};
uint8_t pAppKey[16] = {0};
uint8_t pNwkKey[16] = {0};

uint32_t DevAddres = 0;
uint8_t pAppSKey[16] = {0};
uint8_t pNwkSKey[16] = {0};

uint8_t NewAppEui[8] = LORAWAN_APPLICATION_EUI;
uint8_t NewAppKey[16] = LORAWAN_APPLICATION_KEY;
uint8_t NewNwkKey[16] = LORAWAN_NETWORK_KEY;

uint32_t NewDevAddres = LORAWAN_DEVICE_ADDRESS;
uint8_t NewAppSKey[16] = LORAWAN_APP_S_KEY;
uint8_t NewNwkSKey[16] = LORAWAN_NWK_S_KEY;

#if USE_STM32WL
static uint8_t AppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];

#endif //USE_STM32WL

AstraEngData_t AstraEngData;

/* LOG/TRACE FUNCTIONS */
static char sStdOutString[STD_OUT_ARRAY_LENGHT+1];                              //!< Buffer to store the custom printf output
static uint32_t nCharsToPrint=0;

static uint8_t bEnablePrintingBle=USE_BLE;
static uint8_t bEnablePrintingUart=USE_UART;
static uint8_t bEnablePrintingUsb=USE_USB;
static uint8_t bEnablePrintingTerm=1;

/**
* @}
*/

/** @addtogroup DATA_MANAGEMENT_Private_FunctionPrototypes ASTRA Data management - Private Function Prototypes
  * @brief Private function prototypes
  * @{
 */

/* Private function prototypes -----------------------------------------------*/

#if USE_GNSS

/**
  * @brief  Convert latitude/longitude coordinate from sexagesimal to decimal format
  * @param  float x coordinate
  * @param  uint8_t sign 1 for negative 0 for positive
  * @retval coordinate in decimal format
  */
static float convertCoord( float x, uint8_t sign );

/**
  * @brief  Get GNSS data
  * @param  none
  * @retval none
  */
void GnssGetData(void);

/**
  * @brief  Reset GNSS parser data
  * @param  none
  * @retval none
  */
void ResetGnssParserData(void);

/**
  * @brief  Print GNSS information
  * @param  pParserData: Pointer to GNSSParser_Data_t
  * @retval none
  */
void PrintGnssInfo(GNSSParser_Data_t *pParserData);

#endif //USE_GNSS

#if USE_STM32WL

/**
  * @brief  Set LoRa keys
  * @param  none
  * @retval A 8bit unisigned integers meaning success or error
  */
uint8_t SetLoraKeys(void);

/**
  * @brief  Set LoRa Skeys
  * @param  none
  * @retval A 8bit unisigned integers meaning success or error
  */
uint8_t SetLoraSKeys(void);

/**
  * @brief  Send LoRa packet
  * @param  none
  * @retval none
  */
void LoraSendPacket(void);

/**
  * @brief  Save LoRa validation string
  * @param  nMask: 8bit unsigned integer
  * @retval none
  */
void SaveLoraValidationString(uint8_t nMask);

#endif //#if USE_STM32WL

/**
  * @brief  Returns information about ASTRA USB PRINT configuration
  * @param  none
  * @retval bEnablePrintingUsb: 8bit unsigned integer
  */
uint8_t AstraUsbPrintConfig(void);

/**
* @}
*/

/** @addtogroup DATA_MANAGEMENT_FunctionsDefines ASTRA Data management - Functions Defines
  * @brief Functions defines
  * @{
 */

/* Private function ----------------------------------------------------------*/

/* ADC functions */

uint16_t Astra_GetBatteryVoltage_mV( void )
{
  float battery_voltage = 0;

  battery_voltage =  (( (float) (aADCxConvertedData[ASTRA_ADC_CHANNEL_VBAT] * ASTRA_VREF) / 4096) * ASTRA_ADC_VBAT_RESISTIVE_RATIO);
  return ((uint16_t)(battery_voltage / (float)1.0));
}

uint16_t Astra_GetUSBVoltage_mV(void)
{
  float usb_voltage = 0;
  usb_voltage =  (( (float) (aADCxConvertedData[ASTRA_ADC_CHANNEL_VUSB] * ASTRA_VREF) / 4096) * ASTRA_ADC_VUSB_RESISTIVE_RATIO);

  return ((uint16_t)(usb_voltage / (float)1.0));
}

void Astra_PwrInit(void)
{
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_pwr_init();
  custom_astra_pwr_enable();

  if(!PlatformStatus.b.ADC_INIZIALIZED)
  {
    PlatformStatus.b.ADC_INIZIALIZED = SET;
    custom_astra_adc_init();
  }

  Astra_ReadADC(); //get battery and USB voltage at boot
#endif //USE_ASTRA_MAINBOARD_BSP

  custom_sob_v_reg2_on();
  HAL_Delay(200); //wait VDD rise time
#if USE_STSAFE
  custom_stsafe_reset(1);
#endif //USE_STSAFE
}

#if USE_ASTRA_MAINBOARD_BSP
void Astra_ReadADC(void)
{
  PRINTF_INFO_TIME("Astra_ReadADC()\r\n");

  custom_astra_adc_measurement((uint32_t*)aADCxConvertedData, ASTRA_ADC_CHANNELS, ASTRA_ADC_TIMEOUT);

  NEW_DATA_SET(AstraEngData.f.POW_MAN_NEW_DATA);
  AstraEngData.d.batteryVoltage = Astra_GetBatteryVoltage_mV();
  AstraEngData.d.USBVoltage = Astra_GetUSBVoltage_mV();

  PRINTF_INFO("Battery voltage: %.3f (Raw data %d)\r\n", ( (float) (AstraEngData.d.batteryVoltage/1000.000)), aADCxConvertedData[ASTRA_ADC_CHANNEL_VBAT]);
  PRINTF_INFO("USB voltage: %.3f (Raw data %d)\r\n", ( (float) (AstraEngData.d.USBVoltage/1000.000)), aADCxConvertedData[ASTRA_ADC_CHANNEL_VUSB]);
  
  PRINTF_INFO("Temp sensor: (Raw data %d)\r\n", aADCxConvertedData[ASTRA_ADC_CHANNEL_TEMPSENSOR]);
  PRINTF_INFO("Vref: (Raw data %d)\r\n", aADCxConvertedData[ASTRA_ADC_CHANNEL_VREF]);
}
#endif //USE_ASTRA_MAINBOARD_BSP

#if USE_SENSORS
/* SENSORS functions */
void ReadSensors(void)
{
  PRINTF_INFO_TIME("ReadSensors()\r\n");

  NEW_DATA_SET(AstraEngData.f.ENVIRONMENTAL_NEW_DATA);
  NEW_DATA_SET(AstraEngData.f.INERTIAL_NEW_DATA);
  BSP_SENSOR_LP_ACC_GetAxes(&AstraEngData.d.LPAccValue);
  BSP_SENSOR_LP_ACC_GetTemp(&AstraEngData.d.LPTempValue);
  BSP_SENSOR_ACC_GetAxes(&AstraEngData.d.AccValue);
  BSP_SENSOR_GYR_GetAxes(&AstraEngData.d.GyrValue);
  BSP_SENSOR_PRESS_GetValue(&AstraEngData.d.PressValue);
  BSP_SENSOR_TEMP_GetValue(&AstraEngData.d.TempValue);
  BSP_SENSOR_HUM_GetValue(&AstraEngData.d.HumValue);
  //BSP_SENSOR_LP_ACC_Get_Event_Status(&AstraEngData.d.LPAccInt); //only for debug CB
  //BSP_SENSOR_ACC_Get_Event_Status(&AstraEngData.d.AccInt); //only for debug CB

  PRINTF_INFO("Got axel x %d y %d z %d mg\r\n", AstraEngData.d.AccValue.x, AstraEngData.d.AccValue.y, AstraEngData.d.AccValue.z);
  PRINTF_INFO("Got LP axel x %d y %d z %d mg\r\n", AstraEngData.d.LPAccValue.x, AstraEngData.d.LPAccValue.y, AstraEngData.d.LPAccValue.z);
  PRINTF_INFO("Got gyro x %d y %d z %d mdps\r\n", AstraEngData.d.GyrValue.x, AstraEngData.d.GyrValue.y, AstraEngData.d.GyrValue.z);
  PRINTF_INFO("Got press %f mbar\r\n", AstraEngData.d.PressValue);
  PRINTF_INFO("Got temp %f deg\r\n", AstraEngData.d.TempValue);
  PRINTF_INFO("Got hum %f %\r\n", AstraEngData.d.HumValue);
  PRINTF_INFO("Events LP: 0x%.02X\r\n", AstraEngData.d.LPAccInt );
  PRINTF_INFO("Events: 0x%.02X\r\n", AstraEngData.d.AccInt );

  ManageSensorsEvents();
}

void ManageSensorsEvents(void)
{
  if(PlatformStatus.b.MEMS_INT1_EVENT == 1)
  {
    PRINTF_INFO("MEMS_INT1 detected\r\n");
    PlatformStatus.b.MEMS_INT1_EVENT = 0;
    NEW_DATA_SET(AstraEngData.f.MEMS_EVENT_NEW_DATA);
    BSP_SENSOR_LP_ACC_Get_Event_Status(&AstraEngData.d.LPAccInt);
    BSP_SENSOR_LP_ACC_Clear_Wkp();
  }
  if(PlatformStatus.b.MEMS_INT2_EVENT == 1)
  {
    PRINTF_INFO("MEMS_INT2 detected\r\n");
    PlatformStatus.b.MEMS_INT2_EVENT = 0;

    NEW_DATA_SET(AstraEngData.f.MEMS_EVENT_NEW_DATA);
    BSP_SENSOR_ACC_Get_Event_Status(&AstraEngData.d.AccInt);
  }
}
#endif //USE_SENSORS

uint8_t AstraBlePrintConfig(void)
{
  return bEnablePrintingBle;
}

void AstraDisablePrintBle(void)
{
  bEnablePrintingBle = 0;
}

void AstraEnablePrintBle(void)
{
  bEnablePrintingBle = 1;
}

void AstraEnablePrintUsb(void)
{
  bEnablePrintingUsb = ENABLE;
}

void AstraDisablePrintUsb(void)
{
  bEnablePrintingUsb = DISABLE;
}

uint8_t AstraUsbPrintConfig(void)
{
  return bEnablePrintingUsb;
}

uint8_t DBG_PRINTF_LOG(uint8_t Q, uint16_t L, const char* sFormatString, ...)
{
  uint8_t xRes = 1;
  va_list ap;
  if (  ( PlatformStatus.b.USB_TRACE || PlatformStatus.b.UART_TRACE || PlatformStatus.b.BLE_TRACE ) && (nCharsToPrint < STD_OUT_ARRAY_LENGHT) )
  {
    va_start (ap, sFormatString);
    uint16_t nCharsToAdd = vsnprintf(sStdOutString+nCharsToPrint, STD_OUT_ARRAY_LENGHT-nCharsToPrint, sFormatString, ap);
    nCharsToPrint += nCharsToAdd;
    va_end (ap);

    if(bEnablePrintingTerm && PlatformStatus.b.DEBUGGER_CONNECTED)
    {
      printf(sStdOutString);
    }

    if(!Q)
    {
      if(bEnablePrintingUsb && PlatformStatus.b.USB_TRACE)
      {
#if USE_USB
        xRes |= CUSTOM_USB_SendMessage((uint8_t*)sStdOutString, /*strlen(sStdOutString)*/nCharsToPrint, CUSTOM_USB_DEFAULT_ATTEMPS);
        HAL_Delay(2);
#endif //USE_USB
      }

#if USE_BLE
      if(bEnablePrintingBle && PlatformStatus.b.BLE_TRACE && sFormatString[0]!=TEXT_COLOR_ESCAPE_CHAR)
      {
        xRes |= !(/*BLE_STATUS_SUCCESS*/ 0 == Term_Update((uint8_t*)sStdOutString, nCharsToPrint));
        HAL_Delay(20);
      }
#endif //USE_BLE

      if(bEnablePrintingUart && PlatformStatus.b.UART_TRACE)
      {
        xRes |= HAL_UART_Transmit(&H_DBG_HUART, (uint8_t *)sStdOutString, nCharsToPrint, 0x1000);
      }

      memset(sStdOutString, 0x0, STD_OUT_ARRAY_LENGHT);
      nCharsToPrint = 0;
    }
  }

  return xRes;
}

int GNSS_PRINT(char *pBuffer)
{
  PRINTF_INFO(pBuffer);

  return 0;
}

#if USE_GNSS

/* GNSS functions */
void GnssGetData(void)
{
  GNSSParser_Status_t status, check;

  const CUSTOM_GNSS_Msg_t *gnssMsg;

//  PRINTF_CRITICAL("\r\n");
  for(;;)
  {
  gnssMsg = CUSTOM_GNSS_GetMessage(CUSTOM_TESEO_LIV3F);

  if(gnssMsg == NULL)
  {
    return;
  }
  check = GNSS_PARSER_CheckSanity((uint8_t *)gnssMsg->buf, gnssMsg->len);

  if(check != GNSS_PARSER_ERROR)
  {
    for(uint32_t m = 0; m < NMEA_MSGS_NUM; m++)
    {
      status = GNSS_PARSER_ParseMsg(&GNSSParser_Data, (eNMEAMsg)m, (uint8_t *)gnssMsg->buf);

      if((status != GNSS_PARSER_ERROR) /* && ((eNMEAMsg)m == GPGGA) */ )
      {
        if((eNMEAMsg)m == GPGGA)
        {
#if ASTRA_GNSS_DEBUG
          PRINTF_CRITICAL("GGA\r\n");
#endif //ASTRA_GNSS_DEBUG
        }
        if((eNMEAMsg)m == GPRMC)
        {
#if ASTRA_GNSS_DEBUG
          PRINTF_CRITICAL("RMC\r\n");
#endif //ASTRA_GNSS_DEBUG
        }
        if( ((eNMEAMsg)m == GSA) || ((eNMEAMsg)m == GSV) )
        {
          if(!PlatformStatus.b.GNSS_CHECK_NMEA_CONF)
          {
            PlatformStatus.b.GNSS_WRONG_NMEA = 1;
          }
        }
      }
    }
  }

#if ASTRA_GNSS_DEBUG

      static char string1[10] = "$";
      static char string2[10] = "$PSTMGET";
      static char string3[10] = "$PSTMSET";

      if(PlatformStatus.b.GNSS_NMEA)
      {
        if(0==memcmp(gnssMsg->buf, string1, strlen(string1)))PRINTF_CRITICAL((char const*)gnssMsg->buf);
      }

      if(0==memcmp(gnssMsg->buf, string2, strlen(string2)))PRINTF_CRITICAL((char const*)gnssMsg->buf);
      if(0==memcmp(gnssMsg->buf, string3, strlen(string3)))PRINTF_CRITICAL((char const*)gnssMsg->buf);

      static uint8_t sendflag1 = 0;
      // set teseo nmea mask to send specific sentences
//      const uint32_t NMEA_MASK_GPGGA      = 0x2;
//      const uint32_t NMEA_MASK_GSA        = 0x4;
//      const uint32_t NMEA_MASK_GPRMC      = 0x40;
//      const uint32_t NMEA_MASK_PSTMTG     = 0x100;
//      const uint32_t NMEA_MASK_GSV        = 0x80000;
//      const uint32_t NMEA_MASK_GPGLL      = 0x100000;
//      const uint32_t NMEA_MASK_PSTMCPU    = 0x800000;
      static char strcmd1[] = "$PSTMSETPAR,1201,00000042";
      if(sendflag1)
      {
        GNSS_DATA_SendCommand((uint8_t*)strcmd1);
        HAL_Delay(1000);
        GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
        HAL_Delay(1000);
        sendflag1 = 0;
      }

      static uint8_t sendflag2 = 0;
      // set teseo nmea mask to default (GGA, GSA, RMC, GSV, GLL, PSTMCPU)
      static char strcmd2[] = "$PSTMSETPAR,1201,00980056";
      if(sendflag2)
      {
        GNSS_DATA_SendCommand((uint8_t*)strcmd2);
        HAL_Delay(1000);
        GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
        HAL_Delay(1000);
        sendflag2 = 0;
      }
#endif //ASTRA_GNSS_DEBUG

  CUSTOM_GNSS_ReleaseMessage(CUSTOM_TESEO_LIV3F, gnssMsg);
  }
}

/**
  * @brief  Convert latitude/longitude coordinate from sexagesimal to decimal format
  * @param  float x coordinate
  * @param  uint8_t sign 1 for negative 0 for positive
  * @retval coordinate in decimal format
  */
static float convertCoord( float x, uint8_t sign )
{
  int degrees;
  float minutes;
  float ret;

  degrees = (int)(x / 100.0F);
  minutes = x - degrees*100.0F;
  ret = degrees + minutes / 60.0F;
  if (sign==1)
    ret = -ret;

  return ret;
}

void GNSS_DataInit(void)
{
  GNSS_PARSER_Init(&GNSSParser_Data);
}

char *GnssGetVersion(void)
{
  if(GNSSParser_Data.pstmver_data.pstmver_string[0] != 0)
  {
    return (char*)GNSSParser_Data.pstmver_data.pstmver_string;
  }
  else
  {
    return "NO GNSS VERSION AVAILABLE";
  }
}

void GnssRequestVersion(void)
{
  PRINTF_INFO("TESEO GETVER\r\n");
  if(GNSSParser_Data.pstmver_data.pstmver_string[0] != 0)
  {
    PlatformStatus.b.GNSS_VERSION_SHOWED = 1;
    PRINTF_VERBOSE("GNSS FW version: ");
    PRINTF_VERBOSE((char const*)GNSSParser_Data.pstmver_data.pstmver_string);
    PRINTF_VERBOSE("\r\n");
  }
  else
  {
    PRINTF_INFO("Get Binary Image Version\r\n");
    GNSS_DATA_SendCommand((uint8_t *)"$PSTMGETSWVER,6");
  }
}

void GnssSetNmeaConfigRMC_GGA(void)
{
  char strcmd1[] = "$PSTMSETPAR,1201,00000042";
  GNSS_DATA_SendCommand((uint8_t*)strcmd1);
  HAL_Delay(100);
  GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
  HAL_Delay(100);

  PlatformStatus.b.GNSS_WRONG_NMEA = 0;
}

void GnssSetNmeaConfigDefault(void)
{
  char strcmd1[] = "$PSTMSETPAR,1201,00980056";
  GNSS_DATA_SendCommand((uint8_t*)strcmd1);
  HAL_Delay(100);
  GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
  HAL_Delay(100);

  PlatformStatus.b.GNSS_WRONG_NMEA = 0;
}

void PrintGnssInfo(GNSSParser_Data_t *pParserData)
{
  PRINTF_INFO("Got coordinates: %f %c, %f %c, ALT:%f\r\n",
               ABS(AstraEngData.d.gnss_latitude), LAT_NORD_SUD(AstraEngData.d.gnss_latitude),
               ABS(AstraEngData.d.gnss_longitude), LON_EAST_WEST(AstraEngData.d.gnss_longitude),
               AstraEngData.d.gnss_altitude);
#if 0
  //print additional GNSS data
  PRINTF_INFO("Got coordinates %f %f %f\r\n",AstraEngData.d.gnss_latitude,AstraEngData.d.gnss_longitude,AstraEngData.d.gnss_altitude);
  PRINTF_INFO("TIMERMC %.2d:%.2d:%.2d\r\n",pParserData->gprmc_data.utc.hh,pParserData->gprmc_data.utc.mm,pParserData->gprmc_data.utc.ss);
  PRINTF_INFO("DATERMC %.2d/%.2d/%.2d\r\n",pParserData->gprmc_data.date/10000,(pParserData->gprmc_data.date%10000)/100,pParserData->gprmc_data.date%100);
#endif
  PRINTF_INFO("GNSSVER %s\r\n",pParserData->pstmver_data.pstmver_string);
  PRINTF_INFO("%.2d/%.2d/%.2d %.2d:%.2d:%.2d - %d sats - Fix %d\r\n",
               pParserData->gprmc_data.date/10000,(pParserData->gprmc_data.date%10000)/100,pParserData->gprmc_data.date%100,
               pParserData->gprmc_data.utc.hh,pParserData->gprmc_data.utc.mm,pParserData->gprmc_data.utc.ss,
               pParserData->gpgga_data.sats, pParserData->gpgga_data.valid);
  if(PlatformStatus.b.GNSS_CHECK_NMEA_CONF)
  {
    PRINTF_INFO("%d sats in view\r\n", pParserData->gsv_data.tot_sats);
  }
  if(PlatformStatus.b.GNSS_REPORT)
  {
    PRINTF_TERMINAL("Got coordinates: %f %c, %f %c, ALT:%f\r\n",
                 ABS(AstraEngData.d.gnss_latitude), LAT_NORD_SUD(AstraEngData.d.gnss_latitude),
                 ABS(AstraEngData.d.gnss_longitude), LON_EAST_WEST(AstraEngData.d.gnss_longitude),
                 AstraEngData.d.gnss_altitude);
    PRINTF_TERMINAL("GNSSVER %s\r\n",pParserData->pstmver_data.pstmver_string);
    PRINTF_TERMINAL("%.2d/%.2d/%.2d %.2d:%.2d:%.2d - %d sats - Fix %d\r\n",
                 pParserData->gprmc_data.date/10000,(pParserData->gprmc_data.date%10000)/100,pParserData->gprmc_data.date%100,
                 pParserData->gprmc_data.utc.hh,pParserData->gprmc_data.utc.mm,pParserData->gprmc_data.utc.ss,
                 pParserData->gpgga_data.sats, pParserData->gpgga_data.valid);
    if(PlatformStatus.b.GNSS_CHECK_NMEA_CONF)
    {
      PRINTF_TERMINAL("%d sats in view\r\n", pParserData->gsv_data.tot_sats);
    }
  }
}

void ResetGnssParserData(void)
{
    memset(&GNSSParser_Data.gprmc_data, 0x00, sizeof(GNSSParser_Data.gprmc_data));
    memset(&GNSSParser_Data.gpgga_data, 0x00, sizeof(GNSSParser_Data.gpgga_data));
    memset(&GNSSParser_Data.gsv_data, 0x00, sizeof(GNSSParser_Data.gsv_data));
    GNSSParser_Data.gpgga_data.valid = 0xFF;//INVALID;
    GNSSParser_Data.gpgga_data.sats = 0;
    GNSSParser_Data.gsv_data.tot_sats = -1;
}

void ManageGnss(void)
{
    if(PlatformStatus.b.GNSS_READ_TRIGGERED)
    {
      ReadGnss(GNSS_READ_TIMEOUT, GNSS_WAIT_FIX);
      PlatformStatus.b.GNSS_READ_TRIGGERED = 0;
    }
}

void ReadGnss(uint32_t nTimeout, uint8_t bWaitFix)
{
  PRINTF_INFO_TIME("ReadGnss()\r\n");

  if(!PlatformStatus.b.GNSS_VERSION_SHOWED)
  {
    GnssRequestVersion();
  }

  if(PlatformStatus.b.GNSS_WRONG_NMEA)
  {
    GnssSetNmeaConfigRMC_GGA();
  }

  static uint32_t nEndTick = 0;
  if(HAL_GetTick() > nEndTick)
  {
    nEndTick = nEndTick + nTimeout;
    ResetGnssParserData();
  }

  GnssGetData();
  if( (GNSSParser_Data.gprmc_data.date != 0) && (GNSSParser_Data.gpgga_data.valid != 0xFF) )
  {
    NEW_DATA_SET(AstraEngData.f.GNSS_NEW_DATA);
    AstraEngData.d.gnss_latitude = convertCoord( GNSSParser_Data.gpgga_data.xyz.lat,
                                                GNSSParser_Data.gpgga_data.xyz.ns=='N' ? 0 : 1 );
    AstraEngData.d.gnss_longitude = convertCoord( GNSSParser_Data.gpgga_data.xyz.lon,
                                                 GNSSParser_Data.gpgga_data.xyz.ew=='E' ? 0 : 1 );
    AstraEngData.d.gnss_altitude = GNSSParser_Data.gpgga_data.xyz.alt;
    AstraEngData.d.gnss_sats = GNSSParser_Data.gpgga_data.sats;
    AstraEngData.d.gnss_fix_validity = GNSSParser_Data.gpgga_data.valid;

    PRINTF_INFO_TIME("GGA+RMC");
    if(!PlatformStatus.b.GNSS_CHECK_NMEA_CONF)
    {
      AstraLedColor(ASTRA_LED_OFF);

      PrintGnssInfo(&GNSSParser_Data);
      ResetGnssParserData();
    }
    else if(GNSSParser_Data.gsv_data.tot_sats != -1 && PlatformStatus.b.GNSS_CHECK_NMEA_CONF)
    {
      PRINTF_INFO_TIME("+GSV");
      AstraLedColor(ASTRA_LED_OFF);

      PrintGnssInfo(&GNSSParser_Data);
      ResetGnssParserData();
    }
    else
    {
#if USE_SEQUENCER
      UTIL_SEQ_SetTask( (uint32_t)1<<ASTRA_GNSS_TASK_ID, CFG_SCH_PRIO_0);
#endif //USE_SEQUENCER
    }
    PRINTF_INFO_TIME("\r\n");
  }
  else
  {
#if USE_SEQUENCER
    UTIL_SEQ_SetTask( (uint32_t)1<<ASTRA_GNSS_TASK_ID, CFG_SCH_PRIO_0);
#endif //USE_SEQUENCER
  }
}
#endif //USE_GNSS

/* USE_STSAFE */
void stsafe_process(void)
{
#if USE_STSAFE
  if(PlatformStatus.s.STSAFE_PERS == ('p' | 'e'<<8 | 'r'<<16 | 's'<<24))
  {
    if(!StSafePairing())
    {
      PRINTF_VERBOSE("STSAFE PAIRING: OK\r\n");
    }
    else
    {
      PRINTF_VERBOSE("STSAFE PAIRING: FAIL\r\n");
    }
    PlatformStatus.s.STSAFE_PERS = 0;
  }
#endif //USE_STSAFE

  return;

#if 0
#if USE_STSAFE
  uint8_t Certificate[2048];
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
#endif //USE_STSAFE
#endif //0
}

/* USE_STM32WL */
#if USE_STM32WL
void JoinedCallback(uint8_t bJoinStatus, uint8_t bJoinComplete)
{
  PlatformStatus.b.APP_LORA_JOINED = bJoinStatus;
  PlatformStatus.b.APP_LORA_JOIN_COMPLETED = bJoinComplete;
  PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS = 0;
  if(!LORA_JOIN_ABP)
  {
    PlatformStatus.b.APP_LORA_CONFIG_IN_PROGRESS = 1;
  }
}

void BusyErrorCallback(void)
{

}

void SendConfirmedCallback(void)
{
  PlatformStatus.b.LORA_ACK_RECEIVED = 1;
  PRINTF_VERBOSE("LoRa message confirmation received (ACK)\r\n");
}

void DownlinkMsg(uint32_t nFPort, uint32_t nLen, uint8_t *dlbuf)
{
  PRINTF_VERBOSE("LoRa DownLink message on port %d, len %d bytes\r\n", nFPort, nLen);
  for(uint32_t i=0;i<nLen;i++)
  {
    PRINTF_VERBOSE("%.2X", dlbuf[i]);
  }
  PRINTF_VERBOSE("\r\n");
}

uint8_t GetWlFwVersion()
{
  uint8_t nRet = 1;
  CUSTOM_MODEM_versions_t wlvers;
  PRINTF_VERBOSE("GET STM32WL FW version\r\n");
  if(!BSP_LoRaGetVersion(&wlvers)) //if(!LoRa_GetVersion(&wlvers))
  {
    PRINTF_VERBOSE("GetWlFwVersion OK\r\n");
    PRINTF_VERBOSE("M4 APP_VERSION:     V%d.%d.%d\r\n", wlvers.m4app.s.major, wlvers.m4app.s.minor, wlvers.m4app.s.rev);
    PRINTF_VERBOSE("M0PLUS_APP_VERSION: V%d.%d.%d\r\n", wlvers.m0plusapp.s.major, wlvers.m0plusapp.s.minor, wlvers.m0plusapp.s.rev);
    PRINTF_VERBOSE("MW_LORAWAN_VERSION: V%d.%d.%d\r\n", wlvers.mwlora.s.major, wlvers.mwlora.s.minor, wlvers.mwlora.s.rev);
    PRINTF_VERBOSE("MW_RADIO_VERSION:   V%d.%d.%d\r\n", wlvers.mwradio.s.major, wlvers.mwradio.s.minor, wlvers.mwradio.s.rev);

    PlatformStatus.s.STM32WL_CM4_VER = wlvers.m4app.w;
    PlatformStatus.s.STM32WL_CM0_VER = wlvers.m0plusapp.w;
    PlatformStatus.s.STM32WL_MWLORA_VER = wlvers.mwlora.w;
    PlatformStatus.s.STM32WL_MWRADIO_VER = wlvers.mwradio.w;
    nRet = 0;
  }
  return nRet;
}

uint8_t GetWlLoraKeys()
{
  uint8_t nRet = 1;

  PRINTF_VERBOSE("GET LORA KEYS FROM WL DEVICE\r\n");//just to show them in the traces

  if(!BSP_LoRaGetDeviceEUI(pDevEui))
  {
    PRINTF_VERBOSE("BSP_LoRaGetDeviceEUI OK\r\n");
    PRINTF_VERBOSE("DevEui = ");
    PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                pDevEui[0], pDevEui[1], pDevEui[2], pDevEui[3],
                pDevEui[4], pDevEui[5], pDevEui[6], pDevEui[7]);

    if(!BSP_LoRaGetAppEUI(pAppEui))
    {
      PRINTF_VERBOSE("BSP_LoRaGetAppEUI OK\r\n");
      PRINTF_VERBOSE("AppEUI = ");
      PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pAppEui[0], pAppEui[1], pAppEui[2], pAppEui[3],
                  pAppEui[4], pAppEui[5], pAppEui[6], pAppEui[7]);
      if(!BSP_LoRaGetAppKEY(pAppKey))
      {
        PRINTF_VERBOSE("BSP_LoRaGetAppKEY OK\r\n");
        PRINTF_VERBOSE("AppKEY = ");
        PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                    pAppKey[0], pAppKey[1], pAppKey[2], pAppKey[3],
                    pAppKey[4], pAppKey[5], pAppKey[6], pAppKey[7],
                    pAppKey[8], pAppKey[9], pAppKey[10], pAppKey[11],
                    pAppKey[12], pAppKey[13], pAppKey[14], pAppKey[15]);

        if(!BSP_LoRaGetNwkKEY(pNwkKey))
        {
          PRINTF_VERBOSE("BSP_LoRaGetNwkKEY OK\r\n");
          PRINTF_VERBOSE("NwkKEY = ");
          PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                      pNwkKey[0], pNwkKey[1], pNwkKey[2], pNwkKey[3],
                      pNwkKey[4], pNwkKey[5], pNwkKey[6], pNwkKey[7],
                      pNwkKey[8], pNwkKey[9], pNwkKey[10], pNwkKey[11],
                      pNwkKey[12], pNwkKey[13], pNwkKey[14], pNwkKey[15]);

          nRet = 0;
        }
      }
    }
  }

  return nRet;
}

uint8_t SetLoraKeys(void)
{
  uint8_t nRet = 0;
  PRINTF_VERBOSE("SHOW NEW LoRa KEYS\r\n");

  PRINTF_VERBOSE("NewAppEui = ");
  PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
              NewAppEui[0], NewAppEui[1], NewAppEui[2], NewAppEui[3],
              NewAppEui[4], NewAppEui[5], NewAppEui[6], NewAppEui[7]);
  PRINTF_VERBOSE("NewAppKey = ");
  PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
              NewAppKey[0], NewAppKey[1], NewAppKey[2], NewAppKey[3],
              NewAppKey[4], NewAppKey[5], NewAppKey[6], NewAppKey[7],
              NewAppKey[8], NewAppKey[9], NewAppKey[10], NewAppKey[11],
              NewAppKey[12], NewAppKey[13], NewAppKey[14], NewAppKey[15]);
  PRINTF_VERBOSE("NewNwkKey = ");
  PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
              NewNwkKey[0], NewNwkKey[1], NewNwkKey[2], NewNwkKey[3],
              NewNwkKey[4], NewNwkKey[5], NewNwkKey[6], NewNwkKey[7],
              NewNwkKey[8], NewNwkKey[9], NewNwkKey[10], NewNwkKey[11],
              NewNwkKey[12], NewNwkKey[13], NewNwkKey[14], NewNwkKey[15]);

  if(!SET_APPEUI || !BSP_LoRaSetAppEUI(NewAppEui))
  {
    if(SET_APPEUI)PRINTF_VERBOSE("BSP_LoRaSetAppEUI OK\r\n");

    if(!SET_APPKEY || !BSP_LoRaSetAppKEY(NewAppKey))
    {
      if(SET_APPKEY)PRINTF_VERBOSE("BSP_LoRaSetAppKEY OK\r\n");

      if(!SET_NWKKEY || !BSP_LoRaSetNwkKEY(NewNwkKey))
      {
        if(SET_NWKKEY)PRINTF_VERBOSE("BSP_LoRaSetNwkKEY OK\r\n");
      }
      else
      {
        nRet = 1;
      }
    }
    else
    {
      nRet = 1;
    }
  }
  else
  {
    nRet = 1;
  }
  return nRet;
}



uint8_t GetWlLoraSKeys(void)
{
   uint8_t nRet = 1;

  PRINTF_VERBOSE("GET LORA KEYS FROM WL DEVICE\r\n");//just to show them in the traces

  if(!BSP_LoRaGetDADD(&DevAddres))
  {
    PRINTF_VERBOSE("BSP_LoRaGetDevAddress OK\r\n");
    PRINTF_VERBOSE("DevAddres = ");
    PRINTF_VERBOSE("%02X:%02X:%02X:%02X\r\n",
            ( unsigned )( ( unsigned char * )( &DevAddres ) )[3],
            ( unsigned )( ( unsigned char * )( &DevAddres ) )[2],
            ( unsigned )( ( unsigned char * )( &DevAddres ) )[1],
            ( unsigned )( ( unsigned char * )( &DevAddres ) )[0] );
    if(!BSP_LoRaGetAppSKEY(pAppSKey))
    {
      PRINTF_VERBOSE("BSP_LoRaGetAppKEY OK\r\n");
      PRINTF_VERBOSE("AppSKEY = ");
      PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pAppSKey[0], pAppSKey[1], pAppSKey[2], pAppSKey[3],
                  pAppSKey[4], pAppSKey[5], pAppSKey[6], pAppSKey[7],
                  pAppSKey[8], pAppSKey[9], pAppSKey[10], pAppSKey[11],
                  pAppSKey[12], pAppSKey[13], pAppSKey[14], pAppSKey[15]);

      if(!BSP_LoRaGetNwkSKEY(pNwkSKey))
      {
        PRINTF_VERBOSE("BSP_LoRaGetNwkSKEY OK\r\n");
        PRINTF_VERBOSE("NwkSKEY = ");
        PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                    pNwkSKey[0], pNwkSKey[1], pNwkSKey[2], pNwkSKey[3],
                    pNwkSKey[4], pNwkSKey[5], pNwkSKey[6], pNwkSKey[7],
                    pNwkSKey[8], pNwkSKey[9], pNwkSKey[10], pNwkSKey[11],
                    pNwkSKey[12], pNwkSKey[13], pNwkSKey[14], pNwkSKey[15]);
        nRet = 0;
      }
    }
  }
  return nRet;
}

uint8_t SetLoraSKeys(void)
{
  uint8_t nRet = 0;
  PRINTF_VERBOSE("SHOW NEW LoRa KEYS\r\n");

  PRINTF_VERBOSE("NewDevAddres = ");
  PRINTF_VERBOSE("%02X:%02X:%02X:%02X\r\n",
            ( unsigned )( ( unsigned char * )( &NewDevAddres ) )[3],
            ( unsigned )( ( unsigned char * )( &NewDevAddres ) )[2],
            ( unsigned )( ( unsigned char * )( &NewDevAddres ) )[1],
            ( unsigned )( ( unsigned char * )( &NewDevAddres ) )[0] );
  PRINTF_VERBOSE("NewAppSKey = ");
  PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
              NewAppSKey[0], NewAppSKey[1], NewAppSKey[2], NewAppSKey[3],
              NewAppSKey[4], NewAppSKey[5], NewAppSKey[6], NewAppSKey[7],
              NewAppSKey[8], NewAppSKey[9], NewAppSKey[10], NewAppSKey[11],
              NewAppSKey[12], NewAppSKey[13], NewAppSKey[14], NewAppSKey[15]);
  PRINTF_VERBOSE("NewNwkSKey = ");
  PRINTF_VERBOSE("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
              NewNwkSKey[0], NewNwkSKey[1], NewNwkSKey[2], NewNwkSKey[3],
              NewNwkSKey[4], NewNwkSKey[5], NewNwkSKey[6], NewNwkSKey[7],
              NewNwkSKey[8], NewNwkSKey[9], NewNwkSKey[10], NewNwkSKey[11],
              NewNwkSKey[12], NewNwkSKey[13], NewNwkSKey[14], NewNwkSKey[15]);

  if(!BSP_LoRaSetDADD(NewDevAddres))
  {
    PRINTF_VERBOSE("BSP_LoRaSetDevAddress OK\r\n");
    if(!BSP_LoRaSetAppSKEY(NewAppSKey))
    {
      PRINTF_VERBOSE("BSP_LoRaSetAppSKEY OK\r\n");
      if(!BSP_LoRaSetNwkSKEY(NewNwkSKey))
      {
        PRINTF_VERBOSE("BSP_LoRaSetNwkSKEY OK\r\n");
      }
      else
      {
        nRet = 1;
      }
    }
    else
    {
      nRet = 1;
    }
  }
  else
  {
    nRet = 1;
  }
  return nRet;
}



uint8_t GetLoraValidationFlag(uint8_t nMask)
{
  int32_t nRet = 1;
  int32_t nRetMask = 0;
  uint8_t validationstring[] = "K"; //"DJNA";

  if(nMask & DEV_EUI_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_ReadData( &validationstring[0], EEPROM_LORA_KEYS_VALID_ADDR+0, 1);
    if(!nRet)
    {
      if(validationstring[0] == 'D' )nRetMask |= DEV_EUI_VALIDATION_MASK;
    }
  }

  if(nMask & APP_EUI_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_ReadData( &validationstring[0], EEPROM_LORA_KEYS_VALID_ADDR+1, 1);
    if(!nRet)
    {
      if(validationstring[0] == 'J' )nRetMask |= APP_EUI_VALIDATION_MASK;
    }
  }

  if(nMask & NWK_KEY_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_ReadData( &validationstring[0], EEPROM_LORA_KEYS_VALID_ADDR+2, 1);
    if(!nRet)
    {
      if(validationstring[0] == 'N' )nRetMask |= NWK_KEY_VALIDATION_MASK;
    }
  }

  if(nMask & APP_KEY_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_ReadData( &validationstring[0], EEPROM_LORA_KEYS_VALID_ADDR+3, 1);
    if(!nRet)
    {
      if(validationstring[0] == 'A' )nRetMask |= APP_KEY_VALIDATION_MASK;
    }
  }
  return nRetMask;
}

void SaveLoraValidationString(uint8_t nMask)
{
  int32_t nRet = 1;
  uint8_t validationstring[] = "DJNA";

  if(nMask & DEV_EUI_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_WriteData( &validationstring[0], EEPROM_LORA_KEYS_VALID_ADDR+0, 1);
  }

  if(nMask & APP_EUI_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_WriteData( &validationstring[1], EEPROM_LORA_KEYS_VALID_ADDR+1, 1);
  }

  if(nMask & NWK_KEY_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_WriteData( &validationstring[2], EEPROM_LORA_KEYS_VALID_ADDR+2, 1);
  }

  if(nMask & APP_KEY_VALIDATION_MASK)
  {
    nRet = AstraEEPROM_WriteData( &validationstring[3], EEPROM_LORA_KEYS_VALID_ADDR+3, 1);
  }
  UNUSED(nRet);
}

int32_t SaveLoraAppEui(uint8_t *PtrAppEui)
{
  int32_t nRet = 1;

  nRet = AstraEEPROM_WriteData( PtrAppEui, JOIN_EUI_EEPROM_ADDRESS, JOIN_EUI_EEPROM_LEN);

  SaveLoraValidationString(DEV_EUI_VALIDATION_MASK | APP_EUI_VALIDATION_MASK);

  return nRet;
}

int32_t SaveLoraNwkKey(uint8_t *PtrNwkKEY)
{
  int32_t nRet = 1;

  nRet = AstraEEPROM_WriteData( PtrNwkKEY, NWK_KEY_EEPROM_ADDRESS, NWK_KEY_EEPROM_LEN);

  SaveLoraValidationString(NWK_KEY_VALIDATION_MASK);

  return nRet;
}

int32_t SaveLoraAppKey(uint8_t *PtrAppKey)
{
  int32_t nRet = 1;

  nRet = AstraEEPROM_WriteData( PtrAppKey, APP_KEY_EEPROM_ADDRESS, APP_KEY_EEPROM_LEN);

  SaveLoraValidationString(APP_KEY_VALIDATION_MASK);

  return nRet;
}

int32_t LoadLoraAppEui(uint8_t *pTmpkey)
{
  int32_t nRet = 1;
  if(GetLoraValidationFlag(APP_EUI_VALIDATION_MASK))
  {
    uint8_t tmpbuf[20] = { 0 };

    memset(tmpbuf, 0, JOIN_EUI_EEPROM_LEN);
    nRet = AstraEEPROM_ReadData( tmpbuf, JOIN_EUI_EEPROM_ADDRESS, JOIN_EUI_EEPROM_LEN);
    memcpy(pTmpkey, tmpbuf, JOIN_EUI_EEPROM_LEN);
  }

  return nRet;
}

int32_t LoadLoraNwkKey(uint8_t *pTmpkey)
{
  int32_t nRet = 1;

  if(GetLoraValidationFlag(NWK_KEY_VALIDATION_MASK))
  {
    uint8_t tmpbuf[20] = { 0 };

    memset(tmpbuf, 0, NWK_KEY_EEPROM_LEN);
    nRet = AstraEEPROM_ReadData( tmpbuf, NWK_KEY_EEPROM_ADDRESS, NWK_KEY_EEPROM_LEN);
    memcpy(pTmpkey, tmpbuf, NWK_KEY_EEPROM_LEN);
  }

  return nRet;
}

int32_t LoadLoraAppKey(uint8_t *pTmpkey)
{
  int32_t nRet = 1;

  if(GetLoraValidationFlag(APP_KEY_VALIDATION_MASK))
  {
    uint8_t tmpbuf[20] = { 0 };

    memset(tmpbuf, 0, APP_KEY_EEPROM_LEN);
    nRet = AstraEEPROM_ReadData( tmpbuf, APP_KEY_EEPROM_ADDRESS, APP_KEY_EEPROM_LEN);
    memcpy(pTmpkey, tmpbuf, APP_KEY_EEPROM_LEN);
  }

  return nRet;
}

int32_t LoadLoraKeys(void)
{
  int32_t nRet = 1;
#if USE_MEMORY
    uint8_t id;
  BSP_NFCTAG_ReadID( CUSTOM_ST25DV_0,&id );
  PRINTF_INFO("st25dv_id 0x%.2X \r\n", id);

  ST25DV_UID NFC_Uid = { 0 };      
  BSP_NFCTAG_ReadUID(CUSTOM_ST25DV_0, &NFC_Uid);
  PRINTF_INFO("st25dv UID 0x%.8X 0x%.8X\r\n", NFC_Uid.MsbUid, NFC_Uid.LsbUid);

  uint8_t tmpbuf[20] = { 0 };

  uint8_t a = 0;
  if(a)
  {
    for(uint8_t i=0;i<16;i++)tmpbuf[i] = i;
    nRet = AstraEEPROM_WriteData( tmpbuf, JOIN_EUI_EEPROM_ADDRESS, JOIN_EUI_EEPROM_LEN);
    for(uint8_t i=0;i<16;i++)tmpbuf[i] = 16-i-1;
    nRet = AstraEEPROM_WriteData( tmpbuf, NWK_KEY_EEPROM_ADDRESS, NWK_KEY_EEPROM_LEN);
    for(uint8_t i=0;i<16;i++)tmpbuf[i] = 16+i;
    nRet = AstraEEPROM_WriteData( tmpbuf, APP_KEY_EEPROM_ADDRESS, APP_KEY_EEPROM_LEN);
  }

  nRet = LoadLoraAppEui(NewAppEui);
  nRet = LoadLoraNwkKey(NewNwkKey);
  nRet = LoadLoraAppKey(NewAppKey);

#endif //USE_MEMORY
  return nRet;
}


int32_t SaveLoraKeys(void)
{
  int32_t nRet = 1;
#if USE_MEMORY
  nRet = SaveLoraAppEui(NewAppEui);
  nRet = SaveLoraNwkKey(NewNwkKey);
  nRet = SaveLoraAppKey(NewAppKey);
#endif //USE_MEMORY
  return nRet;
}


void LoraSendPacket(void)
{

  if(SystemConfParams.LORA_ACK && (PlatformStatus.b.LORA_ACK_RECEIVED == 0))
  {
    static uint32_t skip_sending = 0;
    PRINTF_VERBOSE("No ack for previous msg, WL is retrying... %d\r\n", skip_sending);
    if(skip_sending++ < LORA_NUM_SKIP_SENDING_NO_ACK)
    {
      return;
    }
    PlatformStatus.s.nErrorNoAck++;
    PRINTF_VERBOSE("No ack for previous msg, nErrorNoAck = %d\r\n", PlatformStatus.s.nErrorNoAck);
    skip_sending = 0;
  }

  //reset previous ack flag
  PlatformStatus.b.LORA_ACK_RECEIVED = 0;

  PRINTF_VERBOSE("Send DATA via LoRa\r\n");

  uint32_t i = 0;
  uint8_t cchannel=0;

  uint16_t pressure = (uint16_t)AstraEngData.d.PressValue *10;
  int16_t temperature = (int16_t)AstraEngData.d.TempValue * 10;
  uint16_t humidity = (uint16_t)AstraEngData.d.HumValue * 2;
  int32_t latitude = (int32_t)(AstraEngData.d.gnss_latitude*(float)10000.0), longitude = (int32_t)(AstraEngData.d.gnss_longitude*(float)10000.0), altitudeGps=(int32_t)(AstraEngData.d.gnss_altitude*(float)100.0);
  int32_t epoch_value =0;
  int32_t accelero_x = AstraEngData.d.AccValue.x, accelero_y  =AstraEngData.d.AccValue.y, accelero_z = AstraEngData.d.AccValue.z;
  uint16_t battery_voltage = AstraEngData.d.batteryVoltage;

  uint8_t *Buffer;
  uint8_t Confirmed;
  uint8_t DataSize;
  uint8_t Port;
  Buffer = AppDataBuff;

  uint8_t nLoraDr = 0;
  if(!BSP_LoRaGetDataRate(&nLoraDr))
  {
    SystemConfParams.LORA_DR = nLoraDr;
  }
  else
  {
    PRINTF_VERBOSE("BSP_LoRaGetDataRate error\r\n");
  }

  Buffer[i++] = cchannel++;
  Buffer[i++] = LPP_DATATYPE_BAROMETER;
  Buffer[i++] = ( pressure >> 8 ) & 0xFF;
  Buffer[i++] = ( pressure ) & 0xFF;

  if(SystemConfParams.LORA_DR > 1)
  {
    Buffer[i++] = cchannel++;
    Buffer[i++] = LPP_DATATYPE_TEMPERATURE;
    Buffer[i++] = ( temperature >> 8 ) & 0xFF;
    Buffer[i++] = ( temperature ) & 0xFF;

    Buffer[i++] = cchannel++;
    Buffer[i++] = LPP_DATATYPE_HUMIDITY;
    Buffer[i++] = humidity & 0xFF;

    Buffer[i++] = cchannel++;
    Buffer[i++] = LPP_DATATYPE_ACCELEROMETER;
    Buffer[i++] = ( accelero_x >> 8 ) & 0xFF;
    Buffer[i++] = ( accelero_x ) & 0xFF;
    Buffer[i++] = ( accelero_y >> 8 ) & 0xFF;
    Buffer[i++] = ( accelero_y ) & 0xFF;
    Buffer[i++] = ( accelero_z >> 8 ) & 0xFF;
    Buffer[i++] = ( accelero_z ) & 0xFF;

    //if band == 900Mhz, check if DR > 3
    Buffer[i++] = cchannel++;
    Buffer[i++] = LPP_DATATYPE_GPSLOCATION;
    Buffer[i++] = ( latitude >> 16 ) & 0xFF;
    Buffer[i++] = ( latitude >> 8 ) & 0xFF;
    Buffer[i++] = latitude & 0xFF;
    Buffer[i++] = ( longitude >> 16 ) & 0xFF;
    Buffer[i++] = ( longitude >> 8 ) & 0xFF;
    Buffer[i++] = longitude & 0xFF;
    Buffer[i++] = ( altitudeGps >> 16 ) & 0xFF;
    Buffer[i++] = ( altitudeGps >> 8 ) & 0xFF;
    Buffer[i++] = altitudeGps & 0xFF;

    //if band == 900Mhz, check if DR > 0
    Buffer[i++] = cchannel++;
    Buffer[i++] = LPP_DATATYPE_ANALOG_INPUT;
    Buffer[i++] = ( battery_voltage >> 8 ) & 0xFF;
    Buffer[i++] = battery_voltage & 0xFF;

    Buffer[i++] = cchannel++;
    Buffer[i++] = LPP_DATATYPE_DIGITAL_OUTPUT;
    Buffer[i++] = PlatformStatus.s.AppLedStateOn;

    Buffer[i++] = cchannel++;
    Buffer[i++] = (AstraEngData.d.gnss_sats >> 8) & 0xFF;
    Buffer[i++] = AstraEngData.d.gnss_sats;
  }
  UNUSED(epoch_value);

  DataSize = i;
  Port = LPP_APP_PORT;
  Confirmed = SystemConfParams.LORA_ACK;

  if(BSP_LoRaPingWL())
  {
    PRINTF_VERBOSE("STM32WL status NOT OK sI\r\n");
    if(BSP_LoRaPingWL())
    {
      PRINTF_VERBOSE("STM32WL status NOT OK, reset join flag\r\n");
      PlatformStatus.b.APP_LORA_JOINED = 0;
      PlatformStatus.b.APP_LORA_JOIN_COMPLETED = 0;
      PRINTF_VERBOSE("LoRa has NOT joined\r\n");
      setLoraJoinTimer(60*1000, 1);
    }
  }

  //send using lora functions
  if(PlatformStatus.b.APP_LORA_JOINED)
  {
    if(!BSP_LoRaSendBinPacket(Buffer, DataSize, Port, Confirmed))
    {
      PRINTF_VERBOSE("BSP_SendBinPacket() OK\r\n");
    }
    else
    {
      PRINTF_VERBOSE("BSP_SendBinPacket() NOT OK\r\n");
      setLoraJoinTimer(60*1000, 1);
    }
  }
}

void stm32wl_process()
{
  if(!PlatformStatus.b.APP_LORA_ENABLED)
  {
    PRINTF_INFO("LoRa: stm32wl suspended)\r\n");
    return;
  }

  if(PlatformStatus.b.APP_LORA_JOINED)
  {
    unsetLoraJoinTimer();
    BSP_LoRaManageURCs();

    if(PlatformStatus.b.LORA_SEND_TRIGGERED)
    {
      PlatformStatus.b.LORA_SEND_TRIGGERED = 0;
      AstraLedColor(ASTRA_LED_COLOR_RED);
      PRINTF_INFO("LoRa has joined\r\n");

      LoraSendPacket();

      AstraLedColor(ASTRA_LED_OFF);
    }
  }
  else
  {
    PRINTF_INFO("LoRa has NOT joined\r\n");
  }

  if(!PlatformStatus.b.APP_LORA_JOINED && PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS )
  {
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    BSP_LoRaManageURCs();
  }

  if(!PlatformStatus.b.APP_LORA_JOINED && !PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS )
  {
    if(PlatformStatus.b.LORA_JOIN_TRIGGERED)
    {
      AstraLedColor(ASTRA_LED_COLOR_BLUE);

      if(!PlatformStatus.b.APP_LORA_MODEM_CONFIGURED || BSP_LoRaPingWL())
      {
        PlatformStatus.b.APP_LORA_MODEM_CONFIGURED = 0;
        BSP_LoRaModemHwReset();

        PRINTF_VERBOSE("BSP_LoRaSetRegionBandID %s\r\n", GET_REGION_STRING(SystemConfParams.LORA_REGION));
        CUSTOM_MODEM_region_t region = (CUSTOM_MODEM_region_t)SystemConfParams.LORA_REGION;
        if(!BSP_LoRaSetRegionBandID(&region)) //Set active region
        {
          PRINTF_VERBOSE("BSP_LoRaSetRegionBandID OK\r\n");
          PlatformStatus.b.APP_LORA_MODEM_CONFIGURED = 1;
        }
      }

      if(!BSP_LoRaPingWL())
      {
        //go ahead
        PRINTF_INFO("STM32WL status OK\r\n");

        AstraLedToggle(ASTRA_LED_COLOR_BLUE);

        if(!LORA_JOIN_ABP)
        {
          // OTAA join

          //load new keys
#if !USE_COMMISSIONING_H_KEYS
#if USE_MEMORY
          LoadLoraKeys();
#endif //USE_MEMORY
#endif //!USE_COMMISSIONING_H_KEYS
          SetLoraKeys();

          // reset "joined" application flag
          PlatformStatus.b.APP_LORA_JOINED = 0;
          PlatformStatus.b.APP_LORA_JOIN_COMPLETED = 0;
          if(!BSP_LoRaJoin_OTAA())
          {
            PRINTF_VERBOSE("BSP_LoRaJoin() OTAA request in progress\r\n");
            PlatformStatus.b.LORA_JOIN_TRIGGERED = 0;
            PlatformStatus.b.LORA_SEND_TRIGGERED = 0;
            PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS = 1;
          }
        }
        else
        {
          // ABP join
          
#if !USE_COMMISSIONING_H_KEYS
#if USE_MEMORY
          //LoadLoraABPKeys();
#endif //USE_MEMORY
#endif //!USE_COMMISSIONING_H_KEYS

          SetLoraKeys();
          SetLoraSKeys();
          
          PRINTF_VERBOSE("BSP_LoRaJoin_ABP \r\n");
          PlatformStatus.b.LORA_JOIN_TRIGGERED = 0;
          PlatformStatus.b.LORA_SEND_TRIGGERED = 0;

          if(!BSP_LoRaJoin_ABP())
          {
            if(PlatformStatus.b.APP_LORA_JOINED)
            {
              PRINTF_VERBOSE("LoRa has joined ABP\r\n");

              //set ack in order to send first packet
              PlatformStatus.b.LORA_ACK_RECEIVED = 1;

              PRINTF_VERBOSE("BSP_LoRaSetAdaptiveDataRate %s\r\n", (SystemConfParams.LORA_ADR)?"ON":"OFF");
              if(!BSP_LoRaSetAdaptiveDataRate(SystemConfParams.LORA_ADR)) //Turn off adaptive data rate for the SetDataRate to work properly
              {
                PRINTF_VERBOSE("BSP_LoRaSetAdaptiveDataRate OK\r\n");
              }
              PRINTF_VERBOSE("BSP_LoRaSetDataRate to %d\r\n", SystemConfParams.LORA_DR);
              if(!BSP_LoRaSetDataRate(SystemConfParams.LORA_DR)) //Set data rate to 4 to be able to send paquets more frequently at the cost of the distance the data can travel
              {
                PRINTF_VERBOSE("BSP_LoRaSetDataRate OK\r\n");
              }
            }
            else
            {
              PRINTF_VERBOSE("LoRa has NOT joined\r\n");
            }
          }
        }
      }
      else
      {
        AstraLedToggle(ASTRA_LED_COLOR_BLUE);
        PRINTF_VERBOSE("STM32WL not responding\r\n");

        static uint32_t nErrorWLTicks[200];
        if(PlatformStatus.s.nErrorWL < 200)nErrorWLTicks[PlatformStatus.s.nErrorWL] = HAL_GetTick();
        UNUSED(nErrorWLTicks);
        PlatformStatus.s.nErrorWL++;
      }
      AstraLedColor(ASTRA_LED_OFF);
    }
  }

  if(PlatformStatus.b.APP_LORA_CONFIG_IN_PROGRESS )
  {
    PlatformStatus.b.APP_LORA_CONFIG_IN_PROGRESS = 0;
    if(PlatformStatus.b.APP_LORA_JOINED)
    {
      PRINTF_VERBOSE("LoRa has joined OTAA\r\n");

      //set ack in order to send first packet
      PlatformStatus.b.LORA_ACK_RECEIVED = 1;

      PRINTF_VERBOSE("BSP_LoRaSetAdaptiveDataRate %s\r\n", (SystemConfParams.LORA_ADR)?"ON":"OFF");
      if(!BSP_LoRaSetAdaptiveDataRate(SystemConfParams.LORA_ADR)) //Turn off adaptive data rate for the SetDataRate to work properly
      {
        PRINTF_VERBOSE("BSP_LoRaSetAdaptiveDataRate OK\r\n");
      }
      if(SystemConfParams.LORA_ADR == 0)
      {
        PRINTF_VERBOSE("BSP_LoRaSetDataRate to %d\r\n", SystemConfParams.LORA_DR);
        if(!BSP_LoRaSetDataRate(SystemConfParams.LORA_DR)) //Set data rate to 4 to be able to send paquets more frequently at the cost of the distance the data can travel
        {
          PRINTF_VERBOSE("BSP_LoRaSetDataRate OK\r\n");
        }
      }
    }
    else
    {
      PRINTF_VERBOSE("LoRa has NOT joined\r\n");
    }
  }
}

void STM32WL_LowPower(void)
{
  BSP_LoRaWL_LowPower();
}

void STM32WL_RebootDfu(void)
{
  BSP_LoRaWL_RebootDfu();
}
#endif //USE_STM32WL

#if USE_MEMORY
void ManageMemory()
{
  /* Check parameters to be updated in NFC memory */
  AstraEEPROM_CheckDefaultParamsUpdates();

  /* Initialize virtual sensors in NFC memory */
  AstraEEPROM_InitVirtualSensors();
    
  /* There are a RF activity */
  APP_NFC_CheckRFActivity();
}
#endif //USE_MEMORY

void buzzer_process()
{
  // Buzzer process function to be implemented here
}

void SaveBoardName(char *boardName)
{
  int32_t nRet = 1;
  nRet = AstraEEPROM_WriteData( (uint8_t const*)boardName, EEPROM_BOARD_NAME_ADDRESS, EEPROM_BOARD_NAME_LEN);
  UNUSED(nRet);
}

void GetBoardName(char *boardName)
{
  int32_t nRet = 1;
  nRet = AstraEEPROM_ReadData( (uint8_t*)boardName, EEPROM_BOARD_NAME_ADDRESS, EEPROM_BOARD_NAME_LEN);
  UNUSED(nRet);
}

/**
* @} DATA_MANAGEMENT_FunctionsDefines
*/

/**
* @} DATA_MANAGEMENT
*/

/**
* @} ASTRA_ENGINE
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
