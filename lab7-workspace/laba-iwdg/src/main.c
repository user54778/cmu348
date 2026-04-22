// This program shall configure the IWDG and timing loop from
// a written exercise to showcase how the Watchdog timer
// on the stm32 works.
// It shall implement a timing loop, reading GPIO input pins.
// The goal is to find a min and max GPIO (switch) value to allow
// the loop to run indefinitely and compare against the pre-lab.
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_iwdg.h"
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

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  // Wired such that PA8-PA11 and PB3-PB6 are wired to an LED array.
  // PC0-PC7 are wired to a mechanical DIP switch with internal pull ups.
  // The 8-bit switch value will be used as the MSB of the LOOP_PARAMETER.
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  HAL_GPIO_WritePin(GPIOB,
                    GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                        GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11,
                    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
                    GPIO_PIN_RESET);

  // Check if the IWDG was reset
  if ((__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))) {
    for (int i = 0; i < 5; i++) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
      HAL_Delay(100);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
      HAL_Delay(100);
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }

  // Configure IWDG with IWDG->PR, IWDG->RLR.
  // 1. Enable the IWDG by writing 0x0000 CCCC in the IWDG key register
  // (IWDG_KR).
  // 2. Enable register access by writing 0x0000 5555 in the IWDG key register
  // (IWDG_KR).
  // 3. Write the prescaler by programming the IWDG prescaler register (IWDG_PR)
  // from 0 to 7.
  // 4. Write the IWDG reload register (IWDG_RLR).
  // 5.Wait for the registers to be updated (IWDG_SR = 0x0000 0000).
  // 6.Refresh the counter value with IWDG_RLR (IWDG_KR = 0x0000 AAAA).
  IWDG->KR = IWDG_KEY_ENABLE;              // 1
  IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE; // 2
  IWDG->PR = IWDG_PRESCALER_32;            // 3 (given in pre-lab)
  IWDG->RLR = 0x7CF;                       // 4 (decimal 1999)
  while (IWDG->SR)
    ;
  // should take 1.6 seconds to count down.
  // TODO: Implement the following timing loop.
  /*
loopForever: -> read dip switch value, update LED, kick wdg
    load LOOP_PARAMETER into loop counter register
    ; --- some work here (e.g., update output) ---
    ; POINT 1
waitLoop:
    NOP  ; (repeated N times)
    decrement counter
    branch to waitLoop if counter != 0
    ; kick IWDG: write 0xAAAA to IWDG->KR
    MOVS R0, #0xAAAA
    LDR  R1, =0x40003000   ; IWDG base address
    STR  R0, [R1, #0x00]   ; write to IWDG->KR
    ; POINT 1 (next iteration)
    B loopForever
    */
  //  Read the GPIO input pins at the top of each outer loop iteration to
  // form LOOP_PARAMETER. Use the read value as the MSB of a 16-bit counter.
  //  Use the DWT cycle counter to verify the loop timing matches the
  //  formula we pre-computed on paper.
  //   Walk the dog after the delay.
  //  Find the minimum and maximum GPIO (switch) values that allow the
  //  loop to run indefinitely without a watchdog reset.
  while (1) {
    uint8_t dipSwitchValue =
        ~(GPIOC->IDR >> 0) & 0xFF; // extract pins 0-7 of PC0-PC7
    // uint16_t loopParameter = dipSwitchValue << 8;
    //  Pick a constant s.t. you map 8-bit input into 32-bit loop count
    //  Assuming switch value of 128...
    //  Min = 1.
    //  Max < 64.
    const uint32_t scale = 20000;
    uint32_t loopParameter = dipSwitchValue * scale;
    while (loopParameter > 0) {
      __asm volatile("NOP");
      loopParameter--;
    }
    // DIP switch gives 8-bit value; shift it left 8 bits to give MSB of
    // 16-bit inner loop counter. This is so we have a higher value to count
    // from. This inner loop will then count down from the value, doing NOPs per
    // iteration after the inner loop finishes, walk the wdg.
    /*
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_Delay(300);
    */
    IWDG->KR = IWDG_REFRESH; // refresh WDG timer
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
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // Configure DIP switch GPIO inputs
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure LED GPIO outputs
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_4 | GPIO_PIN_5 |
                        GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure user LED PA5
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
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
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
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
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/*
if (dipSwitchValue) {
  HAL_GPIO_WritePin(GPIOB, dipSwitchValue, GPIO_PIN_SET);
} else {
  HAL_GPIO_WritePin(GPIOB, dipSwitchValue, GPIO_PIN_RESET);
}
*/
/*
if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
} else {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}
*/
