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

#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "fixedpt.h"

#define M_PI 3.14159265358979323846
#define WAVE_PERIOD (2 * M_PI)
#define STEP_RADIANS 0.01  // vary this for checking how it affects the output
#define NUM_SAMPLES 628
#define DAC_MAX_VAL 4095

/**
 * @brief  The application entry point.
 * @retval int
 */

// GLOBAL VARIABLES
uint16_t dac_val, start, end, diff;
uint16_t array[NUM_SAMPLES];
uint16_t square_LUT[NUM_SAMPLES];
float angle = 0.0;
char message[100];

// FUNCTION PROTOTYPES
void part1();
void part2_1();
void part2_2();
void part3_square();
void part3_LUT();

// 
int main(void) {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  MX_USART3_Init();
  
  angle = 0.0;
  // part1();
  
  // HAL_DAC_Init(&hdac);
  // HAL_DAC_Start(&hdac, DAC_CHANNEL_1);  // starts the DAC hardware
  
  // part2_1();
  // part2_2();
  
  // part3_square();
  // part3_LUT();
}

// ****** PART 1 ******
void part1() {
	// prescalar: dividing input clock by a certain constant to make it slower
    uint16_t sum = 0;
	while (1) {
		HAL_TIM_Base_Start(&htim6);
		start = __HAL_TIM_GET_COUNTER(&htim6);
		for (uint16_t i = 0; i < 100; i++) {
			sum += i;
		}
		end = __HAL_TIM_GET_COUNTER(&htim6);
		diff = end - start;
		sprintf(message, "execution time = %d\n", diff);
		print_msg(message);
		HAL_Delay(500);
	}
}

// ****** PART 2 ******
// first we need to make a for loop to account for all the possible values of theta (period) 
// 1 sample every 0.01s means 2pi/0.01 = 628.3
// samples/period 1 full period = 2pi radians add offset of 1V to avoid negative values 
// 12 bits resolution means max digital value is 4095

void part2_1() {
    // *** 2.1: SINE WAVE ***
	while (1) {
		dac_val = (uint16_t) ((sin(angle) + 1.0) * DAC_MAX_VAL/2.0);
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_val);  // converts the discrete values dac_val to analog wave
		angle += STEP_RADIANS;
		if (angle > WAVE_PERIOD) {
			angle = 0;
		}
	}
}
void part2_2() {
  // *** 2.1: LUT ***
	// Pre computing the sin(x) values in LUT
  for (int i = 0; i < NUM_SAMPLES; i++) {
	angle += STEP_RADIANS;
	array[i] = (uint16_t)((sin(angle) + 1.0) * (DAC_MAX_VAL / 2.0));  // Scale to DAC
  }
  
  uint16_t index = 0;
	
  while (1) {
    // HAL_TIM_Base_Start(&htim6);
    // start = __HAL_TIM_GET_COUNTER(&htim6);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, array[index]);  // converts the discrete values dac_val to analog wave
    index++;
    if (index > NUM_SAMPLES) {
      index = 0;
      // end = __HAL_TIM_GET_COUNTER(&htim6);
      // diff = end - start;
      // sprintf(message, "execution time = %d\n", diff);
      // print_msg(message);
    }
  }
}
// QUESTIONS:
// 1. Given the board clock frequency = 100MHz, what is the relationship
// between number of points/samples and the output frequency:
//	the frequency depends on the
// 2. When changing the step rate, how does the output change?
//	increasing the step would make the wave have more quatization error
//(less smooth) 	when the sample rate > 2*frequency then aliasing occurs


// ****** PART 3 ********
void part3_square(){
	uint16_t harmonics = 10;	// modify these to see how the behaviour of the output changes
	uint16_t k = 0;
	float square_val = 0.5;
	
	while (1) {
		for (uint16_t n = 0; n < harmonics; n++) {
			k = (2 * n) + 1;
			square_val += (2 * sin(k * angle))/ (k * M_PI);
		}	// adding all harmonics to find the discrete value at that angle
		
		dac_val = (uint16_t) (square_val + 1.0) * DAC_MAX_VAL/2.0;
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_val);
		
		angle += STEP_RADIANS;
		if (angle > WAVE_PERIOD) {
			angle = 0.0;
		}
		
	}
}
void part3_LUT() {
	float square_val = 0.5;
	int harmonics = 10;
	
	// Pre computing the sin(x) values in LUT
	for (int i = 0; i < NUM_SAMPLES; i++) {
		angle += STEP_RADIANS;
		square_LUT[i] = (uint16_t)((sin(angle) + 1.0) * (DAC_MAX_VAL / 2.0));  // Scale to DAC
    }

    uint16_t index = 0;
	uint16_t new_index = 0;
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    while (1) {
		for (int n = 0; n < harmonics; n++) {
			int k = 2 * n + 1;  // Odd harmonics: 1, 3, 5, 7...
			new_index = (index * k) % NUM_SAMPLES;  	// Loop back to start after full cycle
			square_val += (2 * square_LUT[new_index])/ (k * M_PI);
		}
		dac_val = (uint16_t) (square_val + 1.0) * DAC_MAX_VAL/2.0;
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_val);

		angle += STEP_RADIANS;
		if (angle > WAVE_PERIOD) {
			angle = 0.0;
		}
				
    }
}

// Some notes from last time:
// 	when you convert analog signals to digital using ADC it does not convert
// it into volt units but in quantized bits/output 	max from ADC = 2^(ADC
// resolution) - 1; where ADC resolution is the number of bits of the
// quantized output