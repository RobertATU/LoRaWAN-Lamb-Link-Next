/**
  ******************************************************************************
  * @file    astra_sysmng.c
  * @author  SRA Team
  * @version V1.0.1
  * @brief   This file contains the Astra system management
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

#include <string.h>
#include <stdio.h>
#include "astra_sysmng.h"
#include "SM_App.h"
#include "app_debug.h"

#if USE_BLE
#include "BLE_Manager.h"
#endif //USE_BLE

#if USE_STM32WL
#include "custom_modem_control.h"
#endif //USE_STM32WL

#if USE_MEMORY
#include "astra_app_nfc.h"
#include "SmartNFCType.h"
#include "SmartNFC_config.h"
#endif //USE_USB

#if USE_BOOT_UPGRADE
  #include "astra_boot.h"
#endif //USE_BOOT_UPGRADE

/** @addtogroup ASTRA_SYSTEM_MANAGEMENT_Private_Defines ASTRA System management - Private defines
  * @brief Private Defines
  * @{
 */

/* Private defines -----------------------------------------------------------*/

#define DISABLE_PRINT   0
#define ENABLE_PRINT    1

#ifndef CFG_GAP_DEVICE_NAME
#define CFG_GAP_DEVICE_NAME             "Astra"
#endif //CFG_GAP_DEVICE_NAME

/**
* @}
*/

/** @addtogroup ASTRA_SYSTEM_MANAGEMENT_PRIVATE_TYPES ASTRA System management - Private types
  * @brief Private types
  * @{
 */

/* Private typedef -----------------------------------------------------------*/

typedef struct ButtonCallbacks_s
{
  pBtnCb_t Button1ShortCb;
  pBtnCb_t Button1LongCb;
  pBtnCb_t Button2ShortCb;
  pBtnCb_t Button2LongCb;
#if USE_BTN1_ANTI_TAMPER
  pBtnCb_t Button1TamperGlitchCb;
  pBtnCb_t Button1TamperActivatedCb;
  pBtnCb_t Button1TamperRestoredCb;
#endif //USE_BTN1_ANTI_TAMPER
}ButtonCallbacks_t;

/**
  * @brief Command handler function typedef
  */
typedef void (*pCmdHandlerFunc_t)(char *pcInStr, char *pcInParam);

/**
* @brief Command handler descriptor
*/
typedef struct s_cmd_struct {
  char cCmdString[20];                          //!< Command string
  char cCmdHelp[50];                            //!< Command help string
  pCmdHandlerFunc_t pCmdHandlerFunc;            //!< Command function pointer
  uint8_t bShowInHelp;                          //!< To show the function with '?' command
  uint8_t bPrintConfig;                         //!< To disable message printing after command execution
} xCmdStruct_t;

/**
* @}
*/

/** @addtogroup ASTRA_SYSTEM_MANAGEMENT_Private_FunctionPrototypes ASTRA System management - Private Function Prototypes
  * @brief Private function prototypes
  * @{
 */

/* Private function prototypes -----------------------------------------------*/

#ifndef __SM_CONF_H
/**
  * @brief  Astra switch state
  * @param  none
  * @retval none
  */
void AstraSwitchState(void);
#endif //__SM_CONF_H

/**
  * @brief  Schedule state machine change
  * @param  wakeType wake sources active in low power mode
  * @retval none
  */
void AstraUlpSchedule(uint8_t wakeType);

/**
  * @brief  LoRa join trigger
  * @param  none
  * @retval none
  */
void LoraJoinTrigger(void);

/**
  * @brief  LoRa join timeout
  * @param  none
  * @retval none
  */
void LoraJoinTimeout(void);

/**
  * @brief  LoRa send trigger
  * @param  none
  * @retval none
  */
void LoraSendTrigger(void);

/**
  * @brief  BLE manager triggered
  * @param  none
  * @retval none
  */
void BleMngTrigger(void);

/**
  * @brief  Status upgrade trigger
  * @param  none
  * @retval none
  */
void StatusUpdTrigger(void);

/**
  * @brief  NFC data saving upgrade trigger
  * @param  none
  * @retval none
  */
void NFCDataSavingTrigger(void);

/**
  * @brief  Get firmware version
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetFwVersion(char *pcInStr, char *pcInParam);

/**
  * @brief  Get hardware version
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetHwVersion(char *pcInStr, char *pcInParam);

/**
  * @brief  Get hardware version
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetBleMacAddr(char *pcInStr, char *pcInParam);

/**
  * @brief  Get hardware version
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetLoraParams(char *pcInStr, char *pcInParam);

/**
  * @brief  Get verbosity
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetVerbosity(char *pcInStr, char *pcInParam);

/**
  * @brief  Set verbosity
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetVerbosity(char *pcInStr, char *pcInParam);

/**
  * @brief  System reset
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSysReset(char *pcInStr, char *pcInParam);

/**
  * @brief  System reset in USB DFU mode
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSysDfu(char *pcInStr, char *pcInParam);

/**
  * @brief  System reset in USB DFU mode
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSysGetBlVer(char *pcInStr, char *pcInParam);

/**
  * @brief  Put STM32WL in dfu mode
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSysWlPrg(char *pcInStr, char *pcInParam);

/**
  * @brief  Send a command to STM32WL via UART
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgFwdWl(char *pcInStr, char *pcInParam);

/**
  * @brief  Send a CR+LF to STM32WL via UART
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgFwdWlNewline(char *pcInStr, char *pcInParam);

/**
  * @brief  Get STM32WL response on UART
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgWlGetResponse(char *pcInStr, char *pcInParam);

/**
  * @brief  Get STM32WL response on UART
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgWlTransparent(char *pcInStr, char *pcInParam);

/**
  * @brief  Set debug mode
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetDebugMode(char *pcInStr, char *pcInParam);

/**
  * @brief  Low power conf for MEMS event
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetMemsEvConfig(char *pcInStr, char *pcInParam);

/**
  * @brief  Get debug mode
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetDebugMode(char *pcInStr, char *pcInParam);

/**
  * @brief  Set GNSS app configuration
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetGnssAppConfig(char *pcInStr, char *pcInParam);

/**
  * @brief  Get GNSS status report
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetGnssStatus(char *pcInStr, char *pcInParam);

/**
  * @brief  Set GNSS Nmea configuration
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetGnssNmeaConfig(char *pcInStr, char *pcInParam);

/**
  * @brief  Set LoRa app configuration
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetLoraAppConfig(char *pcInStr, char *pcInParam);

/**
  * @brief  Get LoRa app configuration
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgGetLoraAppConfig(char *pcInStr, char *pcInParam);

/**
  * @brief  Stop firmware
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgStopFw(char *pcInStr, char *pcInParam);

/**
  * @brief  System state run
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSystemStateRun(char *pcInStr, char *pcInParam);

/**
  * @brief  System state low power
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSystemStateLowPower(char *pcInStr, char *pcInParam);

/**
  * @brief  Set Asset tracking use case
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgSetUseCase(char *pcInStr, char *pcInParam);

/**
  * @brief  Help
  * @param  pcInStr: Pointer to char
  * @param  pcInParam: Pointer to char
  * @retval none
  */
void ProcDbgHelp(char *pcInStr, char *pcInParam);

/**
  * @brief  Set lora send timer
  * @param  TimerIntv: 32bit unsigned integer
  * @param  bImmediateTrigger: 8bit unsigned integer
  * @retval none
  */
void setLoraSendTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger);

/**
  * @brief  Set BLE manage timer
  * @param  TimerIntv: 32bit unsigned integer
  * @param  bImmediateTrigger: 8bit unsigned integer
  * @retval none
  */
void setBleMngTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger);

/**
  * @brief  Set status update timer
  * @param  TimerIntv: 32bit unsigned integer
  * @param  bImmediateTrigger: 8bit unsigned integer
  * @retval none
  */
void setStatusUpdTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger);

/**
  * @brief  Manage LEDs
  * @param  none
  * @retval none
  */
void manageLeds(void);

/**
  * @brief  Manage decimation tasks
  * @param  none
  * @retval none
  */
void manageDecimationTasks(void);

/**
  * @brief  Manage algorithms
  * @param  none
  * @retval none
  */
void manageAlgorithms(void);

/**
  * @brief  Manage data
  * @param  none
  * @retval none
  */
void manageData(void);

/**
  * @brief  Board reboot
  * @param  none
  * @retval none
  */
void BoardReboot(void);

/**
  * @brief  UART configuartion
  * @param  none
  * @retval none
  */
void UartConfig(void);

/**
  * @brief  Schedule ASTRA engine restart
  * @param  none
  * @retval none
  */
void ScheduleAstraEngineRestart(void);

#if USE_GNSS
/**
  * @brief  GNSS read trigger
  * @param  none
  * @retval none
  */
void GnssReadTrigger(void);

/**
  * @brief  GNSS read timer
  * @param  TimerIntv: 32bit unsigned integer
  * @param  bImmediateTrigger: 8bit unsigned integer
  * @retval none
  */
void setGnssReadTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger);

#endif //USE_GNSS

#if USE_BLE
/**
  * @brief  Manage BLE
  * @param  none
  * @retval none
  */
void ManageBle(void);
#endif //USE_BLE

#if USE_MEMORY
/**
  * @brief  Manage NFC
  * @param  none
  * @retval none
  */
void ManageNFC(void);
#endif //USE_MEMORY

#if USE_USB

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t Astra_STOP_CDC_Receive_FS(uint8_t* Buf, uint32_t *Len);

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t Astra_WL_CDC_Receive_FS(uint8_t* Buf, uint32_t *Len);

#endif //USE_USB

/* USER CODE BEGIN CLI_PROTO */

/* Add your additional function protos here */

/* USER CODE END CLI_PROTO */

/**
* @}
*/

/** @addtogroup ASTRA_SYSTEM_MANAGEMENT_PRIVATE_VARIABLES ASTRA System management - Private variables
  * @brief Private variables
  * @{
 */

/* Private variables ---------------------------------------------------------*/

ButtonCallbacks_t ButtonCallbacks = {0};
static const char *RebootReasonNames[] = {"Normal Reboot", "DFU Reboot", NULL};
static const char *ULPwakeNames[] = {"Only BLE wake", "Only ACC wake", "BLE and ACC wake", "Button wake", NULL};
static const char *EngineUcNames[] = {"FLEET_MNG", "LIVESTOCK_MON", "GOODS_MONITORING", "LOGISTICS", "CUSTOM", NULL};
static const char *VerbLevelNames[]={"INFO", "VERBOSE","CRITICAL","ERROR","TERMINAL","DISABLED",NULL};
static const char *LoRaRegionNames[]={"AS923", "AU915","CN470","CN779","EU433","EU868","KR920","IN865","US915","RU864",NULL};

static const uint32_t EngineUcValues[] = {FLEET_MNG_ENG, LIVESTOCK_MON_ENG, GOODS_MONITORING_ENG, LOGISTICS_ENG, CUSTOM_ENG, NULL};

const xCmdStruct_t xCmdStructVect[] = {
  { "?fwversion",         "View fw info",                                 ProcDbgGetFwVersion,            0, DISABLE_PRINT },
  { "?hwversion",         "View hw info",                                 ProcDbgGetHwVersion,            0, DISABLE_PRINT },
  { "?blemacaddr",        "View BLE mac addr",                            ProcDbgGetBleMacAddr,           0, DISABLE_PRINT },
  { "?loraparams",        "View LoRa params",                             ProcDbgGetLoraParams,           0, DISABLE_PRINT },
  { "?verbosity",         "View verbosity level",                         ProcDbgGetVerbosity,            0, ENABLE_PRINT },
  { "!verbosity",         "Set verbosity level",                          ProcDbgSetVerbosity,            0, ENABLE_PRINT },
  { "!sysreset",          "System reset",                                 ProcDbgSysReset,                0, DISABLE_PRINT },
  { "!sysdfu",            "System DFU mode",                              ProcDbgSysDfu,                  0, DISABLE_PRINT },
  { "?sysblver",          "System Bootloader version",                    ProcDbgSysGetBlVer,             0, DISABLE_PRINT },
  { "!syswlprg",          "Prog STM32WL",                                 ProcDbgSysWlPrg,                0, DISABLE_PRINT },
  { "!fwdwlCRLF_",        "Send cmd CR+LF to STM32WL",                    ProcDbgFwdWlNewline,            0, ENABLE_PRINT },
  { "!fwdwl",             "Send cmd to STM32WL",                          ProcDbgFwdWl,                   0, ENABLE_PRINT },
  { "!wlgetresp",         "Get STM32WL response",                         ProcDbgWlGetResponse,           0, ENABLE_PRINT },
  { "!wltransp",          "STM32WL uart transparent",                     ProcDbgWlTransparent,           0, ENABLE_PRINT },
  { "?gnssstatus",        "GNSS status report",                           ProcDbgSetGnssStatus,           0, DISABLE_PRINT },
  { "!gnssreport",        "GNSS enable detailed report",                  ProcDbgSetGnssAppConfig,        0, DISABLE_PRINT },
  { "!gnssnmea",          "GNSS set NMEA printout",                       ProcDbgSetGnssNmeaConfig,       0, DISABLE_PRINT },
  { "!stop",              "STOP fw execution",                            ProcDbgStopFw,                  0, ENABLE_PRINT },
  { "!loraconf",          "Set LoRa fw configuration",                    ProcDbgSetLoraAppConfig,        0, ENABLE_PRINT },
  { "?loraconf",          "Get LoRa fw configuration",                    ProcDbgGetLoraAppConfig,        0, ENABLE_PRINT },
  { "!lponmemsevent",     "Low power conf for MEMS event",                ProcDbgSetMemsEvConfig,         0, ENABLE_PRINT },
  { "!debug",             "Set the debug mode",                           ProcDbgSetDebugMode,            0, ENABLE_PRINT },
  { "?debug",             "Get the debug mode",                           ProcDbgGetDebugMode,            0, ENABLE_PRINT },
  { "!sysrun",            "Set system state to run",                      ProcDbgSystemStateRun,          0, DISABLE_PRINT },
  { "!syslp",             "Set system state to low power",                ProcDbgSystemStateLowPower,     0, DISABLE_PRINT },
  { "!usecase",           "Set ATR use case",                             ProcDbgSetUseCase,              0, DISABLE_PRINT },
  { "?",                  "View help",                                    ProcDbgHelp,                    0, DISABLE_PRINT },

/* USER CODE BEGIN CLI_COMMANDS */

/* Add your additional commands here */

/* USER CODE END CLI_COMMANDS */
};

/**
* @}
*/

/** @addtogroup ASTRA_SYSTEM_MANAGEMENT_Private_Functions ASTRA System management - Private Functions
  * @brief Private functions
  * @{
 */

/* Private function ----------------------------------------------------------*/

void LoraJoinTimeout(void)
{
  if(PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS)
  {
    PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS = 0;
  }
}

void LoraJoinTrigger(void)
{
  PlatformStatus.b.LORA_JOIN_TRIGGERED = 1;
}

void LoraSendTrigger(void)
{
  PlatformStatus.b.LORA_SEND_TRIGGERED = 1;
}

void BleMngTrigger(void)
{
  PlatformStatus.b.BLE_MNG_TRIGGERED = 1;
}

void StatusUpdTrigger(void)
{
  PlatformStatus.b.STATUS_UPD_TRIGGERED = 1;
}

void NFCDataSavingTrigger(void)
{
  PlatformStatus.b.NFC_DATA_TRIGGERED = 1;
}

#if USE_GNSS
void GnssReadTrigger(void)
{
  PlatformStatus.b.GNSS_READ_TRIGGERED = 1;
}
#endif //USE_GNSS

static uint32_t nSmManagerTurns = 0;
static uint32_t nDecimationLoraJoin = 0;
static uint32_t nLoraJoinTimeoutTurns = 0;
static uint32_t nDecimationLoraSend = 0;
static uint32_t nDecimationBleMng = 0;
static uint32_t nDecimationStatusUpd = 0;
static uint32_t nDecimationNFCDataSaving = 0;

#if USE_GNSS
static uint32_t nDecimationGnssRead = 0;
#endif //USE_GNSS

