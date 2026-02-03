#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
void NMI_Handler(void) {}

void HardFault_Handler(void) {
  while (1)
    ;
}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}
