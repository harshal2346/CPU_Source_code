/*
 * custom_uart.c
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
#include <time.h>
#include <signal.h>
#include <sys/mman.h>
#include "timer.h"

/**************************** EXTERN VARIABLES **********************/


/**************************** MACROS ********************************/
#define 	CUSTOM_UART_1_BASE	(0x43C00000)
#define 	CUSTOM_UART_2_BASE	(0x43C10000)
#define		CUSTOM_UART_3_BASE	(0x43C20000)
#define		CUSTOM_UART_4_BASE	(0x43C30000)
#define 	CUSTOM_UART_RS485	(0x43C40000)

#define		FIFO_WRITE_OFFSET	8
#define		FIFO_READ_OFFSET	9

/**************************** TYPEDEFS ******************************/


/**************************** STATIC VARIABLES **********************/


/**************************** GLOBAL VARIABLES **********************/
uint32_t 	*p32_axi_uart_1,
			*p32_axi_uart_2,
			*p32_axi_uart_3,
			*p32_axi_uart_4,
			*p32_RS485_MB_UNIT;

/**************************** FUNCTION PROTOTYPES *******************/


/**************************** FUNCTIONS *****************************/

void custom_uart_init()
{
	/* 	here all the custom AXI UART IP's will be mapped to virtual
	 * 	base address to it's corresponding physical base address
	 * 	allocated by the Vivado software. for more information regarding
	 * 	base addresses refer "Address Editor" in Vivado software
	 */

	int custom_uart_fd;

	size_t 				uart_size		= 64000;

	/*	Mapping the physical address of AXI Uart IP 1 into virtual address space
	 * 	of Linux kernel using method of "mmap"
	 * 	Mapping the physical address to virtual address using mmap for user space.
	 */

	/*---------------------- Custom Uart 1 ----------------------------*/
	if((custom_uart_fd = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p32_axi_uart_1 = (uint32_t *)mmap(NULL, uart_size, PROT_READ|PROT_WRITE, MAP_SHARED, custom_uart_fd, CUSTOM_UART_1_BASE);
	if(p32_axi_uart_1 == MAP_FAILED)
	{
		printf("Custom Uart 1 memory mapping failed\r\n");
		exit(1);
	}

	close(custom_uart_fd);


	/*	Mapping the physical address of AXI Uart IP 2 into virtual address space
	 * 	of Linux kernel using method of "mmap"
	 * 	Mapping the physical address to virtual address using mmap for user space.
	 */
	/*---------------------- Custom Uart 2 ----------------------------*/
	if((custom_uart_fd = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p32_axi_uart_2 = (uint32_t *)mmap(NULL, uart_size, PROT_READ|PROT_WRITE, MAP_SHARED, custom_uart_fd, CUSTOM_UART_2_BASE);
	if(p32_axi_uart_2 == MAP_FAILED)
	{
		printf("Custom Uart 2 memory mapping failed\r\n");
		exit(1);
	}

	close(custom_uart_fd);

	/*---------------------- Custom Uart 3 ----------------------------*/

	if((custom_uart_fd = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p32_axi_uart_3 = (uint32_t *)mmap(NULL, uart_size, PROT_READ|PROT_WRITE, MAP_SHARED, custom_uart_fd, CUSTOM_UART_3_BASE);
	if(p32_axi_uart_3 == MAP_FAILED)
	{
		printf("Custom Uart 3 memory mapping failed\r\n");
		exit(1);
	}

	close(custom_uart_fd);

	/*---------------------- Custom Uart 4 ----------------------------*/

	if((custom_uart_fd = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p32_axi_uart_4 = (uint32_t *)mmap(NULL, uart_size, PROT_READ|PROT_WRITE, MAP_SHARED, custom_uart_fd, CUSTOM_UART_4_BASE);
	if(p32_axi_uart_4 == MAP_FAILED)
	{
		printf("Custom Uart 4 memory mapping failed\r\n");
		exit(1);
	}

	close(custom_uart_fd);

	/*---------------------- MB Unit RS485 ----------------------------*/

	if((custom_uart_fd = open("/dev/mem",O_RDWR | O_SYNC)) == -1)
	{
		printf("Access memory error");
		exit(1);
	}

	p32_RS485_MB_UNIT = (uint32_t *)mmap(NULL, uart_size, PROT_READ|PROT_WRITE, MAP_SHARED, custom_uart_fd, CUSTOM_UART_RS485);
	if(p32_RS485_MB_UNIT == MAP_FAILED)
	{
		printf("Custom Uart 4 memory mapping failed\r\n");
		exit(1);
	}

	close(custom_uart_fd);

	printf("AXI Uart 1,2,3,4 init\r\n");


}

/*------------------------- Custom Uart 1 : Read and Write --------------------------*/

int custom_uart_1_read(uint8_t *p8_rxdata,uint32_t u32_len)
{

	int 		i_error;
	uint8_t 	u8_ret;
	uint32_t 	u32_count;

	struct itimerspec 	st_timer_specs_1;

	/* 	start the timer before start reading data from the Read FIFO */
    st_timer_specs_1.it_value.tv_sec = 0;
    //st_timer_specs_1.it_value.tv_nsec = 0;
    st_timer_specs_1.it_value.tv_nsec = 800000000;
    st_timer_specs_1.it_interval.tv_sec = 0;
    st_timer_specs_1.it_interval.tv_nsec = 0;

    if (timer_settime(st_timerid_1, 0, &st_timer_specs_1, NULL) == -1)
    	perror("timer_1_settime");

   // printf("timer set\r\n");

    u8_timer1_expired = 0;

	for(u32_count = 0 ; (u32_count < u32_len) ; u32_count++)
	{
		for(u8_ret = 1 ; (u8_ret == 1 && (u8_timer1_expired == 0)) ; )
		{
			u8_ret = *(volatile uint32_t*)(p32_axi_uart_1 + FIFO_READ_OFFSET);
		}
		if(u8_timer1_expired == 1)
			break;
		p8_rxdata[u32_count] = *(volatile uint32_t*)p32_axi_uart_1;
		usleep(1);
	}

	if(u32_count < (u32_len - 1))
	{
		return i_error = -1;
	}
	else
	{
		/* 	Stop the Timer*/
		st_timer_specs_1.it_value.tv_sec = 0;
		st_timer_specs_1.it_value.tv_nsec = 0;
		if (timer_settime(st_timerid_1, 0, &st_timer_specs_1, NULL) == -1)
			perror("timer_1_settime");
		return i_error = u32_count;
	}
}

void custom_uart_1_write(uint8_t *p8_txdata,uint32_t u32_len)
{
	uint32_t 	u32_count;
	uint8_t 	u8_ret;

	uint32_t 	u32_temp = 0;




	/*	For sending command to every Daughter board the command frame length
	 * 	is same for all commands.
	 */
	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 0 ; u8_ret == 1 ; )
		{

			u8_ret = *(volatile uint32_t*)(p32_axi_uart_1 + FIFO_WRITE_OFFSET);
//			printf("wrf %d\r\n",u32_temp);
//			printf("w u8_ret %d\r\n",u8_ret);
			u32_temp++;
		}

		*(volatile uint32_t*)p32_axi_uart_1 = p8_txdata[u32_count];
//		usleep(1);
	}
}


/*------------------------- Custom Uart 2 : Read and Write --------------------------*/

int custom_uart_2_read(uint8_t *p8_rxdata,uint32_t u32_len)
{

	int 		i_error;
	uint8_t 	u8_ret;
	uint32_t 	u32_count;

	struct itimerspec 	st_timer_specs_2;

	/* 	start the timer before start reading data from the Read FIFO */
    st_timer_specs_2.it_value.tv_sec = 0;
    //st_timer_specs_2.it_value.tv_nsec = 0;
    st_timer_specs_2.it_value.tv_nsec = 800000000;
    st_timer_specs_2.it_interval.tv_sec = 0;
    st_timer_specs_2.it_interval.tv_nsec = 0;

    if (timer_settime(st_timerid_2, 0, &st_timer_specs_2, NULL) == -1)
    	perror("timer_2_settime");

    u8_timer2_expired = 0;

	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{

		for(u8_ret = 1 ; (u8_ret == 1 && (u8_timer2_expired == 0)); )
		{
			u8_ret = *(volatile uint32_t*)(p32_axi_uart_2 + FIFO_READ_OFFSET);
//			printf("ret = %d\r\n",u8_ret);
		}

		if(u8_timer2_expired == 1)
			break;
		p8_rxdata[u32_count] = *(volatile uint32_t*)p32_axi_uart_2;
		usleep(1);
	}

	if(u32_count < (u32_len - 1))
	{
		return i_error = -1;
	}
	else
	{
		/* 	Stop the Timer*/
		st_timer_specs_2.it_value.tv_sec = 0;
		st_timer_specs_2.it_value.tv_nsec = 0;
		if (timer_settime(st_timerid_2, 0, &st_timer_specs_2, NULL) == -1)
			perror("timer_2_settime");
		return i_error = u32_count;
	}
}

void custom_uart_2_write(uint8_t *p8_txdata,uint32_t u32_len)
{
	uint32_t 	u32_count;
	uint8_t 	u8_ret;


	/*	For sending command to every Daughter board the command frame length
	 * 	is same for all commands.
	 */
	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 0 ; u8_ret == 1 ; )
		{

			u8_ret = *(volatile uint32_t*)(p32_axi_uart_2 + FIFO_WRITE_OFFSET);
		}

		*(volatile uint32_t*)p32_axi_uart_2 = p8_txdata[u32_count];
//		usleep(1);
	}
}

/*------------------------- Custom Uart 3 : Read and Write --------------------------*/

int custom_uart_3_read(uint8_t *p8_rxdata,uint32_t u32_len)
{

	int 		i_error;
	uint8_t 	u8_ret;
	uint32_t 	u32_count;

	struct itimerspec 	st_timer_specs_3;

	/* 	start the timer before start reading data from the Read FIFO */
    st_timer_specs_3.it_value.tv_sec = 0;
    //st_timer_specs_3.it_value.tv_nsec = 0;
    st_timer_specs_3.it_value.tv_nsec = 800000000;
    st_timer_specs_3.it_interval.tv_sec = 0;
    st_timer_specs_3.it_interval.tv_nsec = 0;

    if (timer_settime(st_timerid_3, 0, &st_timer_specs_3, NULL) == -1)
    	perror("timer_3_settime");

    u8_timer3_expired = 0;

	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 1 ; (u8_ret == 1 && (u8_timer3_expired == 0)) ; )
		{
			u8_ret = *(volatile uint32_t*)(p32_axi_uart_3 + FIFO_READ_OFFSET);
		}

		if(u8_timer3_expired == 1)
			break;
		p8_rxdata[u32_count] = *(volatile uint32_t*)p32_axi_uart_3;
		usleep(1);
	}

	if(u32_count < (u32_len - 1))
	{
		return i_error = -1;
	}
	else
	{
		/* 	Stop the Timer*/
		st_timer_specs_3.it_value.tv_sec = 0;
		st_timer_specs_3.it_value.tv_nsec = 0;
		if (timer_settime(st_timerid_3, 0, &st_timer_specs_3, NULL) == -1)
			perror("timer_3_settime");
		return i_error = u32_count;
	}
}

void custom_uart_3_write(uint8_t *p8_txdata,uint32_t u32_len)
{
	uint32_t 	u32_count;
	uint8_t 	u8_ret;


	/*	For sending command to every Daughter board the command frame length
	 * 	is same for all commands.
	 */
	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 0 ; u8_ret == 1 ; )
		{

			u8_ret = *(volatile uint32_t*)(p32_axi_uart_3 + FIFO_WRITE_OFFSET);
		}

		*(volatile uint32_t*)p32_axi_uart_3 = p8_txdata[u32_count];
//		usleep(1);
	}
}



