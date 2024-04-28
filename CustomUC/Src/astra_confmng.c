/**
  ******************************************************************************
  * @file    astra_confmng.c
  * @author  SRA Team
  * @brief   This file contains the Astra configuration management
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

/** @addtogroup CONFIGURATION Configuration
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "astra_confmng.h"
#include "astra_datamng.h"
#include "astra_sysmng.h"

#if USE_MEMORY
#include "astra_app_nfc.h"
#include "SmartNFCType.h"
#include "SmartNFC_config.h"
#endif //USE_USB

#if USE_USB
#include "usb_device.h"
#include "usbd_cdc_if.h"
#endif //USE_USB

#if USE_BLE
#include "BLE_Manager.h"
#endif //USE_BLE

#if USE_GNSS
#include "teseo_liv3f_conf.h"
#include "custom_gnss.h"
#undef TRUE
#undef FALSE
#include "gnss_data.h"
#endif //USE_GNSS

/* USE_STM32WL */

#if USE_STM32WL
#include "custom_modem_control.h"
#endif //USE_STM32WL

#if USE_STSAFE
#include "stsafea_types.h"
#include "stsafea_core.h"
#include "stsafea_service.h"
#include "pairing.h"
#endif //USE_STSAFE

/* USE_BLE */
//for MX_APPE_Init()
#include "app_entry.h"
#include "app_conf.h"

/** @addtogroup ASTRA_CONFMNG_PRIVATE_VARIABLES ASTRA Configuration management - Private variables
  * @brief Private variables
  * @{
 */

/* Private variables ---------------------------------------------------------*/

/* Platform configuration structure */
PlatformStatus_t PlatformStatus = { 0 };
NO_INIT(time_t AstraDeltaDateTimeStamp);
NO_INIT(uint8_t AstraDeltaDateTimeStampValid1);
NO_INIT(uint8_t AstraDeltaDateTimeStampValid2);


SystemConfParams_t SystemConfParams = { 0 };

#if USE_STSAFE
  StSafeA_Handle_t stsafea_handle;
  /* STSAFE MW requires a data buffer to send/receive bytes ove the bus.
     For memory optimization reasons it's up to the application to allocate it,
     so that the application can deallocate it when STSAFE services are not required anymore */
  uint8_t a_rx_tx_stsafea_data [STSAFEA_BUFFER_MAX_SIZE];

#if ENABLE_STSAFE_ECHO
  StSafeA_LVBuffer_t echo_out;
  uint8_t text_in[STSAFEA_BUFFER_MAX_SIZE] = "Text string to test the STSAFE-A";
  uint8_t text_out[STSAFEA_BUFFER_MAX_SIZE] = "\0";
#endif //ENABLE_STSAFE_ECHO
#endif //USE_STSAFE

/* USB variables */
#define CUSTOM_USB_COMMAND_OFFSET       0

#if USE_USB
uint8_t USBWelcomeMessage[]="\r\n" TEXT_COLOR_1 "STMicroelectronics STEVAL-ASTRA1B\n\rFirmware version 2.0" DEFAULT_TEXT_COLOR "\r\n\r\n";
static uint8_t pcUSBUserRxBuffer[USB_USER_RX_BUFFER_SIZE + 1] = { 0 };
static uint32_t nUsbBytesReceived = 0;
#endif //USE_USB

/**
  * @}
  */

/** @addtogroup ASTRA_CONFMNG_FunctionPrototypes ASTRA Configuration management - Function Prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -----------------------------------------------*/
#if USE_ASTRA_MAINBOARD_BSP

/**
  * @brief This function manages the BTN1 button pushing event
  * @param  none
  * @retval none
  */
void Astra_Button1Pushed(void);

/**
  * @brief This function manages the BTN2 button pushing event
  * @param  none
  * @retval none
  */
void Astra_Button2Pushed(void);

/**
  * @brief This function manages the BTN1 button long pushing event
  * @param  none
  * @retval none
  */
void Astra_Button1PushedLong(void);

/**
  * @brief This function manages the BTN2 button long pushing event
  * @param  none
  * @retval none
  */
void Astra_Button2PushedLong(void);

#if USE_SEQUENCER
/**
  * @brief This function manages the BTN1 button pushing timer elapsed event
  * @param  none
  * @retval none
  */
void Astra_Button1TimerCallback(void);

/**
  * @brief This function manages the BTN2 button pushing timer elapsed event
  * @param  none
  * @retval none
  */
void Astra_Button2TimerCallback(void);
#endif //USE_SEQUENCER

#if USE_BTN1_ANTI_TAMPER
/**
  * @brief This function handles the anti tampering glitch event.
  * @param  none
  * @retval none
  */
void Astra_AntiTamperingGlitch (void);

/**
  * @brief This function handles the anti tampering activated event.
  * @param  none
  * @retval none
  */
void Astra_AntiTamperingActivated(void);

/**
  * @brief This function handles the anti tampering restored event.
  * @param  none
  * @retval none
  */
void Astra_AntiTamperingRestored(void);
#else //USE_BTN1_ANTI_TAMPER

/**
  * @brief This function manages the BTN1 button pushing timer start
  * @param  none
  * @retval none
  */
void Astra_Button1PushTimerStart(void);

/**
  * @brief This function manages the BTN1 button pushing timer stop
  * @param  none
  * @retval none
  */
void Astra_Button1PushTimerStop(void);
#endif //USE_BTN1_ANTI_TAMPER

/**
  * @brief This function manages the BTN2 button pushing timer start
  * @param  none
  * @retval none
  */
void Astra_Button2PushTimerStart(void);

/**
  * @brief This function manages the BTN2 button pushing timer stop
  * @param  none
  * @retval none
  */
void Astra_Button2PushTimerStop(void);

#endif //USE_ASTRA_MAINBOARD_BSP

#if USE_USB
/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Astra_CDC_Receive_FS(uint8_t* Buf, uint32_t *Len);
#endif //USE_USB

/**
  * @brief  Process terminal string
  * @param  none
  * @retval none
  */
void CUSTOM_USB_ProcessTerminalString(void);

/**
  * @brief  USB manager
  * @param  none
  * @retval none
  */
void custom_USB_Manager(void);

/**
  * @}
  */

/** @addtogroup ASTRA_CONFMNG_FunctionDefines ASTRA BLE - Function Defines
  * @brief Function definitions
  * @{
 */

/* Function Definitions ------------------------------------------------------*/

void InitAstraDeltatime(void)
{
  if(AstraDeltaDateTimeStampValid1 == 0xA5 && AstraDeltaDateTimeStampValid2 == 0xA6)
  {
    PlatformStatus.s.AstraDeltaDateTimeStamp = AstraDeltaDateTimeStamp;
  }
  else
  {
    /* TODO: add default time setting */
  }
}

