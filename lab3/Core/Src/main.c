/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include <stdio.h>
#include <math.h>
#include "config.h"
#include "fixedpt.h"


#define WAVE_PERIOD 2*M_PI
#define STEP_RADIANS 0.01	// vary this for checking how it affects the output
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  MX_USART3_Init();

	uint16_t dac_val, start, end, diff;
	char message[100];
	// from last time: 
	// 	when you convert analog signals to digital using ADC it does not convert it into volt units but in quantized bits/output
	// 	max from ADC = 2^(ADC resolution) - 1; where ADC resolution is the number of bits of the quantized output

  while (1)
  {
	// ****** PART 1 ******
	// prescalar: dividing input clock by a certain constant to make it slower
	uint16_t sum = 0;
	
	HAL_TIM_Base_Start(&htim6);
	start = __HAL_TIM_GET_COUNTER(&htim6);
	for (int i = 0; i < 100; i++) {
		sum += 1;
	}
	end = __HAL_TIM_GET_COUNTER(&htim6);
	diff = end  - start;
	sprintf(message, "execution time = %d\n", diff);
	print_msg(message);
	
	// ****** PART 2 ******
	// first we need to make a for loop to account for all the possible values of theta (period)
	// 1 sample every 0.01s means 2pi/0.01 = 628.3 samples/period
	// 1 full period = 2pi radians
	// add offset of 1V to avoid negative values
	// 12 bits resolution means max digital value is 4095
	
	// *** 2.1: SINE WAVE ***
	HAL_DAC_Init(&hdac);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1); // starts the DAC hardware
	// uint16_t masked_dac_val = dac_val & mask;  // Masking lower bits to test the quatization error
	for (int i = 0; i < WAVE_PERIOD; i + STEP_RADIANS) {
		dac_val = (sin(i) + 1)/2;
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_val); // converts the discrete values dac_val to analog wave
	}
	
	// QUESTIONS:
	// 1. Given the board clock frequency = 100MHz, what is the relationship between number of points/samples and the output frequency:
	//	the frequency depends on the 
	// 2. When changing the step rate, how does the output change?
	//	increasing the step would make the wave have more quatization error (less smooth)
	//	when the sample rate > 2*frequency then aliasing occurs

	// *** 2.1: LUT ***
	
	// ****** PART 3 ******
	
	// HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    // HAL_Delay(500);
  }
}
