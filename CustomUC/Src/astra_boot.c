/**
  ******************************************************************************
  * @file    astra_boot.c
  * @author  SRA Team
  * @brief   This file contains the Astra boot management
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

/** @addtogroup BOOT_APPLICATION BOOT application
  * @{
  */

/* Includes ------------------------------------------------------------------*/

#include "astra_boot.h"
#include "astra_boot_FW.h"
#include "flash_driver.h"
#include "string.h"
#include "stm32_lpm_if.h"
#include "stm32_seq.h"

/** @addtogroup ASTRA_BOOT_PRIVATE_DEFINES ASTRA BOOT - Private defines
  * @brief Private defines
  * @{
 */

/* Defines -------------------------------------------------------------------*/

#define BOOT_LED_BLINK_LONG_PAUSE    500
#define BOOT_LED_BLINK_PAUSE    200

/**
* @} ASTRA_BOOT_PRIVATE_DEFINES
*/


/** @addtogroup ASTRA_BOOT_VARIABLES ASTRA BOOT - Variables
  * @brief Variables
  * @{
 */

/* Variables -----------------------------------------------------------------*/

bootVersionFw_t bootVersionFw = {0};

/**
* @} ASTRA_BOOT_VARIABLES
*/

/** @addtogroup ASTRA_BOOT_FUNCTIONS_DEFINITIONS ASTRA BOOT - Functions definitions
  * @brief Functions definitions
  * @{
 */

/* Functions definition ------------------------------------------------------*/


void SystemMemoryBoot(void)
{
  void (*SysMemBootJump)(void);

  /***
   * Set system memory address
   */
  volatile uint32_t addr = 0x1FFF0000;

  /**
   * Set jump memory location for system memory
   * Use address with 4 bytes offset which specifies jump location where program starts
   */
  SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));

  /**
   * Set main stack pointer.
   * This step must be done last otherwise local variables in this function
   * don't have proper value since stack pointer is located on different position.
   * Set direct address location which specifies stack pointer in SRAM location
   */
  __set_MSP(*(uint32_t *)addr);

  /**
   * Actually call our function to jump to set location
   * This will start system memory execution
   */
  SysMemBootJump();

  /**
   * Connect USB<->UART converter to dedicated USART pins and test
   * and test with bootloader works with STM32 Flash Loader Demonstrator software
   */
}


void AstraBootSetSmState(uint16_t s)
{
  extern RTC_HandleTypeDef hrtc;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SM_STATE, s);
}


uint32_t AstraBootGetSmState(void)
{
  extern RTC_HandleTypeDef hrtc;
  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SM_STATE);
}


void Astra_BLE_ResetBootFailureNumber(void)
{
  extern RTC_HandleTypeDef hrtc;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_BLE_HEALTH, 0);
}


uint32_t Astra_BLE_GetBootFailureNumber(void)
{
  extern RTC_HandleTypeDef hrtc;
  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_BLE_HEALTH);
}


void Astra_BLE_IncreaseBootFailureNumber(void)
{
  extern RTC_HandleTypeDef hrtc;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_BLE_HEALTH, 1+HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_BLE_HEALTH));
}


void AstraSaveWakeReason(uint8_t reason)
{
  extern RTC_HandleTypeDef hrtc;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_WAKE_REASON, reason);
}


uint32_t AstraGetWakeReason(void)
{
  extern RTC_HandleTypeDef hrtc;
  return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_WAKE_REASON);
}


void BoardRebootWithReason(uint8_t reason)
{
  extern RTC_HandleTypeDef hrtc;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, reason);
  HAL_NVIC_SystemReset();
}


void BoardRebootBOOTUpdate(void)
{
  extern RTC_HandleTypeDef hrtc;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_BOOT_UPDATE);
  HAL_NVIC_SystemReset();
}


void AstraRebootBOOTUpdate(void)
{
  PRINTF_VERBOSE(TEXT_COLOR_3);
  PRINTF_VERBOSE("SYSTEM REBOOT for BOOT update...\r\n");
  PRINTF_VERBOSE(DEFAULT_TEXT_COLOR);
  AstraLedColor(ASTRA_LED_COLOR_BLUE);
  HAL_Delay(500);
  BoardRebootBOOTUpdate();
}


