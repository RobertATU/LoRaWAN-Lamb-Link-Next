/**
  ******************************************************************************
  * @file    stm32wl_modem.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   This file provides LoRa Modem BSP interface for custom boards
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

/** @addtogroup ASTRA_BSP_MODEM
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32WL_MODEM__H__
#define __STM32WL_MODEM__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/** @addtogroup STM32WL_MODEM_DEFINES STM32WL modem - Defines
  * @brief Defines
  * @{
  */

/* Defines -------------------------------------------------------------------*/

#define STM32WL_MODEM_UART_BUS          0

#define STM32WL_MODEM_OK                       0
#define STM32WL_MODEM_ERROR                   -1

#define URCS_TIMEOUT            1000
#define N_TIME_SLICES           100
#define AT_WAIT_RESP_TIMEOUT    2000
#define WL_RX_BUFFER_LEN        1024

/**
* @} STM32WL_MODEM_DEFINES
*/

/** @addtogroup STM32WL_MODEM_Exported_Types STM32WL modem - Exported types
  * @brief Exported types
  * @{
  */

/* Exported types ------------------------------------------------------------*/

typedef int32_t (*STM32WL_MODEM_Init_Func)(void);
typedef int32_t (*STM32WL_MODEM_DeInit_Func)(void);
typedef int32_t (*STM32WL_MODEM_GetTick_Func)(void);
typedef int32_t (*STM32WL_MODEM_Write_Func)(const uint8_t *, uint32_t);
typedef int32_t (*STM32WL_MODEM_Read_Func)(uint8_t *, uint32_t);
typedef int32_t (*STM32WL_MODEM_Reset_Func)(uint8_t);

typedef struct
{
  STM32WL_MODEM_Init_Func       Init;
  STM32WL_MODEM_DeInit_Func     DeInit;
  uint32_t                      BusType; /*0 means UART */
  uint8_t                       Address; /* not used, reserved for I2C implementation */
  STM32WL_MODEM_Write_Func      Write;
  STM32WL_MODEM_Read_Func       Read;
  STM32WL_MODEM_Reset_Func      Reset;

  STM32WL_MODEM_GetTick_Func    GetTick;
} STM32WL_MODEM_IO_t;

typedef int32_t (*stmmdm_write_ptr)(void *, const uint8_t *,
                                    uint32_t);
typedef int32_t (*stmmdm_read_ptr) (void *, uint8_t *,
                                    uint32_t);

typedef struct {
  /** Component mandatory fields **/
  stmmdm_write_ptr  write;
  stmmdm_read_ptr   read;
  /** Customizable optional pointer **/
  void *handle;
} stmmdm_ctx_t;

/*!
 * LoRaMAC region enumeration
 */
typedef enum eSTM32WL_MODEM_region
{
    /*!
     * AS band on 923MHz
     */
    STM32WL_MODEM_REGION_AS923,
    /*!
     * Australian band on 915MHz
     */
    STM32WL_MODEM_REGION_AU915,
    /*!
     * Chinese band on 470MHz
     */
    STM32WL_MODEM_REGION_CN470,
    /*!
     * Chinese band on 779MHz
     */
    STM32WL_MODEM_REGION_CN779,
    /*!
     * European band on 433MHz
     */
    STM32WL_MODEM_REGION_EU433,
    /*!
     * European band on 868MHz
     */
    STM32WL_MODEM_REGION_EU868,
    /*!
     * South korean band on 920MHz
     */
    STM32WL_MODEM_REGION_KR920,
    /*!
     * India band on 865MHz
     */
    STM32WL_MODEM_REGION_IN865,
    /*!
     * North american band on 915MHz
     */
    STM32WL_MODEM_REGION_US915,
    /*!
     * Russia band on 864MHz
     */
    STM32WL_MODEM_REGION_RU864,
}STM32WL_MODEM_region_t;

typedef union
{
  struct wlver_s
  {
    uint16_t rev;
    uint8_t minor;
    uint8_t major;
  }s;
  uint32_t w;
}wlver_t;

typedef struct STM32WL_MODEM_wlversions_s
{
  wlver_t m4app;
  wlver_t m0plusapp;
  wlver_t mwlora;
  wlver_t mwradio;
}STM32WL_MODEM_wlversions_t;

typedef uint8_t STM32WL_MODEM_confirm_t;
typedef uint8_t STM32WL_MODEM_adr_t;
typedef uint8_t STM32WL_MODEM_dr_t;

typedef struct
{
  STM32WL_MODEM_IO_t     IO;
  stmmdm_ctx_t        Ctx;
  uint8_t             is_initialized;
  uint8_t             is_enabled;
  STM32WL_MODEM_region_t region;
  STM32WL_MODEM_confirm_t ack_enabled;
  STM32WL_MODEM_adr_t adr;
  STM32WL_MODEM_dr_t dr;
} STM32WL_MODEM_Object_t;

/* Modem instance Info */
typedef struct
{
  uint8_t  LoRa;
} STM32WL_MODEM_Capabilities_t;

