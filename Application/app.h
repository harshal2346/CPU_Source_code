/*
 * app.h
 *
 *  Created on: 08-Mar-2024
 *      Author: scope
 */

#ifndef APPLICATION_APP_H_
#define APPLICATION_APP_H_

/**************************** INCLUDES ******************************/
#include <stdint.h>
#include <semaphore.h>
/**************************** MACROS ********************************/

#define DB_1 	0
#define DB_2 	1
#define DB_3 	2
#define DB_4	3
#define DB_5	4
#define DB_6	5
#define DB_7	6
#define DB_8	7
#define DB_9	8
#define DB_10	9
#define DB_11	10

#define ACTIVITY	1
#define ACK_ERR		2

#define ACKNOWLEDGE	0xA5A5
#define ERROR		0x5A5A

#define	CHANNEL_1_ITERATIONS	76
#define CHANNEL_2_ITERATIONS	152
#define CHANNEL_3_ITERATIONS	228
#define CHANNEL_4_ITERATIONS    304

#define CHANNEL_1	1
#define CHANNEL_2	2
#define CHANNEL_3	3
#define CHANNEL_4	4


/**************************** TYPEDEFS ******************************/
typedef struct frame_info
{
	uint16_t u16_header;
	uint16_t u16_card_address;
	uint16_t u16_identifier;
	uint16_t u16_activity;
	uint16_t u16_payload;
	uint16_t p16_data[21];
	uint16_t u16_checksum;
	uint16_t u16_footer;
}frame_info;

typedef enum states{	e_REQ_TEST_SETTINGS,
	e_REQ_HANDSHAKE,
	e_REQ_DATA,
	e_RESP,
	e_CONFIG_PARAMETERS,
	e_CONFIG_READY_FOR_TEST,
	e_OPTO_WAIT,
	e_CONFIG_DIAGNOTICS,
	e_NO_TEST
}states;

typedef enum selection
{
	e_SELECTED,
	e_NOT_SELECTED,
	e_ERR,
	e_ACK,
}selection;

typedef enum cards{
	e_CARD_1 = 0X00,
	e_CARD_2,
	e_CARD_3,
	e_CARD_4,
	e_CARD_5,
	e_CARD_6,
	e_CARD_7,
	e_CARD_8,
	e_CARD_9,
	e_CARD_10,
	e_CARD_11,
}cards;

typedef enum errors{	e_NO_ERR 				= 0,
	e_ERR_HEADER			= 0x0001,
	e_ERR_FOOTER	 		= 0x0002,
	e_ERR_CARD_ADDRESS	 	= 0x0004,
	e_ERR_CHECKSUM		 	= 0x0008,
	e_ERR_FRAME_IDENTIFIER 	= 0x0010,
	e_ERR_ITERATION			= 0x0020,
	e_ERR_ACTIVITY_CDOE		= 0X0040
}errors;

typedef enum Channel_name
{
	e_ch_1,
	e_ch_2,
	e_ch_3,
	e_ch_4,
	e_ch_5,
	e_ch_6,
	e_ch_7,
	e_ch_8,
	e_ch_9,
	e_ch_10,
	e_ch_11,
	e_ch_12
}Channel_name;

typedef enum Board
{
	e_Analog_Baord,
	e_Digital_Board
}Board;

typedef enum tests
{
	/*	For more information regarding the Activity Codes section
	 * 	between the High Level Code and Low Level C code see the Google Sheet on
	 * 	https://docs.google.com/spreadsheets/d/1QftMSRNq4J1zHj5oUTjRXPLYA1rD6ilGQodx-Y-HAX8/edit?gid=0#gid=0
	 * 	It has detailed explanation of each activity code selected for the Communication
	 */
	e_SCRM = 0x14,
	e_TIMING_MAIN,
	e_TIMING_AUXILIARY,
	e_TIMING_PIR,
	e_SIMPLE_OPEN,
	e_SIMPLE_CLOSE,
	e_DCRM_OPEN,
	e_DCRM_CO,
	e_TIMING_OPEN,
	e_TIMING_CLOSE,
	e_TIMING_CO,
	e_TIMING_OC,
	e_TIMING_OCO,
	e_TIMING_O_CO_CO,
	e_PIR_CLOSE,
	e_FIRST_TRIP,
	e_MOTOR_CURRENT,
	e_HARDWARE_CONFIG,
	e_DATA_REC_ERROR = 0x7A,
}tests;