uint8_t AstraSetTimeStamp(RTC_TimeTypeDef CurTime, RTC_DateTypeDef CurDate)
{
  extern RTC_HandleTypeDef hrtc;
  
  RTC_TimeTypeDef RtcTime;
  RTC_DateTypeDef RtcDate;
  struct tm rtcTime;
  struct tm currTime;
  time_t rtcTimeStamp;
  time_t curTimeStamp;

  
  if(HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN) != HAL_OK) {
    return 0;
  }

  if(HAL_RTC_GetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN) != HAL_OK) {
    return 0;
  }
  
  rtcTime.tm_year = RtcDate.Year;
  rtcTime.tm_mday = RtcDate.Date;
  rtcTime.tm_mon  = RtcDate.Month;
  rtcTime.tm_wday = RtcDate.WeekDay;
  rtcTime.tm_hour = RtcTime.Hours;
  rtcTime.tm_min  = RtcTime.Minutes;
  rtcTime.tm_sec  = RtcTime.Seconds;
  rtcTime.tm_isdst = -1;
  
  PRINTF_INFO("\r\nRTC");
  PRINTF_INFO("\r\n\tYear    =%d",rtcTime.tm_year);
  PRINTF_INFO("\r\n\tDate    =%d",rtcTime.tm_mday);
  PRINTF_INFO("\r\n\tMonth   =%d",rtcTime.tm_mon);
  PRINTF_INFO("\r\n\twDay    =%d",rtcTime.tm_wday);
  PRINTF_INFO("\r\n\tHours   =%d",rtcTime.tm_hour);
  PRINTF_INFO("\r\n\tMinutes =%d",rtcTime.tm_min);
  PRINTF_INFO("\r\n\tSeconds =%d",rtcTime.tm_sec);
  PRINTF_INFO("\r\n");
  PRINTF_VERBOSE("\r\nRTC: ");
  PRINTF_VERBOSE("%.2d/%.2d/%.2d - ", rtcTime.tm_mon, rtcTime.tm_mday, rtcTime.tm_year );
  PRINTF_VERBOSE("%.2d:%.2d:%.2d\r\n", rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec );
  
  /* Calculate RTC Epoch Time */
  rtcTimeStamp = mktime(&rtcTime);
  
  currTime.tm_year = CurDate.Year;
  currTime.tm_mday = CurDate.Date;
  currTime.tm_mon  = CurDate.Month;
  currTime.tm_wday = CurDate.WeekDay;
  currTime.tm_hour = CurTime.Hours;
  currTime.tm_min  = CurTime.Minutes;
  currTime.tm_sec  = CurTime.Seconds;
  currTime.tm_isdst = -1;
  
  PRINTF_INFO("\r\nCurrent time");
  PRINTF_INFO("\r\n\tYear    =%d",currTime.tm_year);
  PRINTF_INFO("\r\n\tDate    =%d",currTime.tm_mday);
  PRINTF_INFO("\r\n\tMonth   =%d",currTime.tm_mon);
  PRINTF_INFO("\r\n\twDay    =%d",currTime.tm_wday);
  PRINTF_INFO("\r\n\tHours   =%d",currTime.tm_hour);
  PRINTF_INFO("\r\n\tMinutes =%d",currTime.tm_min);
  PRINTF_INFO("\r\n\tSeconds =%d",currTime.tm_sec);
  PRINTF_INFO("\r\n");
  PRINTF_VERBOSE("\r\nCurrent time: ");
  PRINTF_VERBOSE("%.2d/%.2d/%.2d - ", currTime.tm_mon, currTime.tm_mday, currTime.tm_year );
  PRINTF_VERBOSE("%.2d:%.2d:%.2d\r\n", currTime.tm_hour, currTime.tm_min, currTime.tm_sec );
  
  /* Find the Epoch Time */
  curTimeStamp = mktime(&currTime);

  PlatformStatus.s.AstraDeltaDateTimeStamp = curTimeStamp - rtcTimeStamp;

  AstraDeltaDateTimeStamp = PlatformStatus.s.AstraDeltaDateTimeStamp;
  
  AstraDeltaDateTimeStampValid1 = 0xA5;
  AstraDeltaDateTimeStampValid2 = 0xA6;
  
  return 0;
}

time_t AstraGetTimeStamp(void)
{
  extern RTC_HandleTypeDef hrtc;
  
  RTC_TimeTypeDef CurTime;
  RTC_DateTypeDef CurDate;
  time_t AstraTimeStamp;
  struct tm currTime;
  struct tm *currTime2;

  if(HAL_RTC_GetTime(&hrtc, &CurTime, RTC_FORMAT_BIN) != HAL_OK) {
    return 0;
  }

  if (HAL_RTC_GetDate(&hrtc, &CurDate, RTC_FORMAT_BIN) != HAL_OK) {
    return 0;
  }
  
  currTime.tm_year = CurDate.Year;
  currTime.tm_mday = CurDate.Date;
  currTime.tm_mon  = CurDate.Month;
  currTime.tm_wday = CurDate.WeekDay;
  currTime.tm_hour = CurTime.Hours;
  currTime.tm_min  = CurTime.Minutes;
  currTime.tm_sec  = CurTime.Seconds;
  currTime.tm_isdst = -1;

  PRINTF_INFO("\r\nCurrent time");
  PRINTF_INFO("\r\n\tYear    =%d",currTime.tm_year);
  PRINTF_INFO("\r\n\tDate    =%d",currTime.tm_mday);
  PRINTF_INFO("\r\n\tMonth   =%d",currTime.tm_mon);
  PRINTF_INFO("\r\n\twDay    =%d",currTime.tm_wday);
  PRINTF_INFO("\r\n\tHours   =%d",currTime.tm_hour);
  PRINTF_INFO("\r\n\tMinutes =%d",currTime.tm_min);
  PRINTF_INFO("\r\n\tSeconds =%d",currTime.tm_sec);
  PRINTF_INFO("\r\n");
  PRINTF_INFO("Current RTC time: ");
  PRINTF_INFO("%.2d/%.2d/%.2d - ", currTime.tm_mon, currTime.tm_mday, currTime.tm_year );
  PRINTF_INFO("%.2d:%.2d:%.2d\r\n", currTime.tm_hour, currTime.tm_min, currTime.tm_sec );

  
  /* Find the Epoch Time */
  AstraTimeStamp = mktime(&currTime);
  AstraTimeStamp += PlatformStatus.s.AstraDeltaDateTimeStamp;
  
  currTime2 = localtime(&AstraTimeStamp);
  PRINTF_INFO("Current time: ");
  PRINTF_INFO("%.2d/%.2d/%.2d - ", currTime2->tm_mon, currTime2->tm_mday, currTime2->tm_year );
  PRINTF_INFO("%.2d:%.2d:%.2d\r\n", currTime2->tm_hour, currTime2->tm_min, currTime2->tm_sec );
  
  return AstraTimeStamp;
}

/* LEDs functions */

void AstraToggleBlueLed()
{
  custom_sob_led_r_off();
  custom_sob_led_g_off();
  custom_sob_led_b_toggle();
}

void AstraLedColor(uint32_t nColor)
{
  custom_sob_led_r_off();
  custom_sob_led_g_off();
  custom_sob_led_b_off();
  if(nColor&ASTRA_LED_COLOR_RED)custom_sob_led_r_on();
  if(nColor&ASTRA_LED_COLOR_GREEN)custom_sob_led_g_on();
  if(nColor&ASTRA_LED_COLOR_BLUE)custom_sob_led_b_on();
}

void AstraLedToggle(uint32_t nColor)
{
  if(nColor&ASTRA_LED_COLOR_RED)custom_sob_led_r_toggle();
  if(nColor&ASTRA_LED_COLOR_GREEN)custom_sob_led_g_toggle();
  if(nColor&ASTRA_LED_COLOR_BLUE)custom_sob_led_b_toggle();
}

/* USB functions */
#if USE_USB
void AstraUsbSuspendCallback()
{
  PlatformStatus.b.USB_TRACE = 0;
  PlatformStatus.b.USB_RESUMED = RESET;
  PlatformStatus.b.USB_VCP_OPENED = RESET;
  PlatformStatus.b.USB_WELCOME_MESSAGE_PENDING = RESET;

  if(PlatformStatus.b.ADC_INIZIALIZED)
  {
    //check usb adc value
    if(AstraEngData.d.USBVoltage < 4000)
    {
      PlatformStatus.b.USB_INIZIALIZED = RESET;
    }
  }

  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
}

void AstraUsbResumeCallback()
{
  PlatformStatus.b.USB_TRACE = 1;
  PlatformStatus.b.USB_RESUMED = SET;
  PlatformStatus.b.USB_VCP_OPENED = SET;
  PlatformStatus.b.USB_WELCOME_MESSAGE_PENDING = SET;

  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
}

