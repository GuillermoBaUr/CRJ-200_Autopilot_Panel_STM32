/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define OUT_SPD_Pin GPIO_PIN_13
#define OUT_SPD_GPIO_Port GPIOC
#define OUT_XFR_Pin GPIO_PIN_1
#define OUT_XFR_GPIO_Port GPIOH
#define OUT_APPR_Pin GPIO_PIN_0
#define OUT_APPR_GPIO_Port GPIOC
#define OUT_HDG_Pin GPIO_PIN_1
#define OUT_HDG_GPIO_Port GPIOC
#define OUT_AP_ENG_Pin GPIO_PIN_2
#define OUT_AP_ENG_GPIO_Port GPIOC
#define OUT_TURB_Pin GPIO_PIN_3
#define OUT_TURB_GPIO_Port GPIOC
#define INP_ALT_Pin GPIO_PIN_0
#define INP_ALT_GPIO_Port GPIOA
#define INP_FD1_FD2_Pin GPIO_PIN_1
#define INP_FD1_FD2_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define OUT_VS_Pin GPIO_PIN_4
#define OUT_VS_GPIO_Port GPIOA
#define INP_AP_ENG_Pin GPIO_PIN_5
#define INP_AP_ENG_GPIO_Port GPIOA
#define INP_XFR_Pin GPIO_PIN_6
#define INP_XFR_GPIO_Port GPIOA
#define INP_SPD_Pin GPIO_PIN_7
#define INP_SPD_GPIO_Port GPIOA
#define ENC_CRS1_2_EPB_Pin GPIO_PIN_4
#define ENC_CRS1_2_EPB_GPIO_Port GPIOC
#define OUT_1_2_BANK_Pin GPIO_PIN_0
#define OUT_1_2_BANK_GPIO_Port GPIOB
#define ENC_CRS2_B_Pin GPIO_PIN_1
#define ENC_CRS2_B_GPIO_Port GPIOB
#define ENC_CRS2_B_EXTI_IRQn EXTI1_IRQn
#define ENC_CRS2_A_Pin GPIO_PIN_2
#define ENC_CRS2_A_GPIO_Port GPIOB
#define ENC_CRS2_A_EXTI_IRQn EXTI2_IRQn
#define ENC_SPD_B_Pin GPIO_PIN_10
#define ENC_SPD_B_GPIO_Port GPIOB
#define ENC_SPD_B_EXTI_IRQn EXTI15_10_IRQn
#define ENC_VS_B_Pin GPIO_PIN_12
#define ENC_VS_B_GPIO_Port GPIOB
#define ENC_VS_B_EXTI_IRQn EXTI15_10_IRQn
#define ENC_CRS1_B_Pin GPIO_PIN_13
#define ENC_CRS1_B_GPIO_Port GPIOB
#define ENC_CRS1_B_EXTI_IRQn EXTI15_10_IRQn
#define ENC_CRS1_A_Pin GPIO_PIN_14
#define ENC_CRS1_A_GPIO_Port GPIOB
#define ENC_CRS1_A_EXTI_IRQn EXTI15_10_IRQn
#define ENC_ALT_EPB_Pin GPIO_PIN_15
#define ENC_ALT_EPB_GPIO_Port GPIOB
#define ENC_ALT_B_Pin GPIO_PIN_6
#define ENC_ALT_B_GPIO_Port GPIOC
#define ENC_ALT_B_EXTI_IRQn EXTI9_5_IRQn
#define INP_HDG_EPB_Pin GPIO_PIN_7
#define INP_HDG_EPB_GPIO_Port GPIOC
#define ENC_ALT_A_Pin GPIO_PIN_8
#define ENC_ALT_A_GPIO_Port GPIOC
#define ENC_ALT_A_EXTI_IRQn EXTI9_5_IRQn
#define INP_APRR_Pin GPIO_PIN_9
#define INP_APRR_GPIO_Port GPIOC
#define INP_VS_Pin GPIO_PIN_8
#define INP_VS_GPIO_Port GPIOA
#define INP_1_2_BANK_Pin GPIO_PIN_9
#define INP_1_2_BANK_GPIO_Port GPIOA
#define ENC_SPD_EPB_Pin GPIO_PIN_10
#define ENC_SPD_EPB_GPIO_Port GPIOA
#define ENC_VS_A_Pin GPIO_PIN_11
#define ENC_VS_A_GPIO_Port GPIOA
#define ENC_VS_A_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define OUT_NAV_Pin GPIO_PIN_15
#define OUT_NAV_GPIO_Port GPIOA
#define OUT_FD1_FD2_Pin GPIO_PIN_10
#define OUT_FD1_FD2_GPIO_Port GPIOC
#define INP_HDG_Pin GPIO_PIN_11
#define INP_HDG_GPIO_Port GPIOC
#define OUT_ALT_Pin GPIO_PIN_12
#define OUT_ALT_GPIO_Port GPIOC
#define INP_NAV_Pin GPIO_PIN_2
#define INP_NAV_GPIO_Port GPIOD
#define ENC_HDG_A_Pin GPIO_PIN_3
#define ENC_HDG_A_GPIO_Port GPIOB
#define ENC_HDG_A_EXTI_IRQn EXTI3_IRQn
#define ENC_SPD_A_Pin GPIO_PIN_4
#define ENC_SPD_A_GPIO_Port GPIOB
#define ENC_SPD_A_EXTI_IRQn EXTI4_IRQn
#define ENC_HDG_B_Pin GPIO_PIN_5
#define ENC_HDG_B_GPIO_Port GPIOB
#define ENC_HDG_B_EXTI_IRQn EXTI9_5_IRQn
#define INP_BC_Pin GPIO_PIN_6
#define INP_BC_GPIO_Port GPIOB
#define OUT_BC_Pin GPIO_PIN_7
#define OUT_BC_GPIO_Port GPIOB
#define INP_TURB_Pin GPIO_PIN_8
#define INP_TURB_GPIO_Port GPIOB
#define INP_AP_DISC_Pin GPIO_PIN_9
#define INP_AP_DISC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
