/**
  ******************************************************************************
  * @file    astra_confmng.h
  * @version V1.0.0
  * @author  SRA Team
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

/** @addtogroup ASTRA_ENGINE ASTRA engine
  * @{
  */

/** @addtogroup CONFIGURATION Configuration
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ASTRA_CONFMNG__H__
#define __ASTRA_CONFMNG__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <time.h>
#include "astra_conf.h"

#include <stdarg.h>

#include "compiler.h"
#include "stm32_lpm.h"

#include "ble.h"
#include "tl.h"

#include "custom_sob.h"
#include "custom_astra.h"

#if USE_STSAFE
#include "custom_stsafe.h"
#endif //USE_STSAFE

#if USE_STM32WL
#include "custom_modem_control.h"
#endif //USE_STM32WL

#if USE_USB
#include "usbd_cdc_if.h"
#endif //USE_USB

#include "custom_mems_control.h"
#include "custom_mems_control_ex.h"

#include "RTE_Components.h"
  
#if USE_MEMORY  
#ifdef ST25DV_NFC4_I2C
#include "custom_nfc04a1_nfctag.h"
#endif //ST25DV_NFC4_I2C
#endif //USE_MEMORY

  
/** @addtogroup ASTRA_CubeMX_Settings ASTRA CubeMx settings
  * @brief CubeMX Settings
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

#ifdef CFG_GAP_DEVICE_NAME
#undef CFG_GAP_DEVICE_NAME
#endif //CFG_GAP_DEVICE_NAME
#ifdef CFG_GAP_DEVICE_NAME_LENGTH
#undef  CFG_GAP_DEVICE_NAME_LENGTH
#endif //CFG_GAP_DEVICE_NAME_LENGTH

#define CFG_GAP_DEVICE_NAME             "ASTRA1B"
#define CFG_GAP_DEVICE_NAME_LENGTH      (7)

#define TIMEZONE_CORRECTION             (+1)

#define EXECUTION_FREQUENCY             100

#define CFG_TASK_HCI_ASYNCH_EVT_ID      CFG_TASK_APP_WITH_HCICMD_1
#define ASTRA_TASK_ID                   CFG_TASK_APP_WITH_HCICMD_2
#define ASTRA_GNSS_TASK_ID              CFG_TASK_APP_WITH_HCICMD_3

/* MEMORY related defines */
#if USE_MEMORY
#define EEPROM_LORA_KEYS_OFFSET         (ST25DV_ASTRA_CONF_MEMORY_OFFSET)
#else //USE_MEMORY
#define EEPROM_LORA_KEYS_OFFSET         (0)
#endif //USE_MEMORY
#define EEPROM_LORA_KEYS_VALID_ADDR     (EEPROM_LORA_KEYS_OFFSET)
#define EEPROM_LORA_KEYS_VALID_LEN      (8)
#define JOIN_EUI_EEPROM_ADDRESS         (EEPROM_LORA_KEYS_VALID_ADDR+EEPROM_LORA_KEYS_VALID_LEN) //12
#define JOIN_EUI_EEPROM_LEN             (8)
#define NWK_KEY_EEPROM_ADDRESS          (JOIN_EUI_EEPROM_ADDRESS+JOIN_EUI_EEPROM_LEN) //20
#define NWK_KEY_EEPROM_LEN              (16)
#define APP_KEY_EEPROM_ADDRESS          (NWK_KEY_EEPROM_ADDRESS+NWK_KEY_EEPROM_LEN) //36
#define APP_KEY_EEPROM_LEN              (16)
  //reserved free memory for keys from address 48 (APP_KEY_EEPROM_ADDRESS+APP_KEY_EEPROM_LEN) to 63
#define EEPROM_KEY_RESERVED_ADDR        (APP_KEY_EEPROM_ADDRESS+APP_KEY_EEPROM_LEN)
#define EEPROM_KEY_RESERVED_LEN         (16)

#define EEPROM_ASTRA_ENGINE_PAR_ADDR    (EEPROM_KEY_RESERVED_ADDR+EEPROM_KEY_RESERVED_LEN)
#define EEPROM_ASTRA_ENGINE_PAR_LEN     (20)

#define EEPROM_PLATF_STATUS_ADDR        (EEPROM_ASTRA_ENGINE_PAR_ADDR+EEPROM_ASTRA_ENGINE_PAR_LEN)
#define EEPROM_PLATF_STATUS_LEN         (32)

#define EEPROM_FW_INFO_ADDR             (EEPROM_PLATF_STATUS_ADDR+EEPROM_PLATF_STATUS_LEN)
#define EEPROM_FW_INFO_LEN              (64)

#define EEPROM_BOARD_NAME_ADDRESS       (EEPROM_FW_INFO_ADDR+EEPROM_FW_INFO_LEN)
#define EEPROM_BOARD_NAME_LEN           (8)
#define DATA_START_ADDRESS              (EEPROM_BOARD_NAME_ADDRESS+EEPROM_BOARD_NAME_LEN)
#define DATA_LEN                        (EEPROM_BOARD_NAME_ADDRESS+EEPROM_BOARD_NAME_LEN)

  
/* STM32WL related defines */
#define DEV_EUI_VALIDATION_MASK         0x01
#define APP_EUI_VALIDATION_MASK         0x02
#define NWK_KEY_VALIDATION_MASK         0x04
#define APP_KEY_VALIDATION_MASK         0x08
#define LORA_VALIDATION_MASK_ALL        ( DEV_EUI_VALIDATION_MASK | APP_EUI_VALIDATION_MASK | NWK_KEY_VALIDATION_MASK | APP_KEY_VALIDATION_MASK )
#define LORA_NUM_SKIP_SENDING_NO_ACK    2

