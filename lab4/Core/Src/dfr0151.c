#include "dfr0151.h"

extern I2C_HandleTypeDef hi2c1;


uint8_t bcd2bin(uint8_t data){
 return ((data >> 4) * 10) + (data & 0x0F);
}

uint8_t bin2bcd(uint8_t data){
  return ((data / 10) << 4)|(data % 10);
}

uint8_t rtc_read(uint8_t address)
{
  uint8_t data;

  if (HAL_I2C_Mem_Read(&hi2c1,ADDR_DS1307,address,I2C_MEMADD_SIZE_8BIT,&data,1,100) != HAL_OK) {
    Error_Handler();
  }

  return data;
}

void rtc_write(uint8_t address,uint8_t data)
{
  if (HAL_I2C_Mem_Write(&hi2c1,ADDR_DS1307,address,I2C_MEMADD_SIZE_8BIT,&data,1,100) != HAL_OK){
    Error_Handler();
  }
}

void rtc_init()
{
  // Initialize Real-Time Clock peripheral
  uint8_t rs=0, sqwe=1, out=0;
  
  rs&=3;
  if (sqwe) rs|=0x10;
  if (out) rs|=0x80;
	
  rtc_write(0x07,rs);
	
	uint8_t status = rtc_read(0x00);
  status &= 0x7F;
  rtc_write(0x00, status);
}

void rtc_get_time(uint8_t *hour,uint8_t *min,uint8_t *sec)
{
	*sec = rtc_read(0x00); // seconds register: 0
	*min = rtc_read(0x01); // minutes register: 1
	*hour = rtc_read(0x02);   // hours register: 2
}

void rtc_set_time(uint8_t hour,uint8_t min,uint8_t sec)
{
	uint8_t time[3];
	
	time[0] = bcd2bin(hour);
	time[1] = bcd2bin(min);
	time[2] = bcd2bin(sec);
	
	if (HAL_I2C_Mem_Write(&hi2c1,ADDR_DS1307, 0x00,I2C_MEMADD_SIZE_8BIT,time,4,100) != HAL_OK){
    Error_Handler();
  }
}

void rtc_get_date(uint8_t *week_day,uint8_t *day,uint8_t *month,uint8_t *year)
{
	*week_day = rtc_read(0x03); // weekday register: 3
	*day = rtc_read(0x04);	// date register: 4
	*month = rtc_read(0x05);	// month register: 5
	*year = rtc_read(0x06);	// year register: 6
}

void rtc_set_date(uint8_t week_day,uint8_t day,uint8_t month,uint8_t year)
{
	uint8_t date[4];
	
	date[0] = bcd2bin(week_day);
	date[1] = bcd2bin(day);
	date[2] = bcd2bin(month);
	date[3] = bcd2bin(year);
	
	if (HAL_I2C_Mem_Write(&hi2c1,ADDR_DS1307, 0x03,I2C_MEMADD_SIZE_8BIT,date,4,100) != HAL_OK){
    Error_Handler();
  }
}

void eeprom_write(uint16_t address, uint8_t *data, uint16_t size) {
  // Your code here
}

uint8_t eeprom_read(uint16_t address) {
  // Your code here
  return -1;
}