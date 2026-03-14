// The purpose of this lab is to explore USART on the stm32
// using a global group string, CRC and baud rate
#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_spi.h"
#include "stm32f0xx_hal_uart.h"
#include "system_stm32f0xx.h"

#include "main.h"
#include <stdint.h>
#include <stdio.h>

#define POLY 0xEA
#define QUERY_MAX_LEN 16
#define RESPONSE_MAX_LEN 16

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

int _write(int fd, char *ptr, size_t len) {
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}

// STM32 setup
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

// A bit field containing different flags for
// EXTI lines, each of 1 width.
// NOTE: Global
// extern volatile ExtiFlags extiFlags;

// Compute CRC of USART signal.
unsigned char Crc(char *, unsigned char, unsigned char);
void doUsartComm(void);

// Specific lab globals
char groupString[] = "F070";

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // Configure interrupts
  SysTick_Config(SystemCoreClock / 1000);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  // MX_SPI1_Init();
  MX_USART2_UART_Init();

  // uint8_t msg[] = "F070RB\r\n";
  // HAL_UART_Transmit(&huart2, msg, sizeof(msg) - 1, HAL_MAX_DELAY);

  // printf("%04X\r\n", (unsigned int)(GPIOC->IDR & 0xF));
  // compute CRC of groupString

  doUsartComm();

  while (1) {
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
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
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
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Button_Pin */
  GPIO_InitStruct.Pin = Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Button_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE END MX_GPIO_Init_2 */
}

// --------------- USER CODE ----------------- //

unsigned char Crc(char *string, unsigned char length,
                  unsigned char polynomial) {
  unsigned char crc;
  unsigned char i, j;

  crc = 0;
  for (i = 0; i < length; i++) {
    crc ^= string[i];
    for (j = 0; j < 8; j++) {
      if (0x80 & crc) {
        crc = (crc << 1) ^ polynomial;
      } else {
        crc = crc << 1;
      }
    }
  }
  return crc;
}

// Send USART communication to a PuTTTY instance and receive a response back.
void doUsartComm() {
  char query[QUERY_MAX_LEN + 1];       // string to hold query value;
  char response[RESPONSE_MAX_LEN + 1]; // string to hold response value
  char i;                              // counter
  char crcRx;                          // received CRC value
  unsigned char crcComp;               // computed CRC value
  int errFlag;                         // to hold return values

  /***********TRANSMISSION PREP PORTION**************/

  // compute CRC of groupString
  crcComp = Crc(groupString, 2, POLY);

  // display group string, the CRC, and the current baud rate on the LCD:
  // hint, use sprintf with "%s 0x%02X BR=%d" as your control string

  // uint8_t msg[] = "F070RB\r\n";
  // printf("%s 0x%02X BR=%lu\r\n", groupString, (unsigned int)crcComp,
  //       huart2.Init.BaudRate);
  // wait for button press
  // TODO: wait for the user to press PB1 to initiate transmission
  //
  // Problem: We cant use any while/do while loop since itll block.
  // How can we do a non-blocking approach to let the system blink while waiting
  // for a button click?
  uint32_t timestamp = HAL_GetTick();
  volatile uint32_t blinkTime = 500;
  while (1) {
    if (!(GPIOC->IDR & (1 << 13))) {
      printf("%s 0x%02X BR=%lu\r\n", groupString, (unsigned int)crcComp,
             huart2.Init.BaudRate);
      break;
    } else {
      if (HAL_GetTick() - timestamp >= blinkTime) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        timestamp = HAL_GetTick();
      }
    }
  }
  /*
  while (!(GPIOC->IDR & (1 << 13))) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_Delay(500);
  }
  printf("%s 0x%02X BR=%lu\r\n", groupString, (unsigned int)crcComp,
         huart2.Init.BaudRate);
  HAL_Delay(1000);
  */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(100);

  /****************TRANSMISSION PORTION *************/
  // TODO:  Construct the query by concatentating groupString:crcComp:NULL
  //   Note:  NULL denoted a null byte, not the string "NULL"

  // TODO:  Transmit each byte of the query, including the NULL terminator
  //        hint:  After sending each byte, you should monitor the status
  //               register to see when the transmitter is ready for the next
  //               value.

  /*************RECEPTION PORTION*****************/

  // TODO:  Receive bytes and copy them into 'response' string until
  //        the null character is received or the RESPONSE_MAX_LEN
  //        is reached.
  // Hint:  You must monitor the status register to see when a value
  //        has been received into the data register.

  // TODO:  CRC check
  //        Extract the CRC from the received data and replace the CRC with a
  //        NULL byte. Compute the CRC of the received message. Compare the
  //        received and computed CRC values and display an error if they
  //        don't match

  // TODO Format the response and the crc and display the values on the LCD
  // Hint:  Use sprintf with the control string "%s 0x%x".
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

// Configure EXTI interrupt for SPST switch
// enable peripheral clock of c (needed?)
/*
RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

// reset syscfg as its by default mapped to pa0.
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1;
SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PC;
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;
SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PC;
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PC;

// configure mask bit in IMR
EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
// configure trigger selection on interrupt line on rising edge
// EXTI->RTSR = 0x0001;
// configure trigger select bit on interrupt line on falling edge
EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);

// clear any pr's
EXTI->PR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
// configure NVIC for external interrupt
NVIC_EnableIRQ(EXTI0_1_IRQn);
NVIC_EnableIRQ(EXTI2_3_IRQn);
NVIC_SetPriority(EXTI0_1_IRQn, 0);
NVIC_SetPriority(EXTI2_3_IRQn, 0);

extiFlags.allFlags = 0;
*/

/*
if (extiFlags.flags.pc0Flag) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(500);
  extiFlags.flags.pc0Flag = 0;
} else if (extiFlags.flags.pc1Flag) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(100);
  extiFlags.flags.pc1Flag = 0;
} else if (extiFlags.flags.pc2Flag) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(1000);
  extiFlags.flags.pc2Flag = 0;
} else if (extiFlags.flags.pc3Flag) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(2000);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(2000);
  extiFlags.flags.pc3Flag = 0;
}
*/
// invert to active low
/*
extiFlags.flags.pc0Flag = (GPIOC->IDR & (1 << 0)) ? 0 : 1;
extiFlags.flags.pc1Flag = (GPIOC->IDR & (1 << 1)) ? 0 : 1;
extiFlags.flags.pc2Flag = (GPIOC->IDR & (1 << 2)) ? 0 : 1;
extiFlags.flags.pc3Flag = (GPIOC->IDR & (1 << 3)) ? 0 : 1;
*/
