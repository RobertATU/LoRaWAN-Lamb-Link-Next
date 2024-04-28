/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"
#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"

#include "st25dv_conf.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_RTC_Init(void);
void MX_ADC1_Init(void);
void MX_TIM16_Init(void);
void MX_SPI2_Init(void);
void MX_IPCC_Init(void);
void MX_RF_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define E11_WB_PA2_LPUART1_TX_Pin GPIO_PIN_2
#define E11_WB_PA2_LPUART1_TX_GPIO_Port GPIOA
#define E14_WB_PA1_TIM2CH2_Pin GPIO_PIN_1
#define E14_WB_PA1_TIM2CH2_GPIO_Port GPIOA
#define E13_WB_PA0_WKUP_Pin GPIO_PIN_0
#define E13_WB_PA0_WKUP_GPIO_Port GPIOA
#define E13_WB_PA0_WKUP_EXTI_IRQn EXTI0_IRQn
#define B44_WB_PC3_ADC_Pin GPIO_PIN_3
#define B44_WB_PC3_ADC_GPIO_Port GPIOC
#define B66_WB_PC2_SPI2_MISO_Pin GPIO_PIN_2
#define B66_WB_PC2_SPI2_MISO_GPIO_Port GPIOC
#define E21_WB_PC1_ADC_Pin GPIO_PIN_1
#define E21_WB_PC1_ADC_GPIO_Port GPIOC
#define E23_WB_PB9_I2C1_SDA_Pin GPIO_PIN_9
#define E23_WB_PB9_I2C1_SDA_GPIO_Port GPIOB
#define E48_WB_PC0_I2C3_SCL_Pin GPIO_PIN_0
#define E48_WB_PC0_I2C3_SCL_GPIO_Port GPIOC
#define E24_WB_PB8_I2C1_SCL_Pin GPIO_PIN_8
#define E24_WB_PB8_I2C1_SCL_GPIO_Port GPIOB
#define E27_WB_PB7_USART1_RX_Pin GPIO_PIN_7
#define E27_WB_PB7_USART1_RX_GPIO_Port GPIOB
#define E20_WB_PB5_SPI1_MOSI_Pin GPIO_PIN_5
#define E20_WB_PB5_SPI1_MOSI_GPIO_Port GPIOB
#define B32_WB_PB4_UART_CTS_Pin GPIO_PIN_4
#define B32_WB_PB4_UART_CTS_GPIO_Port GPIOB
#define B28_WB_PB3_UART_RTS_Pin GPIO_PIN_3
#define B28_WB_PB3_UART_RTS_GPIO_Port GPIOB
#define WB_PC10_SOB_WL_RST_Pin GPIO_PIN_10
#define WB_PC10_SOB_WL_RST_GPIO_Port GPIOC
#define E25_WB_PC11_EXTI_Pin GPIO_PIN_11
#define E25_WB_PC11_EXTI_GPIO_Port GPIOC
#define E25_WB_PC11_EXTI_EXTI_IRQn EXTI15_10_IRQn
#define B9_WB_PC12_STSAFE_RST_Pin GPIO_PIN_12
#define B9_WB_PC12_STSAFE_RST_GPIO_Port GPIOC
#define E50_WB_PA13_SWDIO_Pin GPIO_PIN_13
#define E50_WB_PA13_SWDIO_GPIO_Port GPIOA
#define E51_WB_PA14_SWCLK_Pin GPIO_PIN_14
#define E51_WB_PA14_SWCLK_GPIO_Port GPIOA
#define E15_WB_PA15_GPIO_Pin GPIO_PIN_15
#define E15_WB_PA15_GPIO_GPIO_Port GPIOA
#define B42_WB_PA10_UART_RX_Pin GPIO_PIN_10
#define B42_WB_PA10_UART_RX_GPIO_Port GPIOA
#define B34_WB_PB13_GPIO_Pin GPIO_PIN_13
#define B34_WB_PB13_GPIO_GPIO_Port GPIOB
#define B11_WB_PB15_SPI2_MOSI_Pin GPIO_PIN_15
#define B11_WB_PB15_SPI2_MOSI_GPIO_Port GPIOB
#define E26_WB_PB6_USART1_TX_Pin GPIO_PIN_6
#define E26_WB_PB6_USART1_TX_GPIO_Port GPIOB
#define E40_WB_PC13_EXTI_Pin GPIO_PIN_13
#define E40_WB_PC13_EXTI_GPIO_Port GPIOC
#define E40_WB_PC13_EXTI_EXTI_IRQn EXTI15_10_IRQn
#define B23_WB_PB12_SAI1_FS_A_Pin GPIO_PIN_12
#define B23_WB_PB12_SAI1_FS_A_GPIO_Port GPIOB
#define B100_WB_PE4_SOB_VOUT2_CTRL_Pin GPIO_PIN_4
#define B100_WB_PE4_SOB_VOUT2_CTRL_GPIO_Port GPIOE
#define E12_WB_PC5_ADC_Pin GPIO_PIN_5
#define E12_WB_PC5_ADC_GPIO_Port GPIOC
#define E47_WB_PB11_I2C3_SDA_Pin GPIO_PIN_11
#define E47_WB_PB11_I2C3_SDA_GPIO_Port GPIOB
#define E35_WB_PB10_QUADSPI_CLK_Pin GPIO_PIN_10
#define E35_WB_PB10_QUADSPI_CLK_GPIO_Port GPIOB
#define B33_WB_PB2_GPIO_INT_Pin GPIO_PIN_2
#define B33_WB_PB2_GPIO_INT_GPIO_Port GPIOB
#define E29_WB_PC4_EXTI_Pin GPIO_PIN_4
#define E29_WB_PC4_EXTI_GPIO_Port GPIOC
#define E29_WB_PC4_EXTI_EXTI_IRQn EXTI4_IRQn
#define B31_WB_PA9_UART_TX_Pin GPIO_PIN_9
#define B31_WB_PA9_UART_TX_GPIO_Port GPIOA
#define E32_WB_PA7_QUADSPI_BK1_IO2_Pin GPIO_PIN_7
#define E32_WB_PA7_QUADSPI_BK1_IO2_GPIO_Port GPIOA
#define E19_WB_PA6_SPI1_MISO_Pin GPIO_PIN_6
#define E19_WB_PA6_SPI1_MISO_GPIO_Port GPIOA
#define E17_WB_PA5_SPI1_SCK_Pin GPIO_PIN_5
#define E17_WB_PA5_SPI1_SCK_GPIO_Port GPIOA
#define E18_WB_PA4_SPI1_CS_Pin GPIO_PIN_4
#define E18_WB_PA4_SPI1_CS_GPIO_Port GPIOA
#define E10_WB_PA3_LPUART1_RX_Pin GPIO_PIN_3
#define E10_WB_PA3_LPUART1_RX_GPIO_Port GPIOA
#define B4_WB_PH0_SHTDN_Pin GPIO_PIN_0
#define B4_WB_PH0_SHTDN_GPIO_Port GPIOH
#define B7_WB_PH1_MAIN_V_REG2_EN_Pin GPIO_PIN_1
#define B7_WB_PH1_MAIN_V_REG2_EN_GPIO_Port GPIOH
#define E42_WB_PD14_Pin GPIO_PIN_14
#define E42_WB_PD14_GPIO_Port GPIOD
#define B10_WB_PE1_SOB_POWER_GOOD_Pin GPIO_PIN_1
#define B10_WB_PE1_SOB_POWER_GOOD_GPIO_Port GPIOE
#define B10_WB_PE1_SOB_POWER_GOOD_EXTI_IRQn EXTI1_IRQn
#define B8_WB_PD13_SOB_D0_1_2_CTRL_Pin GPIO_PIN_13
#define B8_WB_PD13_SOB_D0_1_2_CTRL_GPIO_Port GPIOD
#define E41_WB_PD12_EXTI_Pin GPIO_PIN_12
#define E41_WB_PD12_EXTI_GPIO_Port GPIOD
#define E41_WB_PD12_EXTI_EXTI_IRQn EXTI15_10_IRQn
#define E33_WB_PD7_QUADSPI_BK1_IO3_Pin GPIO_PIN_7
#define E33_WB_PD7_QUADSPI_BK1_IO3_GPIO_Port GPIOD
#define WB_PC9_SOB_WL_INT_Pin GPIO_PIN_9
#define WB_PC9_SOB_WL_INT_GPIO_Port GPIOC
#define WB_PC9_SOB_WL_INT_EXTI_IRQn EXTI9_5_IRQn
#define E34_WB_PD3_QUADSPI_BK1_NCS_Pin GPIO_PIN_3
#define E34_WB_PD3_QUADSPI_BK1_NCS_GPIO_Port GPIOD
#define E8_WB_PC7_MAIN_VDD_EN_Pin GPIO_PIN_7
#define E8_WB_PC7_MAIN_VDD_EN_GPIO_Port GPIOC
#define E28_WB_PE3_EXTI_Pin GPIO_PIN_3
#define E28_WB_PE3_EXTI_GPIO_Port GPIOE
#define E28_WB_PE3_EXTI_EXTI_IRQn EXTI3_IRQn
#define E30_WB_PD4_QUADSPI_BK1_IO0_Pin GPIO_PIN_4
#define E30_WB_PD4_QUADSPI_BK1_IO0_GPIO_Port GPIOD
#define E7_WB_PD9_EXTI_Pin GPIO_PIN_9
#define E7_WB_PD9_EXTI_GPIO_Port GPIOD
#define E16_WB_PD15_Pin GPIO_PIN_15
#define E16_WB_PD15_GPIO_Port GPIOD
#define B35_WB_PE2_GPIO_Pin GPIO_PIN_2
#define B35_WB_PE2_GPIO_GPIO_Port GPIOE
#define E6_WB_PE0_Pin GPIO_PIN_0
#define E6_WB_PE0_GPIO_Port GPIOE
#define E31_WB_PD5_QUADSPI_BK1_IO1_Pin GPIO_PIN_5
#define E31_WB_PD5_QUADSPI_BK1_IO1_GPIO_Port GPIOD
#define E31_WB_PD5_QUADSPI_BK1_IO1_EXTI_IRQn EXTI9_5_IRQn
#define B24_WB_PD6_SAI1_SD_A_Pin GPIO_PIN_6
#define B24_WB_PD6_SAI1_SD_A_GPIO_Port GPIOD
#define E9_WB_PD11_GPIO_Pin GPIO_PIN_11
#define E9_WB_PD11_GPIO_GPIO_Port GPIOD
#define WB_PC8_SOB_WL_WKUP_Pin GPIO_PIN_8
#define WB_PC8_SOB_WL_WKUP_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