uint8_t CUSTOM_USB_SendMessage(uint8_t* pBuf, uint16_t Lenght, uint32_t AttemptsNumber)
{
  USBD_StatusTypeDef temp_usbd_status = USBD_BUSY;

  if (PlatformStatus.b.USB_VCP_OPENED)
  {
    if (AttemptsNumber != 0xFFFFFFFF)
    {
      while ((temp_usbd_status == USBD_BUSY) && (AttemptsNumber > 0))
      {
        temp_usbd_status = (USBD_StatusTypeDef) CDC_Transmit_FS(pBuf, Lenght);
        AttemptsNumber--;
      }
    }
    else
    {
      while (temp_usbd_status == USBD_BUSY)
      {
        temp_usbd_status = (USBD_StatusTypeDef) CDC_Transmit_FS(pBuf, Lenght);
      }
    }
  }

  return (temp_usbd_status != USBD_OK);
}

static int8_t Astra_CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  extern USBD_HandleTypeDef hUsbDeviceFS;
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  if (*Len > USB_USER_RX_BUFFER_SIZE)
  {
    memcpy((char*)pcUSBUserRxBuffer, (char*)Buf, USB_USER_RX_BUFFER_SIZE);
    PRINTF_INFO("USB Rx buffer size exceeded\r\n");
  }
  else
  {
    if (*Len > USB_USER_RX_BUFFER_SIZE - nUsbBytesReceived)
    {
      memcpy((char*)(pcUSBUserRxBuffer+nUsbBytesReceived), (char*)Buf, (USB_USER_RX_BUFFER_SIZE - nUsbBytesReceived));
      nUsbBytesReceived = USB_USER_RX_BUFFER_SIZE;
    }
    else
    {
      memcpy((char*)(pcUSBUserRxBuffer+nUsbBytesReceived), (char*)Buf, (*Len));
      nUsbBytesReceived += (*Len);
    }
  }

  if (PlatformStatus.b.USB_DBG == DISABLE)
  {
    AstraDisablePrintUsb();
  }

  return (USBD_OK);
}

void CUSTOM_USB_ProcessTerminalString(void)
{
  uint8_t index;

  if (pcUSBUserRxBuffer[0] == 0)
  {
    return;
  }
  else
  {
    if( nUsbBytesReceived >= USB_USER_RX_BUFFER_SIZE)
    {
      nUsbBytesReceived = USB_USER_RX_BUFFER_SIZE;
      pcUSBUserRxBuffer[USB_USER_RX_BUFFER_SIZE - 1] = '\r';
    }

    if( (pcUSBUserRxBuffer[nUsbBytesReceived - 1] != '\r') && (pcUSBUserRxBuffer[nUsbBytesReceived - 1] != '\n') )
    {

    }
    else
    {
      index = 0;
      do
      {
        index++ ;
      } while ((pcUSBUserRxBuffer[nUsbBytesReceived - index] == '\r') || (pcUSBUserRxBuffer[nUsbBytesReceived - index] == '\n'));
      pcUSBUserRxBuffer[nUsbBytesReceived - index + 1] = 0;

      AstraProcessCommand((pcUSBUserRxBuffer+CUSTOM_USB_COMMAND_OFFSET), strlen((char*)(pcUSBUserRxBuffer+CUSTOM_USB_COMMAND_OFFSET)));

      memset(pcUSBUserRxBuffer, 0x00, USB_USER_RX_BUFFER_SIZE);
      nUsbBytesReceived = 0;
    }
  }
}

void custom_USB_Manager(void)
{
  if(PlatformStatus.b.USB_INIZIALIZED)
  {
    extern USBD_HandleTypeDef hUsbDeviceFS;
    if(hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
    {
      if(!PlatformStatus.b.USB_RESUMED)AstraUsbResumeCallback();

      if (PlatformStatus.b.USB_WELCOME_MESSAGE_PENDING == SET)
      {
        HAL_Delay(100);
        if(!CUSTOM_USB_SendMessage(USBWelcomeMessage, sizeof(USBWelcomeMessage), CUSTOM_USB_DEFAULT_ATTEMPS) )
        {
          PlatformStatus.b.USB_WELCOME_MESSAGE_PENDING = RESET;

          //register callback for data RX
          USBD_Interface_fops_FS.Receive = Astra_CDC_Receive_FS;
        }
      }

      CUSTOM_USB_ProcessTerminalString();
    }
    else
    {
      if(PlatformStatus.b.USB_RESUMED)AstraUsbSuspendCallback();
    }
  }
  else
  {
    //check if initialization needed
    if(PlatformStatus.b.ADC_INIZIALIZED)
    {
      //check usb adc value
      if(AstraEngData.d.USBVoltage > 4000)
      {
        MX_USB_Device_Init();

        /* wait usb configured */
        HAL_Delay(2000);

        PlatformStatus.b.USB_INIZIALIZED = 1;
      }
    }
    else
    {
      //read USB ADC pin logic level
    }
  }
}
#endif //USE_USB

/* DEBUG functions */
void debug_init(void)
{
#if (CFG_DEBUGGER_SUPPORTED == 1)
  /**
   * Keep debugger enabled while in any low power mode
   */
  HAL_DBGMCU_EnableDBGSleepMode();
  HAL_DBGMCU_EnableDBGStopMode();

  /***************** ENABLE DEBUGGER *************************************/
  LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_48);

#else //(CFG_DEBUGGER_SUPPORTED == 1)
  GPIO_InitTypeDef gpio_config = {0};

  gpio_config.Pull = GPIO_NOPULL;
  gpio_config.Mode = GPIO_MODE_ANALOG;

  gpio_config.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_Init(GPIOA, &gpio_config);
  __HAL_RCC_GPIOA_CLK_DISABLE();

//  gpio_config.Pin = GPIO_PIN_4 | GPIO_PIN_3;
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//  HAL_GPIO_Init(GPIOB, &gpio_config);
//  __HAL_RCC_GPIOB_CLK_DISABLE();

  HAL_DBGMCU_DisableDBGSleepMode();
  HAL_DBGMCU_DisableDBGStopMode();
  HAL_DBGMCU_DisableDBGStandbyMode();
#endif //(CFG_DEBUGGER_SUPPORTED == 1)

#if USE_USB
  /* run once usb manager */
  custom_USB_Manager();

  custom_USB_Manager();

  SystemConfParams.PrintLevelActive = LEVEL_ACTIVE_DEFAULT;
#endif //USE_USB

#if ENABLE_IAR_TERMINAL_IO
  PlatformStatus.b.DEBUGGER_CONNECTED = 1;
#endif //ENABLE_IAR_TERMINAL_IO
}

/* SENSORS functions */
#if USE_SENSORS

/**
  * @brief  Low Power chip select initialization
  * @param  none
  * @retval none
  */
void custom_astra_lp_acc_cs_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(CUSTOM_LIS2DTW12_0_CS_PORT, CUSTOM_LIS2DTW12_0_CS_PIN, GPIO_PIN_SET);

  /*Configure GPIO pins : CUSTOM_LIS2DTW12_0_CS_PIN */
  GPIO_InitStruct.Pin = CUSTOM_LIS2DTW12_0_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CUSTOM_LIS2DTW12_0_CS_PORT, &GPIO_InitStruct);
}

void sensor_init(void)
{
  PRINTF_INFO("sensor_init()\r\n");

  custom_astra_lp_acc_cs_init();

  BSP_SENSOR_ACC_Init();
  BSP_SENSOR_LP_ACC_Init();
  BSP_SENSOR_GYR_Init();
  BSP_SENSOR_PRESS_Init();
  BSP_SENSOR_TEMP_Init();
  BSP_SENSOR_HUM_Init();

  BSP_SENSOR_ACC_SetOutputDataRate(ACC_ODR);
  BSP_SENSOR_ACC_SetFullScale(ACC_FS);

  BSP_SENSOR_LP_ACC_SetOutputDataRate(ACC_ODR);
  BSP_SENSOR_LP_ACC_SetFullScale(ACC_FS);

  BSP_SENSOR_ACC_Enable_Wake_Up_Detection();
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_sens_interruptpin2_init();
#endif //USE_ASTRA_MAINBOARD_BSP

  sensor_lp_inactivity_detect_init();
}

