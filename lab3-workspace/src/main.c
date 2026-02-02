// A simple stoplight with transitions based on interrupt timers.
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_rcc_ex.h"
#include "system_stm32f0xx.h"
#include <stdint.h>

// the huart handle we wish to log

void SystemClockConfig(void);
void ErrorHandler(void);

int main() {
  // Initialize HAL; reset peripherals, init flash interface and systick
  HAL_Init();

  __enable_irq();

  // Configure system clock
  SystemClockConfig();

  // configure the SysTick interrupt to interrupt every 1 ms
  SysTick_Config(SystemCoreClock / 1000);

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  typedef enum { RED, YELLOW, GREEN } StopState;
  //  default should be RED

  StopState curr_state = RED;
  // startup a interrupt-based timer to cause transition of automata
  uint32_t stoplight_start_time = 0;

  // reset leds

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4,
                    GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  //  PIN 0 = green
  //  PIN 1 = yellow
  //  PIN 4 = red
  while (1) {
    // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4);
    // HAL_Delay(500);
    //  start in red by default
    //  A simple switch-based stoplight controller state machine
    //  Here we treat "go" as an interrupt
    //  I.e., all events are just timer interrupts

    uint32_t elapsed = HAL_GetTick() - stoplight_start_time;
    switch (curr_state) {
    case GREEN:
      if (elapsed >= 6000) {
        // turn off green light
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        // turn on yellow
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        // set state to yellow
        curr_state = YELLOW;
        // start timer
        stoplight_start_time = HAL_GetTick();
      }
      break;
    case YELLOW:
      if (elapsed >= 2000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        curr_state = RED;
        stoplight_start_time = HAL_GetTick();
      }
      break;
    case RED:
      if (elapsed >= 5000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        curr_state = GREEN;
        stoplight_start_time = HAL_GetTick();
      }
      break;
    default:
      // err
      // log error?
      break;
    }
  }
}

// Configure the CPU clock tree
void SystemClockConfig() {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    ErrorHandler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    ErrorHandler();
  }
}

// Custom ErrorHandler
void ErrorHandler(void) {
  __disable_irq();
  while (1) {
  }
}

void SysTick_Handler(void) { HAL_IncTick(); }
