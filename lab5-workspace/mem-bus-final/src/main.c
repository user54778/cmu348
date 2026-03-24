// This implements an 8 bit memory bus, and software sends data
// as either a read or write operation to the 373.
// $CC = write
// $9C = read
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_spi.h"
// #include "stm32f0xx_hal_uart.h"
#include "system_stm32f0xx.h"
#include <stdint.h>

#include "main.h"
// #include <stdint.h>
//  #include <stdio.h>

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void writeToBus(volatile uint8_t address, volatile uint8_t data);

/*
int _write(int fd, char *ptr, size_t len) {
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}
*/

int main(void) {

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  // are peripherals config?
  for (int i = 0; i < 3; i++) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_Delay(300);
  }

  /*
  uint32_t ahb_clk = HAL_RCC_GetHCLKFreq();

  printf("CLK= %lu Hz\n", ahb_clk);
  HAL_Delay(100);
  */

  // clear LED on startup
  /*
  GPIOC->ODR = (GPIOC->ODR & 0xFF00) | 0xCC;
  HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);

  GPIOC->ODR = (GPIOC->ODR & 0xFF00) | 0x00; // turn all LEDs OFF
  HAL_GPIO_WritePin(READ_LINE_GPIO_Port, READ_LINE_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(READ_LINE_GPIO_Port, READ_LINE_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  */
  writeToBus(0xCC, 0x00);

  // uint8_t led_pattern = 0x01;

  for (uint8_t i = 0; i < 16; i++) {
    writeToBus(0xCC, i);
    HAL_Delay(200);
  }

  HAL_Delay(100);
  writeToBus(0xCC, 0x00);

  while (1) {
    /*
    // 1) Send 0xCC in binary to U14 on the memory bus.
    // You should see 11001100 = 0xCC on the logic analyzer.
    // address phase
    GPIOC->ODR = (GPIOC->ODR & 0xFF00) | 0xCC;
    HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // put data on bus
    // data phase
    GPIOC->ODR = (GPIOC->ODR & 0xFF00) | 0x01;
    // toggle PB4 (U9) HIGH for a write
    HAL_GPIO_WritePin(READ_LINE_GPIO_Port, READ_LINE_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_RESET);
    */

    // writeToBus(0xCC, 0x00);
    // HAL_Delay(1000);
  }
}

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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC,
                    ADDR0_Pin | ADDR1_Pin | ADDR2_Pin | ADDR3_Pin | ADDR4_Pin |
                        ADDR5_Pin | ADDR6_Pin | ADDR7_Pin,
                    GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(READ_LINE_GPIO_Port, READ_LINE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ADDR0_Pin ADDR1_Pin ADDR2_Pin ADDR3_Pin
                           ADDR4_Pin ADDR5_Pin ADDR6_Pin ADDR7_Pin */
  // PC0-PC7
  GPIO_InitStruct.Pin = ADDR0_Pin | ADDR1_Pin | ADDR2_Pin | ADDR3_Pin |
                        ADDR4_Pin | ADDR5_Pin | ADDR6_Pin | ADDR7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LE_Pin/G2A U11 */
  GPIO_InitStruct.Pin = LE_Pin; // PA4
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : READ_LINE_Pin */
  GPIO_InitStruct.Pin = READ_LINE_Pin; // PB5
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(READ_LINE_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5; // user led
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

// Write data to the 8 bit bus following the format:
// 1) Decode the address
// 2) Send the relevant data
void writeToBus(volatile uint8_t address, volatile uint8_t data) {
  // Address phase
  GPIOC->ODR = (GPIOC->ODR & 0xFF00) | address;
  HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOA, LE_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);

  // Data phase
  GPIOC->ODR = (GPIOC->ODR & 0xFF00) | data;
  HAL_GPIO_WritePin(READ_LINE_GPIO_Port, READ_LINE_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(READ_LINE_GPIO_Port, READ_LINE_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
