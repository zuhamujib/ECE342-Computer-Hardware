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
#include "main.h"

#include "config.h"
#include "ov7670.h"

#include <stdio.h>
#include <string.h>

uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];
uint8_t buffer[IMG_ROWS*IMG_COLS];
uint8_t dma_flag = 0;


void print_buf(void);
void print_image(void);


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
  // Your start up code here

  while (1)
  {
    // Your code here
		// print_msg("iteration!\n");
		
		/*
		// part 2: setup
		print_buf();
		HAL_Delay(1000);
		*/
		
		
		// part 3: snapshot
		while(dma_flag != 1){};
		if (dma_flag == 1) {	// full frame interrupt triggered
			// print_msg("frame capture success!\n");
			
			// send it to the serial port
			print_image();

			// set the dma_flag back to zero
			dma_flag = 0;
		}
			
		if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
			HAL_Delay(100);  // debounce
			print_msg("Snap!\r\n");
			ov7670_snapshot(snapshot_buff);
		}
		
		/*
		// part 4: video stream
		ov7670_capture(snapshot_buff);
		 if (dma_flag == 1) {  // New frame is ready
				HAL_DCMI_Stop(&hdcmi);  // Prevent tearing
				print_image();
				dma_flag = 0;
				HAL_DCMI_Start(&hdcmi);  // Resume capture
			}
		*/
  }
}

void print_image() {
	
	// int pixel_value = 0xFF;	// 0xFF: white and 0x00: black
	uint8_t* ptr = (uint8_t *)snapshot_buff; // Pointer to the raw image buffer
	print_msg("\r\nPREAMBLE!\r\n");
	// HAL_Delay(1000);
	for (int i=0; i <25056; i++){
		uart_send_bin(ptr +(i*2 + 1), 1);
		//buffer[i] = ptr[i*2 + 1];	// extracting the odd bits only for the gray scale
	}
		/*
	print_msg("\r\nPREAMBLE!\r\n");

	uint16_t* ptr = (uint16_t*)snapshot_buff;  // Correctly interpret as 16-bit pixels

	for (int i = 0; i < (IMG_ROWS * IMG_COLS); i++) {
			uint8_t grayscale_value = (ptr[i] >> 8) & 0xFF;  // Extract the MSB (Y component)
			uart_send_bin(&grayscale_value, 1);  // Send each byte one at a time
	}
		*/
}

void print_buf() {
	int pixel_value = 0x00;	// 0xFF: white and 0x00: black
	for (int i=0; i <(IMG_ROWS*IMG_COLS); i++){
		buffer[i] = pixel_value;
	}
	print_msg("\r\nPREAMBLE!\r\n");
	HAL_Delay(1000);
	uart_send_bin(buffer, 25056);
}
