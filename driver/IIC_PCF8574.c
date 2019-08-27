/*
 * IIC_PCF8574.c
 *
 *  Created on: 8 лют. 2017 р.
 *      Author: Kitaro
 */
 #include "driver/IIC_PCF8574.h"


#define HI(X) (X>>8)
#define LO(X) (X & 0xFF)


uint8 i2c_PCF8574_buffer[i2c_PCF8574_buffer_lenght];

uint8 i2c_PCF8574_Address;
//uint8 i2c_PCF8574_Addresses[8];
//uint8 i2c_PCF8574_count;


//void DoNothing(void)	// Функция пустышка, затыкать несуществующие ссылки
//{
//}

/*
uint8 i2c_PCF8574_WriteByte(uint8 SAddr, uint8 Byte[], uint8 ByteNumbers, IIC_F WhatDo)
{

uint8 i;

if (i2c_Do & i2c_Busy) return 0;

i2c_index = 0;
i2c_ByteCount = ByteNumbers;

//i2c_SlaveAddress = SAddr;

		for (i=0;i<ByteNumbers;i++)
		{
			i2c_Buffer[i]=Byte[i];
		}


i2c_Do = i2c_sawp;

MasterOutFunc = WhatDo;
ErrorOutFunc = WhatDo;

//TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;

i2c_Do |= i2c_Busy;


return 1;
}
*/

/*
uint8 i2c_PCF8574_ReadByte(uint8 SAddr, IIC_F WhatDo)
{
if (i2c_Do & i2c_Busy) return 0;

i2c_index = 0;
i2c_ByteCount = 5;

//i2c_SlaveAddress = SAddr;

//i2c_PageAddress[0] = 0x02;

//i2c_PageAddrIndex = 0;
//i2c_PageAddrCount = 1;

i2c_Do = i2c_sawsarp;

MasterOutFunc = WhatDo;
ErrorOutFunc = WhatDo;

//TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;

i2c_Do |= i2c_Busy;

return 1;
}
*/


void  ICACHE_FLASH_ATTR i2c_PCF8574_Write(uint8 SAddr, uint8 Byte[], uint8 ByteNumbers)
{
	uint8 i=0;
	i2c_PCF8574_Address = SAddr;

		for (i=0;i<ByteNumbers;i++)
		{
			i2c_PCF8574_buffer[i]=Byte[i];
		}
	i2c_PCF8574_buffer_data = ByteNumbers;

	i2c_PCF8574_Write1();
}


void  ICACHE_FLASH_ATTR  i2c_PCF8574_Write1Byte(uint8 SAddr, uint8 Byte)
{
	i2c_PCF8574_Address = SAddr;

	i2c_PCF8574_buffer[0]=Byte;
	i2c_PCF8574_buffer_data = 1;
	i2c_PCF8574_Write1();
}



void  ICACHE_FLASH_ATTR i2c_PCF8574_Write1(void)
{
	if (!i2c_Write_Buffer(i2c_PCF8574_Address, i2c_PCF8574_buffer, i2c_PCF8574_buffer_data, i2c_PCF8574_Writed) )      // Читаем
	{
		os_timer_disarm (&i2c_PCF8574_timer_set);
		os_timer_setfn (&i2c_PCF8574_timer_set, (os_timer_func_t *)i2c_PCF8574_Write1, NULL);
		os_timer_arm(&i2c_PCF8574_timer_set, 5, 0);	// Повторить попытку через 2мс
	}

}

void  ICACHE_FLASH_ATTR i2c_PCF8574_Writed(void)               		// Была попытка чтения
{
	i2c_Do &= i2c_Free;                  		// Освобождаем шину

	if(i2c_Do & (i2c_ERR_NA|i2c_ERR_BF))   		// Ошибки при четнии были?
	{
		os_timer_disarm (&i2c_PCF8574_timer_set);
		os_timer_setfn (&i2c_PCF8574_timer_set, (os_timer_func_t *)i2c_PCF8574_Write1, NULL);
		os_timer_arm(&i2c_PCF8574_timer_set, 5, 0);	// Повторить попытку через 2мс
		//		SetTimerTask(i2c_PCF8574_Write1,i2c_PCF8574_Write_Error_Restart);     		 // Тогда повтор
	}
}



















void ICACHE_FLASH_ATTR i2c_PCF8574_Read(void)
{
/*
	if (!i2c_PCF8574_ReadByte(i2c_PCF8574_Address, i2c_PCF8574_Readed) )      // Читаем
	{
		SetTimerTask(i2c_PCF8574_Read,i2c_PCF8574_Read_Restart);                  		// Если процесс не пошел (шина занята), то повтор через 50мс.
	}*/
}

void ICACHE_FLASH_ATTR i2c_PCF8574_Readed(void)               		// Была попытка чтения
{/*
	i2c_Do &= i2c_Free;                  		// Освобождаем шину

	if(i2c_Do & (i2c_ERR_NA|i2c_ERR_BF))   		// Ошибки при четнии были?
	{
		SetTimerTask(i2c_PCF8574_Read,i2c_PCF8574_Read_Error_Restart);     		 // Тогда повтор
	}*/
}





