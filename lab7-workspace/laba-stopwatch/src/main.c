#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_uart.h"

#include "main.h"
#include <stdint.h>
#include <stdio.h>

UART_HandleTypeDef huart2;
int _write(int fd, char *ptr, size_t len) {
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

// An enumeration of the states our timer detection rollover can be in.
typedef enum {
  INIT = 0,
  HIGH_BIT_SET = 1,
  HIGH_BIT_CLEAR = 2,
  ROLLOVER = 3,
} TimerState;

int main() {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  // flag to mark if button has been pressed
  volatile uint8_t timFlag = 0;

  // Track the exact moment the button changes from one state to another (i.e.,
  // edge detection)
  uint8_t btnNow = 0;
  uint8_t btnPrev = 1;

  // The main goal of this portion is to implement a *free-running* timer, where
  // upon button press once, then again will compute a time slice of that time
  // period.
  // Implement a state machine that monitors the MSB of TIM3->CNT that
  // does the following: 1) Upon initializing the timer, check the MSB
  // immediately
  //    if MSB = 1 -> GOTO HIGH BIT SET
  //    if MSB = 0 -> GOTO HIGH BIT CLEAR
  // 2) HIGH BIT SET - MSB is 1, counter upper half of its range
  //    if MSB = 0 -> GOTO ROLLOVER
  //    if MSB = 1 -> STAY IN HIGH BIT SET
  // 3) ROLLOVER - (MSB transitioned from 1 -> 0)
  //    GOTO HIGH BIT CLEAR
  // 4) HIGH BIT CLEAR - MSB is 0, counter in lower half range
  //    if MSB = 1 -> GOTO HIGH BIT SET
  //    if MSB = 0 -> stay in HIGH BIT CLEAR

  // Setup TIM3 timer to be a free-running upcounter
  // enable peripheral clock of timer
  // set prescaler to 0.5 MHz
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3->PSC = 0x0F;         // prescaler = 0.5MHz
  TIM3->ARR = 0xFFFF;       // arr to max
  TIM3->CR1 |= TIM_CR1_CEN; // start timer

  TimerState state = INIT;
  int32_t timeAccumulator = 0;

  while (1) {
    uint16_t cnt = TIM3->CNT;      // grab the counter value
    uint8_t msb = (cnt >> 15) & 1; // grab its MSB
    // state machine that monitors MSB of CNT value
    switch (state) {
    case INIT:
      if (msb) {
        state = HIGH_BIT_SET;
      } else {
        state = HIGH_BIT_CLEAR;
      }
      break;
    case HIGH_BIT_SET:
      if (!msb) {
        state = ROLLOVER;
      }
      break;
    case HIGH_BIT_CLEAR:
      if (msb) {
        state = HIGH_BIT_SET;
      }
      break;
    case ROLLOVER:
      // Rollover = 65536 ticks * 2 microseconds = 131.072 ms.
      // In fixed point => 131.072e-3 * 65536 = 8590 = 0x218E
      timeAccumulator += 0x218E;
      state = HIGH_BIT_CLEAR;
      break;
    default:
      Error_Handler(); // should never happen
    }
    // HAL version of stopwatch
    btnNow =
        (GPIOC->IDR & (1 << 13)) ? 1 : 0; // 1 = button released, 0 = pressed

    // Act only on when the button got pressed once.
    if (btnPrev == 1 && btnNow == 0) {
      if (!timFlag) {
        timeAccumulator = 0;
        timFlag = 1;
      } else {
        timFlag = 0;
        uint32_t timeAccumulated = timeAccumulator >> 16;
        uint16_t min = timeAccumulated / 60;
        uint16_t sec = timeAccumulated % 60;
        printf("%d:%d\r\n", min, sec);
      }
    }

    btnPrev = btnNow; // lock out re-entry until state changes again (kind of
                      // like a semaphore!)
    // HAL_Delay(50);    // debounce button (remove for real implementation
    // since could cause miss CNT sample)
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
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115400;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5;
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
       if (!(GPIOC->IDR & (1 << 13))) {
         if (!timFlag) {
           startTim = HAL_GetTick();
           timFlag = 1;
         } else {
           endTim = HAL_GetTick() - startTim;
           timFlag = 0;
           printf("Elapsed time: %ld\r\n", endTim);
         }
       } else {
         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
       }
       HAL_Delay(50);
       */
