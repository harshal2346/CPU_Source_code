/*
 * database.c
 *
 *  Created on: 14-Mar-2024
 *      Author: scope
 */

/**************************** INCLUDES ******************************/
#include <stdio.h>
#include <stdint.h>




#include "app.h"

/**************************** EXTERN VARIABLES **********************/
//extern test_parameters st_test_param;

/**************************** MACROS ********************************/
//	Data base Queries that will be fetched upon request
//fp_select = popen("sqlite3 Database_1.db \"SELECT datumClose1,datumClose2,datumOpen1,datumOpen2 FROM Breaker;\"","r");

//#define QRY_SYSTEM_SETTING 		"sqlite3 New_Database.db \"SELECT samplingSpeed, trigger, plotlength, closeCommandDuration, openCommandDuration, delayTOC, delayTCO, timingAuxiliaryContact, timingPirContact, travel, scrmCurrentRange, scrmResistanceRange, dcrmCurrentRange, dcrmResistanceRange  FROM SystemSetting;\"","r"
#define QRY_SYSTEM_SETTING		"sqlite3 Database.db \"SELECT CASE WHEN samplingSpeed = '100 KC' THEN 1 WHEN samplingSpeed = '50 KC' THEN 2 WHEN samplingSpeed = '20 KC' THEN 3 WHEN samplingSpeed = '10 KC' THEN 4 WHEN samplingSpeed = '5 KC' THEN 5 WHEN samplingSpeed = '2 KC' THEN 6 WHEN samplingSpeed = '1 KC' THEN 7 WHEN samplingSpeed = '500 C' THEN 8 WHEN samplingSpeed = '200 C' THEN 9 WHEN samplingSpeed = '100 C' THEN 10 END AS samplingSpeed, CASE WHEN trigger = 'Internal' THEN 0 ELSE 1 END AS trigger, plotLength, closeCommandDuration, openCommandDuration, delayTOC, delayTCO, CASE WHEN timingAuxiliaryContact = 'true' THEN 1 ELSE 0 END AS timingAuxiliaryContact, CASE WHEN timingPIRContact = 'true' THEN 1 ELSE 0 END AS timingPIRContact, scrmCurrentRange, REPLACE(scrmResistanceRange, ' µΩ', '') AS scrmResistanceRange, dcrmCurrentRange, REPLACE(dcrmResistanceRange, ' µΩ', '') AS dcrmResistanceRange, CASE WHEN travelTransducerType = 'Analog' THEN 0 ELSE 1 END AS travelTransducerType, CASE WHEN travelTransducerMechanism = 'Linear' THEN 0 ELSE 1 END AS travelTransducerMechanism FROM SystemSetting;\"","r"

//#define QRY_DCRM				"DCRM"
//#define QRY_SCRM				"SCRM"
//#define QRY_COIL_CURRENT		"sqlite3 New_Database.db \"SELECT channelName, transducerType FROM Travel;\"","r"
//#define QRY_TRAVEL				"sqlite3 New_Database.db \"SELECT channelName, currentRange FROM CoilCurrent;\"","r"

#define	QRY_COIL_CURRENT		"sqlite3 Database.db \"SELECT CASE WHEN enableDisable = 'false' THEN 0 ELSE 1 END AS enableDisable, CAST(SUBSTR(channelName, LENGTH('Coil Current ') + 1) AS INTEGER) AS channelName, CAST(REPLACE(currentRange, 'A', '') AS INTEGER) AS currentRange FROM CoilCurrent;\"","r"
#define QRY_TRAVEL				"sqlite3 Database.db \"SELECT CASE WHEN enableDisable = 'false' THEN 0 ELSE 1 END AS enableDisable, CAST(SUBSTR(channelName, 2) AS INTEGER) AS channelName FROM Travel;\"","r"
#define QRY_ANALOG				"sqlite3 Database.db \"SELECT CASE WHEN enableDisable = 'false' THEN 0 ELSE 1 END AS enableDisable, CAST(SUBSTR(channelName, 4) AS INTEGER) AS channelName FROM Analog;\"","r"