void sensor_lp_lowPerformance(void)
{
  BSP_SENSOR_LP_ACC_SetOutputDataRate(12.5);
}

void sensor_lp_disable(void)
{
  BSP_SENSOR_LP_ACC_Disable();
}

void sensor_lp_algo_de_init(void)
{
  BSP_SENSOR_LP_ACC_Disable_Inactivity_Detection();
  BSP_SENSOR_LP_ACC_Disable_All_On_INT1();
  BSP_SENSOR_LP_ACC_Disable_Wake_Up_Detection();
  BSP_SENSOR_LP_ACC_Clear_Wkp();
}

/**
  * @brief  Sensor lp wakeup detect init
  * @param  none
  * @retval none
  */
void sensor_lp_wakeup_detect_init(void)
{
  sensor_lp_algo_de_init();
  BSP_SENSOR_LP_ACC_Enable_Wake_Up_Detection();
  BSP_SENSOR_LP_ACC_Set_Wake_Up_Threshold( LP_ACC_WKP_THS);
  BSP_SENSOR_LP_ACC_Set_Wake_Up_Duration( LP_ACC_WKP_DUR);

  BSP_SENSOR_LP_ACC_Clear_Wkp();
  BSP_SENSOR_LP_ACC_Enable_Interrupt_Latch();
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_lp_acc_interruptpin_init();
#endif //USE_ASTRA_MAINBOARD_BSP
}

/**
  * @brief  Sensor lp inactivity detect init
  * @param  none
  * @retval none
  */
void sensor_lp_inactivity_detect_init(void)
{
  sensor_lp_algo_de_init();
  BSP_SENSOR_LP_ACC_Enable_Inactivity_Detection();
  BSP_SENSOR_LP_ACC_Set_Wake_Up_Threshold( LP_ACC_WKP_THS);
  BSP_SENSOR_LP_ACC_Set_Sleep_Duration(LP_ACC_SLEEP_DUR);
  BSP_SENSOR_LP_ACC_Enable_All_On_INT1();

  BSP_SENSOR_LP_ACC_Clear_Wkp();
  BSP_SENSOR_LP_ACC_Enable_Interrupt_Latch();
#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_lp_acc_interruptpin_init();
#endif //USE_ASTRA_MAINBOARD_BSP
}
#endif //USE_SENSORS

#if USE_GNSS
/* GNSS functions */

void gnss_init(void)
{
  PRINTF_INFO("gnss_init()\r\n");

  CUSTOM_GNSS_Init(CUSTOM_TESEO_LIV3F);

  GNSS_DataInit();

  HAL_Delay(1000);

  //configuration commands
  GnssRequestVersion();
}
#endif //USE_GNSS

/* STSAFE */

unsigned char CertFromSTsafe[1024];

void store_host_keys(void *vBuf, uint32_t Len)
{
  uint8_t *Buf = (uint8_t *)vBuf;
  uint32_t index = 0;
  PRINTF_INFO("\r\n Sample function to simulate data storage\r\nOutput on terminal\r\n");
  for(index = 0;index<Len/16;index++)
  {
    PRINTF_INFO("%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X\r\n", Buf[16*index], Buf[16*index+1], Buf[16*index+2], Buf[16*index+3],
                                                                                        Buf[16*index+4], Buf[16*index+5], Buf[16*index+6], Buf[16*index+7],
                                                                                        Buf[16*index+8], Buf[16*index+9], Buf[16*index+10], Buf[16*index+11],
                                                                                        Buf[16*index+12], Buf[16*index+13], Buf[16*index+14], Buf[16*index+15]);
  }
  index *= 16;
  if(index<Len)
  {
    for( ;index<Len;index++)
    {
      PRINTF_INFO("%.2X", Buf[index]);
    }
  }

}

#if USE_STSAFE
uint8_t StSafeInit_HAL(void *handle_se)
{
  uint8_t StatusCode = STSAFEA_BUS_OK;
  StSafeA_HostKeySlotBuffer_t HostKeySlot;

  PRINTF_INFO("\r\nCheck if Pairing Host keys available \r\n");

  if(StatusCode == 0)
  {
    StatusCode = StSafeA_HostKeySlotQuery(handle_se, &HostKeySlot, STSAFEA_MAC_NONE);
  }

  if (HostKeySlot.HostKeyPresenceFlag == 0U)      /*  Not populated */
  {
    PRINTF_INFO("\r\nHost key is not set, personalization of STSAFE-A has never been done before !  \r\n");
    return (STSAFEA_KEY_NOT_FOUND);
  }
  else
  {
    PRINTF_INFO("\r\nHost Key is set : STSAFEA_BUS_OK \r\n");
  }

  if (StatusCode != STSAFEA_BUS_OK)
  {
    PRINTF_INFO("\r\nERROR: StatusCode =0x%x \n\r", (uint8_t)StatusCode);
    return ((uint8_t)STSAFEA_BUS_ERR);
  }
  else
  {
    return (STSAFEA_BUS_OK);
  }

}

uint8_t StSafeCheck(void)
{
  uint8_t ret = 1;
  ret = StSafeInit_HAL(&stsafea_handle);
  return ret;
}

uint8_t StSafePairing(void)
{
  uint8_t ret = 1;
  ret = stsafe_pairing(&stsafea_handle);
  return ret;
}

int32_t GetCertificateSize(StSafeA_Handle_t* handle, uint16_t *pCertSize)
{
  #define NUMBER_OF_BYTES_TO_GET_CERTIFICATE_SIZE              4
  uint8_t idx = 0;
  int32_t                       StatusCode = 0;
  uint16_t                      CertificateSize = 0;
   StSafeA_LVBuffer_t sts_read;
#if (!STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
  uint8_t data_sts_read [NUMBER_OF_BYTES_TO_GET_CERTIFICATE_SIZE];
  sts_read.Length = NUMBER_OF_BYTES_TO_GET_CERTIFICATE_SIZE;
  sts_read.Data = data_sts_read;
#endif

  PRINTF_INFO("\n\r %d. Get size of certificate stored through STSAFE-A's zone 0", ++idx);
  PRINTF_INFO("\n\r    %d.1 Read %d bytes of certificate through STSAFE-A's zone 0", idx, NUMBER_OF_BYTES_TO_GET_CERTIFICATE_SIZE);
  PRINTF_INFO("\n\r        => Use StSafeA_Read API");
  if (StatusCode == 0)
  {
    StatusCode = (int32_t)StSafeA_Read(handle, 0, 0, STSAFEA_AC_ALWAYS, 0, 0,
                                            NUMBER_OF_BYTES_TO_GET_CERTIFICATE_SIZE,
                                            NUMBER_OF_BYTES_TO_GET_CERTIFICATE_SIZE,
                                            &sts_read, STSAFEA_MAC_NONE);
  }

  if (StatusCode == 0)
  {
    switch (sts_read.Data[1])
    {
      case 0x81U:
        CertificateSize = (uint16_t)sts_read.Data[2] + 3U;
        break;

      case 0x82U:
        CertificateSize = (((uint16_t)sts_read.Data[2]) << 8) + sts_read.Data[3] + 4U;
        break;

      default:
        if (sts_read.Data[1] < 0x81U)
        {
          CertificateSize = sts_read.Data[1];
        }
        break;
    }
    PRINTF_INFO("\n\r    %d.2 Size of certificate stored through STSAFE-A's zone 0 is %d bytes", idx, CertificateSize);

    if (CertificateSize == 0)
    {
      return 1;
    }
  }

  *pCertSize = CertificateSize;

  return StatusCode;
}