bool AstraHasNewBootFwAvailable(void)
{
  uint32_t currentMajor, currentMinor, currentRev;
  uint32_t availableMajor, availableMinor, availableRev;

  ASTRA_BOOT_PRINTF("Get current bootloader version:\r\n");

  void *blveraddr = (void*)0x08000200;
  uint8_t blverstring[] = "BLE_Ota";
  if(memcmp(blveraddr, blverstring, strlen((char*)blverstring)) == 0)
  {
    sscanf(blveraddr, "BLE_OtaFwVer%ld.%ld.%ld", &currentMajor, &currentMinor, &currentRev);
    ASTRA_BOOT_PRINTF("Current Ver: <%d>.<%d>.<%d>\r\n", currentMajor, currentMinor, currentRev);
    bootVersionFw.currentVersion = MAKE_VERSION(currentMajor, currentMinor, currentRev);
  }
  else
  {
    ASTRA_BOOT_PRINTF("No bootloader version found\r\n");
  }
  if((sizeof(bootAstraFW) < 0x7000) && IS_OB_BOOT_VECTOR_ADDR(*(uint32_t*)bootAstraFW) && IS_OB_BOOT_VECTOR_ADDR(*(uint32_t*)&bootAstraFW[4]))
  {
    ASTRA_BOOT_PRINTF("Get available bootloader version:\r\n");
    if(memcmp(&bootAstraFW[512], blverstring, strlen((char*)blverstring)) == 0)
    {
      sscanf((char*)&bootAstraFW[512], "BLE_OtaFwVer%ld.%ld.%ld", &availableMajor, &availableMinor, &availableRev);
      ASTRA_BOOT_PRINTF("Available Ver: <%d>.<%d>.<%d>\r\n", availableMajor, availableMinor, availableRev);
      bootVersionFw.availableVersion = MAKE_VERSION(availableMajor, availableMinor, availableRev);
      
      if(bootVersionFw.availableVersion > bootVersionFw.currentVersion)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }
  return false;
}


void AstraBootUpgrade(void)
{
    FD_EraseSectors(BOOT_SECTOR_START, BOOT_SECTOR_NUMBERS);
    uint32_t base_address = FLASH_BASE;
    uint8_t write_value_index = 0;
    uint64_t write_value;
    uint32_t count = 0;
    uint32_t size_left = sizeof(bootAstraFW);

    /*
     * The flash is written by bunch of DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING bytes
     * Data are written in flash as long as there are at least DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING bytes
     */
    while(size_left >= (DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING - write_value_index))
    {
      uint32_t NbrOfDataToBeWritten = 1;
      memcpy( (uint8_t*)&write_value + write_value_index,
              bootAstraFW + count,
              DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING - write_value_index );

      while(NbrOfDataToBeWritten > 0)
        NbrOfDataToBeWritten = FD_WriteData(base_address,
                                            &(write_value),
                                            1);
      
      if(*(uint64_t*)(base_address)==write_value)
      {
        base_address += DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING;
        size_left -= (DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING - write_value_index);
        count += (DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING - write_value_index);
        write_value_index = 0;
      }
    }

    /**
     * The Flash shall be written by DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING number of bytes.
     * In case the packet received is not a multiple of DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING bytes,
     * It shall be recorded how much bytes is left to be written in flash
     */
    if(size_left != 0)
    {
      memcpy( (uint8_t*)&write_value + write_value_index,
              bootAstraFW + count,
              size_left );
      write_value_index += size_left;
    }

    /**
     * Write now in Flash the remaining data that has not been written before because they were less than
     * 8 bytes to be written
     */
    if(write_value_index != 0)
    {
      /**
       * As it is mandatory to write DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING bytes, the data to be written shall be padded
       * up to DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING bytes with 0xFF.
       * This is mandatory for correct operation with the Firmware Upgrade Service (FUS) as otherwise, it could be the magic
       * key word in wrongly duplicated
       */
      memset( (uint8_t*)&write_value + write_value_index,
              0xFF,
              DOUBLEWORD_SIZE_FOR_FLASH_PROGRAMMING - write_value_index  );

      while(*(uint64_t*)(base_address) != write_value)
      {
        uint32_t NbrOfDataToBeWritten = 1;

        while(NbrOfDataToBeWritten > 0)
          NbrOfDataToBeWritten = FD_WriteData(base_address,
                                              &(write_value),
                                              1);
      }
    }
}


void HandleBoot(void)
{
  uint8_t a = 0;
  if(a)HAL_NVIC_SystemReset();
  if(a)while(2);

  custom_sob_led_init();

  extern RTC_HandleTypeDef hrtc;

  hrtc.Instance = RTC;
  HAL_PWR_EnableBkUpAccess();

  uint32_t r0 = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_CHECK_0);//RTC_BKP_CHECK_0
  uint32_t r1 = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_CHECK_1);//RTC_BKP_CHECK_1
  uint32_t r2 = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_REBOOT_REASON);//RTC_BKP_REBOOT_REASON

  if( (r0 == BOOT_REASON_CHECK_0) && (r1 == BOOT_REASON_CHECK_1) )
  {
    //ok
    
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_BLE_HEALTH) >= BLE_NUMBER_OF_FAILURES)
    {
      __HAL_RCC_RFWAKEUP_CONFIG(RCC_RFWKPCLKSOURCE_HSE_DIV1024);
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_BLE_HEALTH, 3);
    }

    if(r2 == BOOT_REASON_NORMAL)
    {
      //this is boot normal, reboot now
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_RED);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_RED);pause(BOOT_LED_BLINK_PAUSE);

      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NONE);
      HAL_NVIC_SystemReset();
    }
    else if(r2 == BOOT_REASON_NONE)
    {
      //this is reboot, continue now
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_BLUE);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_BLUE);pause(BOOT_LED_BLINK_PAUSE);
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NORMAL);
    }
    else if(r2 == BOOT_REASON_DFU)
    {
      //this is reboot in dfu, jump to system memory
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NORMAL);
      custom_sob_power_init();
      custom_sob_v_reg1_out_high_value();
      AstraLedColor(ASTRA_LED_OFF);
      SystemMemoryBoot();
    }
    else if(r2 == BOOT_REASON_OPT1) //OPT1(only lp acc)
    {
      //this is reboot in dfu, jump to system memory
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_LONG_PAUSE );
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_LONG_PAUSE );
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_LONG_PAUSE );
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_LONG_PAUSE );

      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NORMAL);
      custom_sob_power_init();
      custom_sob_v_reg1_out_high_value();
      AstraLedColor(ASTRA_LED_OFF);
      
      //lED OPT1
      AstraLedColor(ASTRA_LED_COLOR_GREEN);