#if USE_STM32WL
#define GET_REGION_STRING(REGION)       (REGION==AS923)?"AS923": \
                                        (REGION==AU915)?"AU915": \
                                        (REGION==CN470)?"CN470": \
                                        (REGION==CN779)?"CN779": \
                                        (REGION==EU433)?"EU433": \
                                        (REGION==EU868)?"EU868": \
                                        (REGION==KR920)?"KR920": \
                                        (REGION==IN865)?"IN865": \
                                        (REGION==US915)?"US915": \
                                        (REGION==RU864)?"RU864":"EU868"
#else //USE_STM32WL
  #define GET_REGION_STRING(REGION)       "UNKNOWN"
#endif //USE_STM32WL

/* Buttons */
#define BTN1_TIMEOUT  5000
#define BTN2_TIMEOUT  5000

/* LEDs */
/* Hex color code:                  RRGGBB */                                          
#define ASTRA_LED_COLOR_RED     0x00FF0000
#define ASTRA_LED_COLOR_GREEN   0x0000FF00
#define ASTRA_LED_COLOR_BLUE    0x000000FF
#define ASTRA_LED_COLOR_YELLOW  0x00FFFF00
#define ASTRA_LED_COLOR_MAGENTA 0x00FF00FF
#define ASTRA_LED_COLOR_WHITE   0x00FFFFFF
#define ASTRA_LED_OFF           0x00000000

/* ADC */
#define ASTRA_ADC_CHANNELS              4

#define ASTRA_ADC_CHANNEL_VBAT          0
#define ASTRA_ADC_CHANNEL_VUSB          1
#define ASTRA_ADC_CHANNEL_TEMPSENSOR    2
#define ASTRA_ADC_CHANNEL_VREF          3

#define ASTRA_ADC_TIMEOUT                     1000    /* ms */
#define ASTRA_VREF                            ((uint32_t) 3300)
#define ASTRA_ADC_VBAT_RESISTIVE_RATIO        ((5.6+3)/3)         /* (R_high + R_low)/R_low --> 2.867 */
#define ASTRA_ADC_VUSB_RESISTIVE_RATIO        ((10+5.6)/5.6)      /* (R_high + R_low)/R_low --> 2.786 */

#define DEBUG                           1
#define ENABLE_IAR_TERMINAL_IO          0
#define ASTRA_GNSS_DEBUG                0
#define ASTRA_GNSS_CHECK_NMEA_CONF      0

#define CUSTOM_USB_DEFAULT_ATTEMPS                      10000U
#define USB_USER_RX_BUFFER_SIZE                         100        /* See APP_RX_DATA_SIZE definition in usb_cdc_if.c file */

#define TEXT_COLOR_1            "\033[0;32m"
#define TEXT_COLOR_2            "\033[0;33m"
#define TEXT_COLOR_3            "\033[0;31m"
#define TEXT_COLOR_4            "\033[1;36m"
#define DEFAULT_TEXT_COLOR      "\033[0;39m"
#define TEXT_COLOR_ESCAPE_CHAR  '\033'

#define ABS(F)                          ((F>=0)?(F):(-F))
#define LAT_NORD_SUD(L)                 ((L>=0)?('N'):('S'))
#define LON_EAST_WEST(L)                ((L>=0)?('E'):('W'))

#ifdef DEBUG

/* Message priority levels */
#define LEVEL_ACTIVE_DEFAULT            SW_CONF_PRINTF_LEVEL

  extern uint8_t DBG_PRINTF_LOG(uint8_t Q, uint16_t L, const char* sFormatString, ...);

/* Message priority levels */
#define LEVEL_INFO                      5
#define LEVEL_VERBOSE                   4
#define LEVEL_CRITICAL                  3
#define LEVEL_ERROR                     2
#define LEVEL_TERMINAL                  1
#define LEVEL_NOTHING                   0

#define GET_PRINTF_LEVEL_STRING(LEVEL)  (LEVEL==LEVEL_INFO)?"LEVEL_INFO": \
                                        (LEVEL==LEVEL_VERBOSE)?"LEVEL_VERBOSE": \
                                        (LEVEL==LEVEL_CRITICAL)?"LEVEL_CRITICAL": \
                                        (LEVEL==LEVEL_ERROR)?"LEVEL_ERROR": \
                                        (LEVEL==LEVEL_TERMINAL)?"LEVEL_TERMINAL": \
                                        (LEVEL==LEVEL_NOTHING)?"LEVEL_NOTHING":"INVALID"

#if defined ( __ICCARM__ )
#define LPRINTF(L, ...)            do{if(L<=LEVEL_ACTIVE)DBG_PRINTF_LOG(0, L, __VA_ARGS__);/*extern uint8_t bDebuggerConnected;if(bDebuggerConnected)printf(__VA_ARGS__);*/}while(0)
#else //( __ICCARM__ )
#define LPRINTF(L, ...)            do{if(L<=LEVEL_ACTIVE)DBG_PRINTF_LOG(0, L, __VA_ARGS__);}while(0)
#endif //( __ICCARM__ )
#define LPRINTF_SAFE(L, ...)       do{if(L<=LEVEL_ACTIVE)DBG_PRINTF_LOG(1, L, __VA_ARGS__);}while(0)
#else //DEBUG
#define LPRINTF(L, ...)
#define LPRINTF_SAFE(L, ...)
#endif //DEBUG