int32_t ExtractCertificate(StSafeA_Handle_t* handle, uint16_t CertificateSize, StSafeA_LVBuffer_t *psts_read_cert)
{
  int32_t                       StatusCode = 0;
  uint8_t idx = 0;

  PRINTF_INFO("\n\r %d. Extract certificate stored through STSAFE-A's zone 0", ++idx);
  PRINTF_INFO("\n\r    %d.1 Read %d bytes through STSAFE-A's zone 0 corresponding to IoT certificate", idx, CertificateSize);
  PRINTF_INFO("\n\r        => Use StSafeA_Read API");

  if (StatusCode == 0)
  {
    StatusCode = (int32_t)StSafeA_Read(handle, 0, 0, STSAFEA_AC_ALWAYS, 0, 0, CertificateSize,
                                       CertificateSize, psts_read_cert, STSAFEA_MAC_NONE);
  }

  if (StatusCode == 0)
  {
    PRINTF_INFO("\n\r    %d.2 certificate extracted from STSAFE-A's zone 0\r\n", idx);
  } else {
    PRINTF_INFO("\n\r    %d.2 Error certificate extracted from STSAFE-A's zone 0\r\n", idx);
    return StatusCode;
  }

  if(StatusCode==0U) {
    PRINTF_INFO("\n\r %d.  .der", ++idx);
    PRINTF_INFO("\r\n-----BEGIN DER CERTIFICATE-----\r\n");

    uint32_t index = 0;
    for(index = 0;index<psts_read_cert->Length/16;index++)
    {
      PRINTF_INFO("%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X\r\n",  psts_read_cert->Data[16*index], psts_read_cert->Data[16*index+1], psts_read_cert->Data[16*index+2], psts_read_cert->Data[16*index+3],
                  psts_read_cert->Data[16*index+4], psts_read_cert->Data[16*index+5], psts_read_cert->Data[16*index+6], psts_read_cert->Data[16*index+7],
                  psts_read_cert->Data[16*index+8], psts_read_cert->Data[16*index+9], psts_read_cert->Data[16*index+10], psts_read_cert->Data[16*index+11],
                  psts_read_cert->Data[16*index+12], psts_read_cert->Data[16*index+13], psts_read_cert->Data[16*index+14], psts_read_cert->Data[16*index+15]);
    }
    index *= 16;
    if(index<psts_read_cert->Length)
    {
      for( ;index<psts_read_cert->Length;index++)
      {
        PRINTF_INFO("%.2X", psts_read_cert->Data[index]);
      }
    }

    PRINTF_INFO("\r\n-----END DER CERTIFICATE-----\r\n");
  } else {
    PRINTF_INFO("\n\r %d. Error Converting in base64 .pem", ++idx);
  }

  return StatusCode;
}

void stsafe_init(void)
{
  PRINTF_INFO("custom_stsafe_init()\r\n");
  custom_stsafe_init();
  custom_stsafe_reset(0);
  custom_sob_v_reg2_on();
  HAL_Delay(10);
  custom_stsafe_reset(1);
  HAL_Delay(10);

  /* use following functions to manage stsafe (wrapper func have to be created in custom_sob.c) */
  int32_t BSP_I2C3_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
  int32_t BSP_I2C3_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);

  uint8_t ret = 0;
  uint8_t status_code;

  PRINTF_INFO("\r\nInit STSAFE-A110 [U9] ...\r\n");

  /* Create STSAFE-A's handle */
  status_code = (uint8_t)StSafeA_Init(&stsafea_handle, a_rx_tx_stsafea_data);

  if(status_code != STSAFEA_OK)
  {
    PRINTF_INFO("STSAFE Initialization: FAIL\r\n");
    ret = 1;
  }
  else
  {
    PRINTF_INFO("STSAFE Initialization: OK\r\n");

#if ENABLE_STSAFE_ECHO
    echo_out.Data = text_out;

    /* Check STSAFE-A's echo */
    status_code = StSafeA_Echo(&stsafea_handle, text_in, strlen((char *)text_in), &echo_out, STSAFEA_MAC_NONE);

    if( (status_code != STSAFEA_OK) ||\
      (echo_out.Length != strlen((char *)text_in)) ||\
        (strncmp((char *)echo_out.Data, (char *)text_in, strlen((char *)text_in)) != 0) )
    {
      PRINTF_INFO("STSAFE Echo: FAIL\r\n");
      ret = 2;
    }
    else
    {
      PRINTF_INFO("STSAFE Echo: OK\r\n");
    }
#endif //ENABLE_STSAFE_ECHO

#if 1//STSAFE_PAIRING

    uint8_t a = 0;

    //      Pairing is one time operation and is successfull only if STSAFE-A's host key slot are virgin.
    //   THIS OPERATION MUST BE PERFORMED ONLY IN PRODUCTION LINE
    if(a)
    {
      if (stsafe_pairing(&stsafea_handle) == 0)
      {
        PRINTF_INFO("STSAFE PAIRING: OK\r\n");
      }
      else
      {
        PRINTF_INFO("STSAFE PAIRING: FAIL\r\n");
      }
    }
#endif //STSAFE_PAIRING

#if 0//STSAFE_EXTRACTCERTIFICATE
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
#if (!STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
      free(data_sts_read_cert);
#endif
    }
#endif //STSAFE_EXTRACTCERTIFICATE
  }

  if(ret == 0)
    PRINTF_INFO("STSAFE Test Passed\r\n");
  else
    PRINTF_INFO("STSAFE Test NOT Passed\r\n");

  __NOP();
}
#else //USE_STSAFE
uint8_t StSafeCheck(void){return 1;}
#endif //USE_STSAFE

/* USE_STM32WL */
#if USE_STM32WL
void stm32wl_init(void)
{
  BSP_LoRaModemInit();//  Lora_ModemInit();

  PRINTF_VERBOSE("Lora_ModemHwReset()\r\n");
  if(!BSP_LoRaModemHwReset())
  {
    PRINTF_VERBOSE("BSP_LoRaModemHwReset() OK\r\n");
  }
  else
  {
  }

  GetWlLoraKeys();

  GetWlLoraSKeys();

  GetWlFwVersion();

  uint8_t loravalidflag = GetLoraValidationFlag(LORA_VALIDATION_MASK_ALL);
  if(USE_MEMORY && loravalidflag != LORA_VALIDATION_MASK_ALL)
  {
    AstraSetLoraProcessing(1);
  }

}
#endif //USE_STM32WL

void buzzer_init(void)
{
  PRINTF_INFO("USE_BUZZER = %d\r\n", USE_BUZZER);
  PRINTF_INFO("Buzzer pin can be used for debug purpose\r\n");
  AstraLedColor(ASTRA_LED_OFF);

  //buzzer pin configured as GPIO for debug purpose
  custom_astra_buzzer_init();
  custom_astra_buzzer_pinhigh();
  HAL_Delay(100);
  custom_astra_buzzer_pinlow();
}