void unsetLoraJoinTimer(void)
{
  nDecimationLoraJoin = 0xFFFFFFFF;
}

void setLoraJoinTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger)
{
  nDecimationLoraJoin = (TimerIntv)/PlatformStatus.s.SYS_PROC_INTERVAL;
  if(nDecimationLoraJoin < 1)nDecimationLoraJoin = 1;
  nLoraJoinTimeoutTurns = ((15*1000)/PlatformStatus.s.SYS_PROC_INTERVAL); //15seconds join timeout
  if(bImmediateTrigger)LoraJoinTrigger();
}

void setLoraSendTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger)
{
  nDecimationLoraSend = (TimerIntv)/PlatformStatus.s.SYS_PROC_INTERVAL;
  if(nDecimationLoraSend < 1)nDecimationLoraSend = 1;
  if(bImmediateTrigger)LoraSendTrigger();
}

void setBleMngTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger)
{
  nDecimationBleMng = (TimerIntv)/PlatformStatus.s.SYS_PROC_INTERVAL;
  if(nDecimationBleMng < 1)nDecimationBleMng = 1;
  if(bImmediateTrigger)BleMngTrigger();
}

void setStatusUpdTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger)
{
  nDecimationStatusUpd = (TimerIntv)/PlatformStatus.s.SYS_PROC_INTERVAL;
  if(nDecimationStatusUpd < 1)nDecimationStatusUpd = 1;
  if(bImmediateTrigger)StatusUpdTrigger();
}

void setNFCDataSavingTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger)
{
  nDecimationNFCDataSaving = (TimerIntv)/PlatformStatus.s.SYS_PROC_INTERVAL;
  if(nDecimationNFCDataSaving < 1)nDecimationNFCDataSaving = 1;
  if(bImmediateTrigger)NFCDataSavingTrigger();
}


#if USE_GNSS
void setGnssReadTimer(uint32_t TimerIntv, uint8_t bImmediateTrigger)
{
  nDecimationGnssRead = (TimerIntv)/PlatformStatus.s.SYS_PROC_INTERVAL;
  if(nDecimationGnssRead < 1)nDecimationGnssRead = 1;
  if(bImmediateTrigger)GnssReadTrigger();
}
#endif //USE_GNSS

uint32_t nDecimationLedConn = 10;
uint32_t nDecimationLedDisconn = 20;
void manageLeds(void)
{
  if(!AstraSysIsStateRunning())
  {
    AstraLedColor(ASTRA_LED_OFF);
  }
  else if(PlatformStatus.b.BLE_CONNECTED)
  {
    if(nSmManagerTurns % nDecimationLedConn == 0)
    {
      AstraLedColor(ASTRA_LED_COLOR_GREEN);
    }
    else if(nSmManagerTurns % nDecimationLedConn == 1)
    {
      AstraLedColor(ASTRA_LED_OFF);
    }
  }
  else
  {
    if(nSmManagerTurns % nDecimationLedDisconn == 0)
    {
      AstraLedColor(ASTRA_LED_COLOR_GREEN);
    }
    else if(nSmManagerTurns % nDecimationLedDisconn == 1)
    {
      AstraLedColor(ASTRA_LED_OFF);
    }
  }
}

void manageDecimationTasks(void)
{
  nSmManagerTurns++;
  if(nSmManagerTurns % nDecimationLoraJoin == 0)LoraJoinTrigger();
  if(nSmManagerTurns % nDecimationLoraJoin == nLoraJoinTimeoutTurns)LoraJoinTimeout();
  if(nSmManagerTurns % nDecimationLoraSend == 0)LoraSendTrigger();
  if(nSmManagerTurns % nDecimationBleMng == 0)BleMngTrigger();
  if(nSmManagerTurns % nDecimationStatusUpd == 0)StatusUpdTrigger();
  if(nSmManagerTurns % nDecimationNFCDataSaving == 0)NFCDataSavingTrigger();
#if USE_GNSS
  if(nSmManagerTurns % nDecimationGnssRead == 0)GnssReadTrigger();
#endif //USE_GNSS
}

#ifdef __SM_CONF_H
void AstraSM_Trigger()
{
  SM_NormalTrigger(APP_EV_BP);
}
#endif //__SM_CONF_H

void AstraSysInit(void)
{
#if USE_BTN1_ANTI_TAMPER
  RegisterButtonCallback(BTN1_TAMPER_GLITCH, AstraTamper);
  RegisterButtonCallback(BTN1_TAMPER_ACTIVATED, AstraNothing);
  RegisterButtonCallback(BTN1_TAMPER_RESTORED, AstraNothing);
#else//!USE_BTN1_ANTI_TAMPER
  RegisterButtonCallback(BTN1_SHORT, AstraSendData);
  RegisterButtonCallback(BTN1_LONG, AstraShutdownCommand);
#endif//!USE_BTN1_ANTI_TAMPER
#ifndef __SM_CONF_H
  RegisterButtonCallback(BTN2_SHORT, AstraSwitchState);
#else //__SM_CONF_H
  RegisterButtonCallback(BTN2_SHORT, AstraSM_Trigger);
#endif //__SM_CONF_H
  RegisterButtonCallback(BTN2_LONG, AstraBoardReboot);

  SystemConfParams.PrintLevelActive = LEVEL_ACTIVE_DEFAULT;

  PlatformStatus.b.BLE_CONNECTED = 0;
  *(uint32_t*)&PlatformStatus.s.STM32_UID[0] = HAL_GetUIDw0();
  *(uint32_t*)&PlatformStatus.s.STM32_UID[4] = HAL_GetUIDw1();
  *(uint32_t*)&PlatformStatus.s.STM32_UID[8] = HAL_GetUIDw2();

  PlatformStatus.b.GNSS_CHECK_NMEA_CONF = ASTRA_GNSS_CHECK_NMEA_CONF;
  
  InitAstraDeltatime();

  if(IsNewFwFlashed())
  {
    const char *newEngineName = NULL;

    for(uint32_t i=0;i<countof(EngineUcValues);i++)
    {
      if(EngineUcValues[i] == ASTRA_ENGINE)
      {
        newEngineName = EngineUcNames[i];
        break;
      }
    }

#if USE_MEMORY
    AstraEEPROM_InitNDEFMessage();
#endif //USE_MEMORY
    
    if(newEngineName!=NULL)
    {
      AstraSetEngine(newEngineName);
    }
    
    AstraSaveDefaultParams();

    ResetNewFwFlags();
  }
  
  AstraLoadParams();

  SystemConfParams.BLE_ADV_INTV_MIN = CFG_FAST_CONN_ADV_INTERVAL_MIN;
  SystemConfParams.BLE_ADV_INTV_MAX = CFG_FAST_CONN_ADV_INTERVAL_MAX;

  PlatformStatus.b.ASTRA_ENGINE_RESTART_PENDING = 1;

#ifndef __SM_CONF_H
  PlatformStatus.s.xCurrentState = State_Run;
#endif //__SM_CONF_H
}

uint8_t AstraSysIsStateRunning(void)
{
  return (PlatformStatus.s.xCurrentState == State_Run)?1:0;
}

#if USE_BLE
void AstraCustomEnvNotifyActivate(uint8_t activation)
{
  if(activation)
  {
    PlatformStatus.b.BLE_ENV_SENS_NOTIF_ENABLED = 1;
  }
  else
  {
    PlatformStatus.b.BLE_ENV_SENS_NOTIF_ENABLED = 0;
  }
}

void AstraCustomInertialNotifyActivate(uint8_t activation)
{
  if(activation)
  {
    PlatformStatus.b.BLE_INERTIAL_NOTIF_ENABLED = 1;
  }
  else
  {
    PlatformStatus.b.BLE_INERTIAL_NOTIF_ENABLED = 0;
  }
}

void AstraCustomBatteryNotifyActivate(uint8_t activation)
{
  if(activation)
  {
    PlatformStatus.b.BLE_BATTERY_NOTIF_ENABLED = 1;
  }
  else
  {
    PlatformStatus.b.BLE_BATTERY_NOTIF_ENABLED = 0;
  }
}

void AstraCustomGnssNotifyActivate(uint8_t activation)
{
  if(activation)
  {
    PlatformStatus.b.BLE_GNSS_NOTIF_ENABLED = 1;
  }
  else
  {
    PlatformStatus.b.BLE_GNSS_NOTIF_ENABLED = 0;
  }
}

void ManageBle(void)
{
  if(BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    PlatformStatus.b.BLE_TRACE = 1;
  }
  else
  {
    PlatformStatus.b.BLE_TRACE = 0;
  }

  if(PlatformStatus.b.BLE_BATTERY_NOTIF_ENABLED)
  {
    uint16_t CurrentConsumptionInfoNotAvailable = 0x8000;
    if(NEW_DATA_IS_SET(AstraEngData.f.POW_MAN_NEW_DATA, ble))
    {
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);
      BLE_BatteryUpdate( AstraGetBatteryLevelPerc()/10, AstraEngData.d.batteryVoltage, CurrentConsumptionInfoNotAvailable, AstraGetBatteryStatus());
      AstraLedToggle(ASTRA_LED_OFF);
      NEW_DATA_CLEAR(AstraEngData.f.POW_MAN_NEW_DATA, ble);
    }
  }

  if(PlatformStatus.b.BLE_ENV_SENS_NOTIF_ENABLED)
  { 
    if(NEW_DATA_IS_SET(AstraEngData.f.ENVIRONMENTAL_NEW_DATA, ble))
    {
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);
      BLE_EnvironmentalUpdate((int32_t)(100*AstraEngData.d.PressValue), (uint16_t)(10*AstraEngData.d.HumValue), (int16_t)(10*AstraEngData.d.TempValue), 0);
      AstraLedToggle(ASTRA_LED_OFF);
      NEW_DATA_CLEAR(AstraEngData.f.ENVIRONMENTAL_NEW_DATA, ble);
    }
  }

  if(PlatformStatus.b.BLE_INERTIAL_NOTIF_ENABLED)
  {
    if(NEW_DATA_IS_SET(AstraEngData.f.INERTIAL_NEW_DATA, ble))
    {
      BLE_MANAGER_INERTIAL_Axes_t acc;
      acc.x = AstraEngData.d.AccValue.x;
      acc.y = AstraEngData.d.AccValue.y;
      acc.z = AstraEngData.d.AccValue.z;
      BLE_MANAGER_INERTIAL_Axes_t gyro;
      gyro.x = AstraEngData.d.GyrValue.x;
      gyro.y = AstraEngData.d.GyrValue.y;
      gyro.z = AstraEngData.d.GyrValue.z;
      BLE_MANAGER_INERTIAL_Axes_t mag = { 0 };
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);
      BLE_AccGyroMagUpdate(&acc, &gyro, &mag);
      AstraLedToggle(ASTRA_LED_OFF);
      NEW_DATA_CLEAR(AstraEngData.f.INERTIAL_NEW_DATA, ble);
    }
  }

  if(PlatformStatus.b.BLE_GNSS_NOTIF_ENABLED)
  { 
    if(NEW_DATA_IS_SET(AstraEngData.f.GNSS_NEW_DATA, ble))
    {
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);
      BLE_GnssUpdate((int32_t)(10000000*AstraEngData.d.gnss_latitude), (int32_t)(10000000*AstraEngData.d.gnss_longitude), (uint32_t)(1000*AstraEngData.d.gnss_altitude), (AstraEngData.d.gnss_sats&0xFF), AstraEngData.d.gnss_fix_validity);
      AstraLedToggle(ASTRA_LED_OFF);
      NEW_DATA_CLEAR(AstraEngData.f.GNSS_NEW_DATA, ble);
    }
  }
}
#endif //USE_BLE

#if USE_MEMORY
void ManageNFC(void)
{
  SmarTagAppProcess();
}
#endif  //USE_MEMORY

void switchdebugserialport()
{
  if(PlatformStatus.b.GNSS_VERSION_SHOWED)
  {
    static uint8_t bSwSerPortRunOnce = 1;
    if(bSwSerPortRunOnce)
    {
      bSwSerPortRunOnce=0;

      GPIO_InitTypeDef GPIO_InitStruct;

      //P.B6 TX input
      __HAL_RCC_GPIOB_CLK_ENABLE();

      /**USART1 GPIO Configuration
      PB7     ------> USART1_RX
      PB6     ------> USART1_TX
      */
      GPIO_InitStruct.Pin = GPIO_PIN_6;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      //PA.9 TX AF
      GPIO_InitStruct.Pin = GPIO_PIN_9;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      extern UART_HandleTypeDef huart1;
      HAL_UART_Transmit(&H_DBG_HUART, (uint8_t *)"\r\nUART CONFIGURED\r\n", 19, 0x1000);

      PlatformStatus.b.UART_TRACE = 1;
    }
  }
}

void manageAlgorithms(void)
{
  switch(PlatformStatus.s.UC_RUNALGO)
  {
  case ALGO_MOTION_DETECTION:
    //add your code here for motion detection algorithm
    if(NEW_DATA_IS_SET(AstraEngData.f.MEMS_EVENT_NEW_DATA, algo))
    {
      if (SystemConfParams.b.CONF_MEMS_REBOOT)
      {
        if(AstraEngData.d.LPAccInt.SleepStatus == 1)
        {
          AstraUlpSchedule(WAKE_SRC_DEFAULT);
        }
        else if(AstraEngData.d.LPAccInt.SleepStatus == 0)
        {
          ClearScheduleAstraSmChangeState();
        }      
      }
      NEW_DATA_CLEAR(AstraEngData.f.MEMS_EVENT_NEW_DATA, algo);
    }
    break;
  case ALGO_GEOFENCE:
    //add your code here for geofence algorithm
    break;
  case ALGO_CUSTOM_PROC:
    //add your code here for custom algorithm
    break;
  case ALGO_DISABLE:
  default:
    break;
  }
}

void manageData(void)
{
  /* New data is available. Process function must be added here */
#if USE_BLE
  if(PlatformStatus.b.BLE_MNG_TRIGGERED)
  {
    PlatformStatus.b.BLE_MNG_TRIGGERED = 0;
    ManageBle();
  }
#endif //USE_BLE

#if USE_MEMORY
//  if(PlatformStatus.b.NFC_DATA_TRIGGERED)
//  {
    ManageNFC();
//    PlatformStatus.b.NFC_DATA_TRIGGERED = 0;
//  }
#endif //USE_MEMORY
  
  /* USER CODE BEGIN manageData */

  /* Add your code here */

  /* USER CODE END manageData */
}

void AstraSetNonConnectable(void)
{
#if USE_BLE
  setNotConnectable();
#endif //USE_BLE
}

