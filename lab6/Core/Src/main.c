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

//PURPOSE:
/*
Data Compression:
- run-length encoding (lossless)
- data truncation (lossy):
	- reducing data per frame by only sending the 4 MSB

*/
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "ov7670.h"

/* USER CODE BEGIN PV */
#define PREAMBLE "\r\n!START!\r\n"
#define DELTA_PREAMBLE "\r\n!DELTA!\r\n"
#define SUFFIX "!END!\r\n"

uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];
uint8_t old_snapshot_buff[IMG_ROWS * IMG_COLS];

uint8_t tx_buff[sizeof(PREAMBLE) + 2 * IMG_ROWS * IMG_COLS + sizeof(SUFFIX)];
size_t tx_buff_len = 0;

// This is set in stm32f4xx_it.c in the DCMI_IRQHandler function.
uint8_t dma_flag = 0;

// Add function definitions for any other functions you add here.
void print_buf(void);
uint8_t truncate_pixel_data(uint8_t pixel0, uint8_t pixel1);

int main(void)
{
  /* Reset of all peripherals */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  
  char msg[100];
  
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  
  ov7670_init();
  HAL_Delay(100);
  //ov7670_capture(snapshot_buff);
  
  // Add variables you need here.
	
	
  
  while (1)
  {
		/* 
		// part 1: testing serial monitor
    print_buf();
		HAL_Delay(1000);
		*/
		
		///*
		// Part 2: video with dma
		ov7670_capture(snapshot_buff);
		if (dma_flag == 1) {  // New frame is ready
			HAL_DCMI_Suspend(&hdcmi);  // Prevent tearing
			print_buf();
			dma_flag = 0;
			HAL_DCMI_Resume(&hdcmi);  // Resume capture
		}
			//*/
			
			
  }
}


uint8_t truncate_pixel_data(uint8_t pixel0, uint8_t pixel1) {
	// need to clear extract the 4 msb in each
	pixel0 = (pixel0 >> 4) & 0xF0;
	pixel1 = (pixel1 >> 4) & 0xF0;
	
	return ((pixel0 << 4) | pixel1);
}


void print_buf() {
	if (HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY) {
		tx_buff_len = 0;
		// Create a new buffer from the snapshot_buffer than the DCMI copied the 16-bit pixel values into.
		uint8_t *buffer = (uint8_t *) snapshot_buff;
		
		// Add the START preamble message to the start of the buffer for the serial-monitor program. 
		for (int i = 0; i < sizeof(PREAMBLE); i++) {
			tx_buff[tx_buff_len++] = PREAMBLE[i];
		}

		/*
		// ******************************************* Part 3.1: Data truncation (2 pixels in one byte) *******************************************
		// Write code to copy every other byte from the main frame buffer to 
		// our temporary buffer (this converts the image to grey scale)
		for (int i = 0; i < 25056 *2; i+=4) {
			// tx_buff[tx_buff_len++] = truncate_pixel_data(buffer[i*2 + 1], buffer[(i+1)*2 + 1]);
			uint8_t pixel0 = (buffer[i + 1] >> 4) & 0x0F;  // Extract 4 MSB from first pixel
			uint8_t pixel1 = (buffer[i + 3] >> 4) & 0x0F;  // Extract 4 MSB from second pixel
			
			tx_buff[tx_buff_len++] = (pixel0 << 4) | pixel1;
		}
		*/
		
		// ******************************************* Part 3.2: Combined: truncation + run-length *******************************************
		// Write code to copy every other byte from the main frame buffer to 
		// our temporary buffer (this converts the image to grey scale)
		uint8_t prev = (buffer[1] >> 4) & 0x0F;  // need to set the first pixel as prev
		uint8_t count = 1;
		
		for (int i = 3; i < 25056 *2; i+=2) {
			// Getting the 4 MSB
			uint8_t pixel = (buffer[i] >> 4) & 0x0F;
			
			// truncate the pixel byte as (value (4 bits), count(4 bits))
			if (pixel == prev && count < 15) {
					count++;
			} else {
					tx_buff[tx_buff_len++] = (prev << 4) | count;
					prev = pixel;
					count = 1;
			}
		}
		tx_buff[tx_buff_len++] = (prev << 4) | count;
		
		for (int i = 0; i < sizeof(SUFFIX); i++) {
			tx_buff[tx_buff_len++] = SUFFIX[i];
		}
			
		// Once the data is copied into the buffer, call the function to send it via UART.
		uart_send_bin(tx_buff, tx_buff_len);
	}
}