/*------------------------- Custom Uart 4 : Read and Write --------------------------*/

int custom_uart_4_read(uint8_t *p8_rxdata,uint32_t u32_len)
{

	long int 		i_error;
	uint8_t 	u8_ret;
	uint32_t 	u32_count;

	struct itimerspec 	st_timer_specs_4;

	/* 	start the timer before start reading data from the Read FIFO */
    st_timer_specs_4.it_value.tv_sec = 1;//7;
    st_timer_specs_4.it_value.tv_nsec = 0;
//    st_timer_specs_4.it_value.tv_nsec = 900000000;
    st_timer_specs_4.it_interval.tv_sec = 0;
    st_timer_specs_4.it_interval.tv_nsec = 0;

    if (timer_settime(st_timerid_4, 0, &st_timer_specs_4, NULL) == -1)
    	perror("timer_4_settime");

    u8_timer4_expired = 0;

	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 1 ; (u8_ret == 1 && (u8_timer4_expired == 0)) ; )
		{
			u8_ret = *(volatile uint32_t*)(p32_axi_uart_4 + FIFO_READ_OFFSET);
		}

		if(u8_timer4_expired == 1)
			break;
		p8_rxdata[u32_count] = *(volatile uint32_t*)p32_axi_uart_4;
//		usleep(1);
	}

	if(u32_count < (u32_len - 1))
	{
		return i_error = -1;
	}
	else
	{
		/* 	Stop the Timer*/
		st_timer_specs_4.it_value.tv_sec = 0;
		st_timer_specs_4.it_value.tv_nsec = 0;
		if (timer_settime(st_timerid_4, 0, &st_timer_specs_4, NULL) == -1)
			perror("timer_4_settime");
//		printf("rd count %d\r\n",u32_count);
		return i_error = u32_count;
	}
}