#define PRINTF_INFO(...)                do{LPRINTF(LEVEL_INFO, __VA_ARGS__);}while(0)
#define PRINTF_INFO_TIME(...)           PRINTF_INFO(__VA_ARGS__) //do{LPRINTF(LEVEL_INFO, "%dms: ",uwTick);LPRINTF(LEVEL_INFO, __VA_ARGS__);}while(0)
#define PRINTF_VERBOSE(...)             do{LPRINTF(LEVEL_VERBOSE, __VA_ARGS__);}while(0)
#define PRINTF_CRITICAL(...)            do{LPRINTF(LEVEL_CRITICAL, __VA_ARGS__);}while(0)
#define PRINTF_ERROR(...)               do{LPRINTF(LEVEL_ERROR, __VA_ARGS__);}while(0)
#define PRINTF_TERMINAL(...)            do{LPRINTF(LEVEL_TERMINAL, __VA_ARGS__);}while(0)
                                          
#define LEVEL_ACTIVE                    SystemConfParams.PrintLevelActive

/* variables for state machine*/
#define WAKE_SRC_UNKNOWN                0
#define WAKE_SRC_BLE                    1
#define WAKE_SRC_ACC                    2
#define WAKE_SRC_BLE_ACC                3
#define WAKE_SRC_BUTTON_ONLY            4

#define WAKE_SRC_DEFAULT                SW_CONF_WAKE_SRC

/* variables for gnss */
#define GNSS_READ_TIMEOUT       2000
#define GNSS_WAIT_FIX           0

/* variables for sensors */
#define LP_ACC_SLEEP_DUR  2  /* Sleep duration event. 1LSb = 512 / ODR.[set] */

#define ACC_ODR  100
#define ACC_FS  2000 /* FS = <-2g, 2g> */
#define LP_ACC_WKP_THS  (500/(ACC_FS/64)) /* Threshold for wakeup.1 LSB = FS_XL / 64. */
#define LP_ACC_WKP_DUR  1  /* Wake up duration event. 1LSb = 512 / ODR.[set] */

/* MEMORY */
#define CUSTOM_ST25DV_0 (0)

#define in_range(c, lo, up)    ((uint8_t)c >= lo && (uint8_t)c <= up)                                          //!< in_range UTILITY
#define isdigit(c)             in_range(c, '0', '9')                                                           //!< isdigit UTILITY
#define digit(c)               (c - '0')                                                                       //!< digit UTILITY
#define xdigit(c)              isdigit(c)?digit(c):in_range(c, 'a', 'f')?(10+c-'a'):in_range(c, 'A', 'F')?(10+c-'A'):0                 //!< xdigit UTILITY
#define isxdigit(c)            (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))                  //!< isxdigit UTILITY
#define countof(_A_)   (sizeof(_A_) / sizeof(*(_A_)))

#define MAJOR_VERSION(V)        (((V)&0xFF000000)>>24)
#define MINOR_VERSION(V)        (((V)&0x00FF0000)>>16)
#define REV_VERSION(V)          (((V)&0x0000FF00)>>8)

#define MAKE_VERSION(M, m, r)   (((M)&0xFF)<<24|((m)&0xFF)<<16|((r)&0xFF)<<8)

/**< generic parameters ******************************************************/

/**
 * Define Tx Power
 */
#define CFG_TX_POWER                      (0x1F) /* 6dBm */
#define CFG_TX_POWER_NUM                  6 /* 6dBm */

/**
 * Define Advertising parameters
 */
#define CFG_ADV_BD_ADDRESS                (0x000000000000)
#define CFG_BLE_ADDRESS_TYPE              PUBLIC_ADDR /**< Bluetooth address types defined in ble_legacy.h */

#define CFG_FAST_CONN_ADV_INTERVAL_MIN    (0x0080)   /**< 80ms */
#define CFG_FAST_CONN_ADV_INTERVAL_MAX    (0x00A0)  /**< 100ms */
#define CFG_LP_CONN_ADV_INTERVAL_MIN      (0x640) /**< 1s */
#define CFG_LP_CONN_ADV_INTERVAL_MAX      (0xFA0) /**< 2.5s */
#define ADV_TYPE                          ADV_IND
#define BLE_ADDR_TYPE                     PUBLIC_ADDR
#define ADV_FILTER                        NO_WHITE_LIST_USE

/**
 * Define IO Authentication
 */
#define CFG_BONDING_MODE                 (0)
#define CFG_FIXED_PIN                    (111111)
#define CFG_USED_FIXED_PIN               (0)
#define CFG_ENCRYPTION_KEY_SIZE_MAX      (16)
#define CFG_ENCRYPTION_KEY_SIZE_MIN      (8)

/**
 * Define IO capabilities
 */
#define CFG_IO_CAPABILITY_DISPLAY_ONLY       (0x00)
#define CFG_IO_CAPABILITY_DISPLAY_YES_NO     (0x01)
#define CFG_IO_CAPABILITY_KEYBOARD_ONLY      (0x02)
#define CFG_IO_CAPABILITY_NO_INPUT_NO_OUTPUT (0x03)
#define CFG_IO_CAPABILITY_KEYBOARD_DISPLAY   (0x04)

#define CFG_IO_CAPABILITY              CFG_IO_CAPABILITY_DISPLAY_YES_NO

/**
 * Define MITM modes
 */
#define CFG_MITM_PROTECTION_NOT_REQUIRED      (0x00)
#define CFG_MITM_PROTECTION_REQUIRED          (0x01)

#define CFG_MITM_PROTECTION             CFG_MITM_PROTECTION_REQUIRED

/**
 * Define PHY
 */
#define ALL_PHYS_PREFERENCE                             0x00
#define RX_2M_PREFERRED                                 0x02
#define TX_2M_PREFERRED                                 0x02
#define TX_1M                                           0x01
#define TX_2M                                           0x02
#define RX_1M                                           0x01
#define RX_2M                                           0x02