typedef enum channels
{
	e_COIL_1,
	e_COIL_2,
	e_COIL_3,
	e_COIL_4,
	e_COIL_5,
	e_COIL_6,

	e_SDCRM_1_V_1,
	e_SDCRM_2_V_1,
	e_SDCRM_3_V_1,
	e_SDCRM_4_V_1,
	e_SDCRM_5_V_1,
	e_SDCRM_6_V_1,

	e_SDCRM_1_V_2,
	e_SDCRM_2_V_2,
	e_SDCRM_3_V_2,
	e_SDCRM_4_V_2,
	e_SDCRM_5_V_2,
	e_SDCRM_6_V_2,

	e_SDCRM_1_C_1,
	e_SDCRM_2_C_1,
	e_SDCRM_3_C_1,
	e_SDCRM_4_C_1,
	e_SDCRM_5_C_1,
	e_SDCRM_6_C_1,

	e_SDCRM_1_C_2,
	e_SDCRM_2_C_2,
	e_SDCRM_3_C_2,
	e_SDCRM_4_C_2,
	e_SDCRM_5_C_2,
	e_SDCRM_6_C_2,

	e_TRAVEL_1,
	e_TRAVEL_2,
	e_TRAVEL_3,
	e_TRAVEL_4,
	e_TRAVEL_5,
	e_TRAVEL_6,

	e_ANA_CH_1,
	e_ANA_CH_2,
	e_ANA_CH_3,
	e_ANA_CH_4,
	e_ANA_CH_5,
	e_ANA_CH_6,
	e_ANA_CH_7,
	e_ANA_CH_8,
	e_ANA_CH_9,
	e_ANA_CH_10,
	e_ANA_CH_11,
	e_ANA_CH_12,

	e_MAIN_PIR,
	e_AUX,
}channels;

typedef struct coil_current
{
	uint8_t u8_ch_en[6];
	uint8_t u8_ch[6];
	uint16_t u16_ch_current_range[6];
}coil_current;

typedef struct travel
{
	uint8_t u8_ch_en[6];
	uint8_t u8_ch[6];
}travel;

typedef struct analog
{
	uint8_t u8_ch_en[12];
	uint8_t u8_ch[12];
}analog;

typedef struct hardware_config
{
	uint32_t u32_config_reg_1;
	uint32_t u32_config_reg_2;
	uint32_t u32_config_reg_3;
	uint32_t u32_config_reg_4;
}hardware_config;

typedef struct test_parameters
{
	uint16_t u16_sampling_speed;
	uint16_t u16_trigger;
	uint16_t u16_plot_length;
	uint16_t u16_close_cmd;
	uint16_t u16_open_cmd;
	uint16_t u16_delay_toc;
	uint16_t u16_delay_tco;
	uint16_t u16_tim_auxiliary;
	uint16_t u16_tim_pir;
	uint16_t u16_dcrm_current;
	uint16_t u16_dcrm_resistance;
	uint16_t u16_scrm_current;
	uint16_t u16_scrm_resistance;
	uint16_t u16_travel_trans_type;
	uint16_t u16_travel_trans_mech;
	uint16_t u16_coil_current_range;
	uint16_t u16_coil_current_channels;
	uint16_t u16_travel_ch;
	uint16_t u16_travel_transducer;
	coil_current 	st_coil_curr;
	travel 			st_travel;
	analog			st_analog;
}test_parameters;

typedef struct {
	uint8_t 	u8_slot;
	uint8_t 	u8_card_address;
	uint8_t 	u8_board;
	uint8_t 	u8_activity;
	uint8_t 	u8_ack_error;
	uint8_t 	u8_channel[4];
	uint8_t 	u8_data_arrived;
	int32_t 	*p_data_files[4];
	uint8_t 	u8_process_done;
	uint8_t 	u8_test;
	uint16_t	u16_error_value;
	test_parameters st_test_param;
	uint16_t 	u16_iterations;

}card_info;

//extern card_info cards_t[11];

/**************************** EXPORTED VARIABLES ********************/
extern sem_t 	*sem_write_socket,
				*sem_Slot_1,
				*sem_Slot_2,
				*sem_Slot_3,
				*sem_Slot_4,
				*sem_opto_wait,
				*sem_mb_dcrm;

extern test_parameters st_test_param;

extern uint16_t u16_analog_chn;
extern uint16_t u16_travel_chn;
extern uint16_t u16_coil_curr_chn;

/**************************** FUNCTION PROTOTYPES *******************/
void *communication_slot_1();
void *communication_slot_2();
void *communication_slot_3();
void *communication_slot_4();
void *read_socket();
void create_data_files();
void socket_resp(uint16_t u16_activity, uint16_t u16_ack_err);
void channel_data_rec_resp(uint8_t u8_data_file);

#endif /* APPLICATION_APP_H_ */