void custom_uart_4_write(uint8_t *p8_txdata,uint32_t u32_len)
{
	uint32_t 	u32_count;
	uint8_t 	u8_ret;


	/*	For sending command to every Daughter board the command frame length
	 * 	is same for all commands.
	 */
	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 0 ; u8_ret == 1 ; )
		{

			u8_ret = *(volatile uint32_t*)(p32_axi_uart_4 + FIFO_WRITE_OFFSET);
		}

		*(volatile uint32_t*)p32_axi_uart_4 = p8_txdata[u32_count];
		usleep(1);
//		usleep(5);
	}
}

/*----------------- Custom Uart RS 485 Read and Write -----------------------------*/
/*----------------- RS485 Read ---------------------------------------------------*/
int custom_uart_RS485_read(uint8_t *p8_rxdata,uint32_t u32_len,uint8_t u8_opto)
{

	long int 		i_error;
	uint8_t 	u8_ret;
	uint32_t 	u32_count;

	struct itimerspec 	st_timer_specs_RS485;

	/* 	start the timer before start reading data from the Read FIFO */
	if(u8_opto)
	{
		st_timer_specs_RS485.it_value.tv_sec = 14;
		//st_timer_specs_4.it_value.tv_nsec = 0;
		st_timer_specs_RS485.it_value.tv_nsec = 0;
	}
	else
	{
		st_timer_specs_RS485.it_value.tv_sec = 0;
	    //st_timer_specs_4.it_value.tv_nsec = 0;
		st_timer_specs_RS485.it_value.tv_nsec = 800000000;
	}

	st_timer_specs_RS485.it_interval.tv_sec = 0;
	st_timer_specs_RS485.it_interval.tv_nsec = 0;

    if (timer_settime(st_timerid_RS485, 0, &st_timer_specs_RS485, NULL) == -1)
    	perror("timer_RS485_settime");

    u8_timer_RS485_expired = 0;

    /*	check the length for OPTO signal from the MB Unit
     * 	as it will be either 'O' or 'O','O'
     * 	set the length accordingly 1 or 2
     */

	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 1 ; (u8_ret == 1 && (u8_timer_RS485_expired == 0)) ; )
		{
			u8_ret = *(volatile uint32_t*)(p32_RS485_MB_UNIT + FIFO_READ_OFFSET);
		}

		if(u8_timer_RS485_expired == 1)
			break;
		p8_rxdata[u32_count] = *(volatile uint32_t*)p32_RS485_MB_UNIT;