uint8_t AstraCheckBleInit(void)
{
#if USE_BLE
  if(BLE_StackValue.BoardName[0] != 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
#else //USE_BLE
  return 0;
#endif //USE_BLE
}

void AstraSysSmManager(void)
{
  {
#if 0
    static uint8_t old_s = 0;
    extern RTC_HandleTypeDef hrtc;
    RTC_TimeTypeDef CurTime;
    RTC_DateTypeDef CurDate;
    if(HAL_RTC_GetTime(&hrtc, &CurTime, RTC_FORMAT_BIN) != HAL_OK) {
      PRINTF_VERBOSE("HAL_RTC_GetTime error\r\n");
    }
    else
    {
      
      uint8_t s = CurTime.Seconds;
      if(s != old_s)
      {
        if (HAL_RTC_GetDate(&hrtc, &CurDate, RTC_FORMAT_BIN) != HAL_OK) {
          PRINTF_VERBOSE("HAL_RTC_GetDate error\r\n");
        }
        struct tm currTime;
        currTime.tm_year = CurDate.Year;
        currTime.tm_mday = CurDate.Date;
        currTime.tm_mon  = CurDate.Month;
        currTime.tm_wday = CurDate.WeekDay;
        currTime.tm_hour = CurTime.Hours;
        currTime.tm_min  = CurTime.Minutes;
        currTime.tm_sec  = CurTime.Seconds;
        currTime.tm_isdst = -1;
        
        PRINTF_VERBOSE("Current RTC time TEST: ");
        PRINTF_VERBOSE("%.2d/%.2d/%.2d - ", currTime.tm_mon, currTime.tm_mday, currTime.tm_year );
        PRINTF_VERBOSE("%.2d:%.2d:%.2d\r\n", currTime.tm_hour, currTime.tm_min, currTime.tm_sec );
        
        old_s = s;
      }
    }
#endif //0
    
#if 0
    static uint8_t old_s1 = 0;
    time_t AstraTimeStamp = AstraGetTimeStamp();
    struct tm *currTime2;
    currTime2 = localtime(&AstraTimeStamp);
    uint8_t s = currTime2->tm_sec;
    if(s != old_s1)
    {
      PRINTF_VERBOSE("Current time TEST: ");
      PRINTF_VERBOSE("%.2d/%.2d/%.2d - ", currTime2->tm_mon, currTime2->tm_mday, currTime2->tm_year );
      PRINTF_VERBOSE("%.2d:%.2d:%.2d\r\n", currTime2->tm_hour, currTime2->tm_min, currTime2->tm_sec );
      old_s1 = s;
    }
    
#endif //0
  }
  
#if USE_UART
  switchdebugserialport();
#endif //USE_UART

  manageDecimationTasks();

  manageLeds();

#if USE_BLE
  if(BLE_StackValue.BoardName[0] != 0)
  {
    static uint8_t bBleGetVerRunOnce = 1;
    if(bBleGetVerRunOnce)
    {
      bBleGetVerRunOnce=0;
      uint32_t uWB_CM0_FwVer=0;
      uint32_t uWB_CM0_FusVer=0;

      AstraBleGetCM0ver(&uWB_CM0_FwVer, &uWB_CM0_FusVer);

      PlatformStatus.s.STM32WB_CM0_VER = uWB_CM0_FwVer;
      PlatformStatus.s.STM32WB_CM0_FUS = uWB_CM0_FusVer;
    }
  }
#endif //USE_BLE

  manageAlgorithms();

  manageData();

  if(PlatformStatus.b.STATUS_UPD_TRIGGERED)
  {
    PlatformStatus.b.STATUS_UPD_TRIGGERED = 0;
    UpdateSystemStatus();
  }

  if(PlatformStatus.b.ASTRA_ENGINE_RESTART_PENDING == 1)
  {
    PRINTF_VERBOSE("ASTRA_ENGINE_RESTART_PENDING\r\n");
    PlatformStatus.s.SYS_PROC_INTERVAL = EXECUTION_FREQUENCY;

    AstraEngineParams_t newparams = { 0 };
    GetAstraEngineParams(&newparams);
    PlatformStatus.s.ASTRA_ENGINE_SEL = newparams.ASTRA_ENGINE_SEL;
#if USE_STM32WL
    PlatformStatus.b.APP_LORA_ENABLED = 1;
#endif //USE_STM32WL
    PlatformStatus.s.LORA_SEND_INTERVAL = newparams.LORA_SENDFREQ;
    PlatformStatus.s.BLE_SEND_INTERVAL = newparams.BLE_SENDFREQ;
#if USE_MEMORY
    PlatformStatus.s.NFC_SAVING_INTERVAL = newparams.NFC_DATASAVINGINTV;
#endif //USE_MEMORY
    
#if USE_GNSS
    PlatformStatus.b.GNSS_ENABLED = (newparams.b.INDOORONLY)?0:1;
#endif //USE_GNSS
#if USE_SENSORS
    PlatformStatus.b.SENSORS_ENABLED = 1;
#endif //USE_SENSORS
    PlatformStatus.s.UC_RUNALGO = newparams.RUNALGO;
    PlatformStatus.s.STATUS_UPD_INTERVAL = 60*1000;

    setLoraSendTimer(PlatformStatus.s.LORA_SEND_INTERVAL, 1);

    setLoraJoinTimer(20*1000, 0);

    setBleMngTimer(PlatformStatus.s.BLE_SEND_INTERVAL, 1);

    setStatusUpdTimer(PlatformStatus.s.STATUS_UPD_INTERVAL, 1);
    
    setNFCDataSavingTimer(PlatformStatus.s.NFC_SAVING_INTERVAL, 1);

#if USE_GNSS
    setGnssReadTimer(1000, 0);
#endif //USE_GNSS

    PlatformStatus.b.ASTRA_ENGINE_RESTART_PENDING = 0;
  }

  if(PlatformStatus.b.ASTRA_CHANGE_STATE_PENDING == 1)
  {
    if((PlatformStatus.b.BLE_CONNECTED) == 0 &&
        (PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS == 0) &&
        (PlatformStatus.b.ADC_INIZIALIZED && (AstraEngData.d.USBVoltage < 4000)))
    {
      switch (PlatformStatus.s.NewState)
      {
        case SmChangeState_lpBleWake: //* reboot with only BLE wake up
        {
#if USE_SENSORS
          sensor_lp_algo_de_init();
          sensor_lp_disable();
#endif //USE_SENSORS
          
#if USE_ASTRA_MAINBOARD_BSP
          custom_astra_pwr_disable();
#endif //USE_ASTRA_MAINBOARD_BSP
#if USE_PWRMNG
          custom_sob_v_reg1_out_low_value();
          custom_sob_v_reg2_off();
#endif //USE_PWRMNG
#if USE_STSAFE
          custom_stsafe_reset(0);
#endif
          
          AstraSetLoraProcessing(0);
          
          PRINTF_VERBOSE("Reboot and go to sleep with BLE wake up...\r\n");
          HAL_Delay(500);
//          BoardRebootWithReason(BOOT_REASON_OPT2); //(BLE)
          SM_NormalTrigger(APP_EV_LP);
          break;
        }

        case SmChangeState_lpBleAccWake: //* reboot with BLE and LP ACC wake up
        {
#if USE_SENSORS
          sensor_lp_wakeup_detect_init();
          sensor_lp_lowPerformance();
#endif //USE_SENSORS
          
#if USE_ASTRA_MAINBOARD_BSP
          custom_astra_pwr_disable();
#endif //USE_ASTRA_MAINBOARD_BSP
#if USE_PWRMNG
          custom_sob_v_reg1_out_low_value();
          custom_sob_v_reg2_off();
#endif //USE_PWRMNG
#if USE_STSAFE
          custom_stsafe_reset(0);
#endif
          
          AstraSetLoraProcessing(0);
          
          PRINTF_VERBOSE("Reboot and go to sleep with BLE wake up...\r\n");
          HAL_Delay(500);
//          BoardRebootWithReason(BOOT_REASON_OPT2); //(BLE)
          SM_NormalTrigger(APP_EV_LP);
          break;
        }

        case SmChangeState_lpAccWake: //* reboot with only LP ACC wake up
        {
#if USE_SENSORS
          sensor_lp_wakeup_detect_init();
          sensor_lp_lowPerformance();
          PRINTF_VERBOSE("Reboot and go to sleep with LP ACC wake up...\r\n");
#endif //USE_SENSORS
          HAL_Delay(500);
          // BoardRebootWithReason(BOOT_REASON_OPT1); //(LP ACC)
          SM_NormalTrigger(APP_EV_LP);
          break;
        }

        case SmChangeState_lpButtonWake: //* reboot with only LP ACC wake up
        {
#if USE_SENSORS
          sensor_lp_algo_de_init();
          sensor_lp_disable();
#endif //USE_SENSORS
          
#if USE_ASTRA_MAINBOARD_BSP
          custom_astra_pwr_disable();
#endif //USE_ASTRA_MAINBOARD_BSP
#if USE_PWRMNG
          custom_sob_v_reg1_out_low_value();
          custom_sob_v_reg2_off();
#endif //USE_PWRMNG
#if USE_STSAFE
          custom_stsafe_reset(0);
#endif
          
          AstraSetLoraProcessing(0);
          
          PRINTF_VERBOSE("Reboot and go to sleep with Buttons wake up...\r\n");
          HAL_Delay(500);
//          BoardRebootWithReason(BOOT_REASON_OPT2); //(BLE)
          SM_NormalTrigger(APP_EV_LP);
          break;
        }


      default:
        break;
      }
      PlatformStatus.b.ASTRA_CHANGE_STATE_PENDING = 0;
    }
  }

#ifndef __SM_CONF_H

  if( PlatformStatus.s.ASTRA_ENGINE_SEL == CUSTOM_ENG )
  {
    if(PlatformStatus.s.xNextState == State_Run)
    {
      AstraPrepareHwRun();
    }
    else if(PlatformStatus.s.xNextState == State_LowPower)
    {
      AstraPrepareHwLp();
    }
    else if(PlatformStatus.s.xNextState != State_INVALID)
    {
      PRINTF_VERBOSE("xNextState not managed\r\n");
      PlatformStatus.s.xNextState = State_INVALID;
    }
  }
  else if(PlatformStatus.s.xNextState != State_INVALID)
  {
    PRINTF_VERBOSE("State machine is managed only with USE CASE = CUSTOM\r\n");
    PlatformStatus.s.xNextState = State_INVALID;
  }

#endif //__SM_CONF_H
}

void BoardReboot(void)
{
  HAL_NVIC_SystemReset();
}

void AstraTamper(void)
{
  /* To be implemented */
}

void AstraNothing(void)
{
  /* To be implemented */
}

void AstraSendData()
{
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("TRIGGER SEND DATA\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
  AstraLedColor(ASTRA_LED_COLOR_BLUE);
  HAL_Delay(200);
  AstraLedColor(ASTRA_LED_OFF);
  PRINTF_VERBOSE("TRIGGER LORA SEND\r\n");
  LoraSendTrigger();
  PRINTF_VERBOSE("TRIGGER NFC SEND\r\n");
  NFCDataSavingTrigger();
}

void AstraPrepareHwRun()
{
  PRINTF_VERBOSE("AstraPrepareHwRun\r\n");
  PlatformStatus.s.xCurrentState = State_Run;

  AstraLedColor(ASTRA_LED_COLOR_BLUE);
  HAL_Delay(1000);
  AstraLedColor(ASTRA_LED_OFF);

  custom_sob_v_reg1_out_high_value();
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_pwr_enable();
#endif //USE_ASTRA_MAINBOARD_BSP
#if USE_PWRMNG
  custom_sob_v_reg2_on();
#if USE_STSAFE
  custom_stsafe_reset(1);
#endif /* USE_STSAFE */
#endif //USE_PWRMNG
}

void AstraPrepareHwLp()
{
  PRINTF_VERBOSE("AstraPrepareHwLp\r\n");
  PlatformStatus.s.xCurrentState = State_LowPower;

  custom_sob_power_init();
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_pwr_init();
#endif //USE_ASTRA_MAINBOARD_BSP

  AstraLedColor(ASTRA_LED_COLOR_WHITE);
  HAL_Delay(500);
  AstraLedColor(ASTRA_LED_OFF);

#if USE_PWRMNG
  custom_sob_v_reg1_out_high_value();
#endif //USE_PWRMNG

#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_pwr_enable();
#endif //USE_ASTRA_MAINBOARD_BSP

#if USE_MEMORY
  memory_init();
#endif //USE_MEMORY

  AstraEngineParams_t newparams = { 0 };
  GetAstraEngineParams(&newparams);
  PlatformStatus.s.ASTRA_ENGINE_SEL = newparams.ASTRA_ENGINE_SEL;
  AstraLoadParams();
  SystemConfParams.BLE_ADV_INTV_MIN = CFG_LP_CONN_ADV_INTERVAL_MIN;
  SystemConfParams.BLE_ADV_INTV_MAX = CFG_LP_CONN_ADV_INTERVAL_MAX;

#if USE_MEMORY
  memory_deinit();
#endif //USE_MEMORY

  custom_sob_v_reg1_out_low_value();
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_pwr_disable();
#endif //USE_ASTRA_MAINBOARD_BSP
#if USE_PWRMNG
  custom_sob_v_reg2_off();
#if USE_STSAFE
  custom_stsafe_reset(0);
#endif
#endif //USE_PWRMNG

  MX_RTC_Init();
  MX_IPCC_Init();
  astra_ble_init();  

#if USE_ASTRA_MAINBOARD_BSP
  uint32_t WakeReason = AstraGetWakeReason();

  if(WakeReason == SmChangeState_lpAccWake || WakeReason == SmChangeState_lpBleAccWake)
  {
    custom_astra_lp_acc_interruptpin_init();
    if(custom_astra_lp_acc_intpin_getstate() != 0)
    {
      PlatformStatus.b.MEMS_INT1_EVENT = 1;
    }
  }
#endif //USE_ASTRA_MAINBOARD_BSP

#if USE_ASTRA_MAINBOARD_BSP
  Astra_ButtonInit();
#endif //USE_ASTRA_MAINBOARD_BSP
  
#if USE_STM32WL
  custom_sob_stm32wl_init();
#endif //USE_STM32WL

  APPD_Init();
}

void AstraShutdownCommand()
{
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("SYSTEM SHUTDOWN, disconnect USB\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);

  for(uint8_t i=0;i<3;i++)
  {
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);
    AstraLedColor(ASTRA_LED_OFF);
    HAL_Delay(500);
  }
  SystemShutdown();
#if USE_ASTRA_MAINBOARD_BSP
  HAL_Delay(1000);
  PRINTF_TERMINAL("Or button push to restart\r\n");
  HAL_Delay(1000);
  while(1)
  {
    AstraLedToggle(ASTRA_LED_COLOR_RED);
    HAL_Delay(200);
    AstraLedToggle(ASTRA_LED_COLOR_RED);
    HAL_Delay(200);
    if(custom_astra_btn_get_state(ASTRA_BTN1) == ENABLE)
    {
      AstraBoardReboot();
    }
  }
#endif //USE_ASTRA_MAINBOARD_BSP

}

#ifndef __SM_CONF_H
void AstraSwitchState(void)
{
  if(PlatformStatus.s.xCurrentState == State_Run)
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM GOING LOW POWER\r\n");
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    PlatformStatus.s.xNextState = State_LowPower;
  }
  else
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM GOING FULL RUN\r\n");
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    PlatformStatus.s.xNextState = State_Run;
  }
}
#endif //__SM_CONF_H

