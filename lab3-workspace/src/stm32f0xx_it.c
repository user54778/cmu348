#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
void NMI_Handler(void) {}

void HardFault_Handler(void) {
  while (1)
    ;
}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}

volatile uint8_t button_pressed = 0;

// A ISR for the button 1 that activates the button on a press.
void EXTI4_15_IRQHandler(void) {
  // did D11 GPIO trigger?
  // if (EXTI->PR & (1 << 13)) {
  EXTI->PR |= (1 << 13);

  /*
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
  */
  button_pressed = !button_pressed;
  /*
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
  HAL_Delay(500);
  */
  //}
}
