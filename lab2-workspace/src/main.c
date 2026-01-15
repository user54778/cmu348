/*
 * This corresponds to lab 2. The goal of this program is to compute a digest of
 * a string and put the value on an 8 bit array of LEDs.
 */
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "system_stm32f0xx.h"
#include <stdint.h>

void SystemClockConfig(void);
void ErrorHandler(void);
void Configure_74HC374(void);
void writeValue(unsigned char value);

// The string digest
char test_string[] = "secx";

extern volatile uint8_t button_pressed;

int main() {
  // Initialize HAL; reset peripherals, init flash interface and systick
  HAL_Init();

  __enable_irq();

  // Configure system clock
  SystemClockConfig();

  // configure the SysTick interrupt to interrupt every 1 ms
  SysTick_Config(SystemCoreClock / 1000);

  // init peripherals here
  Configure_74HC374();

  // Lab 2 setup
  unsigned char accum = 0; // accumulator for string hash
  int i = 0;

  // compute 8-bit hash of string value
  while (test_string[i] != 0) {
    accum = accum + test_string[i] + accum;
    i++;
  }

  // conceptually, this is the "output" that gets sent to
  // the LS374 flip flop.
  // This is the output state.
  // writeValue(accum);
  // GPIOA->ODR = (unsigned char)accum;

  while (1) {
    // PC13 reads HIGH (1) when released (not clicked) and LOW (0) when pressed
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
      // button pressed
      writeValue(accum);
    } else {
      // button not pressed
      writeValue(0x00);
    }

    HAL_Delay(10);
    /*
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(500);
    */
    // Flash an LED off the board, otherwise turn it off if user button (13)
    /*
    if (!button_pressed) {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
      HAL_Delay(500);
    }
    GPIOA->ODR &= ~(1 << 7);
    */
    // GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 |
    //                      GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    //  0 1 4 5 6 7 8 9
    // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
    // update first led to flash and reset all others
    /*
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_6 |
                          GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                      GPIO_PIN_RESET);

    // CLK on 374
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_Delay(1000);

    // Now turn on all LEDs
    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                          GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                      GPIO_PIN_SET);

    // CLK 374
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_Delay(1000);
    */
  }
}

// Configure the CPU clock tree, NOT THE CPU TIMERS!!!!
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

// Configure port GPIOA for all D 8-bit registers corresponding to the HC374
// flip-flop.
void Configure_74HC374() {
  // enable clock on ports a and b
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // configure gpioa for d bit registers in 374
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                        GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // configure port b pin PB3 for CLK purposes.
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // enable user button 13
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                        GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

// writeValue writes values to the display where a bit of 1
// should be lit, and 0 unlit.
void writeValue(unsigned char value) {
  GPIOA->ODR = value;

  // Send to CLK on 374
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
}

// Custom ErrorHandler
void ErrorHandler(void) {
  __disable_irq();
  while (1) {
  }
}

void SysTick_Handler(void) { HAL_IncTick(); }

/*
  // Setup NVIC for ISR on button
  NVIC->ISER[0] |= (1 << EXTI4_15_IRQn);
  EXTI->IMR |= (1 << 13);
*/