/* MEMORY */
#if USE_MEMORY
/* memory organization:
addr 0  EEPROM_ memorymap version       len 4
addr 4  EEPROM_LORA_KEYS_VALID_ADDR     len 4
addr 8  JOIN_EUI_EEPROM_ADDRESS         len 8
addr 16 NWK_KEY_EEPROM_ADDRESS          len 16
addr 32 APP_KEY_EEPROM_ADDRESS          len 16
addr 48 DEVICE_EUI_EEPROM_ADDRESS       len 8
addr 56 reserved                        len 8
addr 64 EEPROM_ASTRA_ENGINE_PAR_ADDR    len 16
addr 80 EEPROM_PLATF_STATUS_ADDR        len 16
addr 96 EEPROM_FW_INFO_ADDR             len 16
addr 112  EEPROM_BOARD_NAME_ADDR        len 8
*/
void memory_init(void)
{
  PRINTF_INFO("USE_MEMORY = %d\r\n", USE_MEMORY);

#if USE_ASTRA_MAINBOARD_BSP
  custom_astra_v_reg2_on();
#endif //USE_ASTRA_MAINBOARD_BSP

  HAL_Delay(200); //wait NFC VDD rise time
  
  BSP_GPO_Init();
  
    /* Init I2C interface */                              
  if(BSP_NFCTAG_Init(BSP_NFCTAG_INSTANCE)!=NFCTAG_OK ) {
    PRINTF_VERBOSE("\r\nError during NFCTAG Initialization");
    STNFC_Error_Handler(STNFC_CONFIG_ERROR);
  } else {
    PRINTF_VERBOSE("\r\nNFCTAG has been initialized\r\n");
  }

  PRINTF_INFO("\r\nNFCTAG has been initialized\r\n");
  PlatformStatus.b.NFC_INIZIALIZED = SET;
  PlatformStatus.b.NFC_DATA_TRIGGERED = RESET;
  PlatformStatus.b.NFC_RF_ACTIVITY_STATUS = FIELD_FALLING;
}

void memory_deinit(void)
{
  BSP_GPO_Deinit();
    /* Deinit I2C interface */
  BSP_NFCTAG_DeInit(BSP_NFCTAG_INSTANCE);

  PlatformStatus.b.NFC_DATA_TRIGGERED = RESET;
  PlatformStatus.b.NFC_RF_ACTIVITY_STATUS = FIELD_UNDEF;
  
  #if USE_ASTRA_MAINBOARD_BSP
    custom_astra_v_reg2_off();
  #endif //USE_ASTRA_MAINBOARD_BSP
}

int32_t AstraEEPROM_ReadData( uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  return BSP_NFCTAG_ReadData( CUSTOM_ST25DV_0, (uint8_t*)pData, TarAddr, Size);
}

int32_t AstraEEPROM_WriteData( const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  return BSP_NFCTAG_WriteData( CUSTOM_ST25DV_0, (uint8_t*)pData, TarAddr, Size);
}

void AstraEEPROM_InitVirtualSensors(void)
{
#if USE_BLE
  if ((BLE_StackValue.BoardName[0] != 0) && (!PlatformStatus.b.NFC_VIRTUAL_SENSORS_INITIALIZED))
#else //USE_BLE
  if ((!PlatformStatus.b.NFC_VIRTUAL_SENSORS_INITIALIZED))
#endif //USE_BLE
  {
    extern RTC_HandleTypeDef hrtc;
    SmarTagAppStart();
    HAL_RTC_WaitForSynchro(&hrtc);
    PlatformStatus.b.NFC_VIRTUAL_SENSORS_INITIALIZED = SET;
  }
}

void AstraEEPROM_InitNDEFMessage(void)
{
  InitSTSmartNFC();
  APP_NFC_ForceMemInit();
}

void AstraEEPROM_CheckDefaultParamsUpdates(void)
{
  if ((!PlatformStatus.b.NFC_ASTRA_CONFIGURATION_UPDATED) && (PlatformStatus.b.NFC_VIRTUAL_SENSORS_INITIALIZED))
  {
    /* Check if NFC saving time is updated */
    if ((!PlatformStatus.b.NFC_SAVING_INTERVAL_UPDATED) && (PlatformStatus.s.NFC_SAVING_INTERVAL != 0))
    {
      uint8_t NDEFSavingTime[2] = {0};
      NDEFSavingTime[0] = (uint16_t) (PlatformStatus.s.NFC_SAVING_INTERVAL/1000); // seconds
      
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFSavingTime, SMARTNFC_SAVING_TIME_OFFSET, 2)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      PlatformStatus.b.NFC_SAVING_INTERVAL_UPDATED = SET;
    }
    
    /* Check if MAC address is updated */
#if USE_BLE
    if ((!PlatformStatus.b.NFC_MAC_ADDR_UPDATED) && (BLE_StackValue.BoardName[0] != 0))
#else //USE_BLE
    if ((!PlatformStatus.b.NFC_MAC_ADDR_UPDATED) )
#endif //USE_BLE
    {
      uint8_t NDEFPayloadBLE[ST25DV_NDEF1_PAYLOAD_SIZE] = {0};
      NDEFPayloadBLE[0] = 0x00; /* URI header identifier NONE */
#if USE_BLE
      sprintf((char*)&NDEFPayloadBLE[1],"stapplication://connect?Pin=%ld&Add=%02x:%02x:%02x:%02x:%02x:%02x",
              BLE_StackValue.SecurePIN,
              BLE_StackValue.BleMacAddress[5],
              BLE_StackValue.BleMacAddress[4],
              BLE_StackValue.BleMacAddress[3],
              BLE_StackValue.BleMacAddress[2],
              BLE_StackValue.BleMacAddress[1],
              BLE_StackValue.BleMacAddress[0]);
#else //USE_BLE
      sprintf((char*)&NDEFPayloadBLE[1],"stapplication://connect?Pin=000000&Add=00:00:00:00:00:00");
#endif //USE_BLE
      
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFPayloadBLE, SMARTNFC_MAC_ADDR_OFFSET, ST25DV_NDEF1_PAYLOAD_SIZE)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      PlatformStatus.b.NFC_MAC_ADDR_UPDATED = SET;
    }
    
    /* Check NFC_ASTRA_CONFIGURATION_UPDATED flag */
    if (PlatformStatus.b.NFC_SAVING_INTERVAL_UPDATED && PlatformStatus.b.NFC_MAC_ADDR_UPDATED)
    {
      PlatformStatus.b.NFC_ASTRA_CONFIGURATION_UPDATED = SET;
    }
  }
}

#else //USE_MEMORY
#define EEPROM_SIMULATION_BUFFER_SIZE 8192
static uint8_t EEPROMsimulation[EEPROM_SIMULATION_BUFFER_SIZE + 1];
int32_t AstraEEPROM_ReadData( uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  if(TarAddr+Size > EEPROM_SIMULATION_BUFFER_SIZE)
  {
    Error_Handler();
  }
  else
  {
    memcpy(pData, EEPROMsimulation+TarAddr, Size);
  }
  return 0;
}

int32_t AstraEEPROM_WriteData( const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  if(TarAddr+Size > EEPROM_SIMULATION_BUFFER_SIZE)
  {
    Error_Handler();
  }
  else
  {
    memcpy(EEPROMsimulation+TarAddr, pData, Size);
  }
  return 0;
}
#endif //USE_MEMORY

void SaveAstraEngineParams(AstraEngineParams_t *params)
{
  int32_t nRet = 1;
  if(sizeof(AstraEngineParams_t) > EEPROM_ASTRA_ENGINE_PAR_LEN)
  {
    PRINTF_ERROR("Cannot save Astra engine parameters, check AstraEngineParams_t size");
    Error_Handler();
  }
  else
  {
    nRet = AstraEEPROM_WriteData( (uint8_t*)params, EEPROM_ASTRA_ENGINE_PAR_ADDR+0, sizeof(AstraEngineParams_t));
  }
  UNUSED(nRet);
}

void GetAstraEngineParams(AstraEngineParams_t *params)
{
  int32_t nRet = 1;
  if(sizeof(AstraEngineParams_t) > EEPROM_ASTRA_ENGINE_PAR_LEN)
  {
    PRINTF_ERROR("Cannot retrieve Astra engine parameters, check AstraEngineParams_t size");
    Error_Handler();
  }
  else
  {
    nRet = AstraEEPROM_ReadData( (uint8_t*)params, EEPROM_ASTRA_ENGINE_PAR_ADDR+0, sizeof(AstraEngineParams_t));
  }
  UNUSED(nRet);
}

