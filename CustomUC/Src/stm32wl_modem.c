/**
  ******************************************************************************
  * @file    stm32wl_modem.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   This file provides set of driver functions to manage communication
  *          between BSP and STM32WL LoRa modem.
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

/* Includes ------------------------------------------------------------------*/

#include "stm32wl_modem.h"

#define in_range(c, lo, up)    ((uint8_t)c >= lo && (uint8_t)c <= up)                                          //!< in_range UTILITY
#define isdigit(c)             in_range(c, '0', '9')                                                           //!< isdigit UTILITY
#define digit(c)               (c - '0')                                                                       //!< digit UTILITY
#define xdigit(c)              isdigit(c)?digit(c):in_range(c, 'a', 'f')?(10+c-'a'):in_range(c, 'A', 'F')?(10+c-'A'):0                 //!< xdigit UTILITY
#define isxdigit(c)            (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))                  //!< isxdigit UTILITY

#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif

#define PTR_BUFFER_LEN  128
#define PRINTF_0(...)
#define PRINTF_1(...)

/** @addtogroup STM32WL_MODEM_Private_Variables STM32WL modem - Private variables
  * @brief Private variables
  * @{
  */

/* Private variables  --------------------------------------------------------*/

uint8_t wlbuf[WL_RX_BUFFER_LEN+1];
uint8_t ptr[PTR_BUFFER_LEN+1];

STM32WL_MODEM_CommonDrv_t STM32WL_MODEM_COMMON_Driver =
{
  .Init                 = STM32WL_MODEM_Init,
  .DeInit               = STM32WL_MODEM_DeInit,
  .GetCapabilities      = STM32WL_MODEM_GetCapabilities,
};

STM32WL_MODEM_FuncDrv_t STM32WL_MODEM_LORA_Driver =
{
  .Enable               = STM32WL_MODEM_Enable,
  .Disable              = STM32WL_MODEM_Disable,
  .HwReset              = STM32WL_MODEM_HwReset,
  .Ping                 = STM32WL_MODEM_Ping,
  .ManageURCs           = STM32WL_MODEM_ManageURCs,
  .SendBinPacket        = STM32WL_MODEM_SendBinPacket,
  .GetVersion           = STM32WL_MODEM_GetVersion,
  .GetDeviceEUI         = STM32WL_MODEM_GetDeviceEUI,
  .GetAppEUI            = STM32WL_MODEM_GetAppEUI,
  .GetAppKEY            = STM32WL_MODEM_GetAppKEY,
  .GetNwkKEY            = STM32WL_MODEM_GetNwkKEY,
  .SetAppEUI            = STM32WL_MODEM_SetAppEUI,
  .SetAppKEY            = STM32WL_MODEM_SetAppKEY,
  .SetNwkKEY            = STM32WL_MODEM_SetNwkKEY,

  .GetDADD              = STM32WL_MODEM_GetDADD,
  .GetAppSKEY           = STM32WL_MODEM_GetAppSKEY,
  .GetNwkSKEY           = STM32WL_MODEM_GetNwkSKEY,
  .SetDADD              = STM32WL_MODEM_SetDADD,
  .SetAppSKEY           = STM32WL_MODEM_SetAppSKEY,
  .SetNwkSKEY           = STM32WL_MODEM_SetNwkSKEY,


  .GetDataRate          = STM32WL_MODEM_GetDataRate,
  .SetRegionBandID      = STM32WL_MODEM_SetRegionBandID,
  .Join_OTAA            = STM32WL_MODEM_Join_OTAA,
  .Join_ABP             = STM32WL_MODEM_Join_ABP,
  .SetAdaptiveDataRate  = STM32WL_MODEM_SetAdaptiveDataRate,
  .SetDataRate          = STM32WL_MODEM_SetDataRate,
  .WL_LowPower          = STM32WL_MODEM_WL_LowPower,
  .WL_RebootDfu         = STM32WL_MODEM_WL_RebootDfu,
};

/**
* @} STM32WL_MODEM_Private_Variables
*/

/** @addtogroup STM32WL_MODEM_Private_Types STM32WL modem - Private types
  * @brief Private types
  * @{
  */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  AT_CTRL = 0,
  AT_SET,
  AT_GET,
  AT_TEST,
}AT_cmd_type_t;

typedef enum
{
  AT_OK,
  AT_ERROR
}AT_resp_code_t;

typedef enum //see AT_cmd_strings; take care about the order
{
  AT_AT,
  AT_RESET,
  AT_JOIN,
  AT_NJS,
  AT_DEUI,
  AT_DADDR,
  AT_APPKEY,
  AT_NWKKEY,
  AT_NWKSKEY,
  AT_APPSKEY,
  AT_APPEUI,
  AT_SENDB,
  AT_BAND,
  AT_ADR,
  AT_DR,
  AT_VER,
  AT_CUST_LP,
  AT_CUST_DFU,
}AT_cmd_code_t;

typedef struct ATcommands_s
{
  AT_cmd_code_t code;
  char *cmdstring;
}ATcommands_t;

ATcommands_t AT_cmd_strings[] = {
  { AT_AT,        ""},             /* ATention command */
  { AT_RESET,     "Z"},            /* Reset command */
  { AT_JOIN,      "+JOIN"},        /* +JOIN */
  { AT_NJS,       "+NJS"},         /* +NJS */
  { AT_DEUI,      "+DEUI"},        /* +DEUI device ID */
  { AT_DADDR,     "+DADDR"},       /* +DADDR device Address */
  { AT_APPKEY,    "+APPKEY"},      /* +APPKEY application key */
  { AT_NWKKEY,    "+NWKKEY"},      /* +NWKKEY application key */
  { AT_NWKSKEY,   "+NWKSKEY"},     /* +NWKSKEY Network session Key */
  { AT_APPSKEY,   "+APPSKEY"},     /* +APPSKEY application Session key */
  { AT_APPEUI,    "+APPEUI"},      /* +APPEUI application Identifier */
  { AT_SENDB,     "+SENDB"},       /* +SENDB send data binary format */
  { AT_BAND,      "+BAND"},        /* +BAND Active Region BandID=[0:AS923, 1:AU915, 2:CN470, 3:CN779, 4:EU433, 5:EU868, 6:KR920, 7:IN865, 8:US915, 9:RU864]*/
  { AT_ADR,       "+ADR"},         /* +ADR adaptive data rate  */
  { AT_DR,        "+DR"},          /* +DR data rate */
  { AT_VER,       "+VER"},         /* +VER firmware version */
  { AT_CUST_LP,   "+CUST_LP"},     /* Put WL in low power */
  { AT_CUST_DFU,  "+CUST_DFU"},    /* Put WL in uart programming */
};

