/**
  ******************************************************************************
  * @file    custom_nfc_ex.c
  * @author  System Research & Applications Team - Catania & Agrate Lab.
  * @version 1.0.0
  * @brief   This file provides set of driver functions for using the NFC sensor
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
  
/* Includes ------------------------------------------------------------------*/
#include "SmartNFC_config.h"
#include "custom_nfc_ex.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup SMARTNFC
 * @{
 */

/** @defgroup SMARTNFC_NFCTAG_EX
 * @{
 */
 
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
 
 /* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
/* Global variables ----------------------------------------------------------*/

/** @defgroup SMARTNFC_NFCTAG_EX_Private_Variables
 * @{
 */
//static NFCTAG_DrvTypeDef *Nfctag_Drv = NULL;
//static ST25DVxxKC_Object_t NfcTagObj;

/**
 * @}
 */
 
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/** @defgroup SMARTNFC_NFCTAG_EX_Public_Functions
 * @{
 */

/**
  * @brief  Change the I2C Password protection
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_ChangeI2CPassword(uint32_t MsbPasswd,uint32_t LsbPasswd)
{
  int32_t ret = NFCTAG_OK;
  ST25DV_I2CSSO_STATUS i2csso;
  ST25DV_PASSWD Passwd;
  BSP_NFCTAG_ReadI2CSecuritySession_Dyn( BSP_NFCTAG_INSTANCE, &i2csso );
  if( i2csso == ST25DV_SESSION_CLOSED ) {
    /* if I2C session is closed, present default password to open session */
    Passwd.MsbPasswd = 0;
    Passwd.LsbPasswd = 0;
    ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
    if(ret==NFCTAG_OK) {
      /* Ok we could Change the default Password */
      Passwd.MsbPasswd = MsbPasswd;
      Passwd.LsbPasswd = LsbPasswd;
      ret = BSP_NFCTAG_WriteI2CPassword(BSP_NFCTAG_INSTANCE, Passwd);
      if(ret==NFCTAG_OK) {
        /* Present a wrong password for closing the session we have alredy setted the new one here */
        Passwd.MsbPasswd = ~MsbPasswd;
        Passwd.LsbPasswd = ~LsbPasswd;
        BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
      }
    }
  }
  return ret;
}

/**
  * @brief  Set the I2C protection level creating a Single secure zone
  * @param  uint32_t MsbPasswd MSB 25-bit password
  * @param  uint32_t LsbPasswd LSB 25-bit password
  * ST25DV_PROTECTION_CONF ProtectionLevel
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_SetICPasswordProtectionZone1(uint32_t MsbPasswd,uint32_t LsbPasswd,ST25DV_PROTECTION_CONF ProtectionLevel)
{
  int32_t ret = NFCTAG_OK;
  ST25DV_I2C_PROT_ZONE pProtZone;
  /* Read the Protection levels */
  ret = BSP_NFCTAG_ReadI2CProtectZone(BSP_NFCTAG_INSTANCE, &pProtZone);
  if(ret==NFCTAG_OK) {
    /* Check if the Protect Zone 1 is already on Read and Write Protection */
    if(pProtZone.ProtectZone1!=ProtectionLevel) {
      ST25DV_I2CSSO_STATUS I2CSS;

      /* Read the Session status */
      BSP_NFCTAG_ReadI2CSecuritySession_Dyn( BSP_NFCTAG_INSTANCE, &I2CSS );
      if( I2CSS == ST25DV_SESSION_CLOSED ) {
        ST25DV_PASSWD Passwd;
        /* if I2C session is closed, present password to open session */
        Passwd.MsbPasswd = MsbPasswd;
        Passwd.LsbPasswd = LsbPasswd;
        ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

        if(ret==NFCTAG_OK) {
          ST25DV_MEM_SIZE st25dvmemsize;
          uint32_t st25dvbmsize;

          BSP_NFCTAG_ReadMemSize( BSP_NFCTAG_INSTANCE, &st25dvmemsize );
          /* st25dvmemsize is composed of Mem_Size (number of blocks) and BlockSize (size of each blocks in bytes) */
          st25dvbmsize = (st25dvmemsize.Mem_Size + 1) * (st25dvmemsize.BlockSize + 1);

          /* We create one Memory size == to the whole memory size */
          ret = BSP_NFCTAG_CreateUserZone( BSP_NFCTAG_INSTANCE, st25dvbmsize, 0, 0, 0 );

          if(ret==NFCTAG_OK) {
            /* Set Protection leve for zone 1 for i2c  */
            ret = BSP_NFCTAG_WriteI2CProtectZonex( BSP_NFCTAG_INSTANCE, ST25DV_PROT_ZONE1, ProtectionLevel );
          }
          /* Present a wrong Password for closing the session */
          Passwd.MsbPasswd = ~MsbPasswd;
          Passwd.LsbPasswd = ~LsbPasswd;
          BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
        }
      }
    }
  }
  return ret;
}

