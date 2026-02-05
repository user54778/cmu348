#include "main.h"
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_spi.h"
#include "system_stm32f0xx.h"

#include "babydriver.h"
#include "gojo.h"
#include <stdbool.h>

SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

void setDc(bool state) {
  HAL_GPIO_WritePin(GPIOA, OLED_DC_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setCs(bool state) {
  HAL_GPIO_WritePin(GPIOA, OLED_CS_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void spiTx(uint8_t data) {

  HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, &data, 1, 10);
  while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
    ;

  if (status != HAL_OK) {
    Error_Handler();
  }
}

void resetDisplay() {
  HAL_GPIO_WritePin(GPIOA, OLED_RES_Pin, GPIO_PIN_RESET);
  // HAL_Delay(10);
  for (volatile int i = 0; i < 1000000; i++)
    ;
  HAL_GPIO_WritePin(GPIOA, OLED_RES_Pin, GPIO_PIN_SET);
  // HAL_Delay(10);
  for (volatile int i = 0; i < 1000000; i++)
    ;
}

int main(void) {
  // FIXME: HAL_DELAY IS NOT WORKING
  HAL_Init();
  __enable_irq();

  SystemClock_Config();
  SysTick_Config(SystemCoreClock / 1000);

  MX_GPIO_Init();
  MX_SPI1_Init();

  // start up oled spi
  bd_set_spi_dc(&setDc);
  bd_set_spi_cs(&setCs);
  bd_set_spi_tx(&spiTx);

  // init
  resetDisplay();
  bd_init();
  bd_fill_framebuffer(true);

  while (1) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    // HAL_Delay(200);
    for (volatile int j = 0; j < 200000; j++)
      ;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    // HAL_Delay(200);
    for (volatile int j = 0; j < 200000; j++)
      ;
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  // hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA,
                    OLED_RES_Pin | OLED_DC_Pin | OLED_CS_Pin | USER_GREEN_LED,
                    GPIO_PIN_RESET);

  /*Configure GPIO pins : OLED_RES_Pin OLED_DC_Pin OLED_CS_Pin */
  GPIO_InitStruct.Pin =
      OLED_RES_Pin | OLED_DC_Pin | OLED_CS_Pin | USER_GREEN_LED;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    for (volatile int i = 0; i < 100000000; i++)
      ;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    for (volatile int i = 0; i < 100000000; i++)
      ;
  }
  /* USER CODE END Error_Handler_Debug */
}

void SysTick_Handler(void) { HAL_IncTick(); }
