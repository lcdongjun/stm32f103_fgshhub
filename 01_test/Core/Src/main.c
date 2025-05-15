/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "light_task.h"
#include "oled.h"
#include "bmp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define FRAME_TARGET_MS 16  

uint16_t fan_cont = 0, bat_lev_cont = 0 , temp = 0, fan_lev = 0;
uint32_t start_time = 0, elapsed_time = 0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode);
void OLED_ShowEncoderValue(void);
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
uint8_t BMP3_rotated[72];
u32 timer_seconds = 0 , timer_seconds_cont = 0;


void ShowRunTime_Task(void *arg){
  static  uint32_t time_ = 0, cont = 0;
  time_+=elapsed_time;
  cont++;
  if (cont ==100)
  {
    cont = 0;
    time_/=7200;
    OLED_ShowString(86, 48, "T:", 12, 1);
    OLED_ShowNum(98, 48, time_, 4, 12, 1);
    time_ = 0;
  }
}

void ShowTime_Task(void *arg){
  timer_seconds++;
  if(timer_seconds>=5999)
    timer_seconds = 0;
  OLED_ShowTimer(10, 40, timer_seconds, 16, 1);
}

void ShowBATLev_Task(void *arg){
  bat_lev_cont++;
  bat_lev_cont%=7;
  OLED_ShowPicture(96,0,32,16,BAT_LEVELS[bat_lev_cont],1);
}

void ShowTEMP_Task(void *arg){
  temp++;
  temp%=60;
  OLED_ShowPicture(42,3,16,16,TEMP_ICO,1);
  OLED_ShowNum(56,2,temp,2,16,1);
  OLED_ShowPicture(72,4,12,12,TEMP_C,1);
}

void ShowFAN_Task(void *arg){
    uint8_t level = *(uint8_t *)arg;
    static uint8_t frame_skip_counter = 0;
    uint8_t speed_factor = 8 - level*2;
    if (speed_factor == 0) speed_factor = 1;
    frame_skip_counter++;
    if (frame_skip_counter >= speed_factor && level != 0) {
        frame_skip_counter = 0;
        fan_cont++;
        fan_cont %= 6;
    }
    ShowFanStatus(11, 18, level, fan_cont);
}


void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

  
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx)
{
    // 显示风扇图标（24x24）
    OLED_ShowPicture(x, y, 24, 24, FAN_FRAMES[fan_frame_idx], 1);

    // 根据档位显示不同数量的风格条
    switch (level)
    {
    case 0:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_AIR, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_AIR, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_AIR, 1);
      break;
    case 1:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_AIR, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_AIR, 1);
      break;
    case 2:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_AIR, 1);
      break;
    case 3:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_SOLID, 1);
      break;
    default:
      break;
    }
}

void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode)
{
    u32 minutes = total_seconds / 60;
    u32 seconds = total_seconds % 60;
		OLED_ShowPicture(x,y,24,24,TIMER1,1);
    OLED_ShowNum(x+28,y+4, minutes, 2, size, mode); // ????????2λ??
    OLED_ShowString(x + size * 2 * 1-16+28, y-1+4, ":", size, mode); // ???????6 ???????????
    OLED_ShowNum(x + size * 2 * 1-8+28, y+4, seconds, 2, size, mode); // ?????2λ??
}
void OLED_ShowEncoderValue(void)
{
    int count = __HAL_TIM_GET_COUNTER(&htim1);
    OLED_ShowString(84, 52, "CNT:", 12, 1);         // 显示前缀
    OLED_ShowNum(110, 52, (int)count/4, 3, 12, 1);            // 显示编码器数值
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  DWT_Init();
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	OLED_Init();
	OLED_DisPlay_On();
	OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
	printf("system_ok\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      start_time = DWT->CYCCNT; 
      int count = __HAL_TIM_GET_COUNTER(&htim1);
      count/=8;
      count%=4;
      DelayCall(ShowRunTime_Task,NULL,10);
      DelayCall(ShowTime_Task, NULL, 1000);
      DelayCall(ShowBATLev_Task, NULL, 3000);
      DelayCall(ShowTEMP_Task, NULL, 500);
      DelayCall(ShowFAN_Task, (void *)&count, 8);
			OLED_Refresh();
      elapsed_time =  DWT->CYCCNT - start_time;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
