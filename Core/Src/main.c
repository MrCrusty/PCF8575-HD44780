/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hd44780_12c_8bit.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char custom_patten_tick[8]={0, 1, 3, 22, 28, 8, 0, 0}; //provides a tick icon for HD44780 display
char custom_patten_x[8]  =  {31,17, 10, 4, 10, 17, 31, 0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
hd44780_init_8bit(0x21, BACK_LIGHT_ON);
hd44780_custom_char(0x21, 0x01, custom_patten_tick, 8);
hd44780_custom_char(0x21, 0x00, custom_patten_x, 8);
static LineStruct line_char;
uint32_t ticks_taken_start;
uint32_t ticks_taken_finish;
uint32_t ticks_taken;
char Ans[20];
char temp[1] ;
temp[0]= 'H';
uint8_t char_count;
//hd44780_clr_disp(0x21);
hd44780_write_message_at(0x21, "Copy this  line", 0, 0, true);
ticks_taken_start = HAL_GetTick();
line_char = hd44780_read_line_from(0x21, 0, 0, READ_TO_EOL);
ticks_taken_finish = HAL_GetTick();
ticks_taken = ticks_taken_finish - ticks_taken_start;
hd44780_format8_dec(ticks_taken, Ans);
hd44780_write_message_at(0x21, "Read Ms takes ", 1, 0, true);
hd44780_write_message(0x21, Ans);
char_count = line_char.array[20];
ticks_taken_start = HAL_GetTick();
hd44780_write_line_from(0x21,2,0,line_char,  line_char.array[20]);
ticks_taken_finish = HAL_GetTick();
ticks_taken = ticks_taken_finish - ticks_taken_start;
hd44780_format8_dec(ticks_taken, Ans);
hd44780_write_message_at(0x21, "Write Ms takes ", 3, 0, true);
hd44780_write_message(0x21, Ans);
//hd44780_write_bulk_bytes(0x21, line_char.array, 19);
//hd44780_write_character(0x21, 0);
//hd44780_write_character_at(0x21,0x01,0,16,0);
//hd44780_write_character_at(0x21, '1', 0, 1, true);
//hd44780_write_message_at(0x21, "this is a message", 3, 1, true);

//hd44780_write_message_at(0x21, "line 2 text line", 2, 0, true);
//hd44780_write_character(0x21, 0);
//hd44780_read_data(0x20, 3, 0, false);
//temp[0]= hd44780_read_single_byte(0x21, 3, 2);
//hd44780_set_line(0x21, 0, 5, false);
//hd44780_write_bulk_data(0x21, temp, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//hd44780_write_message_at(0x21, "LINE 1 TEXT", 1, 2, true);
//HAL_Delay(5000);
//hd44780_set_line(0x21, 1, 2, true);
//HAL_Delay(5000);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
#ifdef USE_FULL_ASSERT
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