/**
*   Identity root key used to derive LTK and CSRK
*/
#define CFG_BLE_IRK     {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}

/**
* Encryption root key used to derive LTK and CSRK
*/
#define CFG_BLE_ERK     {0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21, 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21}

/**< specific parameters */
/*****************************************************/
#define  RADIO_ACTIVITY_EVENT   1          /* 1 for OOB Demo */

/**
* AD Element - Group B Feature
*/
/* LSB - First Byte */
#define CFG_FEATURE_THREAD_SWITCH               (0x40)

/* LSB - Second Byte */
#define CFG_FEATURE_OTA_REBOOT                  (0x20)

#define CONN_L(x) ((int)((x)/0.625f))
#define CONN_P(x) ((int)((x)/1.25f))

  /*  L2CAP Connection Update request parameters used for test only with smart Phone */
#define L2CAP_REQUEST_NEW_CONN_PARAM             0

#define L2CAP_INTERVAL_MIN              CONN_P(1000) /* 1s */
#define L2CAP_INTERVAL_MAX              CONN_P(1000) /* 1s */
#define L2CAP_SLAVE_LATENCY             0x0000
#define L2CAP_TIMEOUT_MULTIPLIER        0x1F4

/******************************************************************************
 * BLE Stack
 ******************************************************************************/
/**
 * Maximum number of simultaneous connections that the device will support.
 * Valid values are from 1 to 8
 */
#define CFG_BLE_NUM_LINK            2

/**
 * Maximum number of Services that can be stored in the GATT database.
 * Note that the GAP and GATT services are automatically added so this parameter should be 2 plus the number of user services
 */
#define CFG_BLE_NUM_GATT_SERVICES   8

/**
 * Maximum number of Attributes
 * (i.e. the number of characteristic + the number of characteristic values + the number of descriptors, excluding the services)
 * that can be stored in the GATT database.
 * Note that certain characteristics and relative descriptors are added automatically during device initialization
 * so this parameters should be 9 plus the number of user Attributes
 */
#define CFG_BLE_NUM_GATT_ATTRIBUTES 68

/**
 * Maximum supported ATT_MTU size
 * This parameter is ignored by the CPU2 when CFG_BLE_OPTIONS is set to 1"
 */
#define CFG_BLE_MAX_ATT_MTU             (156)

/**
 * Size of the storage area for Attribute values
 *  This value depends on the number of attributes used by application. In particular the sum of the following quantities (in octets) should be made for each attribute:
 *  - attribute value length
 *  - 5, if UUID is 16 bit; 19, if UUID is 128 bit
 *  - 2, if server configuration descriptor is used
 *  - 2*DTM_NUM_LINK, if client configuration descriptor is used
 *  - 2, if extended properties is used
 *  The total amount of memory needed is the sum of the above quantities for each attribute.
 * This parameter is ignored by the CPU2 when CFG_BLE_OPTIONS is set to 1"
 */
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE    (1344)

/**
 * Prepare Write List size in terms of number of packet
 * This parameter is ignored by the CPU2 when CFG_BLE_OPTIONS is set to 1"
 */
#define CFG_BLE_PREPARE_WRITE_LIST_SIZE         BLE_PREP_WRITE_X_ATT(CFG_BLE_MAX_ATT_MTU)

/**
 * Number of allocated memory blocks
 * This parameter is overwritten by the CPU2 with an hardcoded optimal value when the parameter when CFG_BLE_OPTIONS is set to 1
 */
#define CFG_BLE_MBLOCK_COUNT            (BLE_MBLOCKS_CALC(CFG_BLE_PREPARE_WRITE_LIST_SIZE, CFG_BLE_MAX_ATT_MTU, CFG_BLE_NUM_LINK))

/**
 * Enable or disable the Extended Packet length feature. Valid values are 0 or 1.
 */
#define CFG_BLE_DATA_LENGTH_EXTENSION   1

/**
 * Sleep clock accuracy in Slave mode (ppm value)
 */
#define CFG_BLE_SLAVE_SCA   500

/**
 * Sleep clock accuracy in Master mode
 * 0 : 251 ppm to 500 ppm
 * 1 : 151 ppm to 250 ppm
 * 2 : 101 ppm to 150 ppm
 * 3 : 76 ppm to 100 ppm
 * 4 : 51 ppm to 75 ppm
 * 5 : 31 ppm to 50 ppm
 * 6 : 21 ppm to 30 ppm
 * 7 : 0 ppm to 20 ppm
 */
#define CFG_BLE_MASTER_SCA   0

/**
 * LsSource
 * Some information for Low speed clock mapped in bits field
 * - bit 0:   1: Calibration for the RF system wakeup clock source   0: No calibration for the RF system wakeup clock source
 * - bit 1:   1: STM32WB5M Module device                             0: Other devices as STM32WBxx SOC, STM32WB1M module
 * - bit 2:   1: HSE/1024 Clock config                               0: LSE Clock config
 */
#define CFG_BLE_LS_SOURCE  (SHCI_C2_BLE_INIT_CFG_BLE_LS_NOCALIB | SHCI_C2_BLE_INIT_CFG_BLE_LS_MOD5MM_DEV | SHCI_C2_BLE_INIT_CFG_BLE_LS_CLK_LSE)

/**
 * Start up time of the high speed (16 or 32 MHz) crystal oscillator in units of 625/256 us (~2.44 us)
 */
#define CFG_BLE_HSE_STARTUP_TIME  0x148

/**
 * Maximum duration of the connection event when the device is in Slave mode in units of 625/256 us (~2.44 us)
 */
#define CFG_BLE_MAX_CONN_EVENT_LENGTH  (0xFFFFFFFF)

/**
 * Viterbi Mode
 * 1 : enabled
 * 0 : disabled
 */
