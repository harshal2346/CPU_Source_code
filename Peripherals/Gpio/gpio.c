/*
 * gpio.c
 *
 *  Created on: 08-Mar-2024
 *      Author: scope
 */

/**************************** INCLUDES ******************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>

/**************************** EXTERN VARIABLES **********************/


/**************************** MACROS ********************************/


/**************************** TYPEDEFS ******************************/


/**************************** STATIC VARIABLES **********************/


/**************************** GLOBAL VARIABLES **********************/
uint32_t *p_gpio_rs485;
uint32_t *p_slot_en;
uint32_t *p_spare_gpio;
uint32_t *p_rst_gpio;
uint32_t *p_common_gpio;
uint32_t *p_gpio_spi;
uint32_t *p_len_en;

/**************************** FUNCTION PROTOTYPES *******************/


/**************************** FUNCTIONS *****************************/

void gpio_init()
{
	int fd_gpio_rs485;

	    off_t		rs485_en_base_address 	= 0x41210000;
	    off_t		slot_en_base_address	= 0x41200000;
	    off_t 		spare_gpio_base_address = 0x41230000;
	    off_t 		rst_gpio_base_address 	= 0x41240000;
	    off_t		common_gpio_base_address = 0x41250000;
	    off_t 		gpio_spi_test			= 0x41280000;
	    off_t 		gpio_len_en_base_address = 0x41260000;

	    size_t 		ip_size		= 64000;

	/*-------------------------- LVDS Driver Enable GPIO ----------------------*/
	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_gpio_rs485 = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, rs485_en_base_address);
	if(p_gpio_rs485 == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("rs485 gpio init\r\n");

	/*-------------------------- SLOT Enable GPIO ----------------------*/

	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_slot_en = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, slot_en_base_address);
	if(p_slot_en == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("Slot enable GPIO init\r\n");

	/*-------------------------- Spare  GPIO ----------------------*/
	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_spare_gpio = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, spare_gpio_base_address);
	if(p_spare_gpio == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("Spare GPIO init\r\n");

	/*-------------------------- RST_STR  GPIO ----------------------*/
	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_rst_gpio = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, rst_gpio_base_address);
	if(p_rst_gpio == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("RST STR GPIO init\r\n");




	/*-------------------------- COMMON GPIO ----------------------*/
	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_common_gpio = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, common_gpio_base_address);
	if(p_common_gpio == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("COMMON GPIO init\r\n");

	/*--------------------------- SPI GPIO test --------------------*/

	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_gpio_spi = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, gpio_spi_test);
	if(p_gpio_spi == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("SPI GPIO init\r\n");

	/*--------------------------- LED's GPIO --------------------*/

	if((fd_gpio_rs485 = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p_len_en = (uint32_t *)mmap(NULL, ip_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_gpio_rs485, gpio_len_en_base_address);
	if(p_len_en == MAP_FAILED)
	{
		printf("gpio mapping failed\r\n");
		exit(1);
	}

	close(fd_gpio_rs485);
	printf("SPI GPIO init\r\n");
}

void gpio_rs485_tx_en()
{
		*((uint32_t*)p_gpio_rs485) = 0x154;
	//*((uint32_t*)p_gpio_rs485) = 0xAA;
}

/*	Below function is used for Enabling or disabling the particular slot_en
 * 	to enable the particular Daughter board for the further operations
 * 	Enable bits
 * 	[0] = slot_1_2_en
 * 	[1] = slot_3_en
 * 	[2] = slot_4_5_en
 * 	[3] = slot_6_en
 * 	[4] = slot_7_8_en
 * 	[5] = slot_9_en
 * 	[6] = slot_11_12_en
 */
void gpio_slot_en(uint16_t u8_slot,uint8_t u8_enable)
{
	/*	TODO: 	implement the function which will take the enable or disable command
	 * 			and carry out the desired operation on that particular pin (Enable/Disable)
	 * 			without changing the present states of other pins
	 */
	static uint16_t u8_slot_en_pin = 0;

	if(u8_enable)
	{
		u8_slot_en_pin |= 1 << u8_slot;
	}
	else
	{
		u8_slot_en_pin &= 0 << u8_slot;
	}


	//*((uint32_t*)p_slot_en) = u8_slot_en_pin;
	*((uint32_t*)p_slot_en) = 0xFF;
}

/*	Below function is used for Setting or Resetting the Spare Gpio pins placed on
 * 	the 96 pin Euro connector
 * 	[0] = spare_gpio_1
 * 	[1] = spare_gpio_2
 * 	[2] = spare_gpio_3
 * 	[3] = spare_gpio_4
 * 	[4] = spare_gpio_5
 * 	[5] = spare_gpio_6
 * 	[6] = spare_gpio_7
 * 	[7] = spare_gpio_8
 * 	[8] = spare_gpio_9
 * 	[9] = spare_gpio_10
 * 	[10] = spare_gpio_11
 * 	[11] = spare_gpio_12
 * 	[12] = spare_gpio_13
 * 	[13] = spare_gpio_14
 */
void gpio_spare_en(uint16_t u16_pin, uint8_t u8_enable)
{
	static uint16_t u16_gpio_pins = 0;

	if(u8_enable)
	{
		u16_gpio_pins |= 1 << u16_pin;
	}
	else
	{
		u16_gpio_pins &= 0 << u16_pin;
	}

	//*((uint32_t*)p_spare_gpio) = u16_gpio_pins;
	*((uint32_t*)p_spare_gpio) = 0xFFFF;
}

/*	Below function is used to START OR RESET the communication between
 * 	CPU board and all Daughter Boards
 * 	[0]	= RST : Communication Reset
 * 	[1] = STR : Communication start
 *
 */
void gpio_rst_str(uint8_t u8_pin, uint8_t u8_enable)
{
	static uint16_t u16_gpio_pins = 0;

	if(u8_enable)
	{
		u16_gpio_pins |= 1 << u8_pin;
	}
	else
	{
		u16_gpio_pins &= 0 << u8_pin;
	}

	//*((uint32_t*)p_spare_gpio) = u16_gpio_pins;
	*((uint32_t*)p_rst_gpio) = 0xFFFF;
	printf("rst_str gpio's enabled\r\n");

}


void gpio_common(uint8_t u8_pin, uint8_t u8_enable)
{
	static uint16_t u16_gpio_pins = 0;

	if(u8_enable)
	{
		u16_gpio_pins |= 1 << u8_pin;
	}
	else
	{
		u16_gpio_pins &= 0 << u8_pin;
	}

	//*((uint32_t*)p_spare_gpio) = u16_gpio_pins;
	*((uint32_t*)p_common_gpio) = 0xFFFF;
//	*((uint32_t*)p_common_gpio) = 0x0000;
//	printf("Common gpio's enabled\r\n");
}

void gpio_spi_test(uint8_t u8_high)
{

	if(u8_high)
	{
		*((uint32_t*)p_gpio_spi) = 0xFFFF;
	}
	else
	{
		*((uint32_t*)p_gpio_spi) = 0x0000;
	}
}

void gpio_Led_en(uint8_t u8_high)
{

	if(u8_high)
	{
		*((uint32_t*)p_len_en) = 0xFFFF;
	}
	else
	{
		*((uint32_t*)p_len_en) = 0x0000;
	}
}
