/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

typedef union {
  struct {
    // 1 bit of width for this field
    volatile uint8_t pc0Flag : 1;
    volatile uint8_t pc1Flag : 1;
    volatile uint8_t pc2Flag : 1;
    volatile uint8_t pc3Flag : 1;
  } flags;
  uint8_t allFlags;
} ExtiFlags;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Button_Pin GPIO_PIN_13
#define Button_GPIO_Port GPIOC
#define SPST_0_Pin GPIO_PIN_0
#define SPST_0_GPIO_Port GPIOC
#define SPST_1_Pin GPIO_PIN_1
#define SPST_1_GPIO_Port GPIOC
#define SPST_2_Pin GPIO_PIN_2
#define SPST_2_GPIO_Port GPIOC
#define SPST_3_Pin GPIO_PIN_3
#define SPST_3_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