#define CFG_BLE_VITERBI_MODE  1

/**
 * BLE stack Options flags to be configured with:
 * - SHCI_C2_BLE_INIT_OPTIONS_LL_ONLY
 * - SHCI_C2_BLE_INIT_OPTIONS_LL_HOST
 * - SHCI_C2_BLE_INIT_OPTIONS_NO_SVC_CHANGE_DESC
 * - SHCI_C2_BLE_INIT_OPTIONS_WITH_SVC_CHANGE_DESC
 * - SHCI_C2_BLE_INIT_OPTIONS_DEVICE_NAME_RO
 * - SHCI_C2_BLE_INIT_OPTIONS_DEVICE_NAME_RW
 * - SHCI_C2_BLE_INIT_OPTIONS_POWER_CLASS_1
 * - SHCI_C2_BLE_INIT_OPTIONS_POWER_CLASS_2_3
 * which are used to set following configuration bits:
 * (bit 0): 1: LL only
 *          0: LL + host
 * (bit 1): 1: no service change desc.
 *          0: with service change desc.
 * (bit 2): 1: device name Read-Only
 *          0: device name R/W
 * (bit 7): 1: LE Power Class 1
 *          0: LE Power Class 2-3
 * other bits: reserved (shall be set to 0)
 */
#define CFG_BLE_OPTIONS  (SHCI_C2_BLE_INIT_OPTIONS_LL_HOST | SHCI_C2_BLE_INIT_OPTIONS_WITH_SVC_CHANGE_DESC | SHCI_C2_BLE_INIT_OPTIONS_DEVICE_NAME_RW | SHCI_C2_BLE_INIT_OPTIONS_POWER_CLASS_2_3)

#define CFG_BLE_MAX_COC_INITIATOR_NBR   (32)

#define CFG_BLE_MIN_TX_POWER            (0)

#define CFG_BLE_MAX_TX_POWER            (0)

/* Legacy defines
 */
#define PUBLIC_ADDR                            0X00U
#define RANDOM_ADDR                            0X01U
#define STATIC_RANDOM_ADDR                     0X01U
#define RESOLVABLE_PRIVATE_ADDR                0X02U
#define NON_RESOLVABLE_PRIVATE_ADDR            0X03U

#define ERR_UNSPECIFIED_ERROR                        0x1F

#define evt_blue_aci evt_blecore_aci 

#define EVT_VENDOR                      0xFF

#define EVT_BLUE_GATT_READ_PERMIT_REQ             (0x0C14)

#define EVT_BLUE_GATT_READ_PERMIT_REQ             (0x0C14)

#define EVT_BLUE_GATT_ATTRIBUTE_MODIFIED          (0x0C01)

#undef __WEAK  
#define __WEAK                                 __attribute__((weak))

/**
* @} ASTRA_CONFMNG_Defines
*/

/** @addtogroup ASTRA_CONFMNG_Exported_Types ASTRA Configuration management - Exported Types
  * @brief Exported Types
  * @{
 */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  STARTUP_AFTER_POWER_ON_RESET = 0x01,
  STARTUP_AFTER_SOFTWARE_RESET = 0x02,
  STARTUP_AFTER_PIN_RESET      = 0x03
} StartupMode_t;

typedef enum {
  State_UltraLowPower=1,
  State_PrepareUltraLowPower,
  State_LowPower,
  State_PrepareLowPower,
  State_Run,
  State_Read,
  State_ReadMems,
  State_ReadGnss,
  State_SaveData,
  State_Send,
  State_RetrieveData,
  State_SendData,
  State_IdleAfterSend,
  State_CheckAck,
  State_INVALID
} SystemState_t;

typedef enum {
  SmChangeState_none=0,
  SmChangeState_lpBleWake,
  SmChangeState_lpAccWake,
  SmChangeState_lpBleAccWake,
  SmChangeState_lpButtonWake,
} SmChangeState_t;

/**
 * @brief   ASTRA system status
 * @details This variable stores the status of Lora Smart Tracker system
 */