/**
 * @brief  MOTION_SENSOR driver structure definition
 */
typedef struct
{
  int32_t (*Init)(STM32WL_MODEM_Object_t *);
  int32_t (*DeInit)(STM32WL_MODEM_Object_t *);
  int32_t (*GetCapabilities)(STM32WL_MODEM_Object_t *, STM32WL_MODEM_Capabilities_t *);
} STM32WL_MODEM_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(STM32WL_MODEM_Object_t *);
  int32_t (*Disable)(STM32WL_MODEM_Object_t *);
  int32_t (*HwReset)(STM32WL_MODEM_Object_t *);
  int32_t (*Ping)(STM32WL_MODEM_Object_t *);
  int32_t (*ManageURCs)(STM32WL_MODEM_Object_t *);
  int32_t (*SendBinPacket)(STM32WL_MODEM_Object_t *, uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed);
  int32_t (*GetVersion)(STM32WL_MODEM_Object_t *, STM32WL_MODEM_wlversions_t * ver);
  int32_t (*GetDeviceEUI)(STM32WL_MODEM_Object_t *, uint8_t *pDevEui);
  int32_t (*GetAppEUI)(STM32WL_MODEM_Object_t *, uint8_t *pAppEui);
  int32_t (*GetAppKEY)(STM32WL_MODEM_Object_t *, uint8_t *pAppKey);
  int32_t (*GetNwkKEY)(STM32WL_MODEM_Object_t *, uint8_t *pNwkKey);
  int32_t (*SetAppEUI)(STM32WL_MODEM_Object_t *, uint8_t *NewAppEui);
  int32_t (*SetAppKEY)(STM32WL_MODEM_Object_t *, uint8_t *NewAppKey);
  int32_t (*SetNwkKEY)(STM32WL_MODEM_Object_t *, uint8_t *NewNwkKey);


  int32_t (*GetDADD)(STM32WL_MODEM_Object_t *, uint32_t *pDAddress);
  int32_t (*GetAppSKEY)(STM32WL_MODEM_Object_t *, uint8_t *pAppSKey);
  int32_t (*GetNwkSKEY)(STM32WL_MODEM_Object_t *, uint8_t *pNwkSKey);
  int32_t (*SetDADD)(STM32WL_MODEM_Object_t *, uint32_t NewDAddress);
  int32_t (*SetAppSKEY)(STM32WL_MODEM_Object_t *, uint8_t *NewAppSKey);
  int32_t (*SetNwkSKEY)(STM32WL_MODEM_Object_t *, uint8_t *NewNwkSKey);


  int32_t (*GetDataRate)(STM32WL_MODEM_Object_t *, uint8_t *pnLoraDr);
  int32_t (*SetRegionBandID)(STM32WL_MODEM_Object_t *, STM32WL_MODEM_region_t *region);
  int32_t (*Join_OTAA)(STM32WL_MODEM_Object_t *);
  int32_t (*Join_ABP)(STM32WL_MODEM_Object_t *);
  int32_t (*SetAdaptiveDataRate)(STM32WL_MODEM_Object_t *, uint8_t adr);
  int32_t (*SetDataRate)(STM32WL_MODEM_Object_t *, uint8_t dr);
  int32_t (*WL_LowPower)(STM32WL_MODEM_Object_t *);
  int32_t (*WL_RebootDfu)(STM32WL_MODEM_Object_t *);
} STM32WL_MODEM_FuncDrv_t;

/**
* @} STM32WL_MODEM_Exported_Types
*/

/** @addtogroup STM32WL_MODEM_VARIABLES ASTRA STM32WL modem - External variables
  * @brief External variables
  * @{
  */

/* External variables --------------------------------------------------------*/

extern STM32WL_MODEM_CommonDrv_t STM32WL_MODEM_COMMON_Driver;
extern STM32WL_MODEM_FuncDrv_t STM32WL_MODEM_LORA_Driver;

/**
* @} STM32WL_MODEM_VARIABLES
*/

