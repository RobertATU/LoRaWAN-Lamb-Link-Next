/**
  ******************************************************************************
  * @file    astra_app_nfc.h
  * @version V1.0.0
  * @date    24-OCt-2022
  * @author  SRA Team
  * @brief   Header file contains the Astra demo
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/** @addtogroup NFC_APPLICATION NFC application
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ASTRA_NFC_H__
#define __APP_ASTRA_NFC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SmartNFC.h"
#include "custom_nfc_ex.h"
#include "TagType5.h"

/** @addtogroup ASTRA_NFC_Private_Defines ASTRA NFC - Private defines
  * @brief Private defines
  * @{
 */  
  
/* Defines -------------------------------------------------------------------*/

/* General defines */
#define BITS_MASK_5             0x1F    /*              1 1111 */
#define BITS_MASK_6             0x3F    /*             11 1111 */
#define BITS_MASK_7             0x7F    /*            111 1111 */
#define BITS_MASK_8             0xFF    /*           1111 1111 */
#define BITS_MASK_9             0x1FF   /*         1 1111 1111 */
#define BITS_MASK_10            0x3FF   /*        11 1111 1111 */
#define BITS_MASK_11            0x7FF   /*       111 1111 1111 */
#define BITS_MASK_12            0xFFF   /*      1111 1111 1111 */
#define BITS_MASK_13            0x1FFF  /*    1 1111 1111 1111 */
#define BITS_MASK_14            0x3FFF  /*   11 1111 1111 1111 */
#define BITS_MASK_15            0x7FFF  /*  111 1111 1111 1111 */
#define BITS_MASK_16            0xFFFF  /* 1111 1111 1111 1111 */

#define BITS_LENGTH_5           5       /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_6           6       /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_7           7       /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_8           8       /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_9           9       /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_10          10      /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_11          11      /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_12          12      /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_13          13      /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_14          14      /* To be used to identify TH low, TH High, Max and Min number of bits */
#define BITS_LENGTH_15          15      /* To be used to identify TH low, TH High, Max and Min number of bits */  

  
/* NFC SMARTNFC virtual sensors defines */
#define BITS_MASK_VSID          0x7     /* It depends by number of bits required by SMARTNFC_VIRTUAL_SENSORS_NUM value */
#define BITS_MASK_TH_MOD        0x3   

#define BITS_LENGTH_VSID        3       /* Number of bits required by SMARTNFC_VIRTUAL_SENSORS_NUM value */
#define BITS_LENGTH_TH_MOD      2       /* Number of bits to indentify the type of threshold */

#define BATT_PERCENTAGE_VS_TH_BITS_LENGHT       BITS_LENGTH_10  
#define BATT_VOLTAGE_VS_TH_BITS_LENGHT          BITS_LENGTH_10  
#define STTS22H_VS_TH_BITS_LENGHT               BITS_LENGTH_9  
#define LPS22HH_VS_TH_BITS_LENGHT               BITS_LENGTH_11  
#define HTS221_VS_TH_BITS_LENGHT                BITS_LENGTH_7  
#define LIS2DTW12_VS_TH_BITS_LENGHT             BITS_LENGTH_15  
#define LSM6DSOX32_VS_TH_BITS_LENGHT            BITS_LENGTH_15  
#define ACC_EVENTS_VS_TH_BITS_LENGHT            BITS_LENGTH_8  


/**
* @} ASTRA_NFC_Private_Defines
*/

/** @addtogroup ASTRA_NFC_FUNCTION_PROTOTYPES ASTRA NFC - Function prototypes
  * @brief Function prototype
  * @{
 */  
  
  
/* Function prototypes -------------------------------------------------------*/

/**
 * @brief  Factory Reset NFC
 * @param  None
 * @retval None.
 */
void APP_NFC_FactoryReset(void);

/**
 * @brief  Force NFC initialization
 * @param  None
 * @retval None.
 */
void APP_NFC_ForceMemInit(void);

/**
 * @brief  Force NFC data initialization
 * @param  New epoch start time
 * @retval None.
 */
void APP_NFC_ForceDataInit(uint32_t NewEpochStartTime);

/**
 * @brief  Read ASTRA settings stored in the NFC memory
 * @param  None
 * @retval None.
 */
void APP_NFC_ReadASTRASettings(void);
    
/**
 * @brief  Save ASTRA settings in the NFC memory
 * @param None
 * @retval None.
 */
void APP_NFC_SaveASTRASettings(void);

/**
  * @brief  This function check the NFC RF activity
  * @param  None
  * @return None
  */
void APP_NFC_CheckRFActivity(void);

/**
  * @brief  This function save data into NFC memory 
  * @param  None
  * @return None
  */
void APP_NFC_SaveData(void);

/**
  * @brief  This function initialize the GPIO to manage the NFCTAG GPO pin
  * @param  None
  * @return Status
  */
int32_t BSP_GPO_Init( void );

/**
  * @brief  This function Deinitialize the GPIO to manage the NFCTAG GPO pin
  * @param  None
  * @return Status
  */
int32_t BSP_GPO_Deinit( void );


/**
* @} ASTRA_NFC_FUNCTION_PROTOTYPES
*/


#ifdef __cplusplus
}
#endif

#endif /*__APP_ASTRA_NFC_H__ */

/**
* @} NFC_APPLICATION
*/

/**
* @} ASTRA_ENGINE
*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