/**
  * @brief  Enable the R/F writing protection for Zone 1 using RF_PWD_1 password
  * @param  uint32_t MsbPasswd MSB 25-bit password
  * @param  uint32_t LsbPasswd LSB 25-bit password
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_EnableRFWritingPasswordProtectionZone1(uint32_t MsbPasswd,uint32_t LsbPasswd)
{
  int32_t ret = NFCTAG_OK;
  ST25DV_RF_PROT_ZONE pRfprotZone;
  /* Read Level of R/F protection */
  ret = BSP_NFCTAG_ReadRFZxSS(BSP_NFCTAG_INSTANCE, ST25DV_PROT_ZONE1, &pRfprotZone );
  if(ret==NFCTAG_OK) {
    /* Change the R/F protection level if it's necessary */
    if((pRfprotZone.PasswdCtrl != ST25DV_PROT_PASSWD1 ) & (pRfprotZone.RWprotection != ST25DV_WRITE_PROT)) {
      ST25DV_PASSWD Passwd;
      /* Present password to open session  */
      Passwd.MsbPasswd = MsbPasswd;
      Passwd.LsbPasswd = LsbPasswd;
      ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

      pRfprotZone.PasswdCtrl = ST25DV_PROT_PASSWD1;
      pRfprotZone.RWprotection = ST25DV_WRITE_PROT;
      ret = BSP_NFCTAG_WriteRFZxSS(BSP_NFCTAG_INSTANCE, ST25DV_PROT_ZONE1, pRfprotZone);

      /* present wrong password for closing the session */
      Passwd.MsbPasswd = ~MsbPasswd;
      Passwd.LsbPasswd = ~LsbPasswd;
      BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
    }
  }
  return ret;
}

/**
  * @brief  Disable the R/F writing protection for Zone 1 using RF_PWD_1 password
  * @param  uint32_t MsbPasswd MSB 25-bit password
  * @param  uint32_t LsbPasswd LSB 25-bit password
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_DisableRFWritingPasswordProtectionZone1(uint32_t MsbPasswd,uint32_t LsbPasswd)
{
  int32_t ret = NFCTAG_OK;
  ST25DV_RF_PROT_ZONE pRfprotZone;
  /* Read Level of R/F protection */
  ret = BSP_NFCTAG_ReadRFZxSS(BSP_NFCTAG_INSTANCE, ST25DV_PROT_ZONE1, &pRfprotZone );
  if(ret==NFCTAG_OK) {
    /* Change the R/F protection level if it's necessary */
    if((pRfprotZone.PasswdCtrl != ST25DV_NOT_PROTECTED ) & (pRfprotZone.RWprotection != ST25DV_NO_PROT)) {
      ST25DV_PASSWD Passwd;
      /* Present password to open session  */
      Passwd.MsbPasswd = MsbPasswd;
      Passwd.LsbPasswd = LsbPasswd;
      ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

      pRfprotZone.PasswdCtrl = ST25DV_NOT_PROTECTED;
      pRfprotZone.RWprotection = ST25DV_NO_PROT;
      ret = BSP_NFCTAG_WriteRFZxSS(BSP_NFCTAG_INSTANCE, ST25DV_PROT_ZONE1, pRfprotZone);

      /* present wrong password for closing the session */
      Passwd.MsbPasswd = ~MsbPasswd;
      Passwd.LsbPasswd = ~LsbPasswd;
      BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
    }
  }
  return ret;
}

