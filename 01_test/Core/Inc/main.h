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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <stdbool.h>

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
#define EC_OK_Pin GPIO_PIN_0
#define EC_OK_GPIO_Port GPIOA
#define EC_OK_EXTI_IRQn EXTI0_IRQn
#define BAT_LEV_Pin GPIO_PIN_1
#define BAT_LEV_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_2
#define OLED_CS_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_3
#define OLED_DC_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_4
#define OLED_RST_GPIO_Port GPIOA
#define OLED_SCK_Pin GPIO_PIN_5
#define OLED_SCK_GPIO_Port GPIOA
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_10
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_11
#define KEY2_GPIO_Port GPIOB
#define POWER_Pin GPIO_PIN_12
#define POWER_GPIO_Port GPIOB
#define L1_Pin GPIO_PIN_13
#define L1_GPIO_Port GPIOB
#define L2_Pin GPIO_PIN_14
#define L2_GPIO_Port GPIOB
#define L3_Pin GPIO_PIN_15
#define L3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define SYSClock_MHZ 72

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IOï¿½Úµï¿½Ö·Ó³ï¿½ï¿½
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½ 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½ 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½ 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½ 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //ï¿½ï¿½ï¿? 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //ï¿½ï¿½ï¿½ï¿½


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
