/**
  ******************************************************************************
  * @file    custom_astra.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Astra main board BSP
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

/* Includes ------------------------------------------------------------------*/

#include "custom_astra.h"
#include <string.h>
#include "RTE_Components.h"

/** @addtogroup ASTRA_BSP_MAIN_PRIVATE_VARIABLES ASTRA main board BSP - Private variables
  * @brief Private variables
  * @{
 */

/* Private variables ---------------------------------------------------------*/

uint32_t hwversion = HW_VERSION_UNKNOWN;

/**
* @}
*/

/* Private function ----------------------------------------------------------*/

__weak void Astra_Error_Handler(void)
{
  while(1) {}
}

uint32_t custom_astra_GetHwVersion(void)
{
  return hwversion;
}

void custom_astra_GetHwVersionString(uint8_t *strVersion)
{
  if( hwversion != HW_VERSION_UNKNOWN)
  {
    strVersion[0] = 'M';
    strVersion[1] = (hwversion & 0xFF) >> 0;
    strVersion[2] = (hwversion & 0xFF00) >> 8;
    strVersion[3] = 'S';
    strVersion[4] = (hwversion & 0xFF0000) >> 16;
    strVersion[5] = (hwversion & 0xFF000000) >> 24;
  }
}

void custom_astra_init(void)
{
  uint8_t idvect[] = { 'H', 'w' }; //Hw -> Board Hardware Version
  uint8_t *p_id;
  uint8_t ver[] = { 'X', '0', '0', 'X', '0', '0' };
  p_id = (uint8_t*)(CFG_OTP_BASE_ADDRESS);

  while( ((*( p_id + 6 )) != idvect[0]) && ((*( p_id + 7 )) != idvect[1]) && ( p_id != (uint8_t*)(CFG_OTP_END_ADRESS + 1)) )
  {
    p_id += 8;
  }

  if( (( p_id == (uint8_t*)(CFG_OTP_END_ADRESS + 1)) || ((*( p_id + 6 )) != idvect[0])) && ((*( p_id + 7 )) != idvect[1]) )
  {
    p_id = 0;
  }

  if(p_id != 0)
  {
    memcpy(ver, p_id, 6);
    ver[2] = ver[2] +'0';
    ver[5] = ver[5] +'0';
  }

  if( ver[0] == 'M' && ver[3] == 'S' )
  {
    hwversion = (ver[1] << 0) | (ver[2] << 8) | (ver[4] << 16) | (ver[5] << 24);
  }
}

#if USE_ASTRA_MAINBOARD_BSP

void custom_astra_adc_init(void)
{
  custom_astra_vbat_measurement_circuit_init();
  custom_astra_vbat_measurement_circuit_disable();

  MX_ADC_INIT();
}

void custom_astra_vbat_measurement_circuit_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  MAIN_VBAT_MEAS_EN_CLK_ENABLE();

  HAL_GPIO_WritePin(MAIN_VBAT_MEAS_EN_PORT, MAIN_VBAT_MEAS_EN_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_R_PIN */
  GPIO_InitStruct.Pin = MAIN_VBAT_MEAS_EN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAIN_VBAT_MEAS_EN_PORT, &GPIO_InitStruct);
}

void custom_astra_vbat_measurement_circuit_enable(void)
{
  HAL_GPIO_WritePin(MAIN_VBAT_MEAS_EN_PORT, MAIN_VBAT_MEAS_EN_PIN, GPIO_PIN_SET);
}

void custom_astra_vbat_measurement_circuit_disable(void)
{
  HAL_GPIO_WritePin(MAIN_VBAT_MEAS_EN_PORT, MAIN_VBAT_MEAS_EN_PIN, GPIO_PIN_RESET);
}

void custom_astra_adc_measurement(uint32_t *pBuf, uint32_t nChannels, uint32_t nTimeout)
{
  custom_astra_vbat_measurement_circuit_enable();
  if (HAL_ADC_Start_DMA(&HADC, (uint32_t *)pBuf, nChannels) != HAL_OK)
  {
    /* ADC conversion start error */
    Astra_Error_Handler();
  }

  if (HAL_ADC_PollForConversion(&HADC, nTimeout) != HAL_OK)
  {
    /* ADC poll for conversion error */
    Astra_Error_Handler();
  }

  custom_astra_vbat_measurement_circuit_disable();
}
#endif //USE_ASTRA_MAINBOARD_BSP

void custom_astra_sysshutdown(void)
{
#if USE_SHTDN
#if CHECK_HARDWARE_VERSION
  if(hwversion == HW_VERSION_UNKNOWN || hwversion == HW_VERSION_B01)
  {
    //use B01 pin
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    return;
  }
#endif //CHECK_HARDWARE_VERSION
  HAL_GPIO_WritePin(SHTDN_PORT, SHTDN_PIN, GPIO_PIN_SET);
#endif //USE_SHTDN
}

void custom_astra_sysshutdown_init(void)
{
#if USE_SHTDN
  GPIO_InitTypeDef GPIO_InitStruct = {0};

#if CHECK_HARDWARE_VERSION
  if(hwversion == HW_VERSION_UNKNOWN || hwversion == HW_VERSION_B01)
  {
    //use B01 pin
    __HAL_RCC_GPIOD_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pins : PD.7 */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    return;
  }
#endif //CHECK_HARDWARE_VERSION
  SHTDN_CLK_ENABLE();

  HAL_GPIO_WritePin(SHTDN_PORT, SHTDN_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : SHTDN_PIN */
  GPIO_InitStruct.Pin = SHTDN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHTDN_PORT, &GPIO_InitStruct);
#endif //USE_SHTDN
}

