#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_spi.h"
#include "system_stm32f0xx.h"

#include "babydriver.h"
#include "gojo.h"
#include "kuromi.h"
#include "main.h"

void SystemClockConfig(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

SPI_HandleTypeDef hspi1;

/* Setup babydriver */

void setDc(bool state) {
  HAL_GPIO_WritePin(GPIOA, OLED_DC_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setCs(bool state) {
  HAL_GPIO_WritePin(GPIOA, OLED_CS_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void spiTx(uint8_t data) {

  HAL_SPI_Transmit(&hspi1, &data, 1, 10);
  /*
  while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
    ;

  if (status != HAL_OK) {
    Error_Handler();
  }
  */
}

void resetDisplay(void) {
  HAL_GPIO_WritePin(GPIOA, OLED_RES_Pin, GPIO_PIN_RESET);
  HAL_Delay(40);
  HAL_GPIO_WritePin(GPIOA, OLED_RES_Pin, GPIO_PIN_SET);
  HAL_Delay(40);
}

int main() {
  // Initialize HAL; reset peripherals, init flash interface and systick
  HAL_Init();
  //__enable_irq();

  // Configure system clock
  SystemClockConfig();
  // configure the SysTick interrupt to interrupt every 1 ms
  SysTick_Config(SystemCoreClock / 1000);

  // init peripherals here
  // we need to a) init gpio ports, b) configure LD2 LED
  // configure gpio
  /*
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  */

  // initGPIO();
  /*
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  */

  // InitOled(); // Will this destroy HAL_Delay?
  MX_GPIO_Init();
  MX_SPI1_Init();

  // Register babydriver callbacks
  bd_set_spi_tx(&spiTx);
  bd_set_spi_dc(&setDc);
  bd_set_spi_cs(&setCs);

  // hardware reset display
  resetDisplay();

  HAL_Delay(100);

  // init display
  bd_init();

  // test display
  bd_fill_framebuffer(false);
  // test
  for (int x = 0; x < 128; x++) {
    bd_set_pixel(x, 0, true);  // Top line
    bd_set_pixel(x, 16, true); // Quarter way
    bd_set_pixel(x, 32, true); // Half way
    bd_set_pixel(x, 48, true); // 3/4 way
    bd_set_pixel(x, 63, true); // Bottom line
  }
  bd_render_framebuffer();

  while (1) {
    bd_draw_bitmap(0, 0, NAH_ID_WIN_WIDTH, NAH_ID_WIN_HEIGHT, nah_id_win);
    bd_render_framebuffer();

    HAL_Delay(3000);

    bd_draw_bitmap(0, 0, KUROMI_SMALL_WIDTH, KUROMI_SMALL_HEIGHT, List_kuromi);
    bd_render_framebuffer();

    HAL_Delay(3000);

    bd_draw_bitmap(0, 0, ASTA_WIDTH, ASTA_HEIGHT, asta);
    bd_render_framebuffer();
    HAL_Delay(3000);
  }
}

void SystemClockConfig(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

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
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  /*
  HAL_GPIO_WritePin(GPIOA,
                    OLED_RES_Pin | OLED_DC_Pin | OLED_CS_Pin | USER_GREEN_LED,
                    GPIO_PIN_RESET);
                    */
  // set CS and RES HIGH first (MUST START HIGH)
  HAL_GPIO_WritePin(GPIOA, OLED_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, OLED_RES_Pin, GPIO_PIN_SET);

  // set dc and user led LOW
  HAL_GPIO_WritePin(GPIOA, OLED_DC_Pin | USER_GREEN_LED, GPIO_PIN_RESET);

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
    HAL_Delay(10000);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(10000);
  }
  /* USER CODE END Error_Handler_Debug */
}

void SysTick_Handler(void) { HAL_IncTick(); }

/*
int main() {
  // Initialize HAL; reset peripherals, init flash interface and systick
  HAL_Init();
  __enable_irq();
  // Configure system clock
  SystemClockConfig();
  // configure the SysTick interrupt to interrupt every 1 ms
  SysTick_Config(SystemCoreClock / 1000);

  InitOled();

  // start up oled spi
  bd_set_spi_dc(&setDc);
  bd_set_spi_cs(&setCs);
  bd_set_spi_tx(&spiTx);

  // init
  resetDisplay();
  bd_init();

  // bd_draw_bitmap(0, 0, GOJO_WIDTH, GOJO_HEIGHT, gojo);

  while (1) {
    HAL_Delay(1000);
  }
}
*/

/*
// Initialize GPIO for an SSD1306 OLED to use stm32 HAL.
void InitOled(void) {
  // lets use bus A
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // enable clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_SPI1_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, OLED_RES_PIN | OLED_CS_PIN | OLED_DC_PIN,
                    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, OLED_SCK_PIN | OLED_MOSI_PIN, GPIO_PIN_RESET);

  // configure cs, dc, res general io
  GPIO_InitStruct.Pin = OLED_CS_PIN | OLED_DC_PIN | OLED_RES_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // pb3 pb5
  // NOTE: PP == Push-pull, while OD == Open-drain
  // PP -> Actively drive H & L states.
  // OD -> Only actively drive L state.
  GPIO_InitStruct.Pin = OLED_SCK_PIN | OLED_MOSI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  // active low signals
  //
  HAL_GPIO_WritePin(GPIOA, OLED_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, OLED_RES_PIN, GPIO_PIN_SET);

  // SPI init - this will call HAL_SPI_MspInit()
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
}
*/

/*
// Custom ErrorHandler
void ErrorHandler(void) {
  __disable_irq();
  while (1) {
  }
}
*/

// void SysTick_Handler(void) { HAL_IncTick(); }
