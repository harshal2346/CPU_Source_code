/*
 * database.h
 *
 *  Created on: 14-Mar-2024
 *      Author: scope
 */

#ifndef APPLICATION_DATABASE_H_
#define APPLICATION_DATABASE_H_

/**************************** INCLUDES ******************************/

/**************************** MACROS ********************************/

/**************************** TYPEDEFS ******************************/

/**************************** EXPORTED VARIABLES ********************/

/**************************** FUNCTION PROTOTYPES *******************/
void database_fetch_system_settings();
void database_fetch_coil_current_ch_info();
void database_fetch_travel_ch_info();
void database_fetch_analog_ch_info();

#endif /* APPLICATION_DATABASE_H_ */