typedef struct
{
  struct
  {
    uint8_t DEBUGGER_CONNECTED                  :       1;
    uint8_t USB_RESUMED                         :       1;
    uint8_t USB_VCP_OPENED                      :       1;
    uint8_t USB_WELCOME_MESSAGE_PENDING         :       1;
    uint8_t USB_DBG                             :       1;
    uint8_t USB_TRACE                           :       1;
    uint8_t BLE_CONNECTED                       :       1;
    uint8_t BLE_TRACE                           :       1;
    uint8_t UART_TRACE                          :       1;
    uint8_t DMA_INIZIALIZED                     :       1;
    uint8_t ADC_INIZIALIZED                     :       1;
    uint8_t USB_INIZIALIZED                     :       1;
    uint8_t MEMS_INT1_EVENT                     :       1;
    uint8_t MEMS_INT2_EVENT                     :       1;
    uint8_t BTN1_PUSH_TIMER                     :       1;
    uint8_t BTN1_PUSHED_EVENT                   :       1;
    uint8_t BTN1_PUSHED_LONG_EVENT              :       1;
    uint8_t BTN1_ANTI_TAMPERING_FLAG            :       1;
    uint8_t BTN1_ANTI_TAMPERING_ACTIVATED_EVENT :       1;
    uint8_t BTN1_ANTI_TAMPERING_RESTORED_EVENT  :       1;
    uint8_t BTN2_PUSH_TIMER                     :       1;
    uint8_t BTN2_PUSHED_EVENT                   :       1;
    uint8_t BTN2_PUSHED_LONG_EVENT              :       1;
    uint8_t GNSS_ENABLED                        :       1;
    uint8_t GNSS_VERSION_SHOWED                 :       1;
    uint8_t GNSS_WRONG_NMEA                     :       1;
    uint8_t GNSS_NMEA                           :       1;
    uint8_t GNSS_REPORT                         :       1;
    uint8_t SENSORS_ENABLED                     :       1;
    uint8_t ASTRA_ENGINE_RESTART_PENDING        :       1;
    uint8_t ASTRA_CHANGE_STATE_PENDING          :       1;
    uint8_t APP_LORA_ENABLED                    :       1;
    uint8_t APP_LORA_JOINED                     :       1;
    uint8_t APP_LORA_JOIN_COMPLETED             :       1;
    uint8_t APP_LORA_JOIN_IN_PROGRESS           :       1;
    uint8_t APP_LORA_CONFIG_IN_PROGRESS         :       1;
    uint8_t APP_LORA_MODEM_CONFIGURED           :       1;
    uint8_t LORA_ACK_RECEIVED                   :       1;
    uint8_t BLE_BATTERY_NOTIF_ENABLED           :       1;
    uint8_t BLE_ENV_SENS_NOTIF_ENABLED          :       1;
    uint8_t BLE_INERTIAL_NOTIF_ENABLED          :       1;
    uint8_t BLE_GNSS_NOTIF_ENABLED              :       1;
    uint8_t LORA_SEND_TRIGGERED                 :       1;
    uint8_t LORA_JOIN_TRIGGERED                 :       1;
    uint8_t BLE_MNG_TRIGGERED                   :       1;
    uint8_t STATUS_UPD_TRIGGERED                :       1;
    uint8_t GNSS_READ_TRIGGERED                 :       1;
    uint8_t GNSS_CHECK_NMEA_CONF                :       1;
    uint8_t NFC_INIZIALIZED                     :       1;
    uint8_t NFC_ASTRA_CONFIGURATION_UPDATED     :       1;
    uint8_t NFC_SAVING_INTERVAL_UPDATED         :       1;
    uint8_t NFC_MAC_ADDR_UPDATED                :       1;
    uint8_t NFC_VIRTUAL_SENSORS_INITIALIZED     :       1;
    uint8_t NFC_RF_ACTIVITY                     :       1;
    uint8_t NFC_RF_ACTIVITY_STATUS              :       2;
    uint8_t NFC_DATA_TRIGGERED                  :       1;
  } b;

  struct
  {
    uint8_t ASTRA_ENGINE_SEL;
    SystemState_t xCurrentState;
    SystemState_t xNextState;
    SystemState_t xPrevState;
    uint8_t GNSS_GEOFENCE_MASK;
    uint32_t GNSS_GEOFENCE_RADIUS;
    float GNSS_GEOFENCE_LAT;
    float GNSS_GEOFENCE_LON;
    int32_t RTC_TRIGGER_VALUE;
    uint32_t LORA_SEND_INTERVAL;
    uint32_t BLE_SEND_INTERVAL;
    uint32_t NFC_SAVING_INTERVAL;
    uint32_t SYS_PROC_INTERVAL;
    uint32_t STATUS_UPD_INTERVAL;
    uint16_t lastReadIndex;
    uint16_t lastSavedIndex;
    uint32_t nErrorNoAck;
    uint32_t nErrorWL;
    uint32_t STSAFE_PERS;
    uint8_t _Reserved8;
    uint32_t TX_TIMER_INTERVAL;
    uint8_t AppLedStateOn;
    uint8_t TamperInput;
    uint8_t STM32_UID[12];
    uint32_t STM32WB_CM0_VER;
    uint32_t STM32WB_CM0_FUS;
    uint32_t STM32WL_CM4_VER; //use MAJOR_VERSION, MINOR_VERSION, REV_VERSION macros
    uint32_t STM32WL_CM0_VER;
    uint32_t STM32WL_MWLORA_VER;
    uint32_t STM32WL_MWRADIO_VER;
    uint32_t UC_RUNALGO;
    time_t AstraDeltaDateTimeStamp;
    SmChangeState_t NewState;
  } s;
  uint32_t nFwVersion;
} PlatformStatus_t;

typedef struct
{
  uint32_t RUNALGO;
  uint32_t LORA_SENDFREQ;
  uint32_t BLE_SENDFREQ;
  uint32_t NFC_DATASAVINGINTV;
  uint8_t ASTRA_ENGINE_SEL;
  struct
  {
    uint8_t INDOORONLY  :1;
    uint8_t LOG         :1;
  }b;
}AstraEngineParams_t;

typedef struct
{
  uint32_t PrintLevelActive;
  uint8_t LORA_DR;
  uint8_t LORA_ADR;
  uint8_t LORA_ACK;
  uint8_t LORA_REGION;
  uint32_t BLE_ADV_INTV_MIN;
  uint32_t BLE_ADV_INTV_MAX;
  uint32_t RESERVED0;
  uint32_t RESERVED1;
  struct
  {
    uint8_t CONF_USB                     :1;
    uint8_t CONF_MEMORY                  :1;
    uint8_t CONF_BTN1_ANTI_TAMPER        :1;
    uint8_t CONF_SENSORS                 :1;
    uint8_t CONF_STSAFE                  :1;
    uint8_t CONF_GNSS                    :1;
    uint8_t CONF_ASTRA_MAINBOARD_BSP     :1;
    uint8_t CONF_SEQUENCER               :1;
    uint8_t CONF_BLE                     :1;
    uint8_t CONF_BUZZER                  :1;
    uint8_t CONF_PWRMNG                  :1;
    uint8_t CONF_STM32WL                 :1;
    uint8_t CONF_MEMS_REBOOT             :1;
    uint8_t reserved                     :3;
  }b;
}SystemConfParams_t;