void AstraBoardReboot(void)
{
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("SYSTEM REBOOT\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
  AstraLedColor(ASTRA_LED_COLOR_BLUE);
  HAL_Delay(500);

  BoardReboot();
}

void AstraBoardUsbDfu(void)
{
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("SYSTEM REBOOT in USB DFU mode\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
  AstraLedColor(ASTRA_LED_COLOR_BLUE);
  HAL_Delay(500);

  BoardRebootWithReason(BOOT_REASON_DFU);
}

void Button1Pushed_cb(void)
{
  if(ButtonCallbacks.Button1ShortCb)ButtonCallbacks.Button1ShortCb();
}

void Button1LongPushed_cb(void)
{
  if(ButtonCallbacks.Button1LongCb)ButtonCallbacks.Button1LongCb();
}

void Button2Pushed_cb(void)
{
  if(ButtonCallbacks.Button2ShortCb)ButtonCallbacks.Button2ShortCb();
}

void Button2LongPushed_cb(void)
{
  if(ButtonCallbacks.Button2LongCb)ButtonCallbacks.Button2LongCb();
}

#if USE_BTN1_ANTI_TAMPER
void Button1TamperGlitch_cb(void)
{
  if(ButtonCallbacks.Button1TamperGlitchCb)ButtonCallbacks.Button1TamperGlitchCb();
}

void Button1TamperActivated_cb(void)
{
  if(ButtonCallbacks.Button1TamperActivatedCb)ButtonCallbacks.Button1TamperActivatedCb();
}

void Button1TamperRestored_cb(void)
{
  if(ButtonCallbacks.Button1TamperRestoredCb)ButtonCallbacks.Button1TamperRestoredCb();
}
#endif //USE_BTN1_ANTI_TAMPER

void RegisterButtonCallback(Button_t button, pBtnCb_t BtnCb)
{
  switch(button)
  {
  case BTN1_SHORT:
    ButtonCallbacks.Button1ShortCb = BtnCb;
    break;
  case BTN1_LONG:
    ButtonCallbacks.Button1LongCb = BtnCb;
    break;
  case BTN2_SHORT:
    ButtonCallbacks.Button2ShortCb = BtnCb;
    break;
  case BTN2_LONG:
    ButtonCallbacks.Button2LongCb = BtnCb;
    break;
#if USE_BTN1_ANTI_TAMPER
  case BTN1_TAMPER_GLITCH:
    ButtonCallbacks.Button1TamperGlitchCb = BtnCb;
    break;
  case BTN1_TAMPER_ACTIVATED:
    ButtonCallbacks.Button1TamperActivatedCb = BtnCb;
    break;
  case BTN1_TAMPER_RESTORED:
    ButtonCallbacks.Button1TamperRestoredCb = BtnCb;
    break;
#endif //USE_BTN1_ANTI_TAMPER
  default:
    break;
  }
}

char const **AstraGetEngineNames(void)
{
  return &EngineUcNames[0];
}

uint32_t AstraGetEngineEnum(void)
{
  uint32_t ret = 0xDEADBEEF; // BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN
  for(uint8_t i=0; i<countof(EngineUcNames); i++)
  {
    if(0 == strcmp(EngineUcNames[i], AstraGetUseCaseString()))
    {
      ret = i;
    }
  }
  return ret;
}

char *AstraGetUseCaseString(void)
{
  if(PlatformStatus.s.ASTRA_ENGINE_SEL == GOODS_MONITORING_ENG)
  {
    return "GOODS_MONITORING";
  }
  else if(PlatformStatus.s.ASTRA_ENGINE_SEL == FLEET_MNG_ENG)
  {
    return "FLEET_MNG";
  }
  else if(PlatformStatus.s.ASTRA_ENGINE_SEL == LIVESTOCK_MON_ENG)
  {
    return "LIVESTOCK_MON";
  }
  else if(PlatformStatus.s.ASTRA_ENGINE_SEL == LOGISTICS_ENG)
  {
    return "LOGISTICS";
  }
  else //if(PlatformStatus.s.ASTRA_ENGINE_SEL == CUSTOM_ENG)
  {
    return "CUSTOM";
  }
}

void PrintSystemInfo(void)
{
  PRINTF_TERMINAL("\r\nUSE CASE = %s\r\n", AstraGetUseCaseString());

  PRINTF_TERMINAL("SYSTEM HW CONFIG:\r\n");
  PRINTF_TERMINAL("USE_SENSORS = %d\r\n", USE_SENSORS);
  PRINTF_TERMINAL("USE_GNSS = %d\r\n", USE_GNSS);
  PRINTF_TERMINAL("USE_BLE = %d\r\n", USE_BLE);
  PRINTF_TERMINAL("USE_STM32WL = %d\r\n", USE_STM32WL);
  PRINTF_TERMINAL("USE_STSAFE = %d\r\n", USE_STSAFE);
  PRINTF_TERMINAL("USE_USB = %d\r\n", USE_USB);
  PRINTF_TERMINAL("USE_MEMORY = %d\r\n", USE_MEMORY);
  PRINTF_TERMINAL("USE_ASTRA_MAINBOARD_BSP = %d\r\n", USE_ASTRA_MAINBOARD_BSP);
  PRINTF_TERMINAL("USE_PWRMNG = %d\r\n", USE_PWRMNG);
  PRINTF_TERMINAL("USE_BTN1_ANTI_TAMPER = %d\r\n", USE_BTN1_ANTI_TAMPER);
  PRINTF_TERMINAL("USE_BUZZER = %d\r\n", USE_BUZZER);

  PRINTF_TERMINAL("\r\nSYSTEM FW CONFIG:\r\n");
  PRINTF_TERMINAL("USE_SEQUENCER = %d\r\n", USE_SEQUENCER);

  PRINTF_TERMINAL("\r\nSYSTEM Configuration parameters: SystemConfParams\r\n");
  PRINTF_TERMINAL("PrintLevelActive = %s\r\n", GET_PRINTF_LEVEL_STRING(SystemConfParams.PrintLevelActive));
  PRINTF_TERMINAL("LORA_DR = %d\r\n", SystemConfParams.LORA_DR);
  PRINTF_TERMINAL("LORA_ADR = %d\r\n", SystemConfParams.LORA_ADR);
  PRINTF_TERMINAL("LORA_ACK = %d\r\n", SystemConfParams.LORA_ACK);
  PRINTF_TERMINAL("LORA_REGION = %s\r\n", GET_REGION_STRING(SystemConfParams.LORA_REGION));
  PRINTF_TERMINAL("\r\nSYSTEM Configuration parameters: PlatformStatus.s\r\n");
  PRINTF_TERMINAL("xCurrentState = %d\r\n", PlatformStatus.s.xCurrentState);
  PRINTF_TERMINAL("LORA_SEND_INTERVAL = %d\r\n", PlatformStatus.s.LORA_SEND_INTERVAL);
  PRINTF_TERMINAL("BLE_SEND_INTERVAL = %d\r\n", PlatformStatus.s.BLE_SEND_INTERVAL);
  PRINTF_TERMINAL("nErrorNoAck = %d\r\n", PlatformStatus.s.nErrorNoAck);
  PRINTF_TERMINAL("nErrorWL = %d\r\n", PlatformStatus.s.nErrorWL);
  PRINTF_TERMINAL("TamperInput = %d\r\n", PlatformStatus.s.TamperInput);
  PRINTF_TERMINAL("\r\nSYSTEM Configuration parameters: PlatformStatus.b\r\n");
  PRINTF_TERMINAL("DEBUGGER_CONNECTED = %d\r\n", PlatformStatus.b.DEBUGGER_CONNECTED);
  PRINTF_TERMINAL("GNSS_ENABLED = %d\r\n", PlatformStatus.b.GNSS_ENABLED);
  PRINTF_TERMINAL("GNSS_VERSION_SHOWED = %d\r\n", PlatformStatus.b.GNSS_VERSION_SHOWED);
  PRINTF_TERMINAL("SENSORS_ENABLED = %d\r\n", PlatformStatus.b.SENSORS_ENABLED);
  PRINTF_TERMINAL("USB_RESUMED = %d\r\n", PlatformStatus.b.USB_RESUMED);
  PRINTF_TERMINAL("USB_VCP_OPENED = %d\r\n", PlatformStatus.b.USB_VCP_OPENED);
  PRINTF_TERMINAL("USB_WELCOME_MESSAGE_PENDING = %d\r\n", PlatformStatus.b.USB_WELCOME_MESSAGE_PENDING);
  PRINTF_TERMINAL("USB_DBG = %d\r\n", PlatformStatus.b.USB_DBG);
  PRINTF_TERMINAL("USB_TRACE = %d\r\n", PlatformStatus.b.USB_TRACE);
  PRINTF_TERMINAL("DMA_INIZIALIZED = %d\r\n", PlatformStatus.b.DMA_INIZIALIZED);
  PRINTF_TERMINAL("MEMS_INT1_EVENT = %d\r\n", PlatformStatus.b.MEMS_INT1_EVENT);
  PRINTF_TERMINAL("MEMS_INT2_EVENT = %d\r\n", PlatformStatus.b.MEMS_INT2_EVENT);
  PRINTF_TERMINAL("APP_LORA_ENABLED = %d\r\n", PlatformStatus.b.APP_LORA_JOINED);
  PRINTF_TERMINAL("APP_LORA_JOINED = %d\r\n", PlatformStatus.b.APP_LORA_JOINED);
  PRINTF_TERMINAL("APP_LORA_JOIN_COMPLETED = %d\r\n", PlatformStatus.b.APP_LORA_JOIN_COMPLETED);
  PRINTF_TERMINAL("LORA_ACK_RECEIVED = %d\r\n", PlatformStatus.b.LORA_ACK_RECEIVED);
  PRINTF_TERMINAL("\r\nSYSTEM variables: AstraEngData.d\r\n");
  PRINTF_TERMINAL("gnss_latitude = %f\r\n", AstraEngData.d.gnss_latitude);
  PRINTF_TERMINAL("gnss_longitude = %f\r\n", AstraEngData.d.gnss_longitude);
  PRINTF_TERMINAL("gnss_altitude = %f\r\n", AstraEngData.d.gnss_altitude);
  PRINTF_TERMINAL("gnss_sats = %d\r\n", AstraEngData.d.gnss_sats);
  PRINTF_TERMINAL("gnss_fix_validity = %d\r\n", AstraEngData.d.gnss_fix_validity);
  PRINTF_TERMINAL("AccValue = %d\r\n", AstraEngData.d.AccValue.x, AstraEngData.d.AccValue.y, AstraEngData.d.AccValue.z);
  PRINTF_TERMINAL("LPAccValue = %d\r\n", AstraEngData.d.LPAccValue.x, AstraEngData.d.LPAccValue.y, AstraEngData.d.LPAccValue.z);
  PRINTF_TERMINAL("GyrValue = %d\r\n", AstraEngData.d.GyrValue.x, AstraEngData.d.GyrValue.y, AstraEngData.d.GyrValue.z);
  PRINTF_TERMINAL("PressValue = %f\r\n", AstraEngData.d.PressValue);
  PRINTF_TERMINAL("TempValue = %f\r\n", AstraEngData.d.TempValue);
  PRINTF_TERMINAL("HumValue = %f\r\n", AstraEngData.d.HumValue);
  PRINTF_TERMINAL("LPAccInt = %d\r\n", AstraEngData.d.LPAccInt);
  PRINTF_TERMINAL("AccInt = %d\r\n", AstraEngData.d.AccInt);
  PRINTF_TERMINAL("batteryVoltage = %d\r\n", AstraEngData.d.batteryVoltage);
  PRINTF_TERMINAL("USBVoltage = %d\r\n", AstraEngData.d.USBVoltage);
  PRINTF_TERMINAL("nFwVersion = %d\r\n", AstraEngData.nFwVersion);
}

void AstraProcessCommand(uint8_t *att_data, uint32_t data_length)
{
  att_data[data_length] = '\0';
  AstraEnablePrintBle();
  AstraEnablePrintUsb();
  PRINTF_TERMINAL("Trace messages active\r\n");

  uint32_t i = 0;

  for(i=0;i<countof(xCmdStructVect);i++)
  {
    if(0==strncmp(xCmdStructVect[i].cCmdString, (char*)(att_data),strlen(xCmdStructVect[i].cCmdString)))
    {
      break;
    }
  }

  PlatformStatus.b.USB_TRACE = ENABLE;
  pCmdHandlerFunc_t pHandle = NULL;
  if(data_length > 0)
  {
    if(i<countof(xCmdStructVect))
    {
      pHandle = xCmdStructVect[i].pCmdHandlerFunc;
      PRINTF_TERMINAL("\r\nProcessing command: %s\r\n",att_data);
      PRINTF_TERMINAL("\r\nParameters: %s\r\n", att_data + strlen(xCmdStructVect[i].cCmdString));
      pHandle((char*)(att_data), (char*)(att_data + strlen(xCmdStructVect[i].cCmdString)));
      PRINTF_TERMINAL("Command complete\r\n");
      if(xCmdStructVect[i].bPrintConfig == DISABLE_PRINT)
      {
        PRINTF_TERMINAL("Press {ENTER} to re-activate trace messages\r\n");
        AstraDisablePrintBle();
        AstraDisablePrintUsb();
      }
    }
    else
    {
      PRINTF_TERMINAL("\r\nCommand\r\n%s\r\n",att_data);
      PRINTF_TERMINAL("not found, '?' for HELP\r\n");
    }
  }
}

void ProcDbgGetFwVersion(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL("\r\nSTMicroelectronics %s:\r\n"
                  "\tVersion %d.%d.%d\r\n"
                    "\t%s\r\n",
                    BLE_FW_PACKAGENAME,
                    BLE_VERSION_FW_MAJOR,BLE_VERSION_FW_MINOR,BLE_VERSION_FW_PATCH,
                    BLE_STM32_BOARD);
}

void ProcDbgGetHwVersion(char *pcInStr, char *pcInParam)
{
  uint8_t HwVer[9] = "UNKNOWN";
  custom_astra_GetHwVersionString(HwVer);
  PRINTF_TERMINAL("\r\nBoard HW version\r\n");
  PRINTF_TERMINAL("\r\nSTMicroelectronics %s:\r\n"
                  "\tVersion %s\r\n",
                    BLE_STM32_BOARD,
                    HwVer);
}

void ProcDbgGetLoraParams(char *pcInStr, char *pcInParam)
{
  char pTmpLoraKey[32+1];
  PRINTF_TERMINAL("\r\nLoRa keys saved inside STM32WL:\r\n");
  AstraGetStm32wlDevEuiString(pTmpLoraKey);
  PRINTF_TERMINAL("DevEui: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");
  AstraGetStm32wlAppEuiString(pTmpLoraKey);
  PRINTF_TERMINAL("AppEui: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");
  AstraGetStm32wlAppKeyString(pTmpLoraKey);
  PRINTF_TERMINAL("AppKey: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");
  AstraGetStm32wlNwkKeyString(pTmpLoraKey);
  PRINTF_TERMINAL("NwkKey: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");

  PRINTF_TERMINAL("\r\nLoRa keys configured by STM32WB via at command:\r\n");
  AstraGetLoRaAppEuiString(pTmpLoraKey);
  PRINTF_TERMINAL("AppEui: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");
  AstraGetLoRaAppKeyString(pTmpLoraKey);
  PRINTF_TERMINAL("AppKey: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");
  AstraGetLoRaNwkKeyString(pTmpLoraKey);
  PRINTF_TERMINAL("NwkKey: ");
  PRINTF_TERMINAL(pTmpLoraKey);
  PRINTF_TERMINAL("\r\n");
#if USE_COMMISSIONING_H_KEYS
  PRINTF_TERMINAL("LoRa keys loaded from Commissioning.h file\r\n");
#else //USE_COMMISSIONING_H_KEYS
#if USE_MEMORY
  PRINTF_TERMINAL("LoRa keys loaded from NFC EEPROM\r\n");
#endif //USE_MEMORY
#endif //USE_COMMISSIONING_H_KEYS
}

void ProcDbgGetBleMacAddr(char *pcInStr, char *pcInParam)
{
#if USE_BLE
  PRINTF_TERMINAL("View BLE MAC address\r\n");
  PRINTF_TERMINAL("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                  BLE_StackValue.BleMacAddress[5],
                  BLE_StackValue.BleMacAddress[4],
                  BLE_StackValue.BleMacAddress[3],
                  BLE_StackValue.BleMacAddress[2],
                  BLE_StackValue.BleMacAddress[1],
                  BLE_StackValue.BleMacAddress[0]);
#else
    PRINTF_TERMINAL("WARNING: USE_BLE functionality is disabled\r\n");
#endif //USE_BLE
}

void ProcDbgGetVerbosity(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL("View PRINTF verbosity level\r\n");
  PRINTF_TERMINAL("Current setting: %d - %s\r\n", SystemConfParams.PrintLevelActive, AstraGetVerbLevel());
}

void ProcDbgSetVerbosity(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL("Set PRINTF verbosity level\r\n");
  if(pcInParam[0] == '-')
  {
    if(pcInParam[1] >= '1' && pcInParam[1] <= '5')
    {
      PRINTF_TERMINAL("Set to: %c\r\n", pcInParam[1]);
      SystemConfParams.PrintLevelActive = pcInParam[1] - '0';
      AstraSetDbgConsole(1);
      SavePlatformParams(&SystemConfParams);
    }
    else
    {
      PRINTF_TERMINAL("Value %c out of bounds: %d LEVEL_TERMINAL - %d LEVEL_INFO\r\n", pcInParam[1], LEVEL_TERMINAL, LEVEL_INFO);
    }
  }
  else
  {
    PRINTF_TERMINAL("Usage: !verbosity-X\r\n");
    PRINTF_TERMINAL("where X is in the range: %d LEVEL_TERMINAL - %d LEVEL_INFO\r\n", LEVEL_TERMINAL, LEVEL_INFO);
  }
  PRINTF_TERMINAL("Current setting: %d - %s\r\n", SystemConfParams.PrintLevelActive, AstraGetVerbLevel());
}

void ProcDbgSysReset(char *pcInStr, char *pcInParam)
{
  AstraBoardReboot();
}

void ProcDbgSysGetBlVer(char *pcInStr, char *pcInParam)
{
  PRINTF_VERBOSE("Get BLE bootloader version:\r\n");

  void *blveraddr = (void*)0x08000200;
  uint8_t blverstring[] = "BLE_Ota";
  if(memcmp(blveraddr, blverstring, strlen((char*)blverstring)) == 0)
  {
    PRINTF_VERBOSE("BLE bootloader version found:\r\n");
    PRINTF_VERBOSE(blveraddr);
    PRINTF_VERBOSE("\r\n");
  }
  else
  {
    PRINTF_VERBOSE("No bootloader version found\r\n");
  }
}

void ProcDbgSysDfu(char *pcInStr, char *pcInParam)
{
  AstraBoardUsbDfu();
}

void ProcDbgSysWlPrg(char *pcInStr, char *pcInParam)
{
#if USE_STM32WL
  STM32WL_RebootDfu();
#else //USE_STM32WL
  PRINTF_TERMINAL("WARNING: STM32WL functionality is disabled\r\n");
#endif //USE_STM32WL
  while(1);
}

static uint8_t nStopFwEnterHits = 0;
static uint8_t nStopFwEnterAckedHits = 0;
#if USE_USB
int8_t Astra_STOP_CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  extern USBD_HandleTypeDef hUsbDeviceFS;
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  if(*Buf == '\r')
  {
    nStopFwEnterHits++;
  }
  if(*Buf == '\n')
  {
    nStopFwEnterHits++;
  }

  return (USBD_OK);
}
#endif /* USE_USB */

void ProcDbgStopFw(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL("\r\nSTOP EXECUTION\r\n");
  PRINTF_TERMINAL("\r\nRESTART MANUALLY (RESET or SHUTDOWN)\r\n");
  PRINTF_TERMINAL("\r\nPressing enter twice will reboot\r\n");
#if USE_USB
  USBD_Interface_fops_FS.Receive = Astra_STOP_CDC_Receive_FS;
#endif /* USE_USB */
  while(1)
  {
    AstraLedToggle(ASTRA_LED_COLOR_RED);
    HAL_Delay(200);
    AstraLedToggle(ASTRA_LED_COLOR_RED);
    HAL_Delay(200);
    if(nStopFwEnterHits > nStopFwEnterAckedHits)
    {
      PRINTF_TERMINAL("\r\nPress enter once again\r\n");
      nStopFwEnterAckedHits++;
    }
    if(nStopFwEnterHits == 2)
    {
      AstraBoardReboot();
    }
  }
}

void ProcDbgSystemStateRun(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL("\r\nProcDbgSystemStateRun()\r\n");
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("SYSTEM GOING FULL RUN\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
#ifndef __SM_CONF_H
  PlatformStatus.s.xNextState = State_Run;
#else //__SM_CONF_H
  SM_NormalTrigger(APP_EV_RN);
#endif //__SM_CONF_H
}

void ProcDbgSystemStateLowPower(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL("\r\nProcDbgSystemStateLowPower()\r\n");
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("SYSTEM GOING LOW POWER\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
#ifndef __SM_CONF_H
  PlatformStatus.s.xNextState = State_LowPower;
#else //__SM_CONF_H
  SM_NormalTrigger(APP_EV_LP);
#endif //__SM_CONF_H
}

void ProcDbgSetUseCase(char *pcInStr, char *pcInParam)
{
  uint8_t uc_applied = 0;
  uint8_t newEngine = 0;
  if(strlen(pcInParam) > 0 )
  {
    PRINTF_TERMINAL("Set use case to %s\r\n", pcInParam);
    if(pcInParam[0] == '-')
    {
      if( (pcInParam[1] == '0') || (pcInParam[1] == '1') || (pcInParam[1] == '2') || (pcInParam[1] == '3') || (pcInParam[1] == '4') )
      {
        newEngine = pcInParam[1]-'0';
        PRINTF_TERMINAL("New use case: %s\r\n", EngineUcNames[newEngine]);
        AstraSetEngine(EngineUcNames[newEngine]);
        StatusUpdTrigger();
        uc_applied = 1;
      }
      else
      {
        PRINTF_TERMINAL("Wrong format: uc not supported\r\n");
      }
    }
    else
    {
      PRINTF_TERMINAL("Wrong format: missing '-'\r\n");
    }
  }
  if(!uc_applied)
  {
    PRINTF_TERMINAL("Usage: !usecase-X\r\n");
    PRINTF_TERMINAL("where X can be: \r\n");
    PRINTF_TERMINAL("0->FLEET_MNG\r\n");
    PRINTF_TERMINAL("1->LIVESTOCK_MON\r\n");
    PRINTF_TERMINAL("2->GOODS_MONITORING\r\n");
    PRINTF_TERMINAL("3->LOGISTICS\r\n");
    PRINTF_TERMINAL("4-> CUSTOM\r\n");
  }
}

void ProcDbgFwdWl(char *pcInStr, char *pcInParam)
{
#if USE_STM32WL
  if(strlen(pcInParam) > 0 )
  {
    PRINTF_TERMINAL("\r\nSending: \r\n");
    PRINTF_TERMINAL(pcInParam);
    PRINTF_TERMINAL("\r\nto STM32WL\r\n");
    custom_sob_stm32wl_uartsend((const uint8_t*)pcInParam, strlen(pcInParam));
  }
#else //USE_STM32WL
  PRINTF_TERMINAL("WARNING: STM32WL functionality is disabled\r\n");
#endif //USE_STM32WL
}

void ProcDbgFwdWlNewline(char *pcInStr, char *pcInParam)
{
#if USE_STM32WL
  if(strlen(pcInParam) > 0 )
  {
    PRINTF_TERMINAL("\r\nSending: \r\n");
    PRINTF_TERMINAL(pcInParam);
    PRINTF_TERMINAL("\r\nto STM32WL\r\n");
    custom_sob_stm32wl_uartsend((const uint8_t*)pcInParam, strlen(pcInParam));
  }
  PRINTF_TERMINAL("\r\nSending: \\r\\n to STM32WL\r\n");
  custom_sob_stm32wl_uartsend((uint8_t*)"\r\n", 2);
  
  HAL_Delay(500);
  
  ProcDbgWlGetResponse("","");
#else //USE_STM32WL
  PRINTF_TERMINAL("WARNING: STM32WL functionality is disabled\r\n");
#endif //USE_STM32WL
}

#if USE_STM32WL
#if USE_USB
int8_t Astra_WL_CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  extern USBD_HandleTypeDef hUsbDeviceFS;
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  if(*Buf == 0x7F && *Len == 0x01)
  {
    custom_sob_stm32wl_uartsend(Buf, *Len);
  }
  else if(*Buf == 0x02 && *Len == 0x01)
  {
    custom_sob_stm32wl_uartsend(Buf, *Len);
  }
  else if(*Buf == 0xFD && *Len == 0x01)
  {
    custom_sob_stm32wl_uartsend(Buf, *Len);
  }
  else if(*Buf == 0x00 && *Len == 0x01)
  {
    custom_sob_stm32wl_uartsend(Buf, *Len);
  }
  else if(*Buf == 0xFF && *Len == 0x01)
  {
    custom_sob_stm32wl_uartsend(Buf, *Len);
  }
  else
  {
    uint32_t nLen = *Len;
    static uint32_t nDelayT = 0; //5;
    for(uint32_t i=0;i<nLen;i++)
    {
      custom_sob_stm32wl_uartsend(Buf+i, 1);
      pause(nDelayT);
    }
  }

  AstraLedToggle(ASTRA_LED_COLOR_GREEN);

  return (USBD_OK);
}
#endif /* USE_USB */
#endif //USE_STM32WL

void UartConfig(void)
{
        extern UART_HandleTypeDef hlpuart1;
        HAL_StatusTypeDef ret = HAL_OK;

        hlpuart1.Instance = LPUART1;
        hlpuart1.Init.BaudRate = 115200;
        hlpuart1.Init.WordLength = UART_WORDLENGTH_9B;
        hlpuart1.Init.StopBits = UART_STOPBITS_1;
        hlpuart1.Init.Parity = UART_PARITY_EVEN; //UART_PARITY_NONE; //UART_PARITY_EVEN;
        hlpuart1.Init.Mode = UART_MODE_TX_RX;
        hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
        hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
        if (HAL_UART_Init(&hlpuart1) != HAL_OK)
        {
          ret = HAL_ERROR;
        }
        UNUSED(ret);
}

void ProcDbgWlTransparent(char *pcInStr, char *pcInParam)
{
#if USE_STM32WL
  //dma implementation
  PRINTF_TERMINAL("\r\nFrom now on, USB is virtually connected to STM32WL uart port\r\n");
  PRINTF_TERMINAL("\r\nSend the desired AT commands\r\n");
  PRINTF_TERMINAL("e.g. AT+CUST_DFU and open\r\n");
  PRINTF_TERMINAL("STM32CubeProgrammer to manage STM32WL flash memory\r\n");
#if USE_USB
  USBD_Interface_fops_FS.Receive = Astra_WL_CDC_Receive_FS;
#endif /* USE_USB */
  PRINTF_TERMINAL("Hardware reset or button push to exit\r\n");
  while(1)
  {
    if(custom_sob_stm32wl_get_data_len())
    {
      AstraLedToggle(ASTRA_LED_COLOR_BLUE);

        HAL_Delay(200);

        custom_sob_stm32wl_stop_uartrecv_dma();

        UartConfig();

      static uint32_t nDelayR = 2; //50;
      uint32_t nNewByteNum = 0;
      do
      {
        nNewByteNum = custom_sob_stm32wl_get_data_len();
        HAL_Delay(nDelayR);
      }
      while(nNewByteNum != custom_sob_stm32wl_get_data_len());

#if USE_USB
      CUSTOM_USB_SendMessage(custom_sob_stm32wl_get_data_buf(), custom_sob_stm32wl_get_data_len(), CUSTOM_USB_DEFAULT_ATTEMPS);
#endif //USE_USB

      /* restart receiving, not to miss URCs */
      custom_sob_stm32wl_uartrecv_dma(custom_sob_stm32wl_get_data_buf(), custom_sob_stm32wl_get_buf_size());
      extern UART_HandleTypeDef hlpuart1;
      hlpuart1.Instance->CR1 &= ~USART_CR1_PEIE;
    }

#if USE_ASTRA_MAINBOARD_BSP
    if(custom_astra_btn_get_state(ASTRA_BTN1) == ENABLE)
    {
      AstraBoardReboot();
    }
#endif //USE_ASTRA_MAINBOARD_BSP
  }
#else //USE_STM32WL
  PRINTF_TERMINAL("WARNING: STM32WL functionality is disabled\r\n");
#endif //USE_STM32WL
}

void ProcDbgWlGetResponse(char *pcInStr, char *pcInParam)
{
#if USE_STM32WL
  PRINTF_TERMINAL("\r\nPrinting STM32WL receive buffer\r\n");
  PRINTF_TERMINAL("\r\nStart STM32WL data\r\n");
  PRINTF_TERMINAL((char const*)custom_sob_stm32wl_get_data_buf());
  PRINTF_TERMINAL("\r\nEnd STM32WL data\r\n");

  /* restart receiving, not to miss URCs */
  memset((char *)custom_sob_stm32wl_get_data_buf(), 0x00, custom_sob_stm32wl_get_buf_size());
  custom_sob_stm32wl_uartrecv_dma(custom_sob_stm32wl_get_data_buf(), custom_sob_stm32wl_get_buf_size());
#else //USE_STM32WL
  PRINTF_TERMINAL("WARNING: STM32WL functionality is disabled\r\n");
#endif //USE_STM32WL
}

void ProcDbgSetLoraAppConfig(char *pcInStr, char *pcInParam)
{
  if(pcInParam[0] == '-' && isxdigit(pcInParam[1]))
  {
    if(digit(pcInParam[1]) == 1)
    {
      PRINTF_TERMINAL(TEXT_COLOR_4);
      PRINTF_TERMINAL("Enable LoRa fw module\r\n");

      AstraSetLoraProcessing(1);

      PRINTF_TERMINAL(DEFAULT_TEXT_COLOR);
    }
    else if(digit(pcInParam[1]) == 0)
    {
      PRINTF_TERMINAL(TEXT_COLOR_4);
      PRINTF_TERMINAL("Disable LoRa fw module\r\n");

      AstraSetLoraProcessing(0);

      PRINTF_TERMINAL(DEFAULT_TEXT_COLOR);
    }
    else
    {
      PRINTF_TERMINAL("Param not valid");
    }
  }
  else
  {
    PRINTF_TERMINAL("Command format error\r\n");
    PRINTF_TERMINAL("Use: !lora-N to set lora mode\r\n");
    PRINTF_TERMINAL("where N is 0..1 (0:DISABLED, 1:ENABLED)\r\n");
  }
}

void ProcDbgGetLoraAppConfig(char *pcInStr, char *pcInParam)
{
  PRINTF_TERMINAL(TEXT_COLOR_4);
  if(AstraGetLoraProcessing())
  {
    PRINTF_TERMINAL("LoRa fw module is enabled\r\n");
  }
  else
  {
    PRINTF_TERMINAL("LoRa fw module is disabled\r\n");
  }
  if(AstraGetLoraJoinStatus())
  {
    PRINTF_TERMINAL("LoRa network has joined\r\n");
  }
  else
  {
    PRINTF_TERMINAL("LoRa network has NOT joined\r\n");
  }
  PRINTF_TERMINAL(DEFAULT_TEXT_COLOR);
}

void ProcDbgSetGnssStatus(char *pcInStr, char *pcInParam)
{
#if USE_GNSS
  PRINTF_TERMINAL("GNSS Teseo-LIV3F module status\r\n");
  PRINTF_TERMINAL("gnss_latitude = %f\r\n", AstraEngData.d.gnss_latitude);
  PRINTF_TERMINAL("gnss_longitude = %f\r\n", AstraEngData.d.gnss_longitude);
  PRINTF_TERMINAL("gnss_altitude = %f\r\n", AstraEngData.d.gnss_altitude);
  PRINTF_TERMINAL("gnss_sats = %d\r\n", AstraEngData.d.gnss_sats);
  PRINTF_TERMINAL("gnss_fix_validity = %d\r\n", AstraEngData.d.gnss_fix_validity);
  PRINTF_TERMINAL("GNSS FW version: ");
  PRINTF_TERMINAL((char const*)GnssGetVersion());
  PRINTF_TERMINAL("\r\n");
#else //USE_GNSS
  PRINTF_TERMINAL("WARNING: GNSS functionality is disabled\r\n");
#endif //USE_GNSS
}

void ProcDbgSetGnssAppConfig(char *pcInStr, char *pcInParam)
{
#if USE_GNSS
  if(0 == memcmp(pcInParam, "-on", 3))
  {
    PRINTF_TERMINAL("\r\nGNSS_REPORT = 1\r\n");
    PlatformStatus.b.GNSS_REPORT = 1;
    PlatformStatus.b.GNSS_NMEA = 0;
  }
  else if(0 == memcmp(pcInParam, "-off", 4))
  {
    PRINTF_TERMINAL("\r\nGNSS_REPORT = 0\r\n");
    PlatformStatus.b.GNSS_REPORT = 0;
    PlatformStatus.b.GNSS_NMEA = 0;
  }
  else
  {
    PRINTF_TERMINAL("Command format error\r\n");
    PRINTF_TERMINAL("Use: !gnssreport-on to set GNSS_REPORT flag\r\n");
    PRINTF_TERMINAL("Use: !gnssreport-off to reset GNSS_REPORT flag\r\n");
  }
#else //USE_GNSS
  PRINTF_TERMINAL("WARNING: GNSS functionality is disabled\r\n");
#endif //USE_GNSS
}

void ProcDbgSetGnssNmeaConfig(char *pcInStr, char *pcInParam)
{
#if USE_GNSS
  if(0 == memcmp(pcInParam, "-on", 3))
  {
    PRINTF_TERMINAL("\r\nGNSS_NMEA = 1\r\n");
    PlatformStatus.b.GNSS_NMEA = 1;
    PlatformStatus.b.GNSS_REPORT = 0;
  }
  else if(0 == memcmp(pcInParam, "-off", 4))
  {
    PRINTF_TERMINAL("\r\nGNSS_NMEA = 0\r\n");
    PlatformStatus.b.GNSS_NMEA = 0;
    PlatformStatus.b.GNSS_REPORT = 0;
  }
  else
  {
    PRINTF_TERMINAL("Command format error\r\n");
    PRINTF_TERMINAL("Use: !gnssnmea-on to enable NMEA printout\r\n");
    PRINTF_TERMINAL("Use: !gnssnmea-off to disable NMEA printout\r\n");
  }
#else //USE_GNSS
  PRINTF_TERMINAL("WARNING: GNSS functionality is disabled\r\n");
#endif //USE_GNSS
}

void ProcDbgGetDebugMode(char *pcInStr, char *pcInParam)
{
  switch(SystemConfParams.PrintLevelActive)
  {
  case 0:
    PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 0:LEVEL_NOTHING\r\n");
    break;
  case 1:
    PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 1:LEVEL_TERMINAL\r\n");
    break;
  case 2:
    PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 2:LEVEL_ERROR\r\n");
    break;
  case 3:
    PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 3:LEVEL_CRITICAL\r\n");
    break;
  case 4:
    PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 4:LEVEL_VERBOSE\r\n");
    break;
  case 5:
    PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 5:LEVEL_INFO\r\n");
  default:
    PRINTF_TERMINAL("Param error: %d\r\n", SystemConfParams.PrintLevelActive);
  }
}

void ProcDbgSetMemsEvConfig(char *pcInStr, char *pcInParam)
{
  if(pcInParam[0] == '-' && isxdigit(pcInParam[1]))
  {
    if(xdigit(pcInParam[1]) == 1)
    {
      SystemConfParams.b.CONF_MEMS_REBOOT = 1;
    }
    else if(xdigit(pcInParam[1]) == 0)
    {
      SystemConfParams.b.CONF_MEMS_REBOOT = 0;
      ClearScheduleAstraSmChangeState();
    }
    PRINTF_TERMINAL("New configuration is: %d\r\n", SystemConfParams.b.CONF_MEMS_REBOOT);
    AstraSaveDefaultParams();
  }
  else if(pcInParam[0] != '\0')
  {
    PRINTF_TERMINAL("Command format error\r\n");
    PRINTF_TERMINAL("Use: !lponmemsevent-N to set conf for MEMS event\r\n");
    PRINTF_TERMINAL("where N is 0 or 1 (0:no low power, 1:low power on mems inactivity event)\r\n");
  }
  else
  {
    PRINTF_TERMINAL("Current configuration is: %d\r\n", SystemConfParams.b.CONF_MEMS_REBOOT);
  }
}

void ProcDbgSetDebugMode(char *pcInStr, char *pcInParam)
{
  if(pcInParam[0] == '-' && isxdigit(pcInParam[1]))
  {
    switch(xdigit(pcInParam[1]))
    {
    case 0:
      PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 0:LEVEL_NOTHING\r\n");
      SystemConfParams.PrintLevelActive = 0;
      SavePlatformParams(&SystemConfParams);
      break;
    case 1:
      PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 1:LEVEL_TERMINAL\r\n");
      SystemConfParams.PrintLevelActive = 1;
      SavePlatformParams(&SystemConfParams);
      break;
    case 2:
      PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 2:LEVEL_ERROR\r\n");
      SystemConfParams.PrintLevelActive = 2;
      SavePlatformParams(&SystemConfParams);
      break;
    case 3:
      PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 3:LEVEL_CRITICAL\r\n");
      SystemConfParams.PrintLevelActive = 3;
      SavePlatformParams(&SystemConfParams);
      break;
    case 4:
      PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 4:LEVEL_VERBOSE\r\n");
      SystemConfParams.PrintLevelActive = 4;
      SavePlatformParams(&SystemConfParams);
      break;
    case 5:
      PRINTF_TERMINAL("\r\nDEBUG PrintLevelActive = 5:LEVEL_INFO\r\n");
      SystemConfParams.PrintLevelActive = 5;
      SavePlatformParams(&SystemConfParams);
    default:
      PRINTF_TERMINAL("Param error\r\n");
    }
  }
  else
  {
    PRINTF_TERMINAL("Command format error\r\n");
    PRINTF_TERMINAL("Use: !debug-N to set debug mode\r\n");
    PRINTF_TERMINAL("where N is 0..5 (0:LEVEL_NOTHING, 5:LEVEL_INFO)\r\n");
  }
}

void ProcDbgHelp(char *pcInStr, char *pcInParam)
{
  PrintSystemInfo();
  PRINTF_TERMINAL("\r\nTerminal HELP:\r\n");
  PRINTF_TERMINAL("\r\n");
  for(uint32_t i=0;i<countof(xCmdStructVect);i++)
  {
#ifdef COMMAND_INTERPRETER_H

#else //COMMAND_INTERPRETER_H
    {
      PRINTF_TERMINAL(xCmdStructVect[i].cCmdString);
      PRINTF_TERMINAL("  - ");
      PRINTF_TERMINAL(xCmdStructVect[i].cCmdHelp);
      PRINTF_TERMINAL("\r\n");
    }
#endif //COMMAND_INTERPRETER_H
  }
}

void ConnectionComplete(void)
{
  PRINTF_VERBOSE("ConnectionComplete()\r\n");
  PlatformStatus.b.BLE_CONNECTED = 1;
}

void DisconnectionComplete(void)
{
  PRINTF_VERBOSE("DisconnectionComplete()\r\n");
  PlatformStatus.b.BLE_CONNECTED = 0;
}

NO_INIT(char globalboardname[8]);
void Astra_GetDeviceName(char *boardName)
{
  GetBoardName(globalboardname);
  if(in_range(globalboardname[0],'a','z') || in_range(globalboardname[0],'A','Z'))
  {
    snprintf(boardName, 8,"%s", globalboardname);
  }
  else
  {
    snprintf(boardName, 8,"%s", CFG_GAP_DEVICE_NAME);
    snprintf(globalboardname, 8,"%s", CFG_GAP_DEVICE_NAME);
  }
}

void AstraSetDateTime(char*newdate, char*newtime)
{
  int32_t hh, mm, ss;
  int32_t wd, dd, mt, yy;
  int32_t scanresultt, scanresultd;
  scanresultt = sscanf(newtime, "%ld:%ld:%ld", &hh, &mm, &ss);
  scanresultd = sscanf(newdate, "%ld/%ld/%ld/%ld", &wd, &dd, &mt, &yy);
  PRINTF_VERBOSE("SScanf result: %d %d\r\n", scanresultt, scanresultd);
  PRINTF_VERBOSE("Would like to set time and date\r\n");
  PRINTF_VERBOSE("%.2d/%.2d/%.2d - ", mt, dd, yy );
  PRINTF_VERBOSE("%.2d:%.2d:%.2d\r\n", hh, mm, ss );
  RTC_TimeTypeDef CurTime;
  RTC_DateTypeDef CurDate;
  CurTime.Hours = hh;
  CurTime.Minutes = mm;
  CurTime.Seconds = ss;
  CurDate.Date = dd;
  CurDate.Month = mt;
  CurDate.Year = 100 + yy;
  CurDate.WeekDay = wd;
  AstraSetTimeStamp(CurTime, CurDate);
  newtime[0] = 'T';
  newdate[0] = 'T';
}

void AstraNfcDataInit(void)
{
#if USE_MEMORY
  APP_NFC_ForceDataInit(AstraGetTimeStamp() - (3600 * TIMEZONE_CORRECTION));
#endif //USE_MEMORY
}

void Astra_SetDeviceName(char*newname)
{
  char tmpboardname[8+1];
  snprintf(tmpboardname, 8,"%s", newname);
  snprintf(globalboardname, 8,"%s", newname);
#if USE_BLE
  snprintf(BLE_StackValue.BoardName, 8,"%s", newname);
#endif //USE_BLE
  SaveBoardName(tmpboardname);
}

uint8_t *AstraGetMcuId(void)
{
  return &PlatformStatus.s.STM32_UID[0];
}

char*Astra_GetDevicePowerStatus(void)
{
  if( (AstraEngData.d.USBVoltage > 4000) && (AstraEngData.d.batteryVoltage > 3000))
  {
    return "USB and battery connected";
  }
  else if(AstraEngData.d.USBVoltage > 4000)
  {
    return "USB powered";
  }
  else if(AstraEngData.d.batteryVoltage > 3000)
  {
    return "Running on battery";
  }
  else
  {
    return "Error retrieving power status";
  }
}

void ScheduleAstraEngineRestart(void)
{
  PlatformStatus.b.ASTRA_ENGINE_RESTART_PENDING = 1;
}

char const **AstraGetRebootReasonNames(void)
{
  return &RebootReasonNames[0];
}

char const **AstraGetULPwakeNames(void)
{
  return &ULPwakeNames[0];
}

void AstraRebootWithReason(const char*reasonName)
{
  if(0 == strcmp(reasonName, RebootReasonNames[0])) //"Normal Reboot"
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM REBOOT in Normal mode\r\n");
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);
    BoardRebootWithReason(BOOT_REASON_NORMAL);
  }
  else if(0 == strcmp(reasonName, RebootReasonNames[1])) //"DFU Reboot"
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM REBOOT in DFU mode\r\n");
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);    
    BoardRebootWithReason(BOOT_REASON_DFU);
  }
  else
  {
  }
}