void ResetNewFwFlags(void)
{
  int32_t nRet = 1;
  char FW_INFO[128];
  //  char FW_INFO_EE[128];
  sprintf((char *)FW_INFO,"%s:\n"
          "%s_%d.%d.%d\n"
          "%s_%s_"
#if defined (__IAR_SYSTEMS_ICC__)
          "(IAR)",
#elif defined (__CC_ARM)
          "(KEIL)",
#elif defined (__GNUC__)
          "(STM32CubeIDE)",
#endif
          BLE_STM32_BOARD,
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH,
          __DATE__,__TIME__);

  uint32_t nSize = strlen(FW_INFO);
  if(nSize > EEPROM_FW_INFO_LEN-1)nSize = EEPROM_FW_INFO_LEN-1;

  nRet = AstraEEPROM_WriteData( (const uint8_t*)FW_INFO, EEPROM_FW_INFO_ADDR+0, nSize+1);
  UNUSED(nRet);
}

uint8_t IsNewFwFlashed(void)
{
  int32_t nRet = 1;
  UNUSED(nRet);
  char FW_INFO[128];
  char FW_INFO_EE[128];
  sprintf((char *)FW_INFO,"%s:\n"
          "%s_%d.%d.%d\n"
          "%s_%s_"
#if defined (__IAR_SYSTEMS_ICC__)
          "(IAR)",
#elif defined (__CC_ARM)
          "(KEIL)",
#elif defined (__GNUC__)
          "(STM32CubeIDE)",
#endif
          BLE_STM32_BOARD,
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH,
          __DATE__,__TIME__);

  uint32_t nSize = strlen(FW_INFO);
  if(nSize > EEPROM_FW_INFO_LEN-1)nSize = EEPROM_FW_INFO_LEN-1;

  nRet = AstraEEPROM_ReadData( (uint8_t*)FW_INFO_EE, EEPROM_FW_INFO_ADDR+0, nSize);

  if(0 == strncmp(FW_INFO, FW_INFO_EE, nSize))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

void SavePlatformParams(SystemConfParams_t *params)
{
  int32_t nRet = 1;
  if(sizeof(SystemConfParams_t) > EEPROM_PLATF_STATUS_LEN)
  {
    PRINTF_ERROR("Cannot save Astra engine parameters, check SystemConfParams_t size");
    Error_Handler();
  }
  else
  {
    nRet = AstraEEPROM_WriteData( (uint8_t*)params, EEPROM_PLATF_STATUS_ADDR+0, sizeof(SystemConfParams_t));
  }
  UNUSED(nRet);
}

void LoadPlatformParams(SystemConfParams_t *params)
{
  int32_t nRet = 1;
  if(sizeof(SystemConfParams_t) > EEPROM_PLATF_STATUS_LEN)
  {
    PRINTF_ERROR("Cannot get Astra engine parameters, check SystemConfParams_t size");
    Error_Handler();
  }
  else
  {
    nRet = AstraEEPROM_ReadData( (uint8_t*)params, EEPROM_PLATF_STATUS_ADDR+0, sizeof(SystemConfParams_t));
  }
  UNUSED(nRet);
}

uint32_t GetPrintLevelActive()
{
  return SystemConfParams.PrintLevelActive;
}

/* USE_BLE */
void astra_ble_init(void)
{

  UTIL_LPM_Mode_t temp = UTIL_LPM_GetMode();

#if USE_BLE
  MX_APPE_Init();
#endif //USE_BLE
  if (temp == UTIL_LPM_SLEEPMODE)
  {
    UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  }
  
}

void POWER_GOOD_CB(void)
{
}

/**
  * @brief  Peripheral clock configuration
  * @param  none
  * @retval none
  */
void PeriphClock_Config(void)
{
  MX_RTC_Init();

  extern RTC_HandleTypeDef hrtc;
  HW_TS_Init(hw_ts_InitMode_Full, &hrtc); /**< Initialize the TimerServer */

#if (/*CFG_USB_INTERFACE_ENABLE*/ USE_USB != 0)
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };
  RCC_CRSInitTypeDef RCC_CRSInitStruct = { 0 };

  /**
  * This prevents the CPU2 to disable the HSI48 oscillator when
  * it does not use anymore the RNG IP
  */
  LL_HSEM_1StepLock( HSEM, 5 );

  LL_RCC_HSI48_Enable();

  while(!LL_RCC_HSI48_IsReady());

  /* Select HSI48 as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  /*Configure the clock recovery system (CRS)**********************************/

  /* Enable CRS Clock */
  __HAL_RCC_CRS_CLK_ENABLE();

  /* Default Synchro Signal division factor (not divided) */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

  /* Set the SYNCSRC[1:0] bits according to CRS_Source value */
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

  /* HSI48 is synchronized with USB SOF at 1KHz rate */
  RCC_CRSInitStruct.ReloadValue = RCC_CRS_RELOADVALUE_DEFAULT;
  RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;

  /* Set the TRIM[5:0] to the default value*/
  RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;

  /* Start automatic synchronization */
  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
#endif

  return;
}


void PeriphLowPowerClock_Config(void)
{
  MX_RTC_Init();
  extern RTC_HandleTypeDef hrtc;
  HW_TS_Init(hw_ts_InitMode_Full, &hrtc); /**< Initialize the TimerServer */


  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

  LL_RCC_MSI_Enable();
  while(!LL_RCC_MSI_IsReady());
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_4);
  LL_RCC_SetSMPSClockSource(LL_RCC_SMPS_CLKSOURCE_MSI);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_MSI);

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Enables the Clock Security System
  */
  HAL_RCCEx_EnableLSECSS();

  return;
}

#if USE_ASTRA_MAINBOARD_BSP
uint8_t BTN1_Timer_Id;
void Astra_Button1TimerCallback(void)
{
  PlatformStatus.b.BTN1_PUSHED_LONG_EVENT = SET;
  PlatformStatus.b.BTN1_PUSH_TIMER = RESET;
}

uint8_t BTN2_Timer_Id;
void Astra_Button2TimerCallback(void)
{
  PlatformStatus.b.BTN2_PUSHED_LONG_EVENT = SET;
  PlatformStatus.b.BTN2_PUSH_TIMER = RESET;
}

/**
  * @brief This function initializes buttons.
  */
void Astra_ButtonInit(void)
{
  custom_astra_sysshutdown_init();
#if USE_SEQUENCER
  HW_TS_Create(CFG_TIM_PROC_ID_ISR,
               &(BTN1_Timer_Id),
               hw_ts_SingleShot,
               Astra_Button1TimerCallback);

  HW_TS_Create(CFG_TIM_PROC_ID_ISR,
               &(BTN2_Timer_Id),
               hw_ts_SingleShot,
               Astra_Button2TimerCallback);
#endif //USE_SEQUENCER

  custom_astra_btns_init();

#if USE_BTN1_ANTI_TAMPER
  if (custom_astra_btn_get_state(ASTRA_BTN1) == DISABLE)
  {
    PlatformStatus.b.BTN1_ANTI_TAMPERING_FLAG = SET;
    PlatformStatus.b.BTN1_ANTI_TAMPERING_ACTIVATED_EVENT = SET;
  }
#endif //USE_BTN1_ANTI_TAMPER
}

#if USE_BTN1_ANTI_TAMPER