/**
  * @} ASTRA_CONFMNG_Exported_Types
  */

/** @addtogroup ASTRA_CONFMNG_External_Variables ASTRA Configuration management - External variables
  * @brief External variables
  * @{
  */

/* External variables --------------------------------------------------------*/

extern SystemConfParams_t SystemConfParams;
extern PlatformStatus_t PlatformStatus;

/**
  * @} ASTRA_CONFMNG_External_Variables
  */

/** @addtogroup ASTRA_CONFMNG_FunctionPrototypes ASTRA Configuration management - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/
/**
  * @brief   Get active print level
  * @param  none
  * @retval Print level
  */
uint32_t GetPrintLevelActive(void);

/**
  * @brief  Debug init
  * @param  none
  * @retval none
  */
void debug_init(void);

#if USE_SENSORS
/**
  * @brief  Sensor init
  * @param  none
  * @retval none
  */
void sensor_init(void);

/**
  * @brief  Sensor lp wakeup detect init
  * @param  none
  * @retval none
  */
void sensor_lp_wakeup_detect_init(void);

/**
  * @brief  Sensor lp reduce odr for low performance
  * @param  none
  * @retval none
  */
void sensor_lp_lowPerformance(void);

/**
  * @brief  Sensor lp disable
  * @param  none
  * @retval none
  */
void sensor_lp_disable(void);

/**
  * @brief  Sensor lp disable algorithms
  * @param  none
  * @retval none
  */
void sensor_lp_algo_de_init(void);

/**
  * @brief  Sensor lp inactivity detect init
  * @param  none
  * @retval none
  */
void sensor_lp_inactivity_detect_init(void);

#endif //USE_SENSORS

/**
  * @brief  GNSS init
  * @param  none
  * @retval none
  */
void gnss_init(void);

/**
  * @brief  STM32WL init
  * @param  none
  * @retval none
  */
void stm32wl_init(void);

#if USE_STSAFE
#include "stsafea_core.h"
extern StSafeA_Handle_t stsafea_handle;

/**
  * @brief  STSAFE init
  * @param  handle_se: Pointer to StSafeA_Handle_t
  * @retval Returned values: 0x00 --> STSAFEA_BUS_OK; 0x01 --> STSAFEA_BUS_ERR; 0x0AU --> STSAFEA_KEY_NOT_FOUND
  */
uint8_t StSafeInit_HAL(void *handle_se);

/**
  * @brief  Get certificate size
  * @param  handle: STSAFE handle
  * @param  pCertSize: Pointer to uint16_t
  * @retval Returned values: StSafeA_ResponseCode_t
  */
int32_t GetCertificateSize(StSafeA_Handle_t* handle, uint16_t *pCertSize);

/**
  * @brief  Get certificate size
  * @param  handle: STSAFE handle
  * @param  CertificateSize: Certificate size
  * @param  psts_read_cert: Pointer to StSafeA_LVBuffer_t
  * @retval Returned values: StSafeA_ResponseCode_t
  */
int32_t ExtractCertificate(StSafeA_Handle_t* handle, uint16_t CertificateSize, StSafeA_LVBuffer_t *psts_read_cert);
#endif //USE_STSAFE

/**
  * @brief  Store Host Keys
  * @param  Buf:
  * @param  Len:
  * @retval none
  */
void store_host_keys(void*Buf, uint32_t Len);

/**
  * @brief  STSAFE Init
  * @param  none
  * @retval none
  */
void stsafe_init(void);

/**
  * @brief  STSAFE Process
  * @param  none
  * @retval none
  */
void stsafe_process(void);

/**
  * @brief  STSAFE Check
  * @param  none
  * @retval none
  */
uint8_t StSafeCheck(void);

/**
  * @brief  STSAFE Pairing
  * @param  none
  * @retval none
  */
uint8_t StSafePairing(void);

/**
  * @brief  USB manager
  * @param  none
  * @retval none
  */
void custom_USB_Manager(void);

#if USE_USB
/**
  * @brief  Send a message over USB
  * @param  pBuf: Pointer to uint8_t
  * @param  Lenght: uint16_t
  * @param  AttemptsNumber: uint32_t attempts number
  * @retval Returned values: 0 --> OK; 1 --> Error
  */
uint8_t CUSTOM_USB_SendMessage(uint8_t* pBuf, uint16_t Lenght, uint32_t AttemptsNumber);
#endif //USE_USB

/**
  * @brief ASTRA USB resume callback
  * @param  none
  * @retval none
  */
void AstraUsbResumeCallback(void);

/**
  * @brief ASTRA USB suspend callback
  * @param  none
  * @retval none
  */
void AstraUsbSuspendCallback(void);

/**
  * @brief Buzzer init
  * @param  none
  * @retval none
  */
void buzzer_init(void);

/**
  * @brief Memory init
  * @param  none
  * @retval none
  */
void memory_init(void);

/**
  * @brief Memory deinit
  * @param  none
  * @retval none
  */
void memory_deinit(void);

/**
  * @brief Save ASTRA engine parameters
  * @param  params: Pinter to AstraEngineParams_t
  * @retval none
  */
void SaveAstraEngineParams(AstraEngineParams_t *params);

/**
  * @brief Get ASTRA engine parameters
  * @param  params: Pinter to AstraEngineParams_t
  * @retval none
  */
void GetAstraEngineParams(AstraEngineParams_t *params);

/**
  * @brief Save platform parameters
  * @param  params: Pinter to SystemConfParams_t
  * @retval none
  */
void SavePlatformParams(SystemConfParams_t *params);

/**
  * @brief Get platform parameters
  * @param  params: Pinter to SystemConfParams_t
  * @retval none
  */