/** @addtogroup STM32WL_MODEM_FunctionPrototypes ASTRA STM32WL modem - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief  Initialize the STM32WL_MODEM modem
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_Init(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Deinitialize the STM32WL_MODEM modem
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_DeInit(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Get STM32WL_MODEM modem capabilities
 * @param  pObj: Component object pointer
 * @param  Capabilities pointer to STM32WL_MODEM modem capabilities
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_GetCapabilities(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_Capabilities_t *Capabilities);

/**
 * @brief  Register Component Bus IO operations
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_RegisterBusIO(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_IO_t *pIO);

/**
 * @brief  Enable modem
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_Enable(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Disable modem
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_Disable(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Modem hardware reset
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_HwReset(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Modem pingt
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_Ping(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  URCs management
 * @param  pObj: the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_ManageURCs(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Send bin packet
 * @param  pObj: the device pObj
 * @param  Buffer: pointer to 8bit unsigned integer
 * @param  DataSize: 32bit unsigned integer datasize
 * @param  Port: port
 * @param  Confirmed: confirmed
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_SendBinPacket(STM32WL_MODEM_Object_t *pObj, uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed);

/**
 * @brief  Modem get version
 * @param  pObj: the device pObj
 * @param  ver: pointer to STM32WL_MODEM_wlversions_t
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STM32WL_MODEM_GetVersion(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_wlversions_t * ver);

/**
  * @brief  Get device EUI
  * @param  pObj: the device pObj
  * @param  pDevEui: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetDeviceEUI(STM32WL_MODEM_Object_t *pObj, uint8_t *pDevEui);

/**
  * @brief  Get app EUI
  * @param  pObj: the device pObj
  * @param  pAppEui: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetAppEUI(STM32WL_MODEM_Object_t *pObj, uint8_t *pAppEui);

/**
  * @brief  Get app key
  * @param  pObj: the device pObj
  * @param  pAppKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetAppKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pAppKey);

/**
  * @brief  Get network key
  * @param  pObj: the device pObj
  * @param  pNwkKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetNwkKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pNwkKey);

/**
  * @brief  Set app EUI
  * @param  pObj: the device pObj
  * @param  NewAppEui: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetAppEUI(STM32WL_MODEM_Object_t *pObj, uint8_t *NewAppEui);

/**
  * @brief  Set app key
  * @param  pObj: the device pObj
  * @param  NewAppKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetAppKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewAppKey);

/**
  * @brief  Set network key
  * @param  pObj: the device pObj
  * @param  NewNwkKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetNwkKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewNwkKey);

/**
  * @brief  Get device addres
  * @param  pObj: the device pObj
  * @param  pDAddress: Pointer to 32bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetDADD(STM32WL_MODEM_Object_t *pObj, uint32_t *pDAddress);

/**
  * @brief  Get app Skey
  * @param  pObj: the device pObj
  * @param  pAppSKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetAppSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pAppSKey);

/**
  * @brief  Get network Skey
  * @param  pObj: the device pObj
  * @param  pNwkSKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetNwkSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pNwkSKey);

/**
  * @brief  Set device addres
  * @param  pObj: the device pObj
  * @param  NewDAddress: value of 32-bit device address
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetDADD(STM32WL_MODEM_Object_t *pObj, uint32_t NewDAddress);

/**
  * @brief  Set app Skey
  * @param  pObj: the device pObj
  * @param  NewAppSKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetAppSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewAppSKey);

/**
  * @brief  Set network Skey
  * @param  pObj: the device pObj
  * @param  NewNwkSKey: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetNwkSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewNwkSKey);

/**
  * @brief  Get data rate
  * @param  pObj: the device pObj
  * @param  pnLoraDr: Pointer to 8bit usigned integer
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_GetDataRate(STM32WL_MODEM_Object_t *pObj, uint8_t *pnLoraDr);

/**
  * @brief  Set region band ID
  * @param  pObj: the device pObj
  * @param  region: Pointer to STM32WL_MODEM_region_t
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetRegionBandID(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_region_t *region);

/**
  * @brief  Join OTAA
  * @param  pObj: the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_Join_OTAA(STM32WL_MODEM_Object_t *pObj);

/**
  * @brief  Join ABP
  * @param  pObj: the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_Join_ABP(STM32WL_MODEM_Object_t *pObj);

/**
  * @brief  Set adaptive data rate
  * @param  pObj: the device pObj
  * @param  adr: adaptive data rate
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetAdaptiveDataRate(STM32WL_MODEM_Object_t *pObj, uint8_t adr);

/**
  * @brief  Set data rate
  * @param  pObj: the device pObj
  * @param  adr: data rate
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_SetDataRate(STM32WL_MODEM_Object_t *pObj, uint8_t dr);

/**
  * @brief  Put STM32WL moden in low power
  * @param  pObj: the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_WL_LowPower(STM32WL_MODEM_Object_t *pObj);

/**
  * @brief  Reboot DFU
  * @param  pObj: the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t STM32WL_MODEM_WL_RebootDfu(STM32WL_MODEM_Object_t *pObj);

/** @brief Callback linked to send confirmed event
  * @param none
  * @retval none
  */
void SendConfirmedCallback(void);

/** @brief Callback linked to joined event
  * @param bJoinStatus: 8bit unsigned integer
  * @param bJoinComplete: 8bit unsigned integer
  * @retval none
  */
void JoinedCallback(uint8_t bJoinStatus, uint8_t bJoinComplete);

/** @brief Callback linked to busy error event
  * @param none
  * @retval none
  */
void BusyErrorCallback(void);

/** @brief Downlink messages
  * @param nFPort: 32bit unsigned integer
  * @param nLen: 32bit unsigned integer
  * @param dlbuf: Pointer to 8bit usigned integer
  * @retval none
  */
void DownlinkMsg(uint32_t nFPort, uint32_t nLen, uint8_t *dlbuf);

/**
* @} STM32WL_MODEM_FunctionPrototypes
*/

#ifdef __cplusplus
}
#endif

#endif /* __STM32WL_MODEM__H__ */

/**
* @} ASTRA_BSP_MODEM
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