/**
* @} STM32WL_MODEM_Private_Types
*/

/** @addtogroup STM32WL_MODEM_Private_FunctionPrototypes STM32WL modem - Private function prototypes
  * @brief Private function prototypes
  * @{
  */

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  AT manage
 * @param  pObj: Pointer to STM32WL_MODEM_Object_t
 * @param  cmd_type: Command type AT_cmd_type_t
 * @param  cmd_code: Command code AT_cmd_code_t
 * @param  pInOutData: Pointer to void
 * @param  nDataLen: 32bit unsigned integer length
 * @retval AT_OK if success, otherwise AT_ERROR
 */
AT_resp_code_t AtManage(STM32WL_MODEM_Object_t *pObj, AT_cmd_type_t cmd_type, AT_cmd_code_t cmd_code, void *pInOutData, uint32_t nDataLen);

/**
 * @brief  AT manage empty
 * @param  pObj: Pointer to STM32WL_MODEM_Object_t
 * @retval AT_OK if success, otherwise AT_ERROR
 */
AT_resp_code_t AtManageEmpty(STM32WL_MODEM_Object_t *pObj);

/**
 * @brief  Modem delay
 * @param  pObj: Pointer to STM32WL_MODEM_Object_t
 * @param  msDelay: Milliseconds of delay
 * @retval none
 */
static void STM32WL_MODEM_Delay(STM32WL_MODEM_Object_t *pObj, uint32_t msDelay);

/**
 * @brief  Read wrap
 * @param  Handle: Pointer to void
 * @param  pData: Pointer to 8bit unsigned integer
 * @param  Length: 32bit unsigned integer length
 * @retval BSP_ERROR type
 */
static int32_t ReadWrap(void *Handle, uint8_t *pData, uint32_t Length);

/**
 * @brief  Write wrap
 * @param  Handle: Pointer to void
 * @param  pData: Pointer to 8bit unsigned integer
 * @param  Length: 32bit unsigned integer length
 * @retval BSP_ERROR type
 */
static int32_t WriteWrap(void *Handle, const uint8_t *pData, uint32_t Length);

/**
 * @brief  Reset wrap
 * @param  Handle: Pointer to void
 * @param  pData: Pointer to 8bit unsigned integer
 * @param  Length: 32bit unsigned integer length
 * @retval BSP_ERROR type
 */
static int32_t ResetWrap(void *Handle, uint8_t Level);

/**
 * @brief  Get AT command string
 * @param  cmd_code: AT_cmd_code_t
 * @retval command string
 */
static char *Get_AT_cmd_strings(AT_cmd_code_t cmd_code);

/**
 * @brief  Check URCs
 * @param  pObj: Pointer to STM32WL_MODEM_Object_t
 * @param  urcs_timeout: 32bit unsigned integer
 * @param  time_slices: 32bit unsigned integer
 * @retval none
 */
static void checkURCs(STM32WL_MODEM_Object_t *pObj, uint32_t urcs_timeout, uint32_t time_slices);

/**
* @} STM32WL_MODEM_Private_FunctionPrototypes
*/

/** @addtogroup STM32WL_MODEM_FunctionDefinitions STM32WL modem - Function definitions
  * @brief Function definitions
  * @{
  */

/* Function definitions ------------------------------------------------------*/

__WEAK void SendConfirmedCallback(void) {}
__WEAK void JoinedCallback(uint8_t bJoinStatus, uint8_t bJoinComplete) {}
__WEAK void BusyErrorCallback(void) {}
__WEAK void DownlinkMsg(uint32_t nFPort, uint32_t nLen, uint8_t *dlbuf) {}

int32_t STM32WL_MODEM_Init(STM32WL_MODEM_Object_t *pObj)
{
  /* Select default region. */
  pObj->region = STM32WL_MODEM_REGION_EU868;
  pObj->ack_enabled = 0;
  pObj->adr = 0;
  pObj->dr = 4;
  pObj->is_initialized = 1;

  return STM32WL_MODEM_OK;
}

int32_t STM32WL_MODEM_DeInit(STM32WL_MODEM_Object_t *pObj)
{
  pObj->is_initialized = 0;

  return STM32WL_MODEM_OK;
}

int32_t STM32WL_MODEM_GetCapabilities(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_Capabilities_t *Capabilities)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  Capabilities->LoRa            = 1;
  return STM32WL_MODEM_OK;
}

int32_t STM32WL_MODEM_RegisterBusIO(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_IO_t *pIO)
{
  int32_t ret = STM32WL_MODEM_OK;

  if (pObj == NULL)
  {
    ret = STM32WL_MODEM_ERROR;
  }
  else
  {
    pObj->IO.Init       = pIO->Init;
    pObj->IO.DeInit     = pIO->DeInit;
    pObj->IO.BusType    = pIO->BusType;
    pObj->IO.Address    = pIO->Address;
    pObj->IO.Write      = pIO->Write;
    pObj->IO.Read       = pIO->Read;
    pObj->IO.Reset       = pIO->Reset;
    pObj->IO.GetTick    = pIO->GetTick;

    pObj->Ctx.read      = ReadWrap;
    pObj->Ctx.write     = WriteWrap;
    pObj->Ctx.handle    = pObj;

    if (pObj->IO.Init == NULL)
    {
      ret = STM32WL_MODEM_ERROR;
    }
    else if (pObj->IO.Init() != STM32WL_MODEM_OK)
    {
      ret = STM32WL_MODEM_ERROR;
    }
    else
    {
      /* Continue configuration, e.g. AT ping */
    }
  }

  return ret;
}

