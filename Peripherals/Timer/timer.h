/*
 * timer.h
 *
 *  Created on: 08-Mar-2024
 *      Author: scope
 */

#ifndef PERIPHERALS_TIMER_TIMER_H_
#define PERIPHERALS_TIMER_TIMER_H_

/**************************** INCLUDES ******************************/

/**************************** MACROS ********************************/

/**************************** TYPEDEFS ******************************/

/**************************** VARIABLES ********************/
extern timer_t	st_timerid_1,
				st_timerid_2,
				st_timerid_3,
				st_timerid_4,
				st_timerid_RS485;

extern volatile uint8_t	u8_timer1_expired ,
						u8_timer2_expired ,
						u8_timer3_expired ,
						u8_timer4_expired ,
						u8_timer_RS485_expired;

/**************************** FUNCTION PROTOTYPES *******************/
void timer_init();

#endif /* PERIPHERALS_TIMER_TIMER_H_ */
