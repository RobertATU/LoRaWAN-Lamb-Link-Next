/**
  ******************************************************************************
  * @file    custom_sob.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Astra S.O.B. board BSP header file
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

/** @addtogroup ASTRA_BSP_SOB ASTRA SOB board BSP
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_SOB__H__
#define __CUSTOM_SOB__H__

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

/** @addtogroup ASTRA_BSP_SOB_DEFINES ASTRA System on board BSP - Defines
  * @brief Defines
  * @{
 */

/* Exported defines ----------------------------------------------------------*/

/* VOLTAGE_SEL */
#define VOLTAGE_SEL_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
#define VOLTAGE_SEL_PORT            GPIOD
#define VOLTAGE_SEL_PIN             GPIO_PIN_13

/* POWER_GOOD */
#define USE_POWER_GOOD              1
#define POWER_GOOD_CLK_ENABLE()     __HAL_RCC_GPIOE_CLK_ENABLE()
#define POWER_GOOD_PORT             GPIOE
#define POWER_GOOD_PIN              GPIO_PIN_1
#define POWER_GOOD_LINE_NUM         1
#define POWER_GOOD_INT_NUM          EXTI1_IRQn

/* VOUT2_CTRL */
#define VOUT2_CTRL_CLK_ENABLE()     __HAL_RCC_GPIOE_CLK_ENABLE()
#define VOUT2_CTRL_PORT             GPIOE
#define VOUT2_CTRL_PIN              GPIO_PIN_4

/* LED_R */
#define LED_R_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED_R_PORT                  GPIOD
#define LED_R_PIN                   GPIO_PIN_14

/* LED_G */
#define LED_G_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED_G_PORT                  GPIOD
#define LED_G_PIN                   GPIO_PIN_15

/* LED_B */
#define LED_B_CLK_ENABLE()          __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED_B_PORT                  GPIOE
#define LED_B_PIN                   GPIO_PIN_0

/* WL_RST */
#define WL_RST_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define WL_RST_PORT                 GPIOC
#define WL_RST_PIN                  GPIO_PIN_10

/* WL_WKUP */
#define WL_WKUP_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define WL_WKUP_PORT                GPIOC
#define WL_WKUP_PIN                 GPIO_PIN_8

/* WL_UART */
#define WL_UART_INST                LPUART1
#define WL_UART_HANDLE              hlpuart1

/* WL_INT */
#define WL_INT_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define WL_INT_PORT                 GPIOC
#define WL_INT_PIN                  GPIO_PIN_9
#define WL_INT_LINE_NUM             9
#define WL_INT_INT_NUM              EXTI9_5_IRQn

#define ASTRA_UART_TIMEOUT          0x10000

#define H_DBG_HUART                 huart1

#define POWER_GOOD_SWITCH_CASE      case POWER_GOOD_PIN:POWER_GOOD_CB();break;

/* USER CODE END EC */

/**
* @}
*/

/** @addtogroup ASTRA_BSP_SOB_EXTERNAL_VARIABLES ASTRA System on board BSP - External variables
  * @brief External variables
  * @{
  */

/* External variables --------------------------------------------------------*/

extern UART_HandleTypeDef H_DBG_HUART;

/* USER CODE BEGIN EV */

/* USER CODE END EV */
/**
* @}
*/

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/** @addtogroup ASTRA_BSP_SOB_FunctionPrototypes ASTRA System on board BSP - Function prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/* Exported functions ---------------------------------------------*/

/* USER CODE BEGIN EF */

/**
  * @brief  LED initialization
  * @param  none
  * @retval none
  */
void custom_sob_led_init(void);

/**
  * @brief  LED red ON
  * @param  none
  * @retval none
  */
void custom_sob_led_r_on(void);

/**
  * @brief  LED red OFF
  * @param  none
  * @retval none
  */
void custom_sob_led_r_off(void);

/**
  * @brief  LED red toggle
  * @param  none
  * @retval none
  */
void custom_sob_led_r_toggle(void);

/**
  * @brief  LED green ON
  * @param  none
  * @retval none
  */
void custom_sob_led_g_on(void);

/**
  * @brief  LED green OFF
  * @param  none
  * @retval none
  */
void custom_sob_led_g_off(void);

/**
  * @brief  LED green toggle
  * @param  none
  * @retval none
  */
void custom_sob_led_g_toggle(void);

/**
  * @brief  LED blue ON
  * @param  none
  * @retval none
  */
void custom_sob_led_b_on(void);

/**
  * @brief  LED blue OFF
  * @param  none
  * @retval none
  */
void custom_sob_led_b_off(void);

/**
  * @brief  LED blue toggle
  * @param  none
  * @retval none
  */
void custom_sob_led_b_toggle(void);

/**
  * @brief  Power initialization
  * @param  none
  * @retval none
  */
void custom_sob_power_init(void);

/**
  * @brief  Vreg1 output high value
  * @param  none
  * @retval none
  */
void custom_sob_v_reg1_out_high_value(void);

/**
  * @brief  Vreg1 output low value
  * @param  none
  * @retval none
  */
void custom_sob_v_reg1_out_low_value(void);

/**
  * @brief  Vreg1 ON
  * @param  none
  * @retval none
  */
void custom_sob_v_reg2_on(void);

/**
  * @brief  Vreg1 OFF
  * @param  none
  * @retval none
  */
void custom_sob_v_reg2_off(void);

#if USE_STM32WL
/**
  * @brief  STM32WL initialization
  * @param  none
  * @retval none
  */
int32_t custom_sob_stm32wl_init(void);

/**
  * @brief  STM32WL deinitialization
  * @param  none
  * @retval none
  */
int32_t custom_sob_stm32wl_deinit(void);

/**
  * @brief  STM32WL reset
  * @param  none
  * @retval none
  */
int32_t custom_sob_stm32wl_reset(uint8_t level);

/**
  * @brief  STM32WL UART receive
  * @param  none
  * @retval none
  */
int32_t custom_sob_stm32wl_uartrecv_dma(uint8_t *pData, uint32_t Length);

/**
  * @brief  STM32WL UART send
  * @param  none
  * @retval none
  */
int32_t custom_sob_stm32wl_uartsend(const uint8_t *pData, uint32_t Length);

/**
  * @brief  STM32WL UART stop receiving in dma mode
  * @param  none
  * @retval none
  */
void custom_sob_stm32wl_stop_uartrecv_dma(void);

/**
  * @brief  STM32WL UART get received bytes
  * @param  none
  * @retval Number of received bytes
  */
uint32_t custom_sob_stm32wl_get_data_len(void);

/**
  * @brief  STM32WL UART get pointer to receiving buffer
  * @param  none
  * @retval Pointer to receiving buffer
  */
uint8_t *custom_sob_stm32wl_get_data_buf(void);

/**
  * @brief  STM32WL UART get buffer size
  * @param  none
  * @retval Buffer size
  */
uint32_t custom_sob_stm32wl_get_buf_size(void);
#endif //USE_STM32WL

/* USER CODE END EF */

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_SOB__H__ */

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