void LoadPlatformParams(SystemConfParams_t *params);

/**
  * @brief Check a new firmware flashed
  * @param  none
  * @retval Returned values: 0 --> OK; 1 --> Error
  */
uint8_t IsNewFwFlashed(void);

/**
  * @brief Reset new firmware flag
  * @param  none
  * @retval none
  */
void ResetNewFwFlags(void);

/**
  * @brief  Reads data in the nfctag at specific address
  * @param  pData: pointer to store read data
  * @param  TarAddr: I2C data memory address to read
  * @param  Size: Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t AstraEEPROM_ReadData( uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );

/**
  * @brief  Writes data in the nfctag at specific address
  * @param  pData: pointer to the data to write
  * @param  TarAddr: I2C data memory address to write
  * @param  Size: Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t AstraEEPROM_WriteData( const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );

/**
  * @brief  Initialize virtual sensors
  * @param  none
  * @retval none
  */
void AstraEEPROM_InitVirtualSensors (void);

/**
  * @brief  Initialize nfctag according to ASTRA NDEF message
  * @param  none
  * @retval none
  */
void AstraEEPROM_InitNDEFMessage(void);


/**
  * @brief  Buttons init function
  * @param  none
  * @retval none
  */
void Astra_ButtonInit(void);

/**
  * @brief  Buttons check
  * @param  none
  * @retval none
  */
void Astra_ButtonsCheck(void);

/**
  * @brief  Button 1 callback function
  * @param  none
  * @retval none
  */
void BTN1_CB(void);

/**
  * @brief  Button 2 callback function
  * @param  none
  * @retval none
  */
void BTN2_CB(void);

/**
  * @brief  ASTRA BLE init function
  * @param  none
  * @retval none
  */
void astra_ble_init(void);

/**
  * @brief  Sensor 1 interrupt callback function
  * @param  none
  * @retval none
  */
void SENS_INT1_CB(void);

/**
  * @brief  Sensor 2 interrupt callback function
  * @param  none
  * @retval none
  */
void SENS_INT2_CB(void);

/**
  * @brief  Power good callback function
  * @param  none
  * @retval none
  */
void POWER_GOOD_CB(void);

/**
  * @brief  ST25DV interrupt callback function
  * @param  none
  * @retval none
  */
void ST25DV_INT_PIN_GPO_EXTI_CB(void);

/**
  * @brief  System shutdown
  * @param  none
  * @retval none
  */
void SystemShutdown(void);

/**
  * @brief It drives the RGB LED to set a color
  * @param  nColor: uint32_t (One byte for each primary color according to the position 0x00RRGGBB)
  * @retval none
  */
void AstraLedColor(uint32_t nColor);

/**
  * @brief It toggles the RGB LED to set a color
  * @param  nColor: uint32_t (One byte for each primary color according to the position 0x00RRGGBB)
  * @retval none
  */
void AstraLedToggle(uint32_t nColor);

/**
  * @brief Callback linked to Button 1 pushed event
  * @param  none
  * @retval none
  */
void Button1Pushed_cb(void);

/**
  * @brief Callback linked to Button 1 pushed long event
  * @param  none
  * @retval none
  */
void Button1LongPushed_cb(void);

/**
  * @brief Callback linked to Button 2 pushed event
  * @param  none
  * @retval none
  */
void Button2Pushed_cb(void);

/**
  * @brief Callback linked to Button 2 pushed long event
  * @param  none
  * @retval none
  */
void Button2LongPushed_cb(void);

/**
  * @brief Callback linked to Button 1 tamper glitch event
  * @param  none
  * @retval none
  */
void Button1TamperGlitch_cb(void);

/**
  * @brief Callback linked to Button 1 tapmer activated event
  * @param  none
  * @retval none
  */
void Button1TamperActivated_cb(void);

/**
  * @brief Callback linked to Button 1 tamper restored event
  * @param  none
  * @retval none
  */
void Button1TamperRestored_cb(void);

/* NFC TAG section */

/**
* @brief  SmarTag Application Start
* @param  None
* @retval None
*/
extern void SmarTagAppStart(void);

/**
* @brief  SmarTag Application process
* @param  None
* @retval None
*/
extern void SmarTagAppProcess(void);

/**
* @brief  Understanding the Interrupt from ST25DV
* @param  None
* @retval None
*/
extern void SmarTagAppDetectRFActivity(void);

/**
  * @brief  Peripheral clock configuration
  * @param  none
  * @retval none
  */
void PeriphClock_Config(void);

/**
  * @brief  Peripheral clock configuration for low power
  * @param  none
  * @retval none
  */ 
void PeriphLowPowerClock_Config(void);

/**
  * @brief  Init delta time
  * @param  none
  * @retval none
  */
void InitAstraDeltatime(void);

/**
  * @brief  Set time stamp
  * @param  RTC_TimeTypeDef CurTime
  * @param  RTC_DateTypeDef CurDate
  * @retval uint8_t Function status
  */
uint8_t AstraSetTimeStamp(RTC_TimeTypeDef CurTime, RTC_DateTypeDef CurDate);

/**
  * @brief  Get time stamp
  * @param  none
  * @retval time_t Time stamp
  */
time_t AstraGetTimeStamp(void);


/**
  * @brief  Check if NFC memory is ready to store application parameters
  * @param  none
  * @retval none
  */
void AstraEEPROM_CheckDefaultParamsUpdates(void);



/**
* @} ASTRA_CONFMNG_FunctionPrototypes
*/

#ifdef __cplusplus
}
#endif

#endif /* __ASTRA_CONFMNG__H__ */

/**
* @} CONFIGURATION
*/

/**
* @} ASTRA_ENGINE
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
