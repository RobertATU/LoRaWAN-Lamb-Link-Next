/**
  ******************************************************************************
  * @file    SMARTNFC_config.h
  * @author  System Research & Applications Team - Catania & Agrate Lab.
  * @version 1.0.0
  * @brief   Configuration file for SmarTag application
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMARTNFC_CONFIG_H
#define __SMARTNFC_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
   
#include "custom_nfc.h"
#include "astra_confmng.h"
#include "astra_sysmng.h"
#include "app_conf.h"
#include "st25dv.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup SMARTNFC
  * @{
  */
   
/** @addtogroup SMARTNFC_NFCTAG
  * @{
  */
   
/* Defines -------------------------------------------------------------------*/

/* STEVAL-ASTRA1B includes the st25dv64k */
#define STSMART_NFC_MAX_SIZE NFCTAG_64K_SIZE
   
/* Uncomment the following define for enabling the VCOM printf */
#define SMARTNFC_ENABLE_PRINTF

/* AutoStart after 2 Seconds */
#define SMARTNFC_AUTOSTART_SECONDS 2
   
/* Default Sample value time in seconds */
#define DATA_DEFAULT_SAMPLE_INT 60
   
/* Default Epoch Start time if there is not a Valid Configuration */
  
#define SMARTNFC_DEFAULT_EPOCH_START_TIME 1634745180    /* (GMT)        Wednesday, October 20, 2021 3:53:00 PM */
                                                        /* (Local time) Wednesday, October 20, 2021 5:53:00 PM */

/* SmarTag logging mode functionality */
#define SMARTNFC_LOGMODE_INACTIVE 0
#define SMARTNFC_LOGMODE_ACTIVE   1

/* Package Version only numbers 0->9 */
#define SMARTNFC_VERSION_MAJOR  1
#define SMARTNFC_VERSION_MINOR  0
#define SMARTNFC_VERSION_PATCH  0

/* NFC Protocol Version and Revision */
#define SMARTNFC_RECORD_VERSION 2
#define SMARTNFC_RECORD_REVISION 1

/* Board Id and Firmware Id */
#define SMARTNFC_BOARD_ID 2             /* '2' is the ASTRA board ID */
#define SMARTNFC_FIRMWARE_ID 1

/* NFC field */
#define FIELD_UNDEF   0
#define FIELD_FALLING 1
#define FIELD_RISING  2

/* Events type */
#define NO_EVENT    0x00
#define SYNC_EVENT  0x01
#define ASYNC_EVENT 0x02

/* BSP NFC */
#define BSP_NFCTAG_INSTANCE         (0)
#define BSP_NFCTAG_GPO_PRIORITY     (0)
   
/* Accelorometers events */
#define ENABLE_ACC_EVENTS_VS_ID                 0

#define ACC_EVENTS_FREE_FALL_STATUS             0x80
#define ACC_EVENTS_TAP_STATUS                   0x40
#define ACC_EVENTS_DOUBLE_TAP_STATUS            0x20
#define ACC_EVENTS_WAKEUP_STATUS                0x10
#define ACC_EVENTS_STEP_STATUS                  0x08
#define ACC_EVENTS_TILT_STATUS                  0x04
#define ACC_EVENTS_D6_DORIENTATION_STATUS       0x02
#define ACC_EVENTS_SLEEP_STATUS                 0x01


/* Virtual sensor configuration */
#define SMARTNFC_VIRTUAL_SENSORS_NUM    7
#define VIRTUAL_SENSOR_NUM_EN           7
#define BATT_PERCENTAGE_VS_ID           0
#define BATT_VOLTAGE_VS_ID              1
#define STTS22H_VS_ID                   2
#define LPS22HH_VS_ID                   3
#define HTS221_VS_ID                    4
#define LIS2DTW12_VS_ID                 5
#define LSM6DSOX32_VS_ID                6

#if ENABLE_ACC_EVENTS_VS_ID
#define ACC_EVENTS_VS_ID                7
#endif //ENABLE_ACC_EVENTS_VS_ID

/* Virtual sensor max min values configuration */
#define BATT_PERCENTAGE_MIN_VALUE       0
#define BATT_PERCENTAGE_MAX_VALUE       100
#define BATT_PERCENTAGE_LOW_THRESHOLD   BATT_PERCENTAGE_MIN_VALUE
#define BATT_PERCENTAGE_UPP_THRESHOLD   BATT_PERCENTAGE_MAX_VALUE

#define BATT_VOLTAGE_MIN_VALUE          0               // mVolt
#define BATT_VOLTAGE_MAX_VALUE          8192            // mVolt
#define BATT_VOLTAGE_LOW_THRESHOLD      3300            // mVolt
#define BATT_VOLTAGE_UPP_THRESHOLD      4100            // mVolt

#define STTS22H_MIN_VALUE               -10             // Celsius degrees
#define STTS22H_MAX_VALUE               60              // Celsius degrees
#define STTS22H_LOW_THRESHOLD           22              // Celsius degrees
#define STTS22H_UPP_THRESHOLD           43.2            // Celsius degrees

#define LPS22HH_MIN_VALUE               260             // hPa
#define LPS22HH_MAX_VALUE               1260            // hPa
#define LPS22HH_LOW_THRESHOLD           260             // hPa
#define LPS22HH_UPP_THRESHOLD           1260            // hPa

#define HTS221_MIN_VALUE                20              // % humidity
#define HTS221_MAX_VALUE                80              // % humidity
#define HTS221_LOW_THRESHOLD            30              // % humidity
#define HTS221_UPP_THRESHOLD            60              // % humidity

#define LIS2DTW12_MIN_VALUE             0               // mg
#define LIS2DTW12_MAX_VALUE             16000           // mg
#define LIS2DTW12_UPP_THRESHOLD         30              // mg

#define LSM6DSOX32_MIN_VALUE            0               // mg
#define LSM6DSOX32_MAX_VALUE            32000           // mg
#define LSM6DSOX32_UPP_THRESHOLD        30              // mg

#define ACC_EVENTS_MIN_VALUE            0
#define ACC_EVENTS_MAX_VALUE            0xFF
#define ACC_EVENTS_LOW_THRESHOLD        ACC_EVENTS_WAKEUP_STATUS
#define ACC_EVENTS_UPP_THRESHOLD        ACC_EVENTS_WAKEUP_STATUS

   
/******************  Don't change the following section ***********************/
   
#define ST25_RETRY_NB     ((uint8_t) 15)
#define ST25_RETRY_DELAY  ((uint8_t) 40)

/**
  * @brief Iterate ST25DV command depending on the command return status.
  * @param cmd A ST25DV function returning a NFCTAG_StatusTypeDef status.
  */
#define ST25_RETRY(cmd) {              \
    int st25_retry = ST25_RETRY_NB;    \
    int st25_status = NFCTAG_ERROR;    \
    while((st25_status != NFCTAG_OK) & (st25_retry>0)) { \
      st25_status = cmd;               \
      if(st25_status != NFCTAG_OK) {   \
        HAL_Delay(ST25_RETRY_DELAY);   \
      }                                \
      st25_retry--;                    \
    }                                  \
}
                        

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */ 

#endif /* __SMARTNFC_CONFIG_H */