//		usleep(1);
	}

	if(u32_count < (u32_len - 1))
	{
		return i_error = -1;
	}
	else
	{
		/* 	Stop the Timer*/
		st_timer_specs_RS485.it_value.tv_sec = 0;
		st_timer_specs_RS485.it_value.tv_nsec = 0;
		if (timer_settime(st_timerid_RS485, 0, &st_timer_specs_RS485, NULL) == -1)
			perror("timer_RS485_settime");
//		printf("rd count %d\r\n",u32_count);
		return i_error = u32_count;
	}
}

/*----------------- RS485 Write --------------------------------------------------*/
void custom_uart_RS485_write(uint8_t *p8_txdata,uint32_t u32_len)
{
	uint32_t 	u32_count;
	uint8_t 	u8_ret;


	/*	For sending command to every Daughter board the command frame length
	 * 	is same for all commands.
	 */
	for(u32_count = 0 ; u32_count < u32_len ; u32_count++)
	{
		for(u8_ret = 0 ; u8_ret == 1 ; )
		{

			u8_ret = *(volatile uint32_t*)(p32_RS485_MB_UNIT + FIFO_WRITE_OFFSET);
		}

		*(volatile uint32_t*)p32_RS485_MB_UNIT = p8_txdata[u32_count];
//		usleep(1);
	}
}



/*----------------- Function to Rest the FIFO of Custom Uart -----------------------*/

void custom_uart_fifo_reset(uint32_t *p32_custom_uart)
{
	int8_t 		i_ret;
	uint8_t 	u8_temp;

	uint32_t u32_count = 0;

	//printf("fifo reset\r\n");
	for(i_ret = 0 /*u8_ret = 0*/ ; i_ret == 0; )
	{
		//printf("frt %d\r\n",u32_count);
		i_ret = *(volatile uint32_t*)(p32_custom_uart + FIFO_READ_OFFSET);
		u8_temp = *(volatile uint32_t*)(p32_custom_uart);
//		printf("i_ret %d\r\n",i_ret);
//		printf("frt %d\r\n",u32_count);
		u32_count++;
	}
}
