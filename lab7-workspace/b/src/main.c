// The purpose of this program is to have a better understanding of how
// exceptions work on the cortex-m0 cpu.
// The idea is the main loop will set known values in R0-R3, and trigger
// a HardFault.
// Based on the selection variable in memory, a HardFault handler will
// copy the chosen register's byte into a result variable, patch the PC
// to skip over the hardfault and return. (<- This done all in ASM).
// The program then repeats another iteration with a different selection value.
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_spi.h"
#include "stm32f0xx_hal_uart.h"
#include "system_stm32f0xx.h"

#include "main.h"
#include <stdio.h>

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

int _write(int fd, char *ptr, size_t len) {
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

// A global selection and result variable
volatile uint8_t selection = 0;
volatile uint8_t result = 0;

int main() {

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // Configure interrupts
  SysTick_Config(SystemCoreClock / 1000);

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  for (int i = 0; i < 5; i++) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_Delay(100);
  }

  printf("hello, world!\r\n");

  // The purpose of this is to act as a "breakpoint" in a sense.
  // We hit UDF, and the handler based on the selection value
  // will determine what to do about the given register values.
  result = 0x0;

  /*
  __asm volatile("MOV R0, #0x11"); // register a
  __asm volatile("MOV R1, #0x22"); // register b
  __asm volatile("MOV R2, #0x33"); // register x
  __asm volatile("MOV R3, #0x44"); // register y
  __asm volatile("UDF #0");
  */
  //::: "memory");
  // 0x01, 0x02, 0x04, 0x08
  uint8_t cases[] = {0x01, 0x02, 0x04, 0x08, 0x40, 0x80};
  uint8_t size = 6;

  for (int i = 0; i < size; i++) {
    selection = cases[i];
    __asm volatile("MOV R0, #0x11\n"
                   "MOV R1, #0x22\n"
                   "MOV R2, #0x33\n"
                   "MOV R3, #0x44\n"
                   "UDF #0\n" ::
                       : "r0", "r1", "r2", "r3");
    printf("result: %04x\r\n", result);
  }
  // "::: Clobber" list states r0-r3 will contain new values after this block
  // Compiler, pls dont assume they still hold w/e was held before

  //
  while (1) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(800);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_Delay(800);
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

static void MX_USART2_UART_Init(void) {

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