int32_t STM32WL_MODEM_HwReset(STM32WL_MODEM_Object_t *pObj)
{
  int32_t nRet = STM32WL_MODEM_OK;

  PRINTF_0("MODEM_RESET()\r\n");
  ResetWrap(pObj, 0);
  STM32WL_MODEM_Delay(pObj, 10);

  ReadWrap(pObj, wlbuf, WL_RX_BUFFER_LEN);
  ResetWrap(pObj, 1);
  STM32WL_MODEM_Delay(pObj, 100);
  PRINTF_0("WL response: ");
  PRINTF_0((char*)wlbuf);
  STM32WL_MODEM_ManageURCs(pObj);

  if(AtManage(pObj, AT_CTRL, AT_AT, NULL, 0) == AT_OK)
  {
    //go ahead
    PRINTF_0("STM32WL status OK\r\n");
    nRet = STM32WL_MODEM_OK; //OK
  }
  else
  {
    PRINTF_0("STM32WL not responding\r\n");
    nRet = STM32WL_MODEM_ERROR; //ERROR
  }
  return nRet;
}

int32_t STM32WL_MODEM_Ping(STM32WL_MODEM_Object_t *pObj)
{
  int32_t nRet = STM32WL_MODEM_OK;
  if(AtManage(pObj, AT_CTRL, AT_AT, NULL, 0) != AT_OK)
  {
    nRet = STM32WL_MODEM_ERROR;
  }
  else
  {
    nRet = STM32WL_MODEM_OK;
  }
  return nRet;
}

int32_t STM32WL_MODEM_ManageURCs(STM32WL_MODEM_Object_t *pObj)
{
  checkURCs(pObj, URCS_TIMEOUT, N_TIME_SLICES);
  memset(wlbuf, 0x00, WL_RX_BUFFER_LEN);
  return ReadWrap(pObj, wlbuf, WL_RX_BUFFER_LEN);
}

int32_t STM32WL_MODEM_SendBinPacket(STM32WL_MODEM_Object_t *pObj, uint8_t *Buffer, uint32_t DataSize, uint32_t Port, uint8_t Confirmed)
{
  // fw v1.1.0 .help_string = "AT"AT_SEND"=<Port>:<Ack>:<Payload><CR>. Send binary data with the application Port=[1..199] and Ack=[0:unconfirmed, 1:confirmed]\r\n",// e.g. AT+SEND=99:0:020104030605\r
  char tmpStr[10+1];
  //as a memory optimized alternative

  // send +SENDB=
  PRINTF_1("AT+SEND=");
  WriteWrap(pObj, (const uint8_t *)"AT+SEND=", strlen("AT+SEND="));

  // send 99
  snprintf(tmpStr, 10, "%ld", Port);
  PRINTF_1(tmpStr);
  WriteWrap(pObj, (const uint8_t*)tmpStr, strlen(tmpStr));

  // send :
  PRINTF_1(":");
  WriteWrap(pObj, (const uint8_t *)":", strlen(":"));

  // send 0
  snprintf(tmpStr, 10, "%d", Confirmed);
  PRINTF_1(tmpStr);
  WriteWrap(pObj, (const uint8_t*)tmpStr, strlen(tmpStr));

  // send :
  PRINTF_1(":");
  WriteWrap(pObj, (const uint8_t *)":", strlen(":"));

  // for loop, send payload bytes
  for(uint32_t i = 0; i<DataSize; i++)
  {
    snprintf(tmpStr, 10, "%.2X", Buffer[i] );
    PRINTF_1(tmpStr);
    WriteWrap(pObj, (const uint8_t*)tmpStr, strlen(tmpStr));
  }

  // call specific AtManageEmpty, that sends \r and collect the response
  AT_resp_code_t ret = AtManageEmpty(pObj);
  int32_t nRet = 0;

  if(ret == AT_OK)
  {
    PRINTF_1("AtManageEmpty OK\r\n");
    nRet = STM32WL_MODEM_OK;
  }
  else
  {
    PRINTF_1("AtManageEmpty KO\r\n");
    nRet = STM32WL_MODEM_ERROR;
  }

  return nRet;
}