#if USE_ASTRA_MAINBOARD_BSP
void custom_astra_lp_acc_interruptpin_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  SENS_INT1_CLK_ENABLE();

  /* Configure GPIO pin : RST Pin */
  GPIO_InitStruct.Pin = SENS_INT1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SENS_INT1_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(SENS_INT1_INT_NUM, 0, 0);
  HAL_NVIC_EnableIRQ(SENS_INT1_INT_NUM);
}

GPIO_PinState custom_astra_lp_acc_intpin_getstate(void)
{
  return HAL_GPIO_ReadPin(SENS_INT1_PORT, SENS_INT1_PIN);
}

void custom_astra_sens_interruptpin2_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  SENS_INT2_CLK_ENABLE();

  /* Configure GPIO pin : RST Pin */
  GPIO_InitStruct.Pin = SENS_INT2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SENS_INT2_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(SENS_INT2_INT_NUM, 0, 0);
  HAL_NVIC_EnableIRQ(SENS_INT2_INT_NUM);
}

void custom_astra_pwr_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  MAIN_VDD_EN_CLK_ENABLE();

  HAL_GPIO_WritePin(MAIN_VDD_EN_PORT, MAIN_VDD_EN_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : MAIN_VDD_EN_PIN */
  GPIO_InitStruct.Pin = MAIN_VDD_EN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAIN_VDD_EN_PORT, &GPIO_InitStruct);

#if CHECK_HARDWARE_VERSION
  if(hwversion == HW_VERSION_UNKNOWN || hwversion == HW_VERSION_B01)
  {
    //use B01 pin
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pins : PD.7 */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    return;
  }
#endif //CHECK_HARDWARE_VERSION
  MAIN_VDD2_EN_CLK_ENABLE();

  HAL_GPIO_WritePin(MAIN_VDD2_EN_PORT, MAIN_VDD2_EN_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : MAIN_VDD2_EN_PIN */
  GPIO_InitStruct.Pin = MAIN_VDD2_EN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAIN_VDD2_EN_PORT, &GPIO_InitStruct);
}

void custom_astra_pwr_enable(void)
{
  HAL_GPIO_WritePin(MAIN_VDD_EN_PORT, MAIN_VDD_EN_PIN, GPIO_PIN_SET);
}

void custom_astra_pwr_disable(void)
{
  HAL_GPIO_WritePin(MAIN_VDD_EN_PORT, MAIN_VDD_EN_PIN, GPIO_PIN_RESET);
}

void custom_astra_v_reg2_on(void)
{
#if CHECK_HARDWARE_VERSION
  if(hwversion == HW_VERSION_UNKNOWN || hwversion == HW_VERSION_B01)
  {
    //use B01 pin
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    return;
  }
#endif //CHECK_HARDWARE_VERSION
  HAL_GPIO_WritePin(MAIN_VDD2_EN_PORT, MAIN_VDD2_EN_PIN, GPIO_PIN_SET);
}

void custom_astra_v_reg2_off(void)
{
#if CHECK_HARDWARE_VERSION
  if(hwversion == HW_VERSION_UNKNOWN || hwversion == HW_VERSION_B01)
  {
    //use B01 pin
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    return;
  }
#endif //CHECK_HARDWARE_VERSION
  HAL_GPIO_WritePin(MAIN_VDD2_EN_PORT, MAIN_VDD2_EN_PIN, GPIO_PIN_RESET);
}

#endif //USE_ASTRA_MAINBOARD_BSP

void custom_astra_buzzer_init(void)
{
#if USE_BUZZER
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

  BUZZER_CLK_ENABLE();

  /*Configure GPIO pins : BUZZER_PIN */
  GPIO_InitStruct.Pin = BUZZER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
#endif //USE_BUZZER
}

void custom_astra_buzzer_pinhigh(void)
{
#if USE_BUZZER
  HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
#endif //USE_BUZZER
}

void custom_astra_buzzer_pinlow(void)
{
#if USE_BUZZER
  HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
#endif //USE_BUZZER
}

#if USE_ASTRA_MAINBOARD_BSP
void custom_astra_btns_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  BTN1_CLK_ENABLE();

  /* Configure GPIO pin : RST Pin */
  GPIO_InitStruct.Pin = BTN1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BTN1_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(BTN1_INT_NUM, 0, 0);
  HAL_NVIC_EnableIRQ(BTN1_INT_NUM);

  /* GPIO Ports Clock Enable */
  BTN2_CLK_ENABLE();

  /* Configure GPIO pin : RST Pin */
  GPIO_InitStruct.Pin = BTN2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BTN2_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(BTN2_INT_NUM, 0, 0);
  HAL_NVIC_EnableIRQ(BTN2_INT_NUM);
}

/**
  * @brief This function gets the button state.
  */
FunctionalState custom_astra_btn_get_state(uint8_t btn)
{
  FunctionalState ret_status = DISABLE;
  switch(btn)
  {
  case ASTRA_BTN1:
    //active high
    ret_status = (HAL_GPIO_ReadPin(BTN1_PORT, BTN1_PIN) == GPIO_PIN_SET)?ENABLE:DISABLE;
    break;

  case ASTRA_BTN2:
    //active low
    ret_status = (HAL_GPIO_ReadPin(BTN2_PORT, BTN2_PIN) == GPIO_PIN_RESET)?ENABLE:DISABLE;
    break;

  default:
    break;
  }
  return ret_status;
}
#endif //USE_ASTRA_MAINBOARD_BSP

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
