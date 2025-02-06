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

void restore_time(){
	// read the time from eeprom
	uint8_t data[7];

	data[0] = eeprom_read(0x0000);
	data[1] = eeprom_read(0x0001);
	data[2] = eeprom_read(0x0002);
	data[3] = eeprom_read(0x0003);
	data[4] = eeprom_read(0x0004);
	data[5] = eeprom_read(0x0005);
	data[6] = eeprom_read(0x0006);
	
	rtc_set_time(data[2], data[1], data[0]);
	rtc_set_date(data[3], data[4], data[5], data[6]);
}

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
	// uint8_t blink = 0;
	
	sprintf(msg, "Starting program! or reset!\n");
  print_msg(msg);
	
	// part 4: demo - Set the current rtc time to that saved in eeprom address 0 - 6	
	restore_time();
	uint8_t was_pressed = 0;
	
  while (1)
  {
		uint8_t seconds, minutes, hours;
		uint8_t weekday, date, month, year;
		
		uint8_t alarm[7];
		//if (was_pressed) {
			alarm[0] = eeprom_read(0x0007);
			alarm[1] = eeprom_read(0x0008);
			alarm[2] = eeprom_read(0x0009);
			alarm[3] = eeprom_read(0x000A);
			alarm[4] = eeprom_read(0x000B);
			alarm[5] = eeprom_read(0x000C);
			alarm[6] = eeprom_read(0x000D);
		//}
		
		if (button_press) {
			sprintf(msg, "is button pressed?\n");
			print_msg(msg);
			// part 2.a: demo - Set current date and time to July 1st 2025 01:00:00 (1 PM)
			seconds = 0;	// 
			minutes = 0;	// 0-59 but is 0-79
			hours = 1;	// 1-12 but is 0-79
			rtc_set_time(hours, minutes, seconds);
			weekday = 5;	// 1-7 also is 1-7
			date = 1;	// 1-31 but is 1-39
			month = 7;	// 1-12 but is 1-19
			year = 25;
			rtc_set_date(weekday, date, month, year);
			
			// part 2.b: demo - Set an alarm for July 1st 2025 01:01:00 (1: 01 PM)
			uint16_t size = 7;	// Read current RTC time and date
			rtc_get_time(&alarm[2], &alarm[1], &alarm[0]);
			rtc_get_date(&alarm[3], &alarm[4], &alarm[5], &alarm[6]);
			
			eeprom_write(0x0000, alarm, size); // Store current RTC time and date
			
			alarm[1] += 0b1;	// Increment the minute 
			eeprom_write(0x0007, alarm, size); // Store current alarm time and date
			button_press = 0;	// disable interrupt
			was_pressed = 1;
			// sprintf(msg, "did i set the alarm for 1 minute? \n");
			// print_msg(msg);
		}
		
		// part3: demo
		rtc_get_time(&hours, &minutes, &seconds);
		rtc_get_date(&weekday, &date, &month, &year);
		if (year == alarm[6] && month == alarm[5] && date == alarm[4] && weekday == alarm[3] && hours == alarm[2] && minutes == alarm[1] && seconds == alarm[0]) {
			sprintf(msg, "ALARM\n");
			// blink = 1;
			print_msg(msg);
			while (seconds != alarm[0] + 10) {
				HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
				HAL_GPIO_TogglePin(LD1_GPIO_Port, LD2_Pin);
				HAL_GPIO_TogglePin(LD1_GPIO_Port, LD3_Pin);
				HAL_Delay(1000);
				rtc_get_time(&hours, &minutes, &seconds);
			}
			// blink = 0;
		}
		
		// part1: demo
		// Reading the rtc date and time
		// rtc_get_time(&hours, &minutes, &seconds);
		sprintf(msg, "Time: %02u:%02u:%02u\n", hours, minutes, seconds);
    print_msg(msg);
		
		sprintf(msg, "Date: %u. %u. %u. %u\n", weekday, date, month, year);
    print_msg(msg);
		
		// reading the alarm date and time
		uint8_t data[7];
		data[0] = eeprom_read(0x0007);
		data[1] = eeprom_read(0x0008);
		data[2] = eeprom_read(0x0009);
		data[3] = eeprom_read(0x000A);
		data[4] = eeprom_read(0x000B);
		data[5] = eeprom_read(0x000C);
		data[6] = eeprom_read(0x000D);
		
		sprintf(msg, "Alarm Day: %u, Alarm Date: %u. %u. %u\n", data[3], data[4], data[5], data[6]);
    print_msg(msg);
		
		sprintf(msg, "Alarm Time: %02u:%02u:%02u\n", data[2], data[1], data[0]);
    print_msg(msg);
		
		HAL_Delay(1000);	// 1 sec
		
  }
}