int32_t STM32WL_MODEM_GetVersion(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_wlversions_t * ver)
{
  AT_resp_code_t ret = AT_ERROR;
  PRINTF_0("LoRa_GetVersion()\r\n");

  ret = AtManage(pObj, AT_GET, AT_VER, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    char *token;
    const char s[2] = "\r";

    memset(ver,0x00, sizeof(STM32WL_MODEM_wlversions_t));

    token = strtok((char*)ptr, s);

    while( token != NULL )
    {
      if(token[0] == '\n')
      {
        token++;
      }

      PRINTF_0("%s\r\n", token);

      if(*token != '\0')
      {
        //search "OK" URC
        if(strncmp((char*)token, "OK", strlen("OK")) == 0)
        {
          //    PRINTF_1("- OK - found\r\n");
          break;
        }
        else if(strncmp((char*)token, "APP_VERSION:        V", strlen("APP_VERSION:        V")) == 0)
        {
          //    PRINTF_1("Single core APP_VERSION\r\n");
          ver->m4app.s.major = xdigit(token[21]);
          ver->m4app.s.minor = xdigit(token[23]);
          ver->m4app.s.rev = xdigit(token[25]);
        }
        else if(strncmp((char*)token, "M4 APP_VERSION:     V", strlen("M4 APP_VERSION:     V")) == 0)
        {
          //    PRINTF_1("M4 APP_VERSION\r\n");
          ver->m4app.s.major = xdigit(token[21]);
          ver->m4app.s.minor = xdigit(token[23]);
          ver->m4app.s.rev = xdigit(token[25]);
        }
        else if(strncmp((char*)token, "M0PLUS_APP_VERSION: V", strlen("M0PLUS_APP_VERSION: V")) == 0)
        {
          //    PRINTF_1("M0PLUS_APP_VERSION\r\n");
          ver->m0plusapp.s.major = xdigit(token[21]);
          ver->m0plusapp.s.minor = xdigit(token[23]);
          ver->m0plusapp.s.rev = xdigit(token[25]);
        }
        else if(strncmp((char*)token, "MW_LORAWAN_VERSION: V", strlen("MW_LORAWAN_VERSION: V")) == 0)
        {
          //    PRINTF_1("MW_LORAWAN_VERSION\r\n");
          ver->mwlora.s.major = xdigit(token[21]);
          ver->mwlora.s.minor = xdigit(token[23]);
          ver->mwlora.s.rev = xdigit(token[25]);
        }
        else if(strncmp((char*)token, "MW_RADIO_VERSION:   V", strlen("MW_RADIO_VERSION:   V")) == 0)
        {
          //    PRINTF_1("MW_RADIO_VERSION\r\n");
          ver->mwradio.s.major = xdigit(token[21]);
          ver->mwradio.s.minor = xdigit(token[23]);
          ver->mwradio.s.rev = xdigit(token[25]);
        }
      }

      token = strtok(NULL, s);
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_GetDeviceEUI(STM32WL_MODEM_Object_t *pObj, uint8_t *pDevEui)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetDeviceEUI()\r\n");
  memset(ptr, 0x00, 32);
  ret = AtManage(pObj, AT_GET, AT_DEUI, ptr, 32);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &pDevEui[0], &pDevEui[1], &pDevEui[2], &pDevEui[3],
    &pDevEui[4], &pDevEui[5], &pDevEui[6], &pDevEui[7]);

    if(nVals == 8)
    {
      PRINTF_0("DevEUI = ");
      PRINTF_0("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n", pDevEui[0], pDevEui[1], pDevEui[2], pDevEui[3],
                                                                 pDevEui[4], pDevEui[5], pDevEui[6], pDevEui[7]);
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_GetAppEUI(STM32WL_MODEM_Object_t *pObj, uint8_t *pAppEui)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetAppEUI()\r\n");
  memset(ptr, 0x00, 32);
  ret = AtManage(pObj, AT_GET, AT_APPEUI, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &pAppEui[0], &pAppEui[1], &pAppEui[2], &pAppEui[3],
    &pAppEui[4], &pAppEui[5], &pAppEui[6], &pAppEui[7]);

    if(nVals == 8)
    {
      PRINTF_0("AppEUI = ");
      PRINTF_0("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pAppEui[0], pAppEui[1], pAppEui[2], pAppEui[3],
                  pAppEui[4], pAppEui[5], pAppEui[6], pAppEui[7]);
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_GetAppKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pAppKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetAppKEY()\r\n");
  memset(ptr, 0x00, PTR_BUFFER_LEN);
  ret = AtManage(pObj, AT_GET, AT_APPKEY, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &pAppKey[0], &pAppKey[1], &pAppKey[2], &pAppKey[3],
    &pAppKey[4], &pAppKey[5], &pAppKey[6], &pAppKey[7],
    &pAppKey[8], &pAppKey[9], &pAppKey[10], &pAppKey[11],
    &pAppKey[12], &pAppKey[13], &pAppKey[14], &pAppKey[15]);

    if(nVals == 16)
    {
      PRINTF_0("AppKEY = ");
      PRINTF_0("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pAppKey[0], pAppKey[1], pAppKey[2], pAppKey[3],
                  pAppKey[4], pAppKey[5], pAppKey[6], pAppKey[7],
                  pAppKey[8], pAppKey[9], pAppKey[10], pAppKey[11],
                  pAppKey[12], pAppKey[13], pAppKey[14], pAppKey[15]);
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_GetNwkKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pNwkKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetNwkKEY()\r\n");
  memset(ptr, 0x00, PTR_BUFFER_LEN);
  ret = AtManage(pObj, AT_GET, AT_NWKKEY, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &pNwkKey[0], &pNwkKey[1], &pNwkKey[2], &pNwkKey[3],
    &pNwkKey[4], &pNwkKey[5], &pNwkKey[6], &pNwkKey[7],
    &pNwkKey[8], &pNwkKey[9], &pNwkKey[10], &pNwkKey[11],
    &pNwkKey[12], &pNwkKey[13], &pNwkKey[14], &pNwkKey[15]);

    if(nVals == 16)
    {
      PRINTF_0("NwkKEY = ");
      PRINTF_0("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pNwkKey[0], pNwkKey[1], pNwkKey[2], pNwkKey[3],
                  pNwkKey[4], pNwkKey[5], pNwkKey[6], pNwkKey[7],
                  pNwkKey[8], pNwkKey[9], pNwkKey[10], pNwkKey[11],
                  pNwkKey[12], pNwkKey[13], pNwkKey[14], pNwkKey[15]);
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetAppEUI(STM32WL_MODEM_Object_t *pObj, uint8_t *NewAppEui)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_SetAppEUI()\r\n");
  sprintf((char*)ptr, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
          NewAppEui[0], NewAppEui[1], NewAppEui[2], NewAppEui[3],
          NewAppEui[4], NewAppEui[5], NewAppEui[6], NewAppEui[7]);

  ret = AtManage(pObj, AT_SET, AT_APPEUI, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetAppKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewAppKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_SetAppKEY()\r\n");
  sprintf((char*)ptr, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                  NewAppKey[0], NewAppKey[1], NewAppKey[2], NewAppKey[3],
                  NewAppKey[4], NewAppKey[5], NewAppKey[6], NewAppKey[7],
                  NewAppKey[8], NewAppKey[9], NewAppKey[10], NewAppKey[11],
                  NewAppKey[12], NewAppKey[13], NewAppKey[14], NewAppKey[15]);

  ret = AtManage(pObj, AT_SET, AT_APPKEY, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetNwkKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewNwkKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_SetNwkKEY()\r\n");
  sprintf((char*)ptr, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                  NewNwkKey[0], NewNwkKey[1], NewNwkKey[2], NewNwkKey[3],
                  NewNwkKey[4], NewNwkKey[5], NewNwkKey[6], NewNwkKey[7],
                  NewNwkKey[8], NewNwkKey[9], NewNwkKey[10], NewNwkKey[11],
                  NewNwkKey[12], NewNwkKey[13], NewNwkKey[14], NewNwkKey[15]);

  ret = AtManage(pObj, AT_SET, AT_NWKKEY, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}




int32_t STM32WL_MODEM_GetDADD(STM32WL_MODEM_Object_t *pObj, uint32_t *pDAddress)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetDevAddress()\r\n");
  memset(ptr, 0x00, PTR_BUFFER_LEN);
  ret = AtManage(pObj, AT_GET, AT_DADDR, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx",
                     &((unsigned char *)(pDAddress))[3],
                     &((unsigned char *)(pDAddress))[2],
                     &((unsigned char *)(pDAddress))[1],
                     &((unsigned char *)(pDAddress))[0]);

    if(nVals == 4)
    {
      PRINTF_0("DevAddress = ");
      PRINTF_0("%02X:%02X:%02X:%02X\r\n",
            ( unsigned )( ( unsigned char * )( &pDAddress ) )[3],
            ( unsigned )( ( unsigned char * )( &pDAddress ) )[2],
            ( unsigned )( ( unsigned char * )( &pDAddress ) )[1],
            ( unsigned )( ( unsigned char * )( &pDAddress ) )[0] );
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_GetAppSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pAppSKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetAppSKEY()\r\n");
  memset(ptr, 0x00, PTR_BUFFER_LEN);
  ret = AtManage(pObj, AT_GET, AT_APPSKEY, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &pAppSKey[0], &pAppSKey[1], &pAppSKey[2], &pAppSKey[3],
    &pAppSKey[4], &pAppSKey[5], &pAppSKey[6], &pAppSKey[7],
    &pAppSKey[8], &pAppSKey[9], &pAppSKey[10], &pAppSKey[11],
    &pAppSKey[12], &pAppSKey[13], &pAppSKey[14], &pAppSKey[15]);

    if(nVals == 16)
    {
      PRINTF_0("AppSKEY = ");
      PRINTF_0("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pAppSKey[0], pAppSKey[1], pAppSKey[2], pAppSKey[3],
                  pAppSKey[4], pAppSKey[5], pAppSKey[6], pAppSKey[7],
                  pAppSKey[8], pAppSKey[9], pAppSKey[10], pAppSKey[11],
                  pAppSKey[12], pAppSKey[13], pAppSKey[14], pAppSKey[15]);
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_GetNwkSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *pNwkSKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_GetNwkSKEY()\r\n");
  memset(ptr, 0x00, PTR_BUFFER_LEN);
  ret = AtManage(pObj, AT_GET, AT_NWKSKEY, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);

    uint32_t nVals = sscanf((char*)ptr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &pNwkSKey[0], &pNwkSKey[1], &pNwkSKey[2], &pNwkSKey[3],
    &pNwkSKey[4], &pNwkSKey[5], &pNwkSKey[6], &pNwkSKey[7],
    &pNwkSKey[8], &pNwkSKey[9], &pNwkSKey[10], &pNwkSKey[11],
    &pNwkSKey[12], &pNwkSKey[13], &pNwkSKey[14], &pNwkSKey[15]);

    if(nVals == 16)
    {
      PRINTF_0("NwkKEY = ");
      PRINTF_0("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  pNwkSKey[0], pNwkSKey[1], pNwkSKey[2], pNwkSKey[3],
                  pNwkSKey[4], pNwkSKey[5], pNwkSKey[6], pNwkSKey[7],
                  pNwkSKey[8], pNwkSKey[9], pNwkSKey[10], pNwkSKey[11],
                  pNwkSKey[12], pNwkSKey[13], pNwkSKey[14], pNwkSKey[15]);
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetDADD(STM32WL_MODEM_Object_t *pObj, uint32_t NewDAddress)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_SetDevAddress()\r\n");
  sprintf((char*)ptr, "%02X:%02X:%02X:%02X\r\n",
            (unsigned)((unsigned char *)(&NewDAddress))[3],
            (unsigned)((unsigned char *)(&NewDAddress))[2],
            (unsigned)((unsigned char *)(&NewDAddress))[1],
            (unsigned)((unsigned char *)(&NewDAddress))[0]);

  ret = AtManage(pObj, AT_SET, AT_DADDR, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetAppSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewAppSKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_SetAppSKEY()\r\n");
  sprintf((char*)ptr, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                  NewAppSKey[0], NewAppSKey[1], NewAppSKey[2], NewAppSKey[3],
                  NewAppSKey[4], NewAppSKey[5], NewAppSKey[6], NewAppSKey[7],
                  NewAppSKey[8], NewAppSKey[9], NewAppSKey[10], NewAppSKey[11],
                  NewAppSKey[12], NewAppSKey[13], NewAppSKey[14], NewAppSKey[15]);

  ret = AtManage(pObj, AT_SET, AT_APPSKEY, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetNwkSKEY(STM32WL_MODEM_Object_t *pObj, uint8_t *NewNwkSKey)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_SetNwkSKEY()\r\n");
  sprintf((char*)ptr, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                  NewNwkSKey[0], NewNwkSKey[1], NewNwkSKey[2], NewNwkSKey[3],
                  NewNwkSKey[4], NewNwkSKey[5], NewNwkSKey[6], NewNwkSKey[7],
                  NewNwkSKey[8], NewNwkSKey[9], NewNwkSKey[10], NewNwkSKey[11],
                  NewNwkSKey[12], NewNwkSKey[13], NewNwkSKey[14], NewNwkSKey[15]);

  ret = AtManage(pObj, AT_SET, AT_NWKSKEY, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}






int32_t STM32WL_MODEM_GetDataRate(STM32WL_MODEM_Object_t *pObj, uint8_t *pnLoraDr)
{
  AT_resp_code_t ret = AT_ERROR;
  PRINTF_0("Lora_GetDataRate()\r\n");
  memset(ptr, 0x00, PTR_BUFFER_LEN);
  ret = AtManage(pObj, AT_GET, AT_DR, ptr, PTR_BUFFER_LEN);
  if(ret == AT_OK)
  {
    PRINTF_0("resp: ");
    PRINTF_0((char*)ptr);
    uint8_t nDr;
    uint32_t nVals = sscanf((char*)ptr, "%hhx", &nDr);

    if(nVals == 1)
    {
      PRINTF_0("GET DR = %.2X", nDr);
      *pnLoraDr = nDr;
    }
    else
    {
      ret = AT_ERROR; //actually not an AT error...
    }
  }
  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetRegionBandID(STM32WL_MODEM_Object_t *pObj, STM32WL_MODEM_region_t *region)
{
  AT_resp_code_t ret;
  PRINTF_0("Lora_SetRegionBandID()\r\n");
  uint32_t b = (uint32_t)(*region);
  sprintf((char*)ptr, "%ld", b);

  ret = AtManage(pObj, AT_SET, AT_BAND, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_Join_OTAA(STM32WL_MODEM_Object_t *pObj)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_Join_OTAA()\r\n");
  sprintf((char*)ptr, "%d", 1);

  ret = AtManage(pObj, AT_SET, AT_JOIN, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_Join_ABP(STM32WL_MODEM_Object_t *pObj)
{
  AT_resp_code_t ret;
  PRINTF_0("LoRa_Join_OTAA()\r\n");
  sprintf((char*)ptr, "%d", 0);

  ret = AtManage(pObj, AT_SET, AT_JOIN, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetAdaptiveDataRate(STM32WL_MODEM_Object_t *pObj, uint8_t adr)
{
  AT_resp_code_t ret;
  PRINTF_0("Lora_SetAdaptiveDataRate()\r\n");
  sprintf((char*)ptr, "%d", adr);

  ret = AtManage(pObj, AT_SET, AT_ADR, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_SetDataRate(STM32WL_MODEM_Object_t *pObj, uint8_t dr)
{
  AT_resp_code_t ret;
  PRINTF_0("Lora_SetDataRate()\r\n");
  sprintf((char*)ptr, "%d", dr);

  ret = AtManage(pObj, AT_SET, AT_DR, ptr, PTR_BUFFER_LEN);

  return (ret == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_WL_LowPower(STM32WL_MODEM_Object_t *pObj)
{
  return (AtManage(pObj, AT_CTRL, AT_CUST_LP, NULL, 0) == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_WL_RebootDfu(STM32WL_MODEM_Object_t *pObj)
{
  return (AtManage(pObj, AT_CTRL, AT_CUST_DFU, NULL, 0) == AT_OK)?STM32WL_MODEM_OK:STM32WL_MODEM_ERROR;
}

int32_t STM32WL_MODEM_Enable(STM32WL_MODEM_Object_t *pObj)
{
  int32_t ret = STM32WL_MODEM_OK;

  return ret;
}

int32_t STM32WL_MODEM_Disable(STM32WL_MODEM_Object_t *pObj)
{
  int32_t ret = STM32WL_MODEM_OK;

  return ret;
}

static int32_t ReadWrap(void *Handle, uint8_t *pData, uint32_t Length)
{
  STM32WL_MODEM_Object_t *pObj = (STM32WL_MODEM_Object_t *)Handle;

  return pObj->IO.Read(pData, Length);
}

static int32_t WriteWrap(void *Handle, const uint8_t *pData, uint32_t Length)
{
  STM32WL_MODEM_Object_t *pObj = (STM32WL_MODEM_Object_t *)Handle;

  return pObj->IO.Write(pData, Length);
}

static int32_t ResetWrap(void *Handle, uint8_t Level)
{
  STM32WL_MODEM_Object_t *pObj = (STM32WL_MODEM_Object_t *)Handle;

  return pObj->IO.Reset(Level);
}

char const AT_resp_OK[] = "\r\nOK\r\n";
char const *AT_resp_strings[] = {
  "\r\nOK\r",
};

static uint8_t bOkFound = 0;
static uint8_t bErrorFound = 0;

void DecodeRxSlotMsg(char*evtstring)
{
  PRINTF_1(evtstring);
  PRINTF_1("\r\n");
}

void DecodeDowlinkMsg(char*evtstring)
{
  int32_t nFPort=0, nLen=0;
  uint8_t nParams = sscanf(evtstring, "+EVT:%ld:%ld:", &nFPort, &nLen);
  if(nParams == 2)
  {
    uint8_t i, nSymbols=0;
    for(i=0;i<15;i++)//+EVT:255:255: max len = 13
    {
      if(evtstring[i] == ':')
      {
        nSymbols++;
      }
      if(nSymbols==3)
      {
        break;
      }
    }
    i++;
    uint8_t downlinkbuf[256]= { 0 };
    uint32_t j;
    for(j=0;j<nLen;j++)
    {
      char*todecode = &evtstring[i+(j*2)];
      if(isxdigit(todecode[0]) && isxdigit(todecode[1]))
      {
        downlinkbuf[j] = xdigit(todecode[0]);
        downlinkbuf[j] = downlinkbuf[j] << 4;
        downlinkbuf[j] |= xdigit(todecode[1]);
      }
    }
    if(j==nLen)//parse complete
    {
      DownlinkMsg(nFPort, nLen, downlinkbuf);
    }
    //execute downlink callback
  }
}

void ParseToken(char *_token)
{
  //search "+EVT:JOINED" URC
  if(strncmp((char*)_token, "+EVT:JOINED", strlen("+EVT:JOINED")) == 0)
  {
    PRINTF_1("Network has joined!v1.1.0\r\n");
    //execute JOINED callback
    JoinedCallback(1, 1);
  }

  //search "+EVT:JOIN FAILED" URC
  else if(strncmp((char*)_token, "+EVT:JOIN FAILED", strlen("+EVT:JOIN FAILED")) == 0)
  {
    PRINTF_1("Network has NOT joined!v1.1.0\r\n");
    //execute JOINED callback
    JoinedCallback(0, 1);
  }

  //search "JOINED" URC
  else if(strncmp((char*)_token, "JOINED", strlen("JOINED")) == 0)
  {
    PRINTF_1("Network has joined!v0.6.0\r\n");
    //execute JOINED callback
    JoinedCallback(1, 1);
  }

  //+EVT:SEND_CONFIRMED
  else if(strncmp((char*)_token, "+EVT:SEND_CONFIRMED", strlen("+EVT:SEND_CONFIRMED")) == 0)
  {
    PRINTF_1("Ack received!\r\n");
    //execute SEND_CONFIRMED callback
    SendConfirmedCallback();
  }

  //+EVT:RX_ RX_<slot>:<DR>:<RSSI>:<SNR> or RX_<slot>:<DR>:<RSSI>:<SNR>:<DMODM>:<GWN>
  else if(strncmp((char*)_token, "+EVT:RX_", strlen("+EVT:RX_")) == 0)
  {
    PRINTF_1("EVT:RX_\r\n");
    //execute EVT:RX_ callback
    DecodeRxSlotMsg(_token);
  }

  //+EVT: downlink msg, +EVT:<port>:<size>:<payload> e.g. +EVT:1:04:ababcdcd fport 1, size 4 bytes, payload
  else if(strncmp((char*)_token, "+EVT:", strlen("+EVT:")) == 0)
  {
    PRINTF_1("Downlink message\r\n");
    DecodeDowlinkMsg(_token);
  }

  //search "AT_NO_NETWORK_JOINED" URC
  else if(strncmp((char*)_token, "AT_NO_NETWORK_JOINED", strlen("AT_NO_NETWORK_JOINED")) == 0)
  {
    PRINTF_1("- AT_NO_NETWORK_JOINED - found\r\n");
    //execute AT_NO_NETWORK_JOINED callback
    JoinedCallback(0, 0);
  }

  //search "AT_BUSY_ERROR" URC
  else if(strncmp((char*)_token, "AT_BUSY_ERROR", strlen("AT_BUSY_ERROR")) == 0)
  {
    PRINTF_1("- AT_BUSY_ERROR - found\r\n");
    //execute AT_BUSY_ERROR callback
    BusyErrorCallback();
  }
  else
  {
    uint32_t i = 0;
    if(isdigit(_token[0+i]) && _token[1+i] == 's' && isdigit(_token[2+i]) && isdigit(_token[3+i]) && isdigit(_token[4+i]) && _token[5+i] == ':')
    {
      PRINTF_1("\r\nVERB URC:", _token);
    }
    else
    {
      i++;
      if(isdigit(_token[0+i]) && _token[1+i] == 's' && isdigit(_token[2+i]) && isdigit(_token[3+i]) && isdigit(_token[4+i]) && _token[5+i] == ':')
      {
        PRINTF_1("\r\nVERB URC:", _token);
      }
      else
      {
        i++;
        if(isdigit(_token[0+i]) && _token[1+i] == 's' && isdigit(_token[2+i]) && isdigit(_token[3+i]) && isdigit(_token[4+i]) && _token[5+i] == ':')
        {
          PRINTF_1("\r\nVERB URC:", _token);
        }
        else
        {
          i++;
          if(isdigit(_token[0+i]) && _token[1+i] == 's' && isdigit(_token[2+i]) && isdigit(_token[3+i]) && isdigit(_token[4+i]) && _token[5+i] == ':')
          {
            PRINTF_1("\r\nVERB URC:", _token);
          }
          else
          {
            i++;
            if(isdigit(_token[0+i]) && _token[1+i] == 's' && isdigit(_token[2+i]) && isdigit(_token[3+i]) && isdigit(_token[4+i]) && _token[5+i] == ':')
            {
              PRINTF_1("\r\nVERB URC:", _token);
            }
            else
            {
              i++;
              if(isdigit(_token[0+i]) && _token[1+i] == 's' && isdigit(_token[2+i]) && isdigit(_token[3+i]) && isdigit(_token[4+i]) && _token[5+i] == ':')
              {
                PRINTF_1("\r\nVERB URC:", _token);
              }
              else
              {
                PRINTF_1("URC NOT RECOGNIZED\r\n");
              }
            }
          }
        }
      }
    }
    PRINTF_1("%s\r\n", _token);
  }
}

void OkFoundCallback()
{
  bOkFound = 1;
}

void ErrorFoundCallback()
{
  bErrorFound = 1;
}

void checkURCs(STM32WL_MODEM_Object_t *pObj, uint32_t urcs_timeout, uint32_t time_slices)
{
  uint32_t nIndex = time_slices;
  uint32_t nPreviousLenght = 0;
  uint32_t nCurrentLenght = 0;

  bOkFound = 0;
  bErrorFound = 0;

  PRINTF_0("checkURCs\r\n");

  while(nIndex--)
  {
    if(wlbuf[0] != '\0')
    {
      nCurrentLenght = strlen((char*)wlbuf);
      if(nCurrentLenght == nPreviousLenght)break; // check that no data is flowing
      nPreviousLenght = nCurrentLenght;
    }
    else
    {
      PRINTF_0("NO URCs\r\n");
      break;
    }
    STM32WL_MODEM_Delay(pObj, urcs_timeout/time_slices);
  }

  if(wlbuf[0] != '\0')
  {
    PRINTF_0("\r\nGOT URC\r\n");
    PRINTF_0((char*)wlbuf);

    /* here the unprocessed data must be analysed or at list saved, otherwise info will be lost */
    const char s[2] = "\r";
    char *token;

    /* get the first token */
    token = strtok((char*)wlbuf, s);

    /* walk through other tokens */
    while( token != NULL )
    {
      if(token[0] == '\n')
      {
        token++;
      }

      PRINTF_0("%s\r\n", token);

      if(*token != '\0')
      {
        //search "OK" string
        if(strncmp((char*)token, "OK", strlen("OK")) == 0)
        {
//          PRINTF_1("\r\n- OK - found\r\n");
          //execute OK callback
          OkFoundCallback();
        }
        //search "AT_ERROR" string
        else if(strncmp((char*)token, "AT_ERROR", strlen("AT_ERROR")) == 0)
        {
          PRINTF_1("\r\n- AT_ERROR - found\r\n");
          //execute ERROR callback
          ErrorFoundCallback();
        }
        else
        {
          ParseToken(token);
        }
      }

      token = strtok(NULL, s);
    }
  }
}

void AtWaitResponse(STM32WL_MODEM_Object_t *pObj, uint32_t nTimeout, uint32_t time_slices)
{
  uint32_t nIndex = time_slices;

  uint32_t nPreviousLenght = 0;
  uint32_t nCurrentLenght = 0;
  while(nIndex--)
  {
    if(wlbuf[0] != '\0')
    {
      nCurrentLenght = strlen((char*)wlbuf);
      if(nCurrentLenght == nPreviousLenght)break; // check that no data is flowing
      nPreviousLenght = nCurrentLenght;
    }
    STM32WL_MODEM_Delay(pObj, nTimeout/time_slices);
  }
  if(nIndex != 0xFFFFFFFF)
  {
    PRINTF_0("GOT response\r\n");
    PRINTF_0((char*)wlbuf);
  }
}

AT_resp_code_t AtDecodeResponse(STM32WL_MODEM_Object_t *pObj, AT_cmd_type_t cmd_type, AT_cmd_code_t cmd_code, uint8_t* buftodecode)
{
  AT_resp_code_t nretCode = AT_ERROR;
  uint32_t nBufToDecLen;
  uint32_t nRespOKLen;
  uint32_t nOffset;

  switch(cmd_code)
  {
    //the following commands will have a response that must be decoded (if AT_GET is used)
  case AT_AT:
  case AT_JOIN:
  case AT_NJS:
  case AT_DEUI:
  case AT_DADDR:
  case AT_APPKEY:
  case AT_NWKKEY:
  case AT_NWKSKEY:
  case AT_APPSKEY:
  case AT_APPEUI:
  case AT_SENDB:
  case AT_BAND:
  case AT_ADR:
  case AT_DR:
  case AT_VER:
  case AT_CUST_LP:
  case AT_CUST_DFU:
    nBufToDecLen = strlen((char*)buftodecode);
    nRespOKLen = strlen(AT_resp_OK);
    nOffset = nBufToDecLen - nRespOKLen;
    if(cmd_type != AT_GET)
    {
      checkURCs(pObj, URCS_TIMEOUT, N_TIME_SLICES);
      if(bOkFound)
      {
        nretCode = AT_OK;
      }
      if(bErrorFound)
      {
        nretCode = AT_ERROR;
      }
    }
    else
    {
      if(0==strncmp(AT_resp_OK, (char*)(buftodecode+nOffset), strlen(AT_resp_OK)))
      {
        nretCode = AT_OK;
      }
    }
    break;
  default:
    break;
  }
  return nretCode;
}

AT_resp_code_t AtManage(STM32WL_MODEM_Object_t *pObj, AT_cmd_type_t cmd_type, AT_cmd_code_t cmd_code, void *pInOutData, uint32_t nDataLen)
{
  checkURCs(pObj, URCS_TIMEOUT, N_TIME_SLICES);
  memset(wlbuf, 0x00, WL_RX_BUFFER_LEN);
  ReadWrap(pObj, wlbuf, WL_RX_BUFFER_LEN);

  PRINTF_0("Send AT cmd: AT%s", Get_AT_cmd_strings(cmd_code));
  WriteWrap(pObj, (uint8_t *)"AT", strlen("AT"));
  WriteWrap(pObj, (uint8_t *)Get_AT_cmd_strings(cmd_code), strlen(Get_AT_cmd_strings(cmd_code)));
  if(cmd_type == AT_CTRL)
  {
  }
  else if(cmd_type == AT_SET)
  {
    PRINTF_0("=");
    WriteWrap(pObj, (uint8_t *)"=", strlen("="));
    PRINTF_0(pInOutData);
    WriteWrap(pObj, pInOutData, strlen(pInOutData));
  }
  else if(cmd_type == AT_GET)
  {
    PRINTF_0("=?");
    WriteWrap(pObj, (uint8_t *)"=?", strlen("=?"));
  }
  else if(cmd_type == AT_TEST)
  {
  }
  PRINTF_0("\r\n");
  WriteWrap(pObj, (uint8_t *)"\r", strlen("\r"));
  AtWaitResponse(pObj, AT_WAIT_RESP_TIMEOUT, N_TIME_SLICES);
  AT_resp_code_t ret = AtDecodeResponse(pObj, cmd_type, cmd_code, wlbuf);
  if(ret == AT_OK)
  {
    if(cmd_type == AT_GET)
    {
      memcpy(pInOutData, wlbuf, nDataLen);
    }
  }
  /* restart receiving, not to miss URCs */
  memset(wlbuf, 0x00, WL_RX_BUFFER_LEN);
  ReadWrap(pObj, wlbuf, WL_RX_BUFFER_LEN);

  return ret;
}

AT_resp_code_t AtManageEmpty(STM32WL_MODEM_Object_t *pObj)
{
  checkURCs(pObj, URCS_TIMEOUT, N_TIME_SLICES);
  memset(wlbuf, 0x00, WL_RX_BUFFER_LEN);
  ReadWrap(pObj, wlbuf, WL_RX_BUFFER_LEN);

  PRINTF_0("\r\n");
  WriteWrap(pObj, (uint8_t *)"\r", strlen("\r"));
  AtWaitResponse(pObj, AT_WAIT_RESP_TIMEOUT, N_TIME_SLICES);
  AT_resp_code_t ret = AtDecodeResponse(pObj, AT_SET, AT_AT, wlbuf); // TDODO:FS: check; Decode response like for a generic AT cmd
  if(ret == AT_OK)
  {

  }
  /* restart receiving, not to miss URCs */
  memset(wlbuf, 0x00, WL_RX_BUFFER_LEN);
  ReadWrap(pObj, wlbuf, WL_RX_BUFFER_LEN);

  return ret;
}

static void STM32WL_MODEM_Delay(STM32WL_MODEM_Object_t *pObj, uint32_t msDelay)
{
  uint32_t tickstart = pObj->IO.GetTick();

  while((pObj->IO.GetTick() - tickstart) < msDelay)
  {
  }
}

char *Get_AT_cmd_strings(AT_cmd_code_t cmd_code)
{
  //1. search atcmd_stm32wl_custom.c/.h

  //2. if not found, search AT_cmd_strings

  return AT_cmd_strings[cmd_code].cmdstring;
}

/**
* @} STM32WL_MODEM_FunctionDefinitions
*/

/**
  * @} ASTRA_BSP_MODEM
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