void AstraUlpSchedule(uint8_t wakeType)
{
  if(wakeType == WAKE_SRC_BLE)
  {
    PRINTF_VERBOSE("Reboot scheduled for ULP with %d wake\r\n", WAKE_SRC_BLE);
    ScheduleAstraSmChangeState(SmChangeState_lpBleWake);
  }
  else if(wakeType == WAKE_SRC_ACC)
  {
    PRINTF_VERBOSE("Reboot scheduled for ULP with %d wake\r\n", WAKE_SRC_ACC);
    ScheduleAstraSmChangeState(SmChangeState_lpAccWake);
  }
  else if(wakeType == WAKE_SRC_BLE_ACC)
  {
    PRINTF_VERBOSE("Reboot scheduled for ULP with %d wake\r\n", WAKE_SRC_BLE_ACC);
    ScheduleAstraSmChangeState(SmChangeState_lpBleAccWake);
  }
  else if(wakeType == WAKE_SRC_BUTTON_ONLY)
  {
    PRINTF_VERBOSE("Reboot scheduled for ULP with %d wake\r\n", WAKE_SRC_BUTTON_ONLY);
    ScheduleAstraSmChangeState(SmChangeState_lpButtonWake);
  }
  else
  {
    PRINTF_VERBOSE("Selected ULP mode is invalid: %d\r\n", wakeType);
  }
}