/**
  * @brief  Changing the Energy harvesting mode 
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  ST25DV_EH_MODE_STATUS NewEHMode Energy harvesting mode
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_CheckChangeEHMODE(uint32_t MsbPasswd,uint32_t LsbPasswd,ST25DV_EH_MODE_STATUS NewEHMode)
{
  int32_t ret = NFCTAG_OK;
  ST25DV_EH_MODE_STATUS EHmode;
  ret = BSP_NFCTAG_ReadEHMode(BSP_NFCTAG_INSTANCE, &EHmode );
  if((EHmode != NewEHMode) & (ret==NFCTAG_OK)){
    ST25DV_PASSWD Passwd;
    /* Present password to open session  */
    Passwd.MsbPasswd = MsbPasswd;
    Passwd.LsbPasswd = LsbPasswd;
    ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
    BSP_NFCTAG_WriteEHMode(BSP_NFCTAG_INSTANCE, NewEHMode);
    /* present wrong password for closing the session */
    Passwd.MsbPasswd = ~MsbPasswd;
    Passwd.LsbPasswd = ~LsbPasswd;
    BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );
  }
  return ret;
}

/**
  * @brief  Write GPO configuration:
  *      GPO managed by user             = ST25DVXXKC_GPO1_ENABLE_MASK | ST25DVXXKC_GPO1_RFUSERSTATE_MASK
  *      GPO sensible to RF activity     = ST25DVXXKC_GPO1_ENABLE_MASK | ST25DVXXKC_GPO1_RFACTIVITY_MASK
  *      GPO sensible to RF Field change = ST25DVXXKC_GPO1_ENABLE_MASK | ST25DVXXKC_GPO1_FIELDCHANGE_MASK
  *
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  uint16_t ITConfig Provides the GPO configuration to apply
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_WriteConfigIT(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint16_t ITConfig)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  /* Change the GPO configuration value */
  ret = BSP_NFCTAG_ConfigIT( BSP_NFCTAG_INSTANCE, ITConfig );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  return ret;
}

/**
  * @brief  Change MailboxConfiguration
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  ST25DV_EN_STATUS MB_mode Enable Disable the Mailbox
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_ChangeMBMode(uint32_t MsbPasswd,uint32_t LsbPasswd,const ST25DV_EN_STATUS MB_mode)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  /* Change the Mailbox configuration value */
  ret = BSP_NFCTAG_WriteMBMode( BSP_NFCTAG_INSTANCE, MB_mode );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  return ret;
}

/**
  * @brief  Change the Mailbox watchdog coefficient delay
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  WdgDelay Watchdog duration coefficient to be written (Watch dog duration = MB_WDG*30 ms +/- 6%).
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ChangeMBWDG(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t WdgDelay)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  /* Change the Mailbox configuration value */
  ret = BSP_NFCTAG_WriteMBWDG( BSP_NFCTAG_INSTANCE, WdgDelay );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  return ret;
}

/**
  * @brief  Configures the ITtime duration for the GPO pulse.
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  ITtime Coefficient for the Pulse duration to be written (Pulse duration = 302,06 us - ITtime * 512 / fc)
  * @retval int32_t enum status.
  */
int32_t BSP_NFCTAG_ChangeITPulse(uint32_t MsbPasswd,uint32_t LsbPasswd,const ST25DV_PULSE_DURATION ITtime)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  /* Change the Mailbox configuration value */
  ret = BSP_NFCTAG_WriteITPulse( BSP_NFCTAG_INSTANCE, ITtime );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, Passwd );

  return ret;
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

/**
 * @}
 */
 