void Astra_AntiTamperingGlitch(void)
{
  PRINTF_INFO(TEXT_COLOR_2);
  PRINTF_INFO("WARNING! An anti tampering event has occurred\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button1TamperGlitch_cb();
}

void Astra_AntiTamperingActivated(void)
{
  PRINTF_INFO(TEXT_COLOR_2);
  PRINTF_INFO("WARNING! A tmpering event is in progress\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button1TamperActivated_cb();
}

void Astra_AntiTamperingRestored(void)
{
  PRINTF_INFO(TEXT_COLOR_2);
  PRINTF_INFO("WARNING! Anti tampering circuit was restored\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button1TamperRestored_cb();
}

#else //USE_BTN1_ANTI_TAMPER

#if USE_SEQUENCER
void Astra_Button1PushTimerStart (void)
{
    HW_TS_Start(BTN1_Timer_Id, BTN1_TIMEOUT);
}

void Astra_Button1PushTimerStop (void)
{
    HW_TS_Stop(BTN1_Timer_Id);
}

#else //USE_SEQUENCER
void Astra_Button1PushTimerStart (void)
{
}

void Astra_Button1PushTimerStop (void)
{
}

#endif //USE_SEQUENCER
#endif //USE_BTN1_ANTI_TAMPER

#if USE_SEQUENCER
void Astra_Button2PushTimerStart (void)
{
    HW_TS_Start(BTN2_Timer_Id, BTN2_TIMEOUT);
}

void Astra_Button2PushTimerStop (void)
{
    HW_TS_Stop(BTN2_Timer_Id);
}

#else //USE_SEQUENCER
void Astra_Button2PushTimerStart (void)
{
}

void Astra_Button2PushTimerStop (void)
{
}
#endif //USE_SEQUENCER

void BTN1_CB(void)
{
#if USE_BTN1_ANTI_TAMPER

    if (custom_astra_btn_get_state(ASTRA_BTN1) == DISABLE)
    {
      PlatformStatus.b.BTN1_ANTI_TAMPERING_FLAG = SET;
      PlatformStatus.b.BTN1_ANTI_TAMPERING_ACTIVATED_EVENT = SET;

    }
    else
    {
      PlatformStatus.b.BTN1_ANTI_TAMPERING_FLAG = RESET;
      PlatformStatus.b.BTN1_ANTI_TAMPERING_RESTORED_EVENT = SET;
    }

#else //USE_BTN1_ANTI_TAMPER

    if ((custom_astra_btn_get_state(ASTRA_BTN1) == DISABLE) && PlatformStatus.b.BTN1_PUSH_TIMER)
    {
      PlatformStatus.b.BTN1_PUSH_TIMER = RESET;
      PlatformStatus.b.BTN1_PUSHED_EVENT = SET;
      Astra_Button1PushTimerStop();
    }
    else if ((custom_astra_btn_get_state(ASTRA_BTN1) == ENABLE) && !PlatformStatus.b.BTN1_PUSH_TIMER)
    {
      PlatformStatus.b.BTN1_PUSH_TIMER = SET;
      Astra_Button1PushTimerStart();
    }
#endif //USE_BTN1_ANTI_TAMPER
}

void BTN2_CB(void)
{
  if ((custom_astra_btn_get_state(ASTRA_BTN2) == DISABLE) && PlatformStatus.b.BTN2_PUSH_TIMER)
  {
    PlatformStatus.b.BTN2_PUSH_TIMER = RESET;
    PlatformStatus.b.BTN2_PUSHED_EVENT = SET;
    Astra_Button2PushTimerStop();
  }
  else if ((custom_astra_btn_get_state(ASTRA_BTN2) == ENABLE) && !PlatformStatus.b.BTN2_PUSH_TIMER)
  {
    PlatformStatus.b.BTN2_PUSH_TIMER = SET;
    Astra_Button2PushTimerStart();
  }
}

/**
  * @brief This function manages buttons.
  */
void Astra_ButtonsCheck(void)
{
#if USE_BTN1_ANTI_TAMPER

    if (PlatformStatus.b.BTN1_ANTI_TAMPERING_ACTIVATED_EVENT && PlatformStatus.b.BTN1_ANTI_TAMPERING_RESTORED_EVENT)
    {
      Astra_AntiTamperingGlitch();
      PlatformStatus.b.BTN1_ANTI_TAMPERING_ACTIVATED_EVENT = RESET;
      PlatformStatus.b.BTN1_ANTI_TAMPERING_RESTORED_EVENT = RESET;
    }

    if (PlatformStatus.b.BTN1_ANTI_TAMPERING_ACTIVATED_EVENT && PlatformStatus.b.BTN1_ANTI_TAMPERING_FLAG)
    {
      Astra_AntiTamperingActivated();
      PlatformStatus.b.BTN1_ANTI_TAMPERING_ACTIVATED_EVENT = RESET;
    }

    if (PlatformStatus.b.BTN1_ANTI_TAMPERING_RESTORED_EVENT && !PlatformStatus.b.BTN1_ANTI_TAMPERING_FLAG)
    {
      Astra_AntiTamperingRestored();
      PlatformStatus.b.BTN1_ANTI_TAMPERING_RESTORED_EVENT = RESET;
    }

#else  //USE_BTN1_ANTI_TAMPER

    if (PlatformStatus.b.BTN1_PUSHED_EVENT)
    {
      Astra_Button1Pushed();
      PlatformStatus.b.BTN1_PUSHED_EVENT = RESET;
    }

    if (PlatformStatus.b.BTN1_PUSHED_LONG_EVENT)
    {
      Astra_Button1PushedLong();
      PlatformStatus.b.BTN1_PUSHED_LONG_EVENT = RESET;
    }

    if (PlatformStatus.b.BTN1_PUSH_TIMER)
    {
      HAL_Delay(200);
    }

#endif //USE_BTN1_ANTI_TAMPER

    if (PlatformStatus.b.BTN2_PUSHED_EVENT)
    {
      Astra_Button2Pushed();
      PlatformStatus.b.BTN2_PUSHED_EVENT = RESET;
    }

    if (PlatformStatus.b.BTN2_PUSHED_LONG_EVENT)
    {
      Astra_Button2PushedLong();
      PlatformStatus.b.BTN2_PUSHED_LONG_EVENT = RESET;
    }

    if (PlatformStatus.b.BTN2_PUSH_TIMER)
    {
      HAL_Delay(200);
    }
}

void SystemShutdown(void)
{
  custom_astra_sysshutdown();
}

void Astra_Button1Pushed(void)
{
  PRINTF_INFO(TEXT_COLOR_1);
  PRINTF_INFO("The user button was pressed\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button1Pushed_cb();
}

void Astra_Button2Pushed(void)
{
  PRINTF_INFO(TEXT_COLOR_1);
  PRINTF_INFO("The power button was pressed\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button2Pushed_cb();
}

void Astra_Button1PushedLong(void)
{
  PRINTF_INFO(TEXT_COLOR_1);
  PRINTF_INFO("The user button was pressed for a long time\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button1LongPushed_cb();
}

void Astra_Button2PushedLong(void)
{
  PRINTF_INFO(TEXT_COLOR_1);
  PRINTF_INFO("The power button was pressed for a long time\r\n");
  PRINTF_INFO(DEFAULT_TEXT_COLOR);

  Button2LongPushed_cb();
}

void SENS_INT1_CB(void)
{
  PlatformStatus.b.MEMS_INT1_EVENT = 1;
}

void SENS_INT2_CB(void)
{
  PlatformStatus.b.MEMS_INT2_EVENT = 1;
}


void ST25DV_INT_PIN_GPO_EXTI_CB(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(ST25DV_INT_PIN_GPO_EXTI_PIN);
  PlatformStatus.b.NFC_RF_ACTIVITY = 1;
} 




#endif //USE_ASTRA_MAINBOARD_BSP



__WEAK void Button1Pushed_cb(){}
__WEAK void Button1LongPushed_cb(){}
__WEAK void Button2Pushed_cb(){}
__WEAK void Button2LongPushed_cb(){}
__WEAK void Button1TamperGlitch_cb() {}
__WEAK void Button1TamperActivated_cb() {}
__WEAK void Button1TamperRestored_cb() {}

/**
  * @} ASTRA_CONFMNG_FunctionDefines
  */

/**
  * @} CONFIGURATION
  */

/**
  * @} ASTRA_ENGINE
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
