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

#include "dfr0151.h"
#include "config.h"

#include <stdio.h>
#include <string.h>

uint8_t button_press = 0;

int main(void)
{
  /* Reset of all peripherals. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();  
  rtc_init();

  char msg[100];
	
  // Your start up code here


  while (1)
  {
    // Your code here
    // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		//HAL_Delay(500);
    //print_msg("hiii\b\b\b\btock");

    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    //HAL_Delay(500);
    //print_msg("hiii\b\b\b\btock");
		
		// Setting the date and time
		if (button_press) {
			rtc_set_time(01, 00, 00);
			rtc_set_date(01, 01, 07, 25);
			button_press = 0;
			sprintf(msg, "set!\n");
			print_msg(msg);
		}
		
		// Reading the date and time
		uint8_t seconds, minutes, hours;
		rtc_get_time(&hours, &minutes, &seconds);
		sprintf(msg, "Time: %02d:%02d:%02d\n", bcd2bin(hours), bcd2bin(minutes), bcd2bin(seconds));
    print_msg(msg);
		
		uint8_t weekday, date, month, year;
		rtc_get_date(&weekday, &date, &month, &year);
		sprintf(msg, "Date: %d %d %d %d\n", bcd2bin(weekday), bcd2bin(date), bcd2bin(month), bcd2bin(year));
    print_msg(msg);
		
		HAL_Delay(1000);	// 1 sec
		
  }
}