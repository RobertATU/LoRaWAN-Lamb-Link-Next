/**
  ******************************************************************************
  * @file    custom_astra.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Astra main board BSP header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup ASTRA_BSP ASTRA board support package
  * @{
  */

/** @addtogroup ASTRA_BSP_MAIN ASTRA main board BSP
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_ASTRA__H__
#define __CUSTOM_ASTRA__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "astra_conf.h"
#include "steval_astra1b_conf.h"
#include "steval_astra1b_bus.h"
#include "steval_astra1b_errno.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/** @addtogroup ASTRA_BSP_MAIN_DEFINES ASTRA main board BSP - Defines
  * @brief Defines
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

#define ASTRA_BTN1      1
#define ASTRA_BTN2      2

#define HADC                    hadc1
#define MX_ADC_INIT             MX_ADC1_Init

/* MAIN_VDD_EN */

/* MAIN_VDD2_EN */

/* MAIN_VBAT_MEAS_EN */

/* MAIN_VBAT_ADC */

/* MAIN_USB_ADC */

/* BTN1 */

/* BTN2 */

/* SHTDN */

/* SENS_SPI_CS */

/* SENS_INT1 */

/* SENS_INT2 */

/* BUZZER */

/* MAIN_VDD_EN */
#define MAIN_VDD_EN_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define MAIN_VDD_EN_PORT            GPIOC
#define MAIN_VDD_EN_PIN             GPIO_PIN_7

/* MAIN_VDD2_EN */
#define MAIN_VDD2_EN_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define MAIN_VDD2_EN_PORT           GPIOH
#define MAIN_VDD2_EN_PIN            GPIO_PIN_1

/* MAIN_VBAT_MEAS_EN */
#define MAIN_VBAT_MEAS_EN_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
#define MAIN_VBAT_MEAS_EN_PORT            GPIOD
#define MAIN_VBAT_MEAS_EN_PIN             GPIO_PIN_9

/* MAIN_VBAT_ADC */
#define MAIN_VBAT_ADC_CLK_ENABLE()    __HAL_RCC__CLK_ENABLE()
#define MAIN_VBAT_ADC_PORT
#define MAIN_VBAT_ADC_PIN

/* MAIN_USB_ADC */
#define MAIN_USB_ADC_CLK_ENABLE()    __HAL_RCC__CLK_ENABLE()
#define MAIN_USB_ADC_PORT
#define MAIN_USB_ADC_PIN

/* BTN1 */
#define BTN1_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define BTN1_PORT            GPIOC
#define BTN1_PIN             GPIO_PIN_13
#define BTN1_LINE_NUM        13
#define BTN1_INT_NUM         EXTI15_10_IRQn

/* BTN2 */
#define BTN2_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define BTN2_PORT            GPIOC
#define BTN2_PIN             GPIO_PIN_11
#define BTN2_LINE_NUM        11
#define BTN2_INT_NUM         EXTI15_10_IRQn

/* SHTDN */
#define USE_SHTDN             1
#define SHTDN_CLK_ENABLE()    __HAL_RCC_GPIOH_CLK_ENABLE()
#define SHTDN_PORT            GPIOH
#define SHTDN_PIN             GPIO_PIN_0

/* SENS_SPI_CS */
#define SENS_SPI_CS_CLK_ENABLE()    __HAL_RCC__CLK_ENABLE()
#define SENS_SPI_CS_PORT
#define SENS_SPI_CS_PIN

/* SENS_INT1 */
#define SENS_INT1_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define SENS_INT1_PORT            GPIOC
#define SENS_INT1_PIN             GPIO_PIN_4
#define SENS_INT1_LINE_NUM        4
#define SENS_INT1_INT_NUM         EXTI4_IRQn

/* SENS_INT2 */
#define SENS_INT2_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
#define SENS_INT2_PORT            GPIOD
#define SENS_INT2_PIN             GPIO_PIN_12
#define SENS_INT2_LINE_NUM        12
#define SENS_INT2_INT_NUM         EXTI15_10_IRQn

/* BUZZER */
#define BUZZER_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUZZER_PORT            GPIOA
#define BUZZER_PIN             GPIO_PIN_1

#if USE_ASTRA_MAINBOARD_BSP
#define BTN1_SWITCH_CASE case BTN1_PIN:BTN1_CB();break;
#else //BTN1_PIN
#define BTN1_SWITCH_CASE
#endif //BTN1_PIN

#if USE_ASTRA_MAINBOARD_BSP
#define BTN2_SWITCH_CASE case BTN2_PIN:BTN2_CB();break;
#else //BTN2_PIN
#define BTN2_SWITCH_CASE
#endif //BTN2_PIN

#if USE_ASTRA_MAINBOARD_BSP
#define SENS_INT1_SWITCH_CASE case SENS_INT1_PIN:SENS_INT1_CB();break;
#else // SENS_INT1_PIN
#define SENS_INT1_SWITCH_CASE
#endif // SENS_INT1_PIN