void AstraULP_Schedule(const char*wakeType)
{
  if(0 == strcmp(wakeType, ULPwakeNames[0])) //"LP ble wake"
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM ULP with %s wake\r\n", ULPwakeNames[0]);
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);    

    //* Schedule reboot with BLE wake up
    AstraUlpSchedule(WAKE_SRC_BLE);
  }
  else if(0 == strcmp(wakeType, ULPwakeNames[1])) //"LP Acc Wake"
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM ULP with %s wake\r\n", ULPwakeNames[1]);
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);    

    //* Schedule reboot with LP ACC wake up
    AstraUlpSchedule(WAKE_SRC_ACC);
  }
  else if(0 == strcmp(wakeType, ULPwakeNames[2])) //"LP Acc Wake"
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM ULP with %s wake\r\n", ULPwakeNames[2]);
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);    

    //* Schedule reboot with LP ACC wake up
    AstraUlpSchedule(WAKE_SRC_BLE_ACC);
  }
  else if(0 == strcmp(wakeType, ULPwakeNames[3])) //"Button wake"
  {
    PRINTF_VERBOSE(TEXT_COLOR_3);
    PRINTF_VERBOSE("SYSTEM ULP with %s wake\r\n", ULPwakeNames[3]);
    PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
    AstraLedColor(ASTRA_LED_COLOR_BLUE);
    HAL_Delay(500);    

    //* Schedule reboot with LP ACC wake up
    AstraUlpSchedule(WAKE_SRC_BUTTON_ONLY);
  }
  else
  {
    PRINTF_VERBOSE("Selected wake source is invalid: %s\r\n", wakeType);
  }
}

void ScheduleAstraSmChangeState(SmChangeState_t newstate)
{  
  if (PlatformStatus.b.ASTRA_CHANGE_STATE_PENDING == 1)
  {
    PRINTF_VERBOSE("SYSTEM REBOOT Pending already set\r\n");
    return;
  }
  PRINTF_VERBOSE("SYSTEM REBOOT Pending SET\r\n");
  PlatformStatus.s.NewState = newstate;
  PlatformStatus.b.ASTRA_CHANGE_STATE_PENDING = 1;
  
  AstraSaveWakeReason(newstate);
}

void ClearScheduleAstraSmChangeState(void)
{
  if (PlatformStatus.b.ASTRA_CHANGE_STATE_PENDING == 0)
  {
    PRINTF_VERBOSE("SYSTEM REBOOT Pending already clear\r\n");
    return;
  }
  PRINTF_VERBOSE("SYSTEM REBOOT Pending CLEAR\r\n");
  PlatformStatus.s.NewState = SmChangeState_none;
  PlatformStatus.b.ASTRA_CHANGE_STATE_PENDING = 0;
  
  AstraSaveWakeReason(0);
}

char const **AstraGetVerbLevelNames(void)
{
  return &VerbLevelNames[0];
}

uint32_t AstraGetVerbLevelEnum(void)
{
  uint32_t ret = 0xDEADBEEF; // BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN
  for(uint8_t i=0; i<countof(VerbLevelNames); i++)
  {
    if(0 == strcmp(VerbLevelNames[i], AstraGetVerbLevel()))
    {
      ret = i;
    }
  }
  return ret;
}

void AstraSetVerbLevel(const char*newVerbLevel)
{
  if(0 == strcmp(newVerbLevel, "INFO"))
  {
    SystemConfParams.PrintLevelActive = LEVEL_INFO;
    AstraSetDbgConsole(1);
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newVerbLevel, "VERBOSE"))
  {
    SystemConfParams.PrintLevelActive = LEVEL_VERBOSE;
    AstraSetDbgConsole(1);
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newVerbLevel, "CRITICAL"))
  {
    SystemConfParams.PrintLevelActive = LEVEL_CRITICAL;
    AstraSetDbgConsole(1);
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newVerbLevel, "ERROR"))
  {
    SystemConfParams.PrintLevelActive = LEVEL_ERROR;
    AstraSetDbgConsole(1);
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newVerbLevel, "TERMINAL"))
  {
    SystemConfParams.PrintLevelActive = LEVEL_TERMINAL;
    AstraSetDbgConsole(1);
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newVerbLevel, "DISABLED"))
  {
    AstraSetDbgConsole(0);
  }
}

char *AstraGetVerbLevel(void)
{
  if(SystemConfParams.PrintLevelActive == LEVEL_INFO)
  {
    return "INFO";
  }
  else if(SystemConfParams.PrintLevelActive == LEVEL_VERBOSE)
  {
    return "VERBOSE";
  }
  else if(SystemConfParams.PrintLevelActive == LEVEL_CRITICAL)
  {
    return "CRITICAL";
  }
  else if(SystemConfParams.PrintLevelActive == LEVEL_ERROR)
  {
    return "ERROR";
  }
  else if(SystemConfParams.PrintLevelActive == LEVEL_TERMINAL)
  {
    return "TERMINAL";
  }
  return "UNKNOWN";
}

void AstraSaveDefaultParams(void)
{
  SystemConfParams.PrintLevelActive = LEVEL_ACTIVE_DEFAULT;
#if USE_STM32WL
  SystemConfParams.LORA_REGION = STM32WL_SUBGHZ_REGION;
#endif //USE_STM32WL
  SystemConfParams.LORA_ACK = STM32WL_LORA_CONFIRMED_MSG;
  SystemConfParams.LORA_ADR = STM32WL_ADR;
  SystemConfParams.LORA_DR = STM32WL_DATA_RATE;
  SavePlatformParams(&SystemConfParams);
}

void AstraLoadParams(void)
{
  LoadPlatformParams(&SystemConfParams);
}

