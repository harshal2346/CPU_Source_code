/*
 * custom_uart.h
 *
 *  Created on: 08-Mar-2024
 *      Author: scope
 */

#ifndef PERIPHERALS_CUSTOM_UART_CUSTOM_UART_H_
#define PERIPHERALS_CUSTOM_UART_CUSTOM_UART_H_


/**************************** INCLUDES ******************************/

/**************************** MACROS ********************************/

/**************************** TYPEDEFS ******************************/

/**************************** EXPORTED VARIABLES ********************/
uint32_t 	*p32_axi_uart_1,
			*p32_axi_uart_2,
			*p32_axi_uart_3,
			*p32_axi_uart_4,
			*p32_RS485_MB_UNIT;

/**************************** FUNCTION PROTOTYPES *******************/
void custom_uart_init();

int custom_uart_1_read(uint8_t *p8_rxdata,uint32_t u32_len);
int custom_uart_2_read(uint8_t *p8_rxdata,uint32_t u32_len);
int custom_uart_3_read(uint8_t *p8_rxdata,uint32_t u32_len);
int custom_uart_4_read(uint8_t *p8_rxdata,uint32_t u32_len);

void custom_uart_1_write(uint8_t *p8_txdata,uint32_t u32_len);
void custom_uart_2_write(uint8_t *p8_txdata,uint32_t u32_len);
void custom_uart_3_write(uint8_t *p8_txdata,uint32_t u32_len);
void custom_uart_4_write(uint8_t *p8_txdata,uint32_t u32_len);

void custom_uart_fifo_reset(uint32_t *p32_custom_uart);

#endif /* PERIPHERALS_CUSTOM_UART_CUSTOM_UART_H_ */
