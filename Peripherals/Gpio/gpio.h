/*
 * gpio.h
 *
 *  Created on: 08-Mar-2024
 *      Author: scope
 */

#ifndef PERIPHERALS_GPIO_GPIO_H_
#define PERIPHERALS_GPIO_GPIO_H_

/**************************** INCLUDES ******************************/

/**************************** MACROS ********************************/

/**************************** TYPEDEFS ******************************/

/**************************** EXPORTED VARIABLES ********************/

/**************************** FUNCTION PROTOTYPES *******************/
void gpio_init();
void gpio_rs485_tx_en();
void gpio_slot_en(uint16_t u8_slot,uint8_t u8_enable);
void gpio_spare_en(uint16_t u16_pin, uint8_t u8_enable);
void gpio_rst_str(uint8_t u8_pin, uint8_t u8_enable);
void gpio_common(uint8_t u8_pin, uint8_t u8_enable);
void gpio_spi_test(uint8_t u8_high);
void gpio_Led_en(uint8_t u8_high);

#endif /* PERIPHERALS_GPIO_GPIO_H_ */