void AstraSetEngine(const char*newEngine)
{
  AstraEngineParams_t newparams;
  if(0 == strcmp(newEngine, "FLEET_MNG"))
  {
    newparams.ASTRA_ENGINE_SEL = FLEET_MNG_ENG;

    newparams.RUNALGO = FLEET_MNG_RUNALGO;
    newparams.LORA_SENDFREQ = FLEET_MNG_SENDFREQ;
    newparams.NFC_DATASAVINGINTV = FLEET_MNG_NFCDATASAVING_INTV;
    newparams.BLE_SENDFREQ = FLEET_MNG_BLE_SEND_INTV;
    newparams.b.INDOORONLY = FLEET_MNG_INDOORONLY;
    newparams.b.LOG = FLEET_MNG_LOG;
  }
  else if(0 == strcmp(newEngine, "LIVESTOCK_MON"))
  {
    newparams.ASTRA_ENGINE_SEL = LIVESTOCK_MON_ENG;

    newparams.RUNALGO = LIVESTOCK_MON_RUNALGO;
    newparams.LORA_SENDFREQ = LIVESTOCK_MON_SENDFREQ;
    newparams.NFC_DATASAVINGINTV = LIVESTOCK_MON_NFCDATASAVING_INTV;
    newparams.BLE_SENDFREQ = LIVESTOCK_BLE_SEND_INTV;
    newparams.b.INDOORONLY = LIVESTOCK_MON_INDOORONLY;
    newparams.b.LOG = LIVESTOCK_MON_LOG;
  }
  else if(0 == strcmp(newEngine, "GOODS_MONITORING"))
  {
    newparams.ASTRA_ENGINE_SEL = GOODS_MONITORING_ENG;

    newparams.RUNALGO = GOODS_MON_RUNALGO;
    newparams.LORA_SENDFREQ = GOODS_MON_SENDFREQ;
    newparams.NFC_DATASAVINGINTV = GOODS_MON_NFCDATASAVING_INTV;
    newparams.BLE_SENDFREQ = GOODS_MON_BLE_SEND_INTV;
    newparams.b.INDOORONLY = GOODS_MON_INDOORONLY;
    newparams.b.LOG = GOODS_MON_LOG;
  }
  else if(0 == strcmp(newEngine, "LOGISTICS"))
  {
    newparams.ASTRA_ENGINE_SEL = LOGISTICS_ENG;

    newparams.RUNALGO = LOGISTICS_RUNALGO;
    newparams.LORA_SENDFREQ = LOGISTICS_SENDFREQ;
    newparams.NFC_DATASAVINGINTV = LOGISTICS_NFCDATASAVING_INTV;
    newparams.BLE_SENDFREQ = LOGISTICS_BLE_SEND_INTV;
    newparams.b.INDOORONLY = LOGISTICS_INDOORONLY;
    newparams.b.LOG = LOGISTICS_LOG;
  }
  else if(0 == strcmp(newEngine, "CUSTOM"))
  {
    newparams.ASTRA_ENGINE_SEL = CUSTOM_ENG;

    newparams.RUNALGO = CUSTOM_RUNALGO;
    newparams.LORA_SENDFREQ = CUSTOM_SENDFREQ;
    newparams.NFC_DATASAVINGINTV = CUSTOM_NFCDATASAVING_INTV;
    newparams.BLE_SENDFREQ = CUSTOM_BLE_SEND_INTV;
    newparams.b.INDOORONLY = CUSTOM_INDOORONLY;
    newparams.b.LOG = CUSTOM_LOG;
  }
  else
  {
    newparams.ASTRA_ENGINE_SEL = CUSTOM_ENG;

    newparams.RUNALGO = CUSTOM_RUNALGO;
    newparams.LORA_SENDFREQ = CUSTOM_SENDFREQ;
    newparams.NFC_DATASAVINGINTV = CUSTOM_NFCDATASAVING_INTV;
    newparams.BLE_SENDFREQ = CUSTOM_BLE_SEND_INTV;
    newparams.b.INDOORONLY = CUSTOM_INDOORONLY;
    newparams.b.LOG = CUSTOM_LOG;
  }
  SaveAstraEngineParams(&newparams);
  ScheduleAstraEngineRestart();
}

void AstraSetDbgConsole(int32_t newStatus)
{
  if(newStatus)
  {
    AstraEnablePrintBle();
    AstraEnablePrintUsb();
  }
  else
  {
    AstraDisablePrintBle();
    AstraDisablePrintUsb();
  }
}

uint8_t AstraGetLoraProcessing(void)
{
  return PlatformStatus.b.APP_LORA_ENABLED;
}

uint8_t AstraGetLoraJoinStatus(void)
{
  return PlatformStatus.b.APP_LORA_JOINED;
}

void AstraSetGnssConfig(int32_t newStatus)
{
#if USE_GNSS
  PRINTF_INFO("\r\nGNSS config = %d\r\n", newStatus);

  PlatformStatus.b.GNSS_ENABLED = newStatus;
  if(PlatformStatus.b.GNSS_ENABLED == 0)
  {
  }
  else
  {
  }
#else //USE_GNSS
  PRINTF_TERMINAL("WARNING: GNSS functionality is disabled\r\n");
#endif //USE_GNSS
}

uint8_t AstraGetGnssConfig(void)
{
#if USE_GNSS
  return PlatformStatus.b.GNSS_ENABLED;
#else //USE_GNSS
  PRINTF_TERMINAL("WARNING: GNSS functionality is disabled\r\n");
  return 0;
#endif //USE_GNSS
}

void AstraSetSensorsConfig(int32_t newStatus)
{
#if USE_SENSORS
  PRINTF_INFO("\r\nSENSORS config = %d\r\n", newStatus);

  PlatformStatus.b.SENSORS_ENABLED = newStatus;
  if(PlatformStatus.b.SENSORS_ENABLED == 0)
  {
  }
  else
  {
  }

#else //USE_SENSORS
  PRINTF_TERMINAL("WARNING: SENSORS functionality is disabled\r\n");
#endif //USE_SENSORS

}

uint8_t AstraGetSensorsConfig(void)
{
#if USE_SENSORS
  return PlatformStatus.b.SENSORS_ENABLED;
#else //USE_SENSORS
  PRINTF_TERMINAL("WARNING: SENSORS functionality is disabled\r\n");
  return 0;
#endif //USE_SENSORS
}

void AstraSetLoraProcessing(int32_t newStatus)
{
  PRINTF_VERBOSE("\r\nLora processing = %d\r\n", newStatus);

  PlatformStatus.b.APP_LORA_ENABLED = newStatus;
  if(newStatus == 0)
  {
    unsetLoraJoinTimer();
    PlatformStatus.b.APP_LORA_JOINED = 0;
    PlatformStatus.b.APP_LORA_JOIN_COMPLETED = 0;
    PlatformStatus.b.APP_LORA_JOIN_IN_PROGRESS = 0;
//    PRINTF_INFO("\r\nSTM32WL_LowPower()\r\n");
//    STM32WL_LowPower();
  }
  else
  {
    setLoraJoinTimer(10*1000, 0);
  }
}

void AstraSetBleInterval(int32_t newInterval)
{
  if(newInterval > 20)
  {
    PlatformStatus.s.BLE_SEND_INTERVAL = newInterval;
    PRINTF_INFO("New BLE_SEND_INTERVAL = %d\r\n", PlatformStatus.s.BLE_SEND_INTERVAL);
  }
}

void AstraSetLoraInterval(int32_t newInterval)
{
  if(newInterval > 20)
  {
    PlatformStatus.s.LORA_SEND_INTERVAL = 1000 * newInterval;
    PRINTF_INFO("New LORA_SEND_INTERVAL = %d\r\n", PlatformStatus.s.LORA_SEND_INTERVAL);
  }
}

uint32_t AstraGetLoraInterval(void)
{
  if(PlatformStatus.s.LORA_SEND_INTERVAL < 20)
  {
    AstraSetLoraInterval(20);
  }
  return PlatformStatus.s.LORA_SEND_INTERVAL;
}

void AstraSetLoraDr(int32_t newDr)
{
  if( (newDr >= 0) && (newDr <= 15) )
  {
    SystemConfParams.LORA_DR = newDr;
    PRINTF_INFO("New LORA_DR = %d\r\n", SystemConfParams.LORA_DR);
    SavePlatformParams(&SystemConfParams);
  }
}

uint32_t AstraGetLoraDr(void)
{
  return SystemConfParams.LORA_DR;
}

void AstraSetLoraAdr(int32_t newAdr)
{
  if( (newAdr >= 0) && (newAdr <= 1) )
  {
    SystemConfParams.LORA_ADR = newAdr;
    PRINTF_INFO("New LORA_ADR = %d\r\n", SystemConfParams.LORA_ADR);
    SavePlatformParams(&SystemConfParams);
  }
}

uint32_t AstraGetLoraAdr(void)
{
  return SystemConfParams.LORA_ADR;
}

void AstraSetLoraAck(int32_t newAck)
{
  if( (newAck >= 0) && (newAck <= 1) )
  {
    SystemConfParams.LORA_ACK = newAck;
    PRINTF_INFO("New LORA_ACK = %d\r\n", SystemConfParams.LORA_ACK);
    SavePlatformParams(&SystemConfParams);
  }
}

uint32_t AstraGetLoraAck(void)
{
  return SystemConfParams.LORA_ACK;
}

char const **AstraGetLoRaRegionNames(void)
{
  return &LoRaRegionNames[0];
}

uint32_t AstraGetLoRaRegionEnum(void)
{
  uint32_t ret = 0xDEADBEEF; // BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN
  for(uint8_t i=0; i<countof(LoRaRegionNames); i++)
  {
    if(0 == strcmp(LoRaRegionNames[i], AstraGetLoraRegion()))
    {
      ret = i;
    }
  }
  return ret;
}

void AstraSetLoraRegion(const char*newLoraRegion)
{
#if USE_STM32WL
  if(0 == strcmp(newLoraRegion, "AS923"))
  {
    SystemConfParams.LORA_REGION = AS923;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "AU915"))
  {
    SystemConfParams.LORA_REGION = AU915;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "CN470"))
  {
    SystemConfParams.LORA_REGION = CN470;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "CN779"))
  {
    SystemConfParams.LORA_REGION = CN779;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "EU433"))
  {
    SystemConfParams.LORA_REGION = EU433;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "EU868"))
  {
    SystemConfParams.LORA_REGION = EU868;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "KR920"))
  {
    SystemConfParams.LORA_REGION = KR920;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "IN865"))
  {
    SystemConfParams.LORA_REGION = IN865;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "US915"))
  {
    SystemConfParams.LORA_REGION = US915;
    SavePlatformParams(&SystemConfParams);
  }
  else if(0 == strcmp(newLoraRegion, "RU864"))
  {
    SystemConfParams.LORA_REGION = RU864;
    SavePlatformParams(&SystemConfParams);
  }
  else
#endif //USE_STM32WL
  {
    //ERROR...
  }
}

char *AstraGetLoraRegion(void)
{
#if USE_STM32WL
  if(SystemConfParams.LORA_REGION == AS923)
  {
    return "AS923";
  }
  else if(SystemConfParams.LORA_REGION == AU915)
  {
    return "AU915";
  }
  else if(SystemConfParams.LORA_REGION == CN470)
  {
    return "CN470";
  }
  else if(SystemConfParams.LORA_REGION == CN779)
  {
    return "CN779";
  }
  else if(SystemConfParams.LORA_REGION == EU433)
  {
    return "EU433";
  }
  else if(SystemConfParams.LORA_REGION == EU868)
  {
    return "EU868";
  }
  else if(SystemConfParams.LORA_REGION == KR920)
  {
    return "KR920";
  }
  else if(SystemConfParams.LORA_REGION == IN865)
  {
    return "IN865";
  }
  else if(SystemConfParams.LORA_REGION == US915)
  {
    return "US915";
  }
  else if(SystemConfParams.LORA_REGION == RU864)
  {
    return "RU864";
  }
#endif //USE_STM32WL
  return "UNKNOWN";
}

void AstraGetStm32wlDevEuiString(char*str)
{
  extern uint8_t pDevEui[8];
  if(pDevEui[0]!=0 || pDevEui[1]!=0 || pDevEui[2]!=0 || pDevEui[3]!=0 ||
     pDevEui[4]!=0 || pDevEui[5]!=0 || pDevEui[6]!=0 || pDevEui[7]!=0)
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", pDevEui[0], pDevEui[1], pDevEui[2], pDevEui[3], pDevEui[4], pDevEui[5], pDevEui[6], pDevEui[7]);
  }
  else
  {
    strcpy(str, "STM32WL_NOT_CONF");
  }
}

void AstraGetStm32wlAppEuiString(char*str)
{
  extern uint8_t pAppEui[8];
  if(pAppEui[0]!=0 || pAppEui[1]!=0 || pAppEui[2]!=0 || pAppEui[3]!=0 ||
     pAppEui[4]!=0 || pAppEui[5]!=0 || pAppEui[6]!=0 || pAppEui[7]!=0)
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", pAppEui[0], pAppEui[1], pAppEui[2], pAppEui[3], pAppEui[4], pAppEui[5], pAppEui[6], pAppEui[7]);
  }
  else
  {
    strcpy(str, "-APPEUI_NOT_CONF");
  }
}

void AstraGetLoRaAppEuiString(char*str)
{
  uint8_t EepromAppEui[8];
  int32_t nRet = LoadLoraAppEui(EepromAppEui);
  if(!nRet)
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", EepromAppEui[0], EepromAppEui[1], EepromAppEui[2], EepromAppEui[3], EepromAppEui[4], EepromAppEui[5], EepromAppEui[6], EepromAppEui[7]);
  }
  else
  {
    strcpy(str, "-APPEUI_NOT_CONF");
  }
}

uint8_t AstraSetLoRaAppEui(char*strNewAppEui)
{
  uint8_t nRet = 0;
  if(isxdigit(strNewAppEui[0]) && isxdigit(strNewAppEui[1]) && isxdigit(strNewAppEui[2]) && isxdigit(strNewAppEui[3]) &&
     isxdigit(strNewAppEui[4]) && isxdigit(strNewAppEui[5]) && isxdigit(strNewAppEui[6]) && isxdigit(strNewAppEui[7]) &&
     isxdigit(strNewAppEui[8]) && isxdigit(strNewAppEui[9]) && isxdigit(strNewAppEui[10]) && isxdigit(strNewAppEui[11]) &&
     isxdigit(strNewAppEui[12]) && isxdigit(strNewAppEui[13]) && isxdigit(strNewAppEui[14]) && isxdigit(strNewAppEui[15]))
  {
    uint8_t pNewAppEui[8];
    for (uint8_t i=0;i<8;i++)
    {
      pNewAppEui[i] = xdigit(strNewAppEui[i*2]);
      pNewAppEui[i] = pNewAppEui[i]  << 4;
      pNewAppEui[i] |= xdigit(strNewAppEui[i*2 + 1]);
    }
    PRINTF_INFO("New AppEui = ");
    PRINTF_INFO("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n", pNewAppEui[0], pNewAppEui[1], pNewAppEui[2], pNewAppEui[3],
                pNewAppEui[4], pNewAppEui[5], pNewAppEui[6], pNewAppEui[7]);
    PlatformStatus.b.APP_LORA_ENABLED = 0;
    PlatformStatus.b.APP_LORA_JOINED = 0;
    PlatformStatus.b.APP_LORA_JOIN_COMPLETED = 0;
    SaveLoraAppEui(pNewAppEui);
  }
  else
  {
    PRINTF_INFO("Wrong AppEui format");
    nRet = 1;
  }
  return nRet;
}

