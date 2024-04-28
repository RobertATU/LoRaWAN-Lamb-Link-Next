/**
  ******************************************************************************
  * @file    custom_sob.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Astra S.O.B. board BSP
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

/* Includes ------------------------------------------------------------------*/

#include "custom_sob.h"
#include "main.h"

/** @addtogroup ASTRA_BSP_SOB_FunctionPrototypes ASTRA System on board BSP - Function Prototypes
  * @brief Function prototypes
  * @{
 */

/* Function prototypes -------------------------------------------------------*/

/**
* @}
*/

/** @addtogroup ASTRA_BSP_SOB_FunctionDefinitions ASTRA System on board BSP - Function definitions
  * @brief Function definitions
  * @{
 */

/* Function Definitions ------------------------------------------------------*/

/* USE_STM32WL */
#if USE_STM32WL
int32_t custom_sob_stm32wl_deinit()
{
  return 0;
}

int32_t custom_sob_stm32wl_init()
{
  BSP_LPUART1_Init();
  return BSP_ERROR_NONE;
}

int32_t custom_sob_stm32wl_reset(uint8_t level)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  WL_RST_CLK_ENABLE();

  if(level)
  {
    /*Configure GPIO pins */
    GPIO_InitStruct.Pin = WL_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WL_RST_PORT, &GPIO_InitStruct);
  }
  else
  {
    HAL_GPIO_WritePin(WL_RST_PORT, WL_RST_PIN, GPIO_PIN_RESET);

    /*Configure GPIO pins */
    GPIO_InitStruct.Pin = WL_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WL_RST_PORT, &GPIO_InitStruct);
  }
  return 0;
}

void custom_sob_stm32wl_stop_uartrecv_dma(void)
{
  HAL_UART_StateTypeDef huartstate;
  huartstate = HAL_UART_GetState(&hlpuart1);

  if(huartstate & 0x02 /* HUART STATE RX ONGOING */ )
  {
    HAL_UART_Abort(&hlpuart1);
  }
}

int32_t custom_sob_stm32wl_uartrecv_dma(uint8_t *pData, uint32_t Length)
{
  HAL_UART_StateTypeDef huartstate;
  huartstate = HAL_UART_GetState(&hlpuart1);

  if(huartstate & 0x02 /* HUART STATE RX ONGOING */ )
  {
    HAL_UART_Abort(&hlpuart1);
  }

  if(HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)pData, Length) == HAL_OK)
  {
  }
  else
  {
  }

  return BSP_ERROR_NONE;
}

int32_t custom_sob_stm32wl_uartsend(const uint8_t *pData, uint32_t Length)
{
  if(HAL_UART_Transmit(&hlpuart1, (uint8_t *)pData, Length, ASTRA_UART_TIMEOUT) == HAL_OK)
  {
  }
  else
  {
  }

  return BSP_ERROR_NONE;
}

uint32_t custom_sob_stm32wl_get_data_len(void)
{
  return hlpuart1.RxXferSize - hlpuart1.hdmarx->Instance->CNDTR;
}

uint32_t custom_sob_stm32wl_get_buf_size(void)
{
  return hlpuart1.RxXferSize;
}

uint8_t *custom_sob_stm32wl_get_data_buf(void)
{
  return hlpuart1.pRxBuffPtr;
}

#endif //USE_STM32WL

void custom_sob_led_init()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  LED_R_CLK_ENABLE();

  HAL_GPIO_WritePin(LED_R_PORT, LED_R_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_R_PIN */
  GPIO_InitStruct.Pin = LED_R_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_R_PORT, &GPIO_InitStruct);

  LED_G_CLK_ENABLE();

  HAL_GPIO_WritePin(LED_G_PORT, LED_G_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_G_PIN */
  GPIO_InitStruct.Pin = LED_G_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_G_PORT, &GPIO_InitStruct);

  LED_B_CLK_ENABLE();

  HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_B_PIN */
  GPIO_InitStruct.Pin = LED_B_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_B_PORT, &GPIO_InitStruct);
}

void custom_sob_led_r_on()
{
  HAL_GPIO_WritePin(LED_R_PORT, LED_R_PIN, GPIO_PIN_SET);
}

void custom_sob_led_r_off()
{
  HAL_GPIO_WritePin(LED_R_PORT, LED_R_PIN, GPIO_PIN_RESET);
}

void custom_sob_led_r_toggle()
{
  HAL_GPIO_TogglePin(LED_R_PORT, LED_R_PIN);
}

void custom_sob_led_g_on()
{
  HAL_GPIO_WritePin(LED_G_PORT, LED_G_PIN, GPIO_PIN_SET);
}

void custom_sob_led_g_off()
{
  HAL_GPIO_WritePin(LED_G_PORT, LED_G_PIN, GPIO_PIN_RESET);
}

void custom_sob_led_g_toggle()
{
  HAL_GPIO_TogglePin(LED_G_PORT, LED_G_PIN);
}

void custom_sob_led_b_on()
{
  HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, GPIO_PIN_SET);
}

void custom_sob_led_b_off()
{
  HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, GPIO_PIN_RESET);
}

void custom_sob_led_b_toggle()
{
  HAL_GPIO_TogglePin(LED_B_PORT, LED_B_PIN);
}

void custom_sob_power_init()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* D0_1_2_CTRL */
  VOLTAGE_SEL_CLK_ENABLE();

  HAL_GPIO_WritePin(VOLTAGE_SEL_PORT, VOLTAGE_SEL_PIN, GPIO_PIN_SET);

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = VOLTAGE_SEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VOLTAGE_SEL_PORT, &GPIO_InitStruct);

  /* VOUT2_CTRL */
  VOUT2_CTRL_CLK_ENABLE();

  HAL_GPIO_WritePin(VOUT2_CTRL_PORT, VOUT2_CTRL_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = VOUT2_CTRL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VOUT2_CTRL_PORT, &GPIO_InitStruct);

#if USE_POWER_GOOD
  /* POWER_GOOD */
  POWER_GOOD_CLK_ENABLE();

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = POWER_GOOD_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(POWER_GOOD_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(POWER_GOOD_INT_NUM, 0, 0);
  HAL_NVIC_EnableIRQ(POWER_GOOD_INT_NUM);
#endif //USE_POWER_GOOD

}

void custom_sob_v_reg1_out_high_value()
{
  HAL_GPIO_WritePin(VOLTAGE_SEL_PORT, VOLTAGE_SEL_PIN, GPIO_PIN_SET);
}

void custom_sob_v_reg1_out_low_value()
{
  HAL_GPIO_WritePin(VOLTAGE_SEL_PORT, VOLTAGE_SEL_PIN, GPIO_PIN_RESET);
}

void custom_sob_v_reg2_on()
{
  HAL_GPIO_WritePin(VOUT2_CTRL_PORT, VOUT2_CTRL_PIN, GPIO_PIN_SET);
}

void custom_sob_v_reg2_off()
{
  HAL_GPIO_WritePin(VOUT2_CTRL_PORT, VOUT2_CTRL_PIN, GPIO_PIN_RESET);
}

/**
* @} ASTRA_BSP_SOB_FunctionDefinitions
*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