#if USE_ASTRA_MAINBOARD_BSP
      custom_astra_lp_acc_interruptpin_init();
      custom_astra_btns_init();
#endif //USE_ASTRA_MAINBOARD_BSP

      PWR_EnterOffMode(); // go to low power
      while(3)
      {
        PWR_ExitOffMode();
        if(PlatformStatus.b.MEMS_INT1_EVENT == 1)
        {
          PRINTF_INFO("MEMS_INT1 detected\r\n");
          break;
        }
        PWR_EnterOffMode();
      }  

      //****** Reeboot to normal application ->
      BoardRebootWithReason(BOOT_REASON_NONE);
      //****** Continue to normal application ->

    }
    else if(r2 == BOOT_REASON_OPT2) //OPT2 (solo BLE)
    {
      //this is reboot in dfu, jump to system memory
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_LONG_PAUSE );
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_LONG_PAUSE );
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_LONG_PAUSE );
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_LONG_PAUSE );

      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NORMAL);
      custom_sob_power_init();
      custom_sob_v_reg1_out_high_value();
      AstraLedColor(ASTRA_LED_OFF);
      
#if USE_ASTRA_MAINBOARD_BSP
      custom_astra_btns_init();
#endif //USE_ASTRA_MAINBOARD_BSP
      
      //lED OPT2
      AstraLedColor(ASTRA_LED_COLOR_RED);

      MX_IPCC_Init();

      MX_RTC_Init();
      extern RTC_HandleTypeDef hrtc;
      HW_TS_Init(hw_ts_InitMode_Full, &hrtc); /**< Initialize the TimerServer */
      // PeriphLowPowerClock_Config();

      // UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
      astra_ble_init();

#if USE_ASTRA_MAINBOARD_BSP
      Astra_ButtonInit();
#endif //USE_ASTRA_MAINBOARD_BSP      

#if USE_STM32WL
      custom_sob_stm32wl_init();
#endif //USE_STM32WL

      /* Deinit SWO pin to minimize current leakage */
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = GPIO_PIN_15;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


      while(3)
      {
        UTIL_SEQ_Run( UTIL_SEQ_DEFAULT );
         if(PlatformStatus.b.BLE_CONNECTED)
           break;
         if(PlatformStatus.b.BTN1_PUSHED_EVENT)
           break;
         if(PlatformStatus.b.BTN1_PUSHED_LONG_EVENT)
           break;
         if(PlatformStatus.b.BTN2_PUSHED_EVENT)
           break;
         if(PlatformStatus.b.BTN2_PUSHED_LONG_EVENT)
           break;
      }
      //****** Reeboot to normal application ->
      BoardRebootWithReason(BOOT_REASON_NONE);
    }
    else if(r2 == BOOT_REASON_OPT3) //OPT3
    {
      //this is reboot in dfu, jump to system memory
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NORMAL);
      custom_sob_power_init();
      custom_sob_v_reg1_out_high_value();
      AstraLedColor(ASTRA_LED_OFF);
      
      //lED OPT3
      AstraLedColor(0x00FF00FF);
      while(3);

    }

#if USE_BOOT_UPGRADE
    else if(r2 == BOOT_REASON_BOOT_UPDATE) //BOOT UPGRADE
    {
      //this is reboot in dfu, jump to system memory
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_GREEN);pause(BOOT_LED_BLINK_PAUSE);
      AstraLedColor(ASTRA_LED_COLOR_YELLOW);pause(BOOT_LED_BLINK_PAUSE);
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NORMAL); 
      custom_sob_power_init();
      custom_sob_v_reg1_out_high_value();
      AstraLedColor(ASTRA_LED_OFF);
      //lED BOOT UPGRADE
      AstraLedColor(ASTRA_LED_COLOR_BLUE);

      //clear flash and write new fw
      AstraBootUpgrade();

        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, 0x01);
        // restart application
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REBOOT_REASON, BOOT_REASON_NONE);
        HAL_NVIC_SystemReset();
      }
    #endif //USE_BOOT_UPGRADE
  }
  else
  {
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, BOOT_REASON_CHECK_0);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, BOOT_REASON_CHECK_1);
    HAL_NVIC_SystemReset();
  }
}

/**
* @} ASTRA_BOOT_FUNCTIONS_DEFINITIONS
*/

/**
* @} BOOT_APPLICATION
*/

/**
* @} ASTRA_ENGINE
*/

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