/**************************** TYPEDEFS ******************************/


/**************************** STATIC VARIABLES **********************/


/**************************** GLOBAL VARIABLES **********************/


/**************************** FUNCTION PROTOTYPES *******************/
void travel_chn_selection_test();
void coil_curr_chn_selection_test();
void ana_chn_selection_test();
/**************************** FUNCTIONS *****************************/



void database_fetch_system_settings()
{
	FILE *fp_qry;
	uint16_t u16_count = 0;
	char c_qry[1000] = {0};
	char *p_char;
	p_char = c_qry;
	//    fp_select = popen("sqlite3 Database_1.db \"SELECT datumClose1,datumClose2,datumOpen1,datumOpen2 FROM Breaker;\"","r");
	//    if (fp_select == NULL) {
	//      printf("Failed to run command\n" );
	//      exit(1);
	//    }
	//    usleep(500000);

	/* Read the output a line at a time - output it. */
	//     while (fgets(path, sizeof(path), fp_select) != NULL) {
	//       printf("%s", path);
	//     }

	//     while(*ptr != '\0')
	//     {
	////    	 printf("%c",ptr[i]);
	////    	 if(ptr[i] == '|')
	////    		 printf("\r\n");
	//    	 float val = strtof(ptr,NULL);
	//    	 printf("val = %f\r\n",val);
	//    	 while(*ptr != '|')
	//    		 ptr++;
	//    	 ptr++;
	//
	//    	 //i++;
	//     }


//	samplingSpeed,
//	trigger,
//	plotlength,
//	closeCommandDuration,
//	openCommandDuration,
//	delayTOC,
//	delayTCO,
//	timingAuxiliaryContact,
//	timingPirContact,
//	travel,
//	scrmCurrentRange,
//	scrmResistanceRange,
//	dcrmCurrentRange,
//	dcrmResistanceRange

	//fp_qry = popen("sqlite3 New_Database.db \"SELECT samplingSpeed, trigger, plotlength, closeCommandDuration, openCommandDuration, delayTOC, delayTCO, timingAuxiliaryContact, timingPirContact, travel, scrmCurrentRange, scrmResistanceRange, dcrmCurrentRange, dcrmResistanceRange  FROM SystemSetting;\"","r");
	fp_qry = popen(QRY_SYSTEM_SETTING);
	if(fp_qry == NULL)
	{
		printf("Failed to run Database system settings query\r\n");

	}
	else
	{
		printf("fp_qry %x\r\n",fp_qry);
		printf("Database opened\r\n");
		while((fgets(c_qry,sizeof(c_qry),fp_qry) != NULL))
		{
			printf("%s", c_qry);
		}

		st_test_param.u16_sampling_speed	= strtol(p_char,NULL,10);

		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_trigger 			= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_plot_length 		= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_close_cmd 		= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_open_cmd 			= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_delay_toc 		= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_delay_tco 		= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_tim_auxiliary 	= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_tim_pir 			= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_scrm_current 			= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_scrm_resistance 		= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_dcrm_current 	= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_dcrm_resistance 		= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_travel_trans_type 	= strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.u16_travel_trans_mech 	= strtol(p_char,NULL,10);

		/*	Below printf functions are for testing purpose only
		 * 	After testing of Database query comment the below section
		 */

		printf("u16_sampling_speed %d\r\n",st_test_param.u16_sampling_speed);
		printf("u16_trigger %d\r\n",st_test_param.u16_trigger);
		printf("u16_plot_length %d\r\n",st_test_param.u16_plot_length);

		printf("u16_close_cmd %d\r\n",st_test_param.u16_close_cmd);
		printf("u16_open_cmd %d\r\n",st_test_param.u16_open_cmd);
		printf("u16_delay_toc %d\r\n",st_test_param.u16_delay_toc);
		printf("u16_delay_tco %d\r\n",st_test_param.u16_delay_tco);
		printf("u16_tim_auxiliary %d\r\n",st_test_param.u16_tim_auxiliary);
		printf("u16_tim_pir %d\r\n",st_test_param.u16_tim_pir);
		//printf("u16_travel %d\r\n",st_test_param.u16_travel);
		printf("u16_scrm_current %d\r\n",st_test_param.u16_scrm_current);
		printf("u16_scrm_resistance %d\r\n",st_test_param.u16_scrm_resistance);
		printf("u16_dcrm_current %d\r\n",st_test_param.u16_dcrm_current);
		printf("u16_dcrm_resistance %d\r\n",st_test_param.u16_dcrm_resistance);


		printf("u16_travel_trans_type %d\r\n",st_test_param.u16_travel_trans_type);
		printf("u16_travel_trans_mech %d\r\n",st_test_param.u16_travel_trans_mech);

		pclose(fp_qry);


	}
}


