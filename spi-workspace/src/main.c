#include "stm32f070xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_gpio_ex.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_rcc_ex.h"
#include "stm32f0xx_hal_spi.h"
#include "system_stm32f0xx.h"

#include "babydriver.h"
#include "gojo.h"

#define OLED_SCK_PIN GPIO_PIN_3  // PB3
#define OLED_MOSI_PIN GPIO_PIN_5 // PB5

#define OLED_RES_PIN GPIO_PIN_1 // PA1
#define OLED_DC_PIN GPIO_PIN_0  // PA0
#define OLED_CS_PIN GPIO_PIN_4  // PA4

void SystemClockConfig(void);
void ErrorHandler(void);
static void InitOled(void);

SPI_HandleTypeDef hspi1;

/* Setup babydriver */

void setDc(bool state) {
  HAL_GPIO_WritePin(GPIOA, OLED_DC_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setCs(bool state) {
  HAL_GPIO_WritePin(GPIOA, OLED_CS_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void spiTx(uint8_t data) { HAL_SPI_Transmit(&hspi1, &data, 1, 10); }

void resetDisplay() {
  HAL_GPIO_WritePin(GPIOA, OLED_RES_PIN, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOA, OLED_RES_PIN, GPIO_PIN_SET);
  HAL_Delay(10);
}

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
  /*
  oled_memset(BD_FRAMEBUFFER, 0xFF, BD_FB_SIZE);
  bd_render_framebuffer();
  */

  while (1) {
    HAL_Delay(1000);
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

// Initialize GPIO for an SSD1306 OLED to use stm32 HAL.
static void InitOled(void) {
  // lets use bus A
  // TODO: find which GPIOA pins support MOSI/MISO for their custom SPI
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

// Add this function to your code:
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hspi->Instance == SPI1) {
    // Enable clocks
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure SPI pins (PB3=SCK, PB5=MOSI)
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

// Custom ErrorHandler
void ErrorHandler(void) {
  __disable_irq();
  while (1) {
  }
}

void SysTick_Handler(void) { HAL_IncTick(); }