#if USE_ASTRA_MAINBOARD_BSP
#define SENS_INT2_SWITCH_CASE case SENS_INT2_PIN:SENS_INT2_CB();break;
#else // SENS_INT2_PIN
#define SENS_INT2_SWITCH_CASE
#endif // SENS_INT2_PIN

#if USE_ASTRA_MAINBOARD_BSP
#define  ST25DV_INT_SWITCH_CASE case ST25DV_INT_PIN_GPO_EXTI_PIN:ST25DV_INT_PIN_GPO_EXTI_CB();break;
#else // ST25DV_INT_PIN_GPO_EXTI_PIN
#define ST25DV_INT_SWITCH_CASE
#endif // ST25DV_INT_PIN_GPO_EXTI_PIN

#define CFG_OTP_BASE_ADDRESS    OTP_AREA_BASE

#define CFG_OTP_END_ADRESS      OTP_AREA_END_ADDR

#define HW_VERSION_B01          (('B' << 0) | ('1' << 8) | ('C' << 16) | ('1' << 24))
#define HW_VERSION_UNKNOWN      0

/**
* @}
*/

/** @addtogroup ASTRA_BSP_MAIN_EXTERNAL_VARIABLES ASTRA main board BSP - External variables
  * @brief External variables
  * @{
  */

/* External variables --------------------------------------------------------*/

extern ADC_HandleTypeDef HADC;
extern void Error_Handler(void);
extern void MX_ADC_INIT(void);

/**
* @}
*/

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/** @addtogroup ASTRA_BSP_MAIN_FunctionPrototypes ASTRA main board BSP - Function prototypes
  * @brief External variables
  * @{
  */

/* Exported functions ---------------------------------------------*/

/**
  * @brief  App initialization
  * @param  none
  * @retval none
  */
void custom_astra_init(void);

/**
  * @brief  Get hardware version
  * @param  none
  * @retval hardware version
  */
uint32_t custom_astra_GetHwVersion(void);

/**
  * @brief  Get hardware version string
  * @param  strVersion
  * @retval none
  */
void custom_astra_GetHwVersionString(uint8_t *strVersion);

/**
  * @brief  ADC initialization
  * @param  none
  * @retval none
  */
void custom_astra_adc_init(void);

/**
  * @brief  Vbat measurement circuit initialization
  * @param  none
  * @retval none
  */
void custom_astra_vbat_measurement_circuit_init(void);

/**
  * @brief  Vbat measurement circuit enable
  * @param  none
  * @retval none
  */
void custom_astra_vbat_measurement_circuit_enable(void);

/**
  * @brief  Vbat measurement circuit disable
  * @param  none
  * @retval none
  */
void custom_astra_vbat_measurement_circuit_disable(void);

/**
  * @brief  ADC measurement
  * @param  pBuf pointer to buffer
  * @param  nChannels number of channels
  * @param  nTimeout timeout
  * @retval none
  */
void custom_astra_adc_measurement(uint32_t *pBuf, uint32_t nChannels, uint32_t nTimeout);

/**
  * @brief  Power initialization
  * @param  none
  * @retval none
  */
void custom_astra_pwr_init(void);

/**
  * @brief  Power enable
  * @param  none
  * @retval none
  */
void custom_astra_pwr_enable(void);

/**
  * @brief  Power disable
  * @param  none
  * @retval none
  */
void custom_astra_pwr_disable(void);

/**
  * @brief  Vreg2 enable
  * @param  none
  * @retval none
  */
void custom_astra_v_reg2_on(void);

/**
  * @brief  Vreg2 disable
  * @param  none
  * @retval none
  */
void custom_astra_v_reg2_off(void);

/**
  * @brief  Low Power interrupt pin 1 initialization
  * @param  none
  * @retval none
  */
void custom_astra_lp_acc_interruptpin_init(void);

/**
  * @brief  Low Power interrupt pin 1 get state
  * @param  none
  * @retval pin state
  */
GPIO_PinState custom_astra_lp_acc_intpin_getstate(void);

/**
  * @brief  Sensor interrupt pin 2 initialization
  * @param  none
  * @retval none
  */
void custom_astra_sens_interruptpin2_init(void);

/**
  * @brief  Buzzer initialization
  * @param  none
  * @retval none
  */
void custom_astra_buzzer_init(void);

/**
  * @brief  Buzzer pin high level
  * @param  none
  * @retval none
  */
void custom_astra_buzzer_pinhigh(void);

/**
  * @brief  Buzzer pin low level
  * @param  none
  * @retval none
  */
void custom_astra_buzzer_pinlow(void);

/**
  * @brief  Buttons initialization
  * @param  none
  * @retval none
  */
void custom_astra_btns_init(void);

/**
  * @brief  Button get state
  * @param  btn: Button
  * @retval state
  */
FunctionalState custom_astra_btn_get_state(uint8_t btn);

/**
  * @brief  System shutdown
  * @param  none
  * @retval none
  */
void custom_astra_sysshutdown(void);

/**
  * @brief  System shutdown initialization
  * @param  none
  * @retval none
  */
void custom_astra_sysshutdown_init(void);

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_ASTRA__H__ */

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