void database_fetch_coil_current_ch_info()
{
	FILE *fp_qry;
	uint16_t u16_count = 0;
	char c_qry[500] = {0};
	char *p_char = c_qry;

	printf("database_fetch_coil_current_ch_info\r\n");
	fp_qry = popen(QRY_COIL_CURRENT);
	if(fp_qry == NULL)
	{
		printf("Failed to run Database coil current query\r\n");
	}
	else
	{
		printf("fp_qry %x\r\n",fp_qry);
		printf("Database opened\r\n");

		for(u16_count = 0; u16_count < 500 ; u16_count++)
		{
			c_qry[u16_count] = fgetc(fp_qry);
		}

		printf("%s\r\n",c_qry);
		/*--------------------- Channel 1 -----------------------*/
		st_test_param.st_coil_curr.u8_ch_en[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u8_ch[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u16_ch_current_range[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d current range %d\r\n",st_test_param.st_coil_curr.u8_ch[e_ch_1],
																st_test_param.st_coil_curr.u8_ch_en[e_ch_1],
																st_test_param.st_coil_curr.u16_ch_current_range[e_ch_1]);

		/*--------------------- Channel 2 -----------------------*/
		st_test_param.st_coil_curr.u8_ch_en[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u8_ch[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u16_ch_current_range[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d current range %d\r\n",st_test_param.st_coil_curr.u8_ch[e_ch_2],
															st_test_param.st_coil_curr.u8_ch_en[e_ch_2],
															st_test_param.st_coil_curr.u16_ch_current_range[e_ch_2]);

		/*--------------------- Channel 3 -----------------------*/
		st_test_param.st_coil_curr.u8_ch_en[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u8_ch[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u16_ch_current_range[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d current range %d\r\n",st_test_param.st_coil_curr.u8_ch[e_ch_3],
															st_test_param.st_coil_curr.u8_ch_en[e_ch_3],
															st_test_param.st_coil_curr.u16_ch_current_range[e_ch_3]);

		/*--------------------- Channel 4 -----------------------*/
		st_test_param.st_coil_curr.u8_ch_en[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u8_ch[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u16_ch_current_range[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d current range %d\r\n",st_test_param.st_coil_curr.u8_ch[e_ch_4],
															st_test_param.st_coil_curr.u8_ch_en[e_ch_4],
															st_test_param.st_coil_curr.u16_ch_current_range[e_ch_4]);

		/*--------------------- Channel 5 -----------------------*/
		st_test_param.st_coil_curr.u8_ch_en[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u8_ch[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u16_ch_current_range[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d current range %d\r\n",st_test_param.st_coil_curr.u8_ch[e_ch_5],
															st_test_param.st_coil_curr.u8_ch_en[e_ch_5],
															st_test_param.st_coil_curr.u16_ch_current_range[e_ch_5]);

		/*--------------------- Channel 6 -----------------------*/
		st_test_param.st_coil_curr.u8_ch_en[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u8_ch[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_coil_curr.u16_ch_current_range[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d current range %d\r\n",st_test_param.st_coil_curr.u8_ch[e_ch_6],
															st_test_param.st_coil_curr.u8_ch_en[e_ch_6],
															st_test_param.st_coil_curr.u16_ch_current_range[e_ch_6]);

		pclose(fp_qry);
	}

	coil_curr_chn_selection_test();

}

void database_fetch_travel_ch_info()
{
	FILE *fp_qry;
	uint16_t u16_count = 0;
	char c_qry[500] = {0};
	char *p_char = c_qry;

	printf("database_fetch_coil_current_ch_info\r\n");
	fp_qry = popen(QRY_TRAVEL);
	if(fp_qry == NULL)
	{
		printf("Failed to run Database coil current query\r\n");
	}
	else
	{
		printf("fp_qry %x\r\n",fp_qry);
		printf("Database opened\r\n");

		for(u16_count = 0; u16_count < 500 ; u16_count++)
		{
			c_qry[u16_count] = fgetc(fp_qry);
		}

		printf("%s\r\n",c_qry);

		/*--------------------- Channel 1 -----------------------*/
		st_test_param.st_travel.u8_ch_en[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_travel.u8_ch[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_travel.u8_ch[e_ch_1],
										st_test_param.st_travel.u8_ch_en[e_ch_1]);

		/*--------------------- Channel 2 -----------------------*/
		st_test_param.st_travel.u8_ch_en[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_travel.u8_ch[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_travel.u8_ch[e_ch_2],
										st_test_param.st_travel.u8_ch_en[e_ch_2]);

		/*--------------------- Channel 3 -----------------------*/
		st_test_param.st_travel.u8_ch_en[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_travel.u8_ch[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_travel.u8_ch[e_ch_3],
										st_test_param.st_travel.u8_ch_en[e_ch_3]);

		/*--------------------- Channel 4 -----------------------*/
		st_test_param.st_travel.u8_ch_en[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_travel.u8_ch[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_travel.u8_ch[e_ch_4],
										st_test_param.st_travel.u8_ch_en[e_ch_4]);

		/*--------------------- Channel 5 -----------------------*/
		st_test_param.st_travel.u8_ch_en[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_travel.u8_ch[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_travel.u8_ch[e_ch_5],
										st_test_param.st_travel.u8_ch_en[e_ch_5]);

		/*--------------------- Channel 6 -----------------------*/
		st_test_param.st_travel.u8_ch_en[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_travel.u8_ch[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_travel.u8_ch[e_ch_6],
										st_test_param.st_travel.u8_ch_en[e_ch_6]);

		pclose(fp_qry);
	}

	travel_chn_selection_test();

}

void database_fetch_analog_ch_info()
{
	FILE *fp_qry;
	uint16_t u16_count = 0;
	char c_qry[500] = {0};
	char *p_char = c_qry;

	printf("database_fetch_coil_current_ch_info\r\n");
	fp_qry = popen(QRY_ANALOG);
	if(fp_qry == NULL)
	{
		printf("Failed to run Database coil current query\r\n");
	}
	else
	{
		printf("fp_qry %x\r\n",fp_qry);
		printf("Database opened\r\n");

		for(u16_count = 0; u16_count < 500 ; u16_count++)
		{
			c_qry[u16_count] = fgetc(fp_qry);
		}

		printf("%s\r\n",c_qry);

		/*--------------------- Channel 1 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_1] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_1],
										st_test_param.st_analog.u8_ch_en[e_ch_1]);

		/*--------------------- Channel 2 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_2] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_2],
										st_test_param.st_analog.u8_ch_en[e_ch_2]);

		/*--------------------- Channel 3 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_3] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_3],
										st_test_param.st_analog.u8_ch_en[e_ch_3]);

		/*--------------------- Channel 4 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_4] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_4],
										st_test_param.st_analog.u8_ch_en[e_ch_4]);

		/*--------------------- Channel 5 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_5] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_5],
										st_test_param.st_analog.u8_ch_en[e_ch_5]);

		/*--------------------- Channel 6 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_6] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_6],
										st_test_param.st_analog.u8_ch_en[e_ch_6]);

		/*--------------------- Channel 7 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_7] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_7] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_7],
										st_test_param.st_analog.u8_ch_en[e_ch_7]);

		/*--------------------- Channel 8 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_8] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_8] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_8],
										st_test_param.st_analog.u8_ch_en[e_ch_8]);

		/*--------------------- Channel 9 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_9] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_9] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_9],
										st_test_param.st_analog.u8_ch_en[e_ch_9]);

		/*--------------------- Channel 10 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_10] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_10] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_10],
										st_test_param.st_analog.u8_ch_en[e_ch_10]);

		/*--------------------- Channel 11 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_11] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_11] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_11],
										st_test_param.st_analog.u8_ch_en[e_ch_11]);

		/*--------------------- Channel 12 -----------------------*/
		st_test_param.st_analog.u8_ch_en[e_ch_12] = strtol(p_char,NULL,10);
		while(*p_char != '|')
			p_char++;
		p_char++;

		st_test_param.st_analog.u8_ch[e_ch_12] = strtol(p_char,NULL,10);
		while(*p_char != '\n')
			p_char++;
		p_char++;

		printf("Channel %d enable %d\r\n",st_test_param.st_analog.u8_ch[e_ch_12],
										st_test_param.st_analog.u8_ch_en[e_ch_12]);

		pclose(fp_qry);
	}
	ana_chn_selection_test();
}

void travel_chn_selection_test()
{
	/*	the structure "test_parameters" is a global structure and can be accessed
	 * 	without any prior pointer needed just need to extern that structure in
	 * 	this file for access.
	 */
	uint16_t u16_count = 0,u16_channels = 0,u16_max_chn = 6;

	for(u16_count = 0 ; u16_count < u16_max_chn ; u16_count++)
	{
		if(st_test_param.st_travel.u8_ch_en[u16_count] == 1)
		{
//			u16_channels = u16_channels << 1;
			u16_travel_chn = u16_travel_chn | (1 << (u16_count));
//			u16_channels = u16_channels | 1;

		}
		else
		{
//			u16_channels = u16_channels << 1;
			u16_travel_chn = u16_travel_chn | (0 << (u16_count));
//			u16_channels = u16_channels | 0;

		}
	}

	printf("Travel channels %x\r\n",u16_travel_chn);
}

void coil_curr_chn_selection_test()
{
	uint16_t u16_count = 0,u16_channels = 0,u16_max_chn = 6;

	for(u16_count = 0 ; u16_count < u16_max_chn ; u16_count++)
	{
		if(st_test_param.st_coil_curr.u8_ch_en[u16_count] == 1)
		{
//			u16_channels = u16_channels << 1;
			u16_coil_curr_chn = u16_coil_curr_chn | (1 << (u16_count));
//			u16_channels = u16_channels | 1;

		}
		else
		{
//			u16_channels = u16_channels << 1;
			u16_coil_curr_chn = u16_coil_curr_chn | (0 << (u16_count));
//			u16_channels = u16_channels | 0;

		}
	}

	printf("Coil Current channels %x\r\n",u16_coil_curr_chn);

}

void ana_chn_selection_test()
{
	uint16_t u16_count = 0,u16_channels = 0,u16_max_chn = 12;

	for(u16_count = 0 ; u16_count < u16_max_chn ; u16_count++)
	{
		if(st_test_param.st_analog.u8_ch_en[u16_count] == 1)
		{
//			u16_channels = u16_channels << 1;
			u16_analog_chn = u16_analog_chn | (1 << (u16_count));

		}
		else
		{
//			u16_channels = u16_channels << 1;
			u16_analog_chn = u16_analog_chn | (0 << (u16_count));

		}
	}
	printf("Analog channels %x\r\n",u16_analog_chn);
}