void AstraGetStm32wlNwkKeyString(char*str)
{
  extern uint8_t pNwkKey[16];
  if(pNwkKey[0]!=0 || pNwkKey[1]!=0 || pNwkKey[2]!=0 || pNwkKey[3]!=0 ||
     pNwkKey[4]!=0 || pNwkKey[5]!=0 || pNwkKey[6]!=0 || pNwkKey[7]!=0 ||
     pNwkKey[8]!=0 || pNwkKey[9]!=0 || pNwkKey[10]!=0 || pNwkKey[11]!=0 ||
     pNwkKey[12]!=0 || pNwkKey[13]!=0 || pNwkKey[14]!=0 || pNwkKey[15]!=0 )
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
            pNwkKey[0], pNwkKey[1], pNwkKey[2], pNwkKey[3], pNwkKey[4], pNwkKey[5], pNwkKey[6], pNwkKey[7],
            pNwkKey[8], pNwkKey[9], pNwkKey[10], pNwkKey[11], pNwkKey[12], pNwkKey[13], pNwkKey[14], pNwkKey[15] );
  }
  else
  {
    strcpy(str, "- NWK KEY IS NOT CONFIGURED YET-");
  }
}

void AstraGetLoRaNwkKeyString(char*str)
{
  uint8_t EepromNwkKey[16];
  int32_t nRet = LoadLoraNwkKey(EepromNwkKey);
  if(!nRet)
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
            EepromNwkKey[0], EepromNwkKey[1], EepromNwkKey[2], EepromNwkKey[3], EepromNwkKey[4], EepromNwkKey[5], EepromNwkKey[6], EepromNwkKey[7],
            EepromNwkKey[8], EepromNwkKey[9], EepromNwkKey[10], EepromNwkKey[11], EepromNwkKey[12], EepromNwkKey[13], EepromNwkKey[14], EepromNwkKey[15] );
  }
  else
  {
    strcpy(str, "- NWK KEY IS NOT CONFIGURED YET-");
  }
}

uint8_t AstraSetLoRaNwkKey(char*strNewNwkKey)
{
  uint8_t nRet = 0;
  if(isxdigit(strNewNwkKey[0]) && isxdigit(strNewNwkKey[1]) && isxdigit(strNewNwkKey[2]) && isxdigit(strNewNwkKey[3]) &&
     isxdigit(strNewNwkKey[4]) && isxdigit(strNewNwkKey[5]) && isxdigit(strNewNwkKey[6]) && isxdigit(strNewNwkKey[7]) &&
     isxdigit(strNewNwkKey[8]) && isxdigit(strNewNwkKey[9]) && isxdigit(strNewNwkKey[10]) && isxdigit(strNewNwkKey[11]) &&
     isxdigit(strNewNwkKey[12]) && isxdigit(strNewNwkKey[13]) && isxdigit(strNewNwkKey[14]) && isxdigit(strNewNwkKey[15]) &&
     isxdigit(strNewNwkKey[16]) && isxdigit(strNewNwkKey[17]) && isxdigit(strNewNwkKey[18]) && isxdigit(strNewNwkKey[19]) &&
     isxdigit(strNewNwkKey[20]) && isxdigit(strNewNwkKey[21]) && isxdigit(strNewNwkKey[22]) && isxdigit(strNewNwkKey[23]) &&
     isxdigit(strNewNwkKey[24]) && isxdigit(strNewNwkKey[25]) && isxdigit(strNewNwkKey[26]) && isxdigit(strNewNwkKey[27]) &&
     isxdigit(strNewNwkKey[28]) && isxdigit(strNewNwkKey[29]) && isxdigit(strNewNwkKey[30]) && isxdigit(strNewNwkKey[31]))
  {
    uint8_t pNewNwkKey[16];
    for (uint8_t i=0;i<16;i++)
    {
      pNewNwkKey[i] = xdigit(strNewNwkKey[i*2]);
      pNewNwkKey[i] = pNewNwkKey[i]  << 4;
      pNewNwkKey[i] |= xdigit(strNewNwkKey[i*2 + 1]);
    }

    PRINTF_INFO("New AppKEY = ");
    PRINTF_INFO("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                pNewNwkKey[0], pNewNwkKey[1], pNewNwkKey[2], pNewNwkKey[3],
                pNewNwkKey[4], pNewNwkKey[5], pNewNwkKey[6], pNewNwkKey[7],
                pNewNwkKey[8], pNewNwkKey[9], pNewNwkKey[10], pNewNwkKey[11],
                pNewNwkKey[12], pNewNwkKey[13], pNewNwkKey[14], pNewNwkKey[15]);

    PlatformStatus.b.APP_LORA_ENABLED = 0;
    PlatformStatus.b.APP_LORA_JOINED = 0;
    PlatformStatus.b.APP_LORA_JOIN_COMPLETED = 0;
    SaveLoraNwkKey(pNewNwkKey);
  }
  else
  {
    PRINTF_INFO("Wrong AppKey format");
    nRet = 2;
  }
  return nRet;
}

void AstraGetStm32wlAppKeyString(char*str)
{
  extern uint8_t pAppKey[16];
  if(pAppKey[0]!=0 || pAppKey[1]!=0 || pAppKey[2]!=0 || pAppKey[3]!=0 ||
     pAppKey[4]!=0 || pAppKey[5]!=0 || pAppKey[6]!=0 || pAppKey[7]!=0 ||
     pAppKey[8]!=0 || pAppKey[9]!=0 || pAppKey[10]!=0 || pAppKey[11]!=0 ||
     pAppKey[12]!=0 || pAppKey[13]!=0 || pAppKey[14]!=0 || pAppKey[15]!=0 )
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
            pAppKey[0], pAppKey[1], pAppKey[2], pAppKey[3], pAppKey[4], pAppKey[5], pAppKey[6], pAppKey[7],
            pAppKey[8], pAppKey[9], pAppKey[10], pAppKey[11], pAppKey[12], pAppKey[13], pAppKey[14], pAppKey[15] );
  }
  else
  {
    strcpy(str, "- APP KEY IS NOT CONFIGURED YET-");
  }
}

void AstraGetLoRaAppKeyString(char*str)
{
  uint8_t EepromAppKey[16];
  int32_t nRet = LoadLoraAppKey(EepromAppKey);
  if(!nRet)
  {
    sprintf(str, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
            EepromAppKey[0], EepromAppKey[1], EepromAppKey[2], EepromAppKey[3], EepromAppKey[4], EepromAppKey[5], EepromAppKey[6], EepromAppKey[7],
            EepromAppKey[8], EepromAppKey[9], EepromAppKey[10], EepromAppKey[11], EepromAppKey[12], EepromAppKey[13], EepromAppKey[14], EepromAppKey[15] );
  }
  else
  {
    strcpy(str, "- APP KEY IS NOT CONFIGURED YET-");
  }
}

uint8_t AstraSetLoRaAppKey(char*strNewAppKey)
{
  uint8_t nRet = 0;
  if(isxdigit(strNewAppKey[0]) && isxdigit(strNewAppKey[1]) && isxdigit(strNewAppKey[2]) && isxdigit(strNewAppKey[3]) &&
     isxdigit(strNewAppKey[4]) && isxdigit(strNewAppKey[5]) && isxdigit(strNewAppKey[6]) && isxdigit(strNewAppKey[7]) &&
     isxdigit(strNewAppKey[8]) && isxdigit(strNewAppKey[9]) && isxdigit(strNewAppKey[10]) && isxdigit(strNewAppKey[11]) &&
     isxdigit(strNewAppKey[12]) && isxdigit(strNewAppKey[13]) && isxdigit(strNewAppKey[14]) && isxdigit(strNewAppKey[15]) &&
     isxdigit(strNewAppKey[16]) && isxdigit(strNewAppKey[17]) && isxdigit(strNewAppKey[18]) && isxdigit(strNewAppKey[19]) &&
     isxdigit(strNewAppKey[20]) && isxdigit(strNewAppKey[21]) && isxdigit(strNewAppKey[22]) && isxdigit(strNewAppKey[23]) &&
     isxdigit(strNewAppKey[24]) && isxdigit(strNewAppKey[25]) && isxdigit(strNewAppKey[26]) && isxdigit(strNewAppKey[27]) &&
     isxdigit(strNewAppKey[28]) && isxdigit(strNewAppKey[29]) && isxdigit(strNewAppKey[30]) && isxdigit(strNewAppKey[31]))
  {
    uint8_t pNewAppKey[16];
    for (uint8_t i=0;i<16;i++)
    {
      pNewAppKey[i] = xdigit(strNewAppKey[i*2]);
      pNewAppKey[i] = pNewAppKey[i]  << 4;
      pNewAppKey[i] |= xdigit(strNewAppKey[i*2 + 1]);
    }

    PRINTF_INFO("New AppKEY = ");
    PRINTF_INFO("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                pNewAppKey[0], pNewAppKey[1], pNewAppKey[2], pNewAppKey[3],
                pNewAppKey[4], pNewAppKey[5], pNewAppKey[6], pNewAppKey[7],
                pNewAppKey[8], pNewAppKey[9], pNewAppKey[10], pNewAppKey[11],
                pNewAppKey[12], pNewAppKey[13], pNewAppKey[14], pNewAppKey[15]);

    PlatformStatus.b.APP_LORA_ENABLED = 0;
    PlatformStatus.b.APP_LORA_JOINED = 0;
    PlatformStatus.b.APP_LORA_JOIN_COMPLETED = 0;
    SaveLoraAppKey(pNewAppKey);
  }
  else
  {
    PRINTF_INFO("Wrong AppKey format");
    nRet = 2;
  }
  return nRet;
}

char runningby[]="10000d 21h 45m 15s";
char *AstraRunningBy(void)
{
  uint32_t s = HAL_GetTick()/1000;
  uint32_t d = s/(24*3600);
  uint32_t h = (s%(24*3600))/3600;
  uint32_t m = (s%(3600))/60;
  s = (s%(60));
  sprintf(runningby, "%ldd %ldh %ldm %lds", d, h, m, s);
  return runningby;
}

uint8_t AstraGetBatteryStatus(void)
{
  if(AstraEngData.d.USBVoltage > 3000)
  {
    return 3;
  }
  else
  {
    return 1;
  }
}

/* return 0.1% battery level */
uint16_t AstraGetBatteryLevelPerc(void)
{
  uint32_t maxVoltage = 4200;
  uint32_t minVoltage = 3000;
  uint32_t battVoltage = AstraEngData.d.batteryVoltage;
  if(battVoltage > maxVoltage)battVoltage = maxVoltage;
  if(battVoltage < minVoltage)battVoltage = minVoltage;

  uint32_t percentage = (1000 * (uint32_t)(battVoltage - minVoltage));
  percentage = percentage / (maxVoltage - minVoltage);

  return (uint16_t)(0xFFFF&percentage);
}

uint8_t AstraGetUseCase(void)
{
  return PlatformStatus.s.ASTRA_ENGINE_SEL;
}

void BoardLedPlay(uint32_t nMillisec)
{
  uint32_t nLoopDuration = 500;
  for(uint32_t j=0;j<nMillisec/nLoopDuration; j++)
  {
    for(uint32_t i=0;i<nLoopDuration; i++)
    {
      if(i==0)AstraLedColor(ASTRA_LED_COLOR_GREEN);
      if(i==nLoopDuration/5)AstraLedColor(ASTRA_LED_COLOR_RED);
      if(i==2*nLoopDuration/5)AstraLedColor(ASTRA_LED_COLOR_BLUE);
      if(i==3*nLoopDuration/5)AstraLedColor(ASTRA_LED_COLOR_YELLOW);
      if(i==4*nLoopDuration/5)AstraLedColor(ASTRA_LED_COLOR_WHITE);
      if(i==nLoopDuration-1)AstraLedColor(ASTRA_LED_OFF);
      HAL_Delay(1);
    }
  }
}

/*
  "format": "enum_string",
  "name": "System status",
  0 -> "System in low power",
  1 -> "LoRa joined, GPS FIX OK",
  2 -> "LoRa joined, GPS NO FIX",
  3 -> "LoRa joined",
  4 -> "LoRa NOT joined, GPS FIX OK",
  5 -> "LoRa NOT joined, GPS NO FIX",
  6 -> "LoRa NOT joined",
*/
uint8_t AstraGetEnumSysStatus(void)
{
  if(PlatformStatus.s.xCurrentState == State_LowPower)
  {
    return 0;
  }
  else if(PlatformStatus.b.APP_LORA_JOINED)
  {
    if(PlatformStatus.b.GNSS_ENABLED)
    {
      if(AstraEngData.d.gnss_fix_validity)
      {
        return 1;
      }
      else
      {
        return 2;
      }
    }
    else
    {
      return 3;
    }
  }
  else
  {
    if(PlatformStatus.b.GNSS_ENABLED)
    {
      if(AstraEngData.d.gnss_fix_validity)
      {
        return 4;
      }
      else
      {
        return 5;
      }
    }
    else
    {
      return 6;
    }
  }
}

/* "format": "enum_icon",
  1 LoRa joined,
  2 LoRa NOT joined,
  3 GPS fix OK,
  4 GPS no fix,
  5 GPS disabled,
  6 System in low power,
  7 NFC enabled */
uint8_t AstraGetEnumSysStatusIcon(void)
{
  if(PlatformStatus.b.APP_LORA_JOIN_COMPLETED)
  {
    if(PlatformStatus.b.APP_LORA_JOINED)
    {
      return 1;
    }
    else
    {
      return 2;
    }
  }
  else
  {
    if(AstraEngData.d.gnss_fix_validity)
    {
      return 3;
    }
    else
    {
      return 4;
    }
  }
}

void UpdateSystemStatus(void)
{
  /* check that this function is not called so often, e.g. 1min for battery level update
  Immediate call for change on:
  - use case selection change
  - lora join status
  - gps fix status
  */
#if USE_BLE
  if(BLE_StackValue.BoardName[0] != 0)
  {
    if(!PlatformStatus.b.BLE_CONNECTED)
    {
      PRINTF_VERBOSE("setConnectable() (RTC_BKP_BLE_HEALTH %d)\r\n", Astra_BLE_GetBootFailureNumber());
      updateAdvData();
    }
  }
  else
  {
    //ble not configured after boot, reset the board
    PRINTF_VERBOSE("Ble not configured, reboot (%d)\r\n", Astra_BLE_GetBootFailureNumber());
    Astra_BLE_IncreaseBootFailureNumber();
    BoardReboot();
  }
#endif //USE_BLE
}

void AstraGetMacAddress(uint8_t *BleMacAddress)
{
#if USE_BLE
  memcpy(BleMacAddress, BLE_StackValue.BleMacAddress, 6);
#endif //USE_BLE
}

void AstraGetSecurePin(uint32_t *securepin)
{
#if USE_BLE
  *securepin = BLE_StackValue.SecurePIN;
#else //USE_BLE
  *securepin = 0;
#endif //USE_BLE
}

#if !USE_STM32WL
void STM32WL_LowPower(){}
int32_t SaveLoraAppEui(uint8_t *PtrAppEui){return 1;}
int32_t SaveLoraNwkKey(uint8_t *PtrNwkKEY){return 1;}
int32_t SaveLoraAppKey(uint8_t *PtrAppKey){return 1;}
int32_t LoadLoraAppEui(uint8_t *pTmpkey){return 1;}
int32_t LoadLoraNwkKey(uint8_t *pTmpkey){return 1;}
int32_t LoadLoraAppKey(uint8_t *pTmpkey){return 1;}
#endif //USE_STM32WL

#if !USE_ASTRA_MAINBOARD_BSP
void SystemShutdown(){}
#endif //USE_ASTRA_MAINBOARD_BSP

void pause(uint32_t ms)
{
  for(uint32_t i = 0; i< 3000 * ms; i++)
  {
    __NOP();
  }
}

void Astra_Error_Handler(void)
{
  while(1)
  {
    AstraLedColor(ASTRA_LED_COLOR_YELLOW);
    HAL_Delay(500);
    AstraLedColor(ASTRA_LED_OFF);
    HAL_Delay(500);
#if USE_ASTRA_MAINBOARD_BSP
    if(custom_astra_btn_get_state(ASTRA_BTN1) == ENABLE)
    {
      break;
    }
#endif //USE_ASTRA_MAINBOARD_BSP
 }
}

/* USER CODE BEGIN CLI_FUNCTIONS */

/* Add your code here */

/* USER CODE END CLI_FUNCTIONS */

/**
* @} SYSTEM_MANAGEMENT_Private_Functions
*/

/**
* @} SYSTEM_MANAGEMENT
*/

/**
* @} ASTRA_ENGINE
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
