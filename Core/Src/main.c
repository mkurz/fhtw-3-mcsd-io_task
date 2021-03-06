/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
int leds_on = 1; // Global state if the LED are switched on or off
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void blinky(int, int);
int buttoncheck(int, int);
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
  /* USER CODE BEGIN 2 */

  // First, let's turn off all 3 RGB LEDs
  HAL_GPIO_WritePin(GPIOA, RGB_LED_BLUE_Pin|RGB_LED_RED_Pin|RGB_LED_GREEN_Pin, GPIO_PIN_SET);

  enum LED {
    RED,
    GREEN,
    BLUE
  };

  // Start values
  int current_color = RED;
  int brightness_level = 0; // Three brightness levels can be used: 0 (lowest), 1 (middle), 2 (highest)

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    blinky(current_color, brightness_level); // Let the current LED shine in the current brightness

    if(buttoncheck(current_color, brightness_level)) { // Was button pressed?
      if(brightness_level == 2) { // Did we reach the "last" brightness?
        brightness_level = 0; // If yes, let's start with the first brightness again
        // Also switch to next color
        if(current_color == BLUE) { // Did we reach the last color?
          current_color = RED; // Let's start with red again
        } else {
          current_color++;
        }
      } else {
        brightness_level++;
      }
    }
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RGB_LED_BLUE_Pin|RGB_LED_RED_Pin|RGB_LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RGB_LED_BLUE_Pin RGB_LED_RED_Pin RGB_LED_GREEN_Pin */
  GPIO_InitStruct.Pin = RGB_LED_BLUE_Pin|RGB_LED_RED_Pin|RGB_LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
  * @brief  Makes the given LED shine with the given brightness level (0, 1 or 2), implemented via software Pulse Width Modulation (PWM).
  *         This method takes (technically: at least) 22 ms to run.
  * @param  color: The color of the RGB LED to use (0=red, 1=green, 2=blue)
  * @param  brightness_level: Possible values: 0, 1, 2
  * @retval None
  */
void blinky(int color, int brightness_level) {
  if(!leds_on) {
    return; // If LEDs were turned off globally, this method does not need to run.
  }

  // Correct (possible) wrong parameters
  if(color < 0) {
    color = 0;
  } else if(color > 2) {
    color = 2;
  }
  if(brightness_level < 0) {
    brightness_level = 0;
  } else if(brightness_level > 2) {
    brightness_level = 2;
  }

  int led[3]      = {RGB_LED_RED_Pin, RGB_LED_GREEN_Pin, RGB_LED_BLUE_Pin};
  int on_time[3]  = {2 , 12, 20 };
  int off_time[3] = {20, 10, 2 };

  HAL_GPIO_TogglePin(GPIOA, led[color]); // Turn desired LED on
  HAL_Delay(on_time[brightness_level]);  // Keep on for x milliseconds
  HAL_GPIO_TogglePin(GPIOA, led[color]); // Turn LED off
  HAL_Delay(off_time[brightness_level]); // Keep off for x milliseconds
  // In the end delays were 22 ms in total
}

/**
  * @brief  Checks and returns if the button was pressed or, when button was pressed for one second, turns off the LEDs. Also handles debouncing.
  * @param  color: The color of the RGB LED to use (0=red, 1=green, 2=blue)
  * @param  brightness_level: Possible values: 0, 1, 2
  * @retval status:  - 0  button was not pressed.
  *                  - 1  button was pressed.
  */
int buttoncheck(int color, int brightness_level) {
  int leds_on_before_button_press = leds_on; // Were the LEDs on before the button was pressed?
  int button_pressed = 0; // The return value: If the button was "pressed". However, for us, the button was not "pressed" when held longer than 1 second.
  int while_logic_done = 0;

  int counter = 0;
  while(!HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin)) { // While the button is pressed
    if(!while_logic_done) {
      if(counter > 45) { // Button was pressed and held for ~1 second (see calculation in comment, when calling blinky, below)
        if (leds_on_before_button_press) { // Were LEDs on before even touching the button?
          leds_on = 0; // Turn LEDs off (globally)
          button_pressed = 0; // Do not change brightness anymore (with this line the main method will later not handle this button press)
        }
        while_logic_done = 1; // Nothing to do in this while anymore, let's safe CPU cycles, just keep blinking as long as the button is pressed ;)
      } else if(counter > 3 && !button_pressed) { // Debouncing: blinky was called 3 times (22 ms x 3) so after that delay we are sure the Button was pressed
        if(leds_on_before_button_press) {
          button_pressed = 1;
          // Here, the while logic is not done here yet, because user could keep pressing the button
        } else {
          leds_on = 1; // In case the LEDs were off, we switch them on again
          while_logic_done = 1; // Again, nothing to do in this while anymore, just keep blinking as long as the button is still pressed
        }
      }
      counter++;
    }
    // blinky needs 22 milliseconds to finish one run -> calling blinky 46 times x 22 ms each = at least 1012 ms (~1 second) have passed
    blinky(color, brightness_level); // While pressing the button, we keep the LEDs on
  }
  return button_pressed;
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
