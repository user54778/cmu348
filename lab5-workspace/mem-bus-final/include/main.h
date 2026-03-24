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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADDR0_Pin GPIO_PIN_0
#define ADDR0_GPIO_Port GPIOC
#define ADDR1_Pin GPIO_PIN_1
#define ADDR1_GPIO_Port GPIOC
#define ADDR2_Pin GPIO_PIN_2
#define ADDR2_GPIO_Port GPIOC
#define ADDR3_Pin GPIO_PIN_3
#define ADDR3_GPIO_Port GPIOC
#define LE_Pin GPIO_PIN_4
#define LE_GPIO_Port GPIOA
#define DATA_1_Pin GPIO_PIN_6
#define DATA_1_GPIO_Port GPIOA
#define DATA_2_Pin GPIO_PIN_7
#define DATA_2_GPIO_Port GPIOA
#define ADDR4_Pin GPIO_PIN_4
#define ADDR4_GPIO_Port GPIOC
#define ADDR5_Pin GPIO_PIN_5
#define ADDR5_GPIO_Port GPIOC
#define ADDR6_Pin GPIO_PIN_6
#define ADDR6_GPIO_Port GPIOC
#define ADDR7_Pin GPIO_PIN_7
#define ADDR7_GPIO_Port GPIOC
#define DATA_3_Pin GPIO_PIN_8
#define DATA_3_GPIO_Port GPIOA
#define DATA_4_Pin GPIO_PIN_9
#define DATA_4_GPIO_Port GPIOA
#define DATA_5_Pin GPIO_PIN_10
#define DATA_5_GPIO_Port GPIOA
#define DATA_6_Pin GPIO_PIN_11
#define DATA_6_GPIO_Port GPIOA
#define DATA_7_Pin GPIO_PIN_12
#define DATA_7_GPIO_Port GPIOA
#define DATA_8_Pin GPIO_PIN_15
#define DATA_8_GPIO_Port GPIOA
#define READ_LINE_Pin GPIO_PIN_5
#define READ_LINE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
