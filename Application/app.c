/*
 * app.c
 *
 *  Created on: 08-Mar-2024
 *      Author: scope
 */
/**************************** INCLUDES ******************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "app.h"
#include "custom_uart.h"
#include "database.h"

/**************************** EXTERN VARIABLES **********************/

/**************************** MACROS ********************************/
#define 	MAX_LEN 			4000
#define		HEADER				0
#define		CARD_ADDRESS		1
#define		FRAME_IDENTIFIER	2
#define		ACTIVITY_CODE		3
#define		FRAME_PAYLOAD		4
#define		CC_CHECKSUM			27
#define		D_CHECKSUM			3998
#define		FOOTER				28
#define 	MAX_AI_ITERATIONS	228
#define 	MAX_DI_ITERATIONS	76
#define 	OPTO_RECEPTION_SUCCESS	7
#define		OPTO_RECEPTION_FAILED	6

//#define		SOC_SERVER_IP		"10.42.0.220"
#define		SOC_SERVER_IP		"192.168.1.10"
//#define		SOC_PORT			"8000"
#define		SOC_PORT			"3001"
//#define		SOC_PORT			"3000"


#define		SLOT_1	1
#define		SLOT_2	2
#define		SLOT_3	3
#define		SLOT_4	4
#define		MB_UNIT	5
#define		DCRM_1	6
#define		DCRM_2	7
#define		DCRM_3	8
#define		DCRM_4	9
#define		DCRM_5	10
#define		DCRM_6	11
#define		HDS_ARR_LEN	56

/**************************** TYPEDEFS ******************************/


/**************************** STATIC VARIABLES **********************/


/**************************** GLOBAL VARIABLES **********************/
FILE 	*ptr_ch_1,
*ptr_ch_2,
*ptr_ch_3,
*ptr_ch_4,
*ptr_ch_5,
*ptr_ch_6,
*ptr_ch_7,
*ptr_ch_8,
*ptr_slot_1_config,
*ptr_slot_2_config,
*ptr_slot_3_config,
*err_ptr;

sem_t 	*sem_write_socket,
		*sem_Slot_1,
		*sem_Slot_2,
		*sem_Slot_3,
		*sem_Slot_4,
		*sem_opto_wait,
		*sem_mb_dcrm;


typedef struct st_data_shm_ptr
{
	int *p_coil_1;
	int *p_coil_2;
	int *p_coil_3;
	int *p_coil_4;
	int *p_coil_5;
	int *p_coil_6;

	int *p_sdcrm_1_v_1;
	int *p_sdcrm_2_v_1;
	int *p_sdcrm_3_v_1;
	int *p_sdcrm_4_v_1;
	int *p_sdcrm_5_v_1;
	int *p_sdcrm_6_v_1;

	int *p_sdcrm_1_v_2;
	int *p_sdcrm_2_v_2;
	int *p_sdcrm_3_v_2;
	int *p_sdcrm_4_v_2;
	int *p_sdcrm_5_v_2;
	int *p_sdcrm_6_v_2;

	int *p_sdcrm_1_c_1;
	int *p_sdcrm_2_c_1;
	int *p_sdcrm_3_c_1;
	int *p_sdcrm_4_c_1;
	int *p_sdcrm_5_c_1;
	int *p_sdcrm_6_c_1;

	int *p_sdcrm_1_c_2;
	int *p_sdcrm_2_c_2;
	int *p_sdcrm_3_c_2;
	int *p_sdcrm_4_c_2;
	int *p_sdcrm_5_c_2;
	int *p_sdcrm_6_c_2;

	int *p_travel_1;
	int *p_travel_2;
	int *p_travel_3;
	int *p_travel_4;
	int *p_travel_5;
	int *p_travel_6;

	int *p_ana_ch_1;
	int *p_ana_ch_2;
	int *p_ana_ch_3;
	int *p_ana_ch_4;
	int *p_ana_ch_5;
	int *p_ana_ch_6;
	int *p_ana_ch_7;
	int *p_ana_ch_8;
	int *p_ana_ch_9;
	int *p_ana_ch_10;
	int *p_ana_ch_11;
	int *p_ana_ch_12;

	int *p_main_pir;
	int *p_aux;
}st_data_shm_ptr;

typedef union u_data_p
{
	st_data_shm_ptr st_data_shm;
	int *p_data_shm[50];
}u_data_p;

u_data_p u_data_ptr;

frame_info frame_t ;//= { 	.u16_header = 0xAAAA,
//		.u16_footer = 0x5555,
//};


//card_info cards_t[] = {
//		{.u8_slot = 0x01,.u8_card_address = 0x01},
//		{.u8_slot = 0x01,.u8_card_address = 0x02},
//		{.u8_slot = 0x01,.u8_card_address = 0x03},
//		{.u8_slot = 0x02,.u8_card_address = 0x04},
//		{.u8_slot = 0x02,.u8_card_address = 0x05},
//		{.u8_slot = 0x02,.u8_card_address = 0x06},
//		{.u8_slot = 0x03,.u8_card_address = 0x07},
//		{.u8_slot = 0x03,.u8_card_address = 0x08},
//		{.u8_slot = 0x03,.u8_card_address = 0x09},
//		{.u8_slot = 0x04,.u8_card_address = 0x0A},
//		{.u8_slot = 0x04,.u8_card_address = 0x0B},
//};

card_info cards_t[11];

int 	sockfd, portno, rec_fd;
struct 	sockaddr_in serv_addr;

test_parameters st_test_param;

uint8_t u8_req_configured = e_NO_TEST;
uint8_t u8_test_configured = e_NO_TEST;
uint8_t u8_opto_check = 0;

uint16_t u16_analog_chn = 0;
uint16_t u16_travel_chn = 0;
uint16_t u16_coil_curr_chn = 0;

/**************************** FUNCTION PROTOTYPES *******************/
uint16_t frame_respone_validate(uint16_t *p_frame,uint32_t u32_len);
void frame_req_slot_1(frame_info *pt_frame,uint16_t u16_error);
void frame_req_slot_2(frame_info *pt_frame,uint16_t u16_error);
void frame_req_slot_3(frame_info *pt_frame,uint16_t u16_error);
void frame_req_slot_4(frame_info *pt_frame,uint16_t u16_error);
uint16_t farme_checksum(uint16_t* data_frame,uint16_t length_of_frame);
void system_configuration();
int config_hardware(uint8_t u8_slot,uint8_t u8_card_address);
void dummy_hardware_config();

int opto_wait();

/*---------------------- EXTRA Information -------------------------
 * 	DB %d Handshake Response Read count Error  			: DB %d HSR Read count Error
 *	DB %d Handshake Response Frame Validation Error		: DB %d HSR Frame Validation Error
 *	DB %d Handshake Response DB Response Error			: DB %d HSR DB Response Error

 *	DB %d Test Parameter Response Read count Error			: DB %d TPR Read count Error
 *	DB %d Test Parameter Response Frame Validation Error	: DB %d TPR Frame Validation Error
 *	DB %d Test Parameter Response DB Response Error			: DB %d TPR DB Response Error

 *	DB %d Data Response Read count Error 			: DB %d DR Read count Error
 *	DB %d Data Response Frame Validation Error		: DB %d DR Frame Validation Error
 *	DB %d Data Response DB Response Error			: DB %d DR DB Response Error
 *
 ---------------------------------------------------------------------*/



/**************************** FUNCTIONS *****************************/
void socket_init(const char* hostname, const char* port)
{
	int flag;
	int option_value;

	portno = atoi(port);
	// create a new socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// check is socket was successfuly created
	if (sockfd < 0)
	{
		perror("socket :");
		exit(1);
	}

	memset((char *) &serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET; // set address IPv4 or IPv6

	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	// convert char hostname to binary form
	if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0)
	{
		perror("HOST_NULL :");
		exit(1);
		//return HOST_NULL;
	}
	serv_addr.sin_port = htons(portno); // swap the endianness of an int

	flag = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR , &option_value,sizeof(option_value));
	if(flag < 0)
	{
		perror("option set failed\r\n");
		exit(1);
	}

	flag = bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
	if(flag < 0)
	{
		perror("Error in bind\r\n");
		exit(1);
	}

	printf("Socket init\r\n");
}

void *read_socket()
{
	/* 	This thread should be blocked until any command shall be received
	 * 	from the socket. upon receiving command from socket, thread should
	 * 	decode the command and unlock the desired "semaphore" and the
	 * 	respective thread for particular operation.
	 *
	 * 	For above operation the socket should be created and should be open
	 * 	to receive data from client.
	 */
	int 		flag;
	uint16_t 	u16_sock_rx[6];
	uint16_t 	u16_sock_tx[6];

	u16_sock_tx[0] = 0xBBBB;
	u16_sock_tx[5] = 0x5555;
	int 		i_read_count;
	uint16_t 	u16_state;

	static uint8_t u8_first_hardware_config = 1;

	static uint8_t u8_valid_test = 0;
	//printf("read socket thread %d\r\n",gettid());

	socket_init(SOC_SERVER_IP,SOC_PORT);

	flag = listen(sockfd, 3);
	if(flag < 0)
	{
		/* 	if there is error in "socket listen" system call then
		 * 	this thread will go in infinite while loop
		 * 	and application should be restarted,
		 *
		 * 	check how can be listen for connection with specific time interval
		 * 	on the socket
		 */
		perror("Socket listen :");
		fflush(stdout);
//		while(1);
		exit(1);
	}
	printf("SOC : waiting for connection\r\n");
	fflush(stdout);

	flag = fcntl(sockfd, F_GETFL);
	if (fcntl(sockfd, F_SETFL, flag | FASYNC /*| O_NONBLOCK*/) == -1)
		perror("fcntl(F_SETFL)");

//	printf("node server started\r\n");
//system("node /home/root/new_files/server_03_01/server/app.js &");
//		/home/root/new_files/server/server
//		/home/root/new_files/server_03_01/server

	printf("waiting for socket connection\r\n");
//	fflush(stdout);
	sleep(3);

		socklen_t st_addr_size = sizeof(struct sockaddr_in);


	rec_fd = accept(sockfd, (struct sockaddr*)&serv_addr, &st_addr_size);
	if(rec_fd > 0)
	{
		printf("accept(): OK\n");
		fflush(stdout);

		/*	TODO :below is testing code for testing communication between Node server
		 * 	and C code, remove it after testing is complete
		 */
//		for(int count = 0 ; count < 100 ; count++)
//		{
//			u16_sock_tx[0] = count;
//			u16_sock_tx[1] = count;
//			u16_sock_tx[2] = count;
//			u16_sock_tx[3] = count;
//			u16_sock_tx[4] = count;
//			u16_sock_tx[5] = count;
//			write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//			sleep(1);
//		}
	}
	else
	{
		printf("SOC : Connection Error\r\n");
		perror("Error : ");
//		fflush(stdout);
	}

	printf("after accept system call\r\n");
//	fflush(stdout);
	//sleep(1);
	while(1)
	{

		if(u8_valid_test == 1 || u8_first_hardware_config == 1)
		{
			if (u8_first_hardware_config == 1)
			{
				u8_req_configured = e_REQ_HANDSHAKE;
				u8_test_configured = e_HARDWARE_CONFIG;
//				system_configuration();

				/*	TODO : below function is added for "Elecrama purpose"
				 * 			remove it after the event is over
				 */
				dummy_hardware_config();

			}
			u8_first_hardware_config = 0;
//
////			sem_post(sem_Slot_1);
//		//sem_post(sem_Slot_2);
////			sem_post(sem_Slot_3);
			sem_post(sem_Slot_4);
//		sem_post(sem_mb_dcrm);
//		//sched_yield();
		}

		i_read_count = read(rec_fd,u16_sock_rx,sizeof(u16_sock_rx));
		//printf("SOC : read_count %d\r\n",i_read_count);

		if(i_read_count > 0)
		{
			/*	take action on the various ID's received in the socket
			 * 	frame and unblock the threads
			 */
						for(int i = 0 ; i < 6 ; i++)
						{
							printf("u16_sock_rx[%d] = %X\r\n",i,u16_sock_rx[i]);
//							printf("u16_sock_rx[%d] = %d\r\n",i,u16_sock_rx[i]);
							fflush(stdout);
						}
						printf("\r\n");
						fflush(stdout);
			//
						u16_state = u16_sock_rx[1];

						printf("test configured %x\r\n",u16_state);
						fflush(stdout);
			////			u8_socket_frame_decode(u8_soc_frame);
			//
			/* 	Below code is for testing the

//			/*	Currently for testing purpose we will use only one
//			 * 	slot for communication with only one semaphore -> lock and unlock
//			 */
//			usleep(1000);
//						database_fetch_system_settings();
//						printf("data base 1\r\n");
//						database_fetch_coil_current_ch_info();
//						printf("data base 2\r\n");
//						database_fetch_travel_ch_info();
//						printf("data base 3\r\n");
//						database_fetch_analog_ch_info();
//						printf("data base 4\r\n");
			//			while(1);
						switch(u16_state)
						{
						case e_SCRM :
							printf("Test : e_SCRM\r\n");
							u16_sock_tx[1] = e_SCRM;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_SCRM;
							break;

						case e_TIMING_MAIN :
							printf("Test : e_TIMING_MAIN\r\n");
							u16_sock_tx[1] = e_TIMING_MAIN;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_MAIN;
							break;

						case e_TIMING_AUXILIARY :
							printf("Test : e_TIMING_AUXILIARY\r\n");
							u16_sock_tx[1] = e_TIMING_AUXILIARY;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_AUXILIARY;
							break;

						case e_TIMING_PIR :
							printf("Test : e_TIMING_PIR\r\n");
							u16_sock_tx[1] = e_TIMING_PIR;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_PIR;
							break;

						case e_SIMPLE_OPEN :
							printf("Test : e_SIMPLE_OPEN\r\n");
							fflush(stdout);
							u16_sock_tx[1] = e_SIMPLE_OPEN;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							sleep(2);
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_SIMPLE_OPEN;

							break;

						case e_SIMPLE_CLOSE :
							printf("Test : e_SIMPLE_CLOSE\r\n");
							fflush(stdout);
							sleep(5);
							u16_sock_tx[1] = e_SIMPLE_CLOSE;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_SIMPLE_CLOSE;

							break;

						case e_DCRM_OPEN :
							printf("Test : e_DCRM_OPEN\r\n");
							u16_sock_tx[1] = e_DCRM_OPEN;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_DCRM_OPEN;

							break;

						case e_DCRM_CO :
							printf("Test : e_DCRM_CO\r\n");
							u16_sock_tx[1] = e_DCRM_CO;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_DCRM_CO;

							break;

						case e_TIMING_OPEN :
							printf("Test : e_TIMING_OPEN\r\n");
							u16_sock_tx[1] = e_TIMING_OPEN;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_OPEN;

							break;

						case e_TIMING_CLOSE :
							printf("Test : e_TIMING_CLOSE\r\n");
							u16_sock_tx[1] = e_TIMING_CLOSE;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_CLOSE;

							break;

						case e_TIMING_CO :
							printf("Test : e_TIMING_CO\r\n");
							u16_sock_tx[1] = e_TIMING_CO;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_CO;

							break;

						case e_TIMING_OC :
							printf("Test : e_TIMING_OC\r\n");
							u16_sock_tx[1] = e_TIMING_OC;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_OC;

							break;

						case e_TIMING_OCO :
							printf("Test : e_TIMING_OCO\r\n");
							u16_sock_tx[1] = e_TIMING_OCO;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_OCO;

							break;

						case e_TIMING_O_CO_CO :
							printf("Test : e_TIMING_O_CO_CO\r\n");
							u16_sock_tx[1] = e_TIMING_O_CO_CO;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_TIMING_O_CO_CO;

							break;

						case e_FIRST_TRIP :
							printf("Test : e_FIRST_TRIP\r\n");
							u16_sock_tx[1] = e_FIRST_TRIP;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_FIRST_TRIP;

							break;

						case e_MOTOR_CURRENT :
							printf("Test : e_MOTOR_CURRENT\r\n");
							u16_sock_tx[1] = e_MOTOR_CURRENT;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_TEST_SETTINGS;
							u8_test_configured = e_MOTOR_CURRENT;

							break;

						case e_HARDWARE_CONFIG :
							printf("Test : e_HARDWARE_CONFIG\r\n");
							u16_sock_tx[1] = e_HARDWARE_CONFIG;
							u16_sock_tx[2] = 0xA5A5;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 1;
							u8_req_configured = e_REQ_HANDSHAKE;
							u8_test_configured = e_HARDWARE_CONFIG;

							/*	TODO : below function is added for "Elecrama purpose"
							 * 			remove it after the event is over
							 */
							dummy_hardware_config();


							break;

						default :
							printf("Wrong Test ID\r\n");
							fflush(stdout);
							u16_sock_tx[1] = u16_state;
							u16_sock_tx[2] = 0x5A5A;
							write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
							u8_valid_test = 0;
							break;

						}
//			printf("SOC : command received\r\n");

		}
	}
}


/*------------------------- Thread for communication with Slot 1 ------------------------------*/

/* 	below code is for testing the module coding method with structures for
 * 	for every Activity in the Communication between CPU board and Daughter board.
 * 	this is testing code, if successful then implement in the Production code
 * 	if not successful then use above code for production code.
 */

void *communication_slot_1()
{
	/* 	Cards --> 1,2,3 */
	uint8_t 	u8_data_resp[4004] = {0};
	uint8_t 	u8_c_buff[MAX_LEN] = {0};
	int 		i_read_count,i_ret = 0;
	int32_t 	*p32_data = NULL;
	int32_t 	*p32_data_shm[4] ;
	int32_t 	i32_data = 0;

	uint8_t 	u8_state = e_REQ_DATA;
	uint8_t		u8_ret = 0,u8_req_count = 0;
	uint16_t 	u16_iteration = 0,u16_max_iter = 0;
	uint16_t 	u16_boards 	= 0;

	uint32_t 	u32_count = 0;

	FILE *log_slot_1;

	int i_sem_ret;

	log_slot_1 = fopen("log_slot_1.txt","w+");
	if(log_slot_1 == NULL)
	{
		perror("");
		exit(1);
	}

	printf("communication_slot_1 t thread\r\n");

	while(1)
	{

		printf("waiting for sem 1\r\n");
		i_sem_ret = sem_wait(sem_Slot_1);
		printf("semphore 1 success\r\n");

		/* 	There are 2 types of tests
		 * 	1. Hardware Config
		 * 	2. User Tests
		 */
		u8_state = u8_req_configured;

		switch(u8_state)
		{
		//		custom_uart_fifo_reset(p32_axi_uart_1);
		case e_REQ_HANDSHAKE:
			gpio_rs485_tx_en();
			usleep(5);

			/* 	below code is to make the handshake requests modular for 3 Daughter boards
			 * 	otherwise code will be duplicated for 3 boards and number of lines will
			 * 	increase.
			 */
			u8_req_count = 0;
			for(u16_iteration = 1 ; u16_iteration <= 3 ; )
			{
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= u16_iteration;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.u16_footer			= 0x5555;
				printf("Req Handshake\r\n");
				fprintf(log_slot_1,"DB %d HSR\r\n",u16_iteration);

				custom_uart_fifo_reset(p32_axi_uart_1);
				frame_req_slot_1(&frame_t,i_ret);
				memset(u8_c_buff,0,MAX_LEN);
				usleep(60000);

				i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),56);
				printf("rd %d\r\n",i_read_count);
				fprintf(log_slot_1,"rd %d\r\n",i_read_count);

				/*	comment the below line of after testing will be complete */
//				i_read_count = 2;
				if(i_read_count > 0)
				{
					uint16_t *p16_d_1 = (u8_c_buff + 1);
//					for(u16_count = 0 ; u16_count < 28 ; u16_count++)
//					{
//						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
//					}
					u8_ret = frame_respone_validate((p16_d_1),i_read_count);
					if(u8_ret == 0)
					{
						if(p16_d_1[2] == 0x5C5C)
						{
//							u8_state = e_REQ_TEST_SETTINGS;

							/*	TODO : send message over socket for present data sampling channels
							 * 			present in the particular Daughter Board
							 */

							printf("Handshake Response\r\n");
							fprintf(log_slot_1,"DB %d HSR Success\r\n",u16_iteration);
							set_channels(p16_d_1[5]);
							u16_iteration++;
						}
						else
						{
							printf("Response Error %x\r\n",p16_d_1[5]);
							fprintf(log_slot_1,"DB %d HSR DB Response Error\r\n",u16_iteration);
						}
					}
					else
					{
						/* 	set the flag to send the request again to the
						 * 	corresponding Daughter board with current activity
						 */
						u8_req_count++;
//						u8_state = e_REQ_HANDSHAKE;
						printf("DB %d\r\n",u16_iteration);
						fprintf(log_slot_1,"DB %d HSR Frame Validation Error %d\r\n",u16_iteration,u8_ret);
						if(u8_req_count > 2)
						{
							/*	if consecutive request count exceeds 3 iterations then
							 * 	send the message to High level software that particular
							 * 	channels of Corresponding Daughter Boards are not
							 * 	responding to Handshake request and give message to user
							 */
							printf("Error in Communication with %d\r\n",u16_iteration);

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_iteration++;
							u8_req_count = 0;
						}

					}
				}
				else
				{
					u8_req_count++;
					fprintf(log_slot_1,"DB %d HSR Data Read Error %d\r\n",u16_iteration,i_read_count);
					if(u8_req_count > 2)
					{
						/*	if consecutive request count exceeds 3 iterations then
						 * 	send the message to High level software that particular
						 * 	channels of Corresponding Daughter Boards are not
						 * 	responding to Handshake request and give message to user
						 */
						printf("Error in Communication with %d\r\n",u16_iteration);
						/*	send the Error response of activity on the socket */
						sem_wait(sem_write_socket);
						socket_resp(u8_test_configured,ERROR);
						sem_post(sem_write_socket);

						u16_iteration++;
						u8_req_count = 0;
					}
				}
			}
			//			printf("infinite loop\r\n");
			//			while(1);
			//sleep(4);
			break;
		case e_REQ_TEST_SETTINGS :
			//printf("th_1 e_REQ_CONFIG\r\n");
			gpio_rs485_tx_en();
			usleep(5);
			u8_req_count = 0;
			for(u16_boards = 0 ; u16_boards < 3 ; /*u16_boards++*/)
			{
				/*	Daughter Board 1 send command and receive response */
				if(cards_t[u16_boards].u8_test == e_SELECTED)
				{
					frame_t.u16_header			= 0xAAAA;
					frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
					frame_t.u16_activity		= 0x0002;
					frame_t.u16_identifier		= 0x5B5B;
					frame_t.u16_payload			= 22;
					frame_t.u16_footer			= 0x5555;

					/*	Make provision to copy the Payload data using the structure copy
					 * 	as "cards_t" structure will have the information regarding the parameters
					 * 	that will be sent to Daughter board
					 */
					frame_t.p16_data[2]			= 0xAABB;
					frame_t.p16_data[3]			= 0xAACC;

					// 	Below code is for testing the correct configuration of
					//	of the board and TX an RX are working properly with RS485
//					while(1)
//					{
						printf("DB %d Test Param Req\r\n",u16_boards);
						fprintf(log_slot_1,"DB %d TPR\r\n",u16_boards);

						custom_uart_fifo_reset(p32_axi_uart_1);
						frame_req_slot_1(&frame_t,i_ret);
						memset(u8_c_buff,0,MAX_LEN);
//					}
					usleep(60000);

					i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),56);
					printf("rd %d\r\n",i_read_count);
					fprintf(log_slot_1,"rd %d\r\n",i_read_count);

					/*	purposely making "i_read_count" greater than "0" for testing after
					 * 	testing remove the below line : HT
					 */
					//i_read_count = 2;
					if(i_read_count > 0)
					{
						uint16_t *p16_d_1 = (u8_c_buff + 1);

						//						for(u16_count = 0 ; u16_count < 28 ; u16_count++)
						//						{
						//							printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
						//						}

						u8_ret = frame_respone_validate((p16_d_1),i_read_count);
						if(u8_ret == 0)
						{
							if(p16_d_1[2] == 0x5C5C)
							{
//								for(int i = 0; i < i_read_count/2 ; i++)
//								{
//									fprintf(ptr_slot_1_config,"%d,",p16_d_1[i]);
//								}
//								fprintf(ptr_slot_1_config,"\r\n");
								fprintf(log_slot_1,"DB %d TPR Success\r\n",u16_boards);
								u16_boards++;
							}
							else
							{
								printf("Response Error %x\r\n",p16_d_1[5]);
								fprintf(log_slot_1,"DB %d TPR DB Response Error\r\n",u16_boards);
							}
						}
						else
						{
							/* 	set the flag to send the request again to the
							 * 	corresponding Daughter board with current activity
							 */
							u8_req_count++;
							printf("fame validation failed with Errors %x\r\n",u8_ret);
							fprintf(log_slot_1,"DB %d TPR Frame Validation Error %d\r\n",u16_boards,u8_ret);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								sem_wait(sem_write_socket);
								socket_resp(u8_test_configured,ERROR);
								sem_post(sem_write_socket);

								u16_boards++;
								u8_req_count = 0;
							}
						}
					}
					else
					{
						u8_req_count++;
						printf("data read error\r\n");
						fprintf(log_slot_1,"DB %d TPR Data Read Error %d\r\n",u16_boards,i_read_count);
						if(u8_req_count > 2)
						{
							cards_t[u16_boards].u8_ack_error = e_ERR;

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_boards++;
							u8_req_count = 0;
						}
					}
				}
				else
				{
					u16_boards++;
				}
			}


			printf("Test requests complete\r\n");
			system("sync");
//			fclose(ptr_slot_1_config);
//			fclose(ptr_slot_2_config);
//			fclose(ptr_slot_3_config);
			//u8_state = e_REQ_DATA;

			//			printf("infinite loop after Test setting\r\n");
			//			while(1);

			//			printf("thread 1 : infinite loop\r\n");
			//			while(1);
			//break;

//			sleep(1);

		case e_OPTO_WAIT :

			printf("slot_1 e_OPTO_WAIT\r\n");
//			sem_wait(sem_opto_wait);
			if(u8_opto_check == 0)
			{
				opto_wait();
			}
			else
			{

			}
//			sem_post(sem_opto_wait);

		case e_REQ_DATA :
			gpio_rs485_tx_en();
			usleep(50);
			/*	Create Data file here to store the complete data of 1.2 MB data of 4 channels.
			 * 	Need to create logic for storing separate files for each channels.
			 *
			 * 	Create the 12 Data shared objects according to Daughter boards configured
			 * 	and switch between them according to board numbers
			 *
			 * 	The third board will create issue for creating the shared objects as it
			 * 	will be special board which will act as tunnel between CPU board and other
			 * 	application boards such as MB Unit, DCRM Boards
			 */

			/*	check the acknowledgement response from the data structure and then send the
			 * 	data request to corresponding boards. if there is error to "test parameter config"
			 * 	request then don't send the Data request to that particular board
			 */

			// 	Below code is for testing purpose comment it after testing is complete
//			cards_t[0].u8_ack_error = e_ACK;
//			cards_t[1].u8_ack_error = e_ACK;
//			cards_t[2].u8_ack_error = e_ACK;

			cards_t[0].u8_board = e_Analog_Baord;
			cards_t[1].u8_board = e_Analog_Baord;
			cards_t[2].u8_board = e_Analog_Baord;



			//	Above testing code Ends here

			u8_req_count = 0;

			printf("Data request Starts\r\n");

			for(u16_boards = 0 ; u16_boards < 3 ;)
			{
//				if(u16_boards == 1)
//				{
//					fclose(log);
//					system("sync");
//					printf("pointer log file closed\r\n");
//					printf("exiting application\r\n");
//					exit(1);
//				}
				if(cards_t[u16_boards].u8_ack_error == e_ERR)
				{
					printf("Board %d has Comm Error\r\n",u16_boards);
					fprintf(log_slot_1,"DB %d has Comm Error\r\n",u16_boards);
					u16_boards++;
				}
				else
				{
					/* 	send the data request to board which has received correct
					 * 	test parameters of the current test selected by user
					 */
					p32_data_shm[0] = cards_t[u16_boards].p_data_files[0];
//					printf("p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);
					fprintf(log_slot_1,"p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);


					p32_data_shm[1] = cards_t[u16_boards].p_data_files[1];
//					printf("p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);
					fprintf(log_slot_1,"p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);


					p32_data_shm[2] = cards_t[u16_boards].p_data_files[2];
//					printf("p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);
					fprintf(log_slot_1,"p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);


					p32_data_shm[3] = cards_t[u16_boards].p_data_files[3];
//					printf("p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);
					fprintf(log_slot_1,"p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);


					if(cards_t[u16_boards].u8_board == e_Digital_Board)
					{
						u16_max_iter = MAX_DI_ITERATIONS;
					}
					else if(cards_t[u16_boards].u8_board == e_Analog_Baord)
					{
						u16_max_iter = MAX_AI_ITERATIONS;
					}

					for(u16_iteration = 1 ; u16_iteration <= u16_max_iter; /*u6_iteration++*/)
					{
//						sleep(1);
						//printf("DB : send CMD\r\n");
						frame_t.u16_header			= 0xAAAA;
						frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
						frame_t.u16_activity		= 0x0003;
						frame_t.u16_identifier		= 0x5B5B;
						frame_t.u16_payload			= 22;
						frame_t.p16_data[0]			= u16_iteration;
						frame_t.u16_footer			= 0x5555;

						/*	logic implemented for detecting the channels : specifically for
						 * 	Analog channels but it can be implemented for Daughter board also
						 */
						if(u16_iteration <= CHANNEL_1_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_1;
						}
						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_2;
						}
						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_3;
						}
						else
						{
							frame_t.p16_data[1]		= CHANNEL_4;
						}

						custom_uart_fifo_reset(p32_axi_uart_1);
						memset(u8_data_resp,0,4004);

						fprintf(log_slot_1,"DB %d Req %d iter\r\n",u16_boards,u16_iteration);
						frame_req_slot_1(&frame_t,0);

						usleep(8000);

						i_read_count = custom_uart_1_read((uint8_t*)(u8_data_resp),MAX_LEN);
						fprintf(log_slot_1,"rd %d\r\n",i_read_count);
						//						printf("rd %d\r\n",i_read_count);
						uint16_t *p16_d = u8_data_resp + 1;

						/*	TODO: below code of "if" and "else" statements is for testing purpose
						 * 	comment it after testing is complete : HT
						 */
//						if(u16_iteration <= CHANNEL_1_ITERATIONS)
//						{
//							p16_d[6] = 1;
//						}
//						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
//						{
//							p16_d[6] = 2;
//						}
//						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
//						{
//							p16_d[6] = 3;
//						}
//						else
//						{
//							p16_d[6] = 4;
//						}

//						for(int i = 0 ; i < 100 ; i++)
//						{
//							printf("%d = %x\r\n",i,p16_d[i]);
//						}
//						printf("\r\n");
//						printf("rd %d\r\n",i_read_count);

//						uint16_t *p16_d = u8_data_resp + 1;
//						while(1);

						//	TODO: Below line is added for testing purpose comment it after
						// 	After testing is complete
//						i_read_count = 2;

						if(i_read_count > 0)
						{
							//printf("DB Resp frame validate\r\n");
							u8_ret = frame_respone_validate((p16_d),i_read_count);

							//	TODO: Below line is added for testing purpose comment it after
							// 	After testing is complete
//							u8_ret = 0;

							if(u8_ret == 0)
							{
								/*	check which channel data is arrived in Uart buffer and store that
								 * 	data into respective data storing files
								 */
								//ptr_data = ptr_ch_1;
//								printf("DB Resp Data fill\r\n");
//								p16_d[6] = 1
								if(p16_d[6] == 1)
								{
									p32_data = p32_data_shm[0];
//									printf("p32_data_shm[0] %p\r\n",p32_data_shm[0]);
									fprintf(log_slot_1,"p32_data_shm[0] %p\r\n",p32_data_shm[0]);

								}
								else if(p16_d[6] == 2)
								{
									p32_data = p32_data_shm[1];
//									printf("p32_data_shm[1] %p\r\n",p32_data_shm[1]);
									fprintf(log_slot_1,"p32_data_shm[1] %p\r\n",p32_data_shm[1]);


								}
								else if(p16_d[6] == 3)
								{
									p32_data = p32_data_shm[2];
//									printf("p32_data_shm[2] %p\r\n",p32_data_shm[2]);
									fprintf(log_slot_1,"p32_data_shm[2] %p\r\n",p32_data_shm[2]);

								}
								else if(p16_d[6] == 4)
								{
									p32_data = p32_data_shm[3];
//									printf("p32_data_shm[3] %p\r\n",p32_data_shm[3]);
									fprintf(log_slot_1,"p32_data_shm[3] %p\r\n",p32_data_shm[3]);


								}

								//	TODO : Need to implement the logic if no Error response received
								//  		while receiving the complete 76 iterations of one channel
								//			then send the response of complete data of one channel
								//			received to UI using the socket communications
								//			Check the method to check if Error generated for any iteration.

								//printf("DB file pointers attached\r\n");
								//  change the pointer address to point to actual data by adding the
								// 	the additional 6 bytes address

								uint8_t *p8_data = (uint8_t *)(u8_data_resp + 15);
								// 	Below function is for validation of Data reception and it's filling in
								// 	the files
								// 	For Actual Data storage section we need to put data into the shared objects
								// 	using the integer pointer and increment that integer pointer
								// 	Only the data section except Header, Footer and other attributes will be stored in
								//	the Data file so increment the pointer accordingly
								// 	Check how many data samples will be received in one Data frame.
								//	Total Data bytes = 3982
								// 	Total Data samples for Analog Board : 1 Samples = 3 bytes : 3982/3 = 1327
								// 	until now the testing was done with 2 bytes data but in actual scenario we need
								// 	to test the data filling with 3 bytes for both Analog and Digital boards and
								// 	According to that the logic should be implemented

//								printf("local pointer %p\r\n",p32_data);
								fprintf(log_slot_1,"local pointer %p\r\n",p32_data);

								for(u32_count = 0 ;u32_count < /*3981*/3981 ; u32_count = u32_count + 3 /*2*/)
								{
									i32_data = 0;

									i32_data = 	(p8_data[u32_count + 2] << 16) |
												(p8_data[u32_count + 1] << 8)|
												p8_data[u32_count];
									*(uint32_t *)p32_data = i32_data;
									p32_data++;
								}

//								printf("DB frame fill complete\r\n");
								//	Increment the shared object pointer after writing the 3981 bytes data
								// 	and update the particular pointer
								p32_data_shm[p16_d[6]-1] = p32_data;

//								printf("p32_data %p\r\n",p32_data);
								fprintf(log_slot_1,"p32_data %p\r\n",p32_data);

								//
								//						fseek(ptr_data,0,SEEK_CUR);
//								for(uint16_t i = 0 ; i < /*(u16_frame_size/1)*/ (i_read_count/2); /*i = i+4*/i++)
//								{
//									fprintf(ptr_data,"%d,",p16_d[i]);
//									//usleep(100);
//								}
								//fprintf(ptr_data,"\r\n");

								if(u16_iteration == CHANNEL_1_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_1_ITERATIONS)
									{
										/*	if the iteration reach the 76 then first channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	first channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[0]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_2_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_2_ITERATIONS)
									{
										/*	if the iteration reach the 152 then Second channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	Second channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[1]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_3_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_3_ITERATIONS)
									{
										/*	if the iteration reach the 228 then Third channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	Third channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[2]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_4_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_4_ITERATIONS)
									{
										/*	if the iteration reach the u16_max_iter then fourth channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	fourth channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[3]);
										sem_post(sem_write_socket);
									}
								}

								u16_iteration++;
							}
							else
							{
								/*	If Any Error Occurs then send the Same Data request again
								 * 	if the same Data request is send for consecutive 3 times then
								 * 	give Error that Data request Error to High level software
								 * 	and stop data fetching process
								 */
								printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
								fprintf(log_slot_1,"DB %d DR Frame Validation Error\r\n",u16_boards);

								u8_req_count++;
								printf("frame Err %x\r\n",u8_ret);
								if(u8_req_count > 2)
								{
									cards_t[u16_boards].u8_ack_error = e_ERR;

									/*	send the Error response of activity on the socket */
									sem_wait(sem_write_socket);
									socket_resp(e_DATA_REC_ERROR,ERROR);
									sem_post(sem_write_socket);

//									u16_sock_tx[1] = 0x0003;
//
//									/*	setting the Error bytes in the socket request */
//									u16_sock_tx[2] = 0x5A5A;
//
//									write(rec_fd,u16_sock_tx,sizeof(u16_sock_tx));

									/*	when any of the data request doens't receive data for 3 consecutive
									 * 	iterations then break the current for loop of data fetching of current board
									 * 	and go to next Daughter Card for data fetch activity
									 */
									u8_req_count = 0;
									break;

									u16_iteration++;

								}
								u8_ret = 0;
							}
						}
						else
						{
							u8_req_count++;
							printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
//							printf("data rd err\r\n");
							fprintf(log_slot_1,"DB %d DR Read Count Error\r\n",u16_boards);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								/*	send the Error response of activity on the socket */
								sem_wait(sem_write_socket);
								socket_resp(e_DATA_REC_ERROR,ERROR);
								sem_post(sem_write_socket);

//								u16_sock_tx[1] = 0x0003;
//
//								/*	setting the Error bytes in the socket request */
//								u16_sock_tx[2] = 0x5A5A;
//
//								write(rec_fd,u16_sock_tx,sizeof(u16_sock_tx));

								/*	when any of the data request doens't receive data for 3 consecutive
								 * 	iterations then break the current for loop of data fetching of current board
								 * 	and go to next Daughter Card for data fetch activity
								 */
								u8_req_count = 0;
								break;

								u16_iteration++;

							}
						}
					}
					u16_boards++;
				}
			}
			fclose(log_slot_1);
			system("sync");
			if(		(cards_t[DB_1].u8_ack_error == e_ERR)
					& (cards_t[DB_2].u8_ack_error == e_ERR)
					& (cards_t[DB_3].u8_ack_error == e_ERR) )
			{
				printf("data sampling failed due to Errors\r\n");
			}
			else
			{
				printf("slot 1 sampling complete\r\n");
			}

//			exit(1);
//			fclose(ptr_ch_1);
//			fclose(ptr_ch_2);
//			fclose(ptr_ch_3);
//			fclose(ptr_ch_4);
//			//fclose(err_ptr);
//			printf("file closed\r\n");
			//while(1);
			break;
		}

		//sem_post(sem_Slot_1);
	}
}

/*------------------------- Communication Code with Slot 1 Ends -------------------------------*/

/* --------------------------------------------------------------------------------------------*/

/*------------------------- Thread for communication with Slot 2 ------------------------------*/

void *communication_slot_2()
{
	/* 	Cards --> 1,2,3 */
	uint8_t 	u8_data_resp[4004] = {0};
	uint8_t 	u8_c_buff[MAX_LEN] = {0};
	int 		i_read_count,i_ret = 0;
	int32_t 	*p32_data = NULL;
	int32_t 	*p32_data_shm[4] ;
	int32_t 	i32_data = 0;

	uint8_t 	u8_state = e_REQ_DATA;
	uint8_t		u8_ret = 0,u8_req_count = 0;
	uint16_t 	u16_iteration = 0,u16_max_iter = 0;
	uint16_t 	u16_boards 	= 0;

	uint32_t 	u32_count = 0;

	FILE *log_slot_2;

	int i_sem_ret;

	log_slot_2 = fopen("log_slot_2.txt","w+");
	if(log_slot_2 == NULL)
	{
		perror("");
		exit(1);
	}

	printf("communication_slot_2 t thread\r\n");

	while(1)
	{

		printf("waiting for sem 2\r\n");
		i_sem_ret = sem_wait(sem_Slot_2);
		printf("semphore 2 success\r\n");

		/* 	There are 2 types of tests
		 * 	1. Hardware Config
		 * 	2. User Tests
		 */
		u8_state = u8_req_configured;

		switch(u8_state)
		{
		//		custom_uart_fifo_reset(p32_axi_uart_1);
		case e_REQ_HANDSHAKE:
			gpio_rs485_tx_en();
			usleep(5);

			/* 	below code is to make the handshake requests modular for 3 Daughter boards
			 * 	otherwise code will be duplicated for 3 boards and number of lines will
			 * 	increase.
			 */
			u8_req_count = 0;
			for(u16_iteration = 1 ; u16_iteration <= 3 ; )
			{
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= u16_iteration;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.u16_footer			= 0x5555;
				printf("Req Handshake\r\n");
				fprintf(log_slot_2,"DB %d HSR\r\n",u16_iteration);

				custom_uart_fifo_reset(p32_axi_uart_2);
				frame_req_slot_2(&frame_t,i_ret);
				memset(u8_c_buff,0,MAX_LEN);
				usleep(60000);

				i_read_count = custom_uart_2_read((uint8_t*)(u8_c_buff),56);
				printf("rd %d\r\n",i_read_count);
				fprintf(log_slot_2,"rd %d\r\n",i_read_count);

				/*	comment the below line of after testing will be complete */
//				i_read_count = 2;
				if(i_read_count > 0)
				{
					uint16_t *p16_d_1 = (u8_c_buff + 1);
//					for(u16_count = 0 ; u16_count < 28 ; u16_count++)
//					{
//						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
//					}
					u8_ret = frame_respone_validate((p16_d_1),i_read_count);
					if(u8_ret == 0)
					{
						if(p16_d_1[2] == 0x5C5C)
						{
//							u8_state = e_REQ_TEST_SETTINGS;

							/*	TODO : send message over socket for present data sampling channels
							 * 			present in the particular Daughter Board
							 */

							printf("Handshake Response\r\n");
							fprintf(log_slot_2,"DB %d HSR Success\r\n",u16_iteration);
							set_channels(p16_d_1[5]);
							u16_iteration++;
						}
						else
						{
							printf("Response Error %x\r\n",p16_d_1[5]);
							fprintf(log_slot_2,"DB %d HSR DB Response Error\r\n",u16_iteration);
						}
					}
					else
					{
						/* 	set the flag to send the request again to the
						 * 	corresponding Daughter board with current activity
						 */
						u8_req_count++;
//						u8_state = e_REQ_HANDSHAKE;
						printf("DB %d\r\n",u16_iteration);
						fprintf(log_slot_2,"DB %d HSR Frame Validation Error %d\r\n",u16_iteration,u8_ret);
						if(u8_req_count > 2)
						{
							/*	if consecutive request count exceeds 3 iterations then
							 * 	send the message to High level software that particular
							 * 	channels of Corresponding Daughter Boards are not
							 * 	responding to Handshake request and give message to user
							 */
							printf("Error in Communication with %d\r\n",u16_iteration);

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_iteration++;
							u8_req_count = 0;
						}

					}
				}
				else
				{
					u8_req_count++;
					fprintf(log_slot_2,"DB %d HSR Data Read Error %d\r\n",u16_iteration,i_read_count);
					if(u8_req_count > 2)
					{
						/*	if consecutive request count exceeds 3 iterations then
						 * 	send the message to High level software that particular
						 * 	channels of Corresponding Daughter Boards are not
						 * 	responding to Handshake request and give message to user
						 */
						printf("Error in Communication with %d\r\n",u16_iteration);
						/*	send the Error response of activity on the socket */
						sem_wait(sem_write_socket);
						socket_resp(u8_test_configured,ERROR);
						sem_post(sem_write_socket);

						u16_iteration++;
						u8_req_count = 0;
					}
				}
			}

			break;
		case e_REQ_TEST_SETTINGS :
			//printf("th_1 e_REQ_CONFIG\r\n");
			gpio_rs485_tx_en();
			usleep(5);
			u8_req_count = 0;
			for(u16_boards = DB_4 ; u16_boards <= DB_6 ; /*u16_boards++*/)
			{
				/*	Daughter Board 1 send command and receive response */
				if(cards_t[u16_boards].u8_test == e_SELECTED)
				{
					frame_t.u16_header			= 0xAAAA;
					frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
					frame_t.u16_activity		= 0x0002;
					frame_t.u16_identifier		= 0x5B5B;
					frame_t.u16_payload			= 22;
					frame_t.u16_footer			= 0x5555;

					/*	Make provision to copy the Payload data using the structure copy
					 * 	as "cards_t" structure will have the information regarding the parameters
					 * 	that will be sent to Daughter board
					 */
					frame_t.p16_data[2]			= 0xAABB;
					frame_t.p16_data[3]			= 0xAACC;

					// 	Below code is for testing the correct configuration of
					//	of the board and TX an RX are working properly with RS485
//					while(1)
//					{
						printf("DB %d Test Param Req\r\n",u16_boards);
						fprintf(log_slot_2,"DB %d TPR\r\n",u16_boards);

						custom_uart_fifo_reset(p32_axi_uart_2);
						frame_req_slot_2(&frame_t,i_ret);
						memset(u8_c_buff,0,MAX_LEN);
//					}
					usleep(60000);

					i_read_count = custom_uart_2_read((uint8_t*)(u8_c_buff),56);
					printf("rd %d\r\n",i_read_count);
					fprintf(log_slot_2,"rd %d\r\n",i_read_count);

					/*	purposely making "i_read_count" greater than "0" for testing after
					 * 	testing remove the below line : HT
					 */
					//i_read_count = 2;
					if(i_read_count > 0)
					{
						uint16_t *p16_d_1 = (u8_c_buff + 1);

						//						for(u16_count = 0 ; u16_count < 28 ; u16_count++)
						//						{
						//							printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
						//						}

						u8_ret = frame_respone_validate((p16_d_1),i_read_count);
						if(u8_ret == 0)
						{
							if(p16_d_1[2] == 0x5C5C)
							{
//								for(int i = 0; i < i_read_count/2 ; i++)
//								{
//									fprintf(ptr_slot_1_config,"%d,",p16_d_1[i]);
//								}
//								fprintf(ptr_slot_1_config,"\r\n");
								fprintf(log_slot_2,"DB %d TPR Success\r\n",u16_boards);
								u16_boards++;
							}
							else
							{
								printf("Response Error %x\r\n",p16_d_1[5]);
								fprintf(log_slot_2,"DB %d TPR DB Response Error\r\n",u16_boards);
							}
						}
						else
						{
							/* 	set the flag to send the request again to the
							 * 	corresponding Daughter board with current activity
							 */
							u8_req_count++;
							printf("fame validation failed with Errors %x\r\n",u8_ret);
							fprintf(log_slot_2,"DB %d TPR Frame Validation Error %d\r\n",u16_boards,u8_ret);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								sem_wait(sem_write_socket);
								socket_resp(u8_test_configured,ERROR);
								sem_post(sem_write_socket);

								u16_boards++;
								u8_req_count = 0;
							}
						}
					}
					else
					{
						u8_req_count++;
						printf("data read error\r\n");
						fprintf(log_slot_2,"DB %d TPR Data Read Error %d\r\n",u16_boards,i_read_count);
						if(u8_req_count > 2)
						{
							cards_t[u16_boards].u8_ack_error = e_ERR;

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_boards++;
							u8_req_count = 0;
						}
					}
				}
				else
				{
					u16_boards++;
				}
			}


			printf("Test requests complete\r\n");
			system("sync");

		case e_OPTO_WAIT :

			printf("slot_2 e_OPTO_WAIT\r\n");
			sem_wait(sem_opto_wait);
			if(u8_opto_check == 0)
			{
				opto_wait();
			}
			else
			{

			}
			sem_post(sem_opto_wait);


		case e_REQ_DATA :
			gpio_rs485_tx_en();
			usleep(50);

			// 	Below code is for testing purpose comment it after testing is complete
//			cards_t[0].u8_ack_error = e_ACK;
//			cards_t[1].u8_ack_error = e_ACK;
//			cards_t[2].u8_ack_error = e_ACK;

			cards_t[0].u8_board = e_Analog_Baord;
			cards_t[1].u8_board = e_Analog_Baord;
			cards_t[2].u8_board = e_Analog_Baord;

			//	Above testing code Ends here

			u8_req_count = 0;

			printf("Data request Starts\r\n");

			for(u16_boards = DB_4 ; u16_boards <= DB_6 ;)
			{
//				if(u16_boards == 1)
//				{
//					fclose(log);
//					system("sync");
//					printf("pointer log file closed\r\n");
//					printf("exiting application\r\n");
//					exit(1);
//				}
				if(cards_t[u16_boards].u8_ack_error == e_ERR)
				{
					printf("Board %d has Comm Error\r\n",u16_boards);
					fprintf(log_slot_2,"DB %d has Comm Error\r\n",u16_boards);
					u16_boards++;
				}
				else
				{
					/* 	send the data request to board which has received correct
					 * 	test parameters of the current test selected by user
					 */
					p32_data_shm[0] = cards_t[u16_boards].p_data_files[0];
//					printf("p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);
					fprintf(log_slot_2,"p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);


					p32_data_shm[1] = cards_t[u16_boards].p_data_files[1];
//					printf("p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);
					fprintf(log_slot_2,"p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);


					p32_data_shm[2] = cards_t[u16_boards].p_data_files[2];
//					printf("p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);
					fprintf(log_slot_2,"p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);


					p32_data_shm[3] = cards_t[u16_boards].p_data_files[3];
//					printf("p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);
					fprintf(log_slot_2,"p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);


					if(cards_t[u16_boards].u8_board == e_Digital_Board)
					{
						u16_max_iter = MAX_DI_ITERATIONS;
					}
					else if(cards_t[u16_boards].u8_board == e_Analog_Baord)
					{
						u16_max_iter = MAX_AI_ITERATIONS;
					}

					for(u16_iteration = 1 ; u16_iteration <= u16_max_iter; /*u6_iteration++*/)
					{
//						sleep(1);
						//printf("DB : send CMD\r\n");
						frame_t.u16_header			= 0xAAAA;
						frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
						frame_t.u16_activity		= 0x0003;
						frame_t.u16_identifier		= 0x5B5B;
						frame_t.u16_payload			= 22;
						frame_t.p16_data[0]			= u16_iteration;
						frame_t.u16_footer			= 0x5555;

						/*	logic implemented for detecting the channels : specifically for
						 * 	Analog channels but it can be implemented for Daughter board also
						 */
						if(u16_iteration <= CHANNEL_1_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_1;
						}
						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_2;
						}
						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_3;
						}
						else
						{
							frame_t.p16_data[1]		= CHANNEL_4;
						}

						custom_uart_fifo_reset(p32_axi_uart_2);
						memset(u8_data_resp,0,4004);

						fprintf(log_slot_2,"DB %d Req %d iter\r\n",u16_boards,u16_iteration);
						frame_req_slot_2(&frame_t,0);

						usleep(8000);

						i_read_count = custom_uart_2_read((uint8_t*)(u8_data_resp),MAX_LEN);
						fprintf(log_slot_2,"rd %d\r\n",i_read_count);
						//						printf("rd %d\r\n",i_read_count);
						uint16_t *p16_d = u8_data_resp + 1;

						/*	TODO: below code of "if" and "else" statements is for testing purpose
						 * 	comment it after testing is complete : HT
						 */
//						if(u16_iteration <= CHANNEL_1_ITERATIONS)
//						{
//							p16_d[6] = 1;
//						}
//						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
//						{
//							p16_d[6] = 2;
//						}
//						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
//						{
//							p16_d[6] = 3;
//						}
//						else
//						{
//							p16_d[6] = 4;
//						}

//						for(int i = 0 ; i < 100 ; i++)
//						{
//							printf("%d = %x\r\n",i,p16_d[i]);
//						}
//						printf("\r\n");
//						printf("rd %d\r\n",i_read_count);

//						uint16_t *p16_d = u8_data_resp + 1;
//						while(1);

						//	TODO: Below line is added for testing purpose comment it after
						// 	After testing is complete
//						i_read_count = 2;

						if(i_read_count > 0)
						{
							//printf("DB Resp frame validate\r\n");
							u8_ret = frame_respone_validate((p16_d),i_read_count);

							//	TODO: Below line is added for testing purpose comment it after
							// 	After testing is complete
//							u8_ret = 0;

							if(u8_ret == 0)
							{
								/*	check which channel data is arrived in Uart buffer and store that
								 * 	data into respective data storing files
								 */
								//ptr_data = ptr_ch_1;
//								printf("DB Resp Data fill\r\n");
//								p16_d[6] = 1
								if(p16_d[6] == 1)
								{
									p32_data = p32_data_shm[0];
//									printf("p32_data_shm[0] %p\r\n",p32_data_shm[0]);
									fprintf(log_slot_2,"p32_data_shm[0] %p\r\n",p32_data_shm[0]);

								}
								else if(p16_d[6] == 2)
								{
									p32_data = p32_data_shm[1];
//									printf("p32_data_shm[1] %p\r\n",p32_data_shm[1]);
									fprintf(log_slot_2,"p32_data_shm[1] %p\r\n",p32_data_shm[1]);


								}
								else if(p16_d[6] == 3)
								{
									p32_data = p32_data_shm[2];
//									printf("p32_data_shm[2] %p\r\n",p32_data_shm[2]);
									fprintf(log_slot_2,"p32_data_shm[2] %p\r\n",p32_data_shm[2]);

								}
								else if(p16_d[6] == 4)
								{
									p32_data = p32_data_shm[3];
//									printf("p32_data_shm[3] %p\r\n",p32_data_shm[3]);
									fprintf(log_slot_2,"p32_data_shm[3] %p\r\n",p32_data_shm[3]);


								}

								//	TODO : Need to implement the logic if no Error response received
								//  		while receiving the complete 76 iterations of one channel
								//			then send the response of complete data of one channel
								//			received to UI using the socket communications
								//			Check the method to check if Error generated for any iteration.

								//printf("DB file pointers attached\r\n");
								//  change the pointer address to point to actual data by adding the
								// 	the additional 6 bytes address

								uint8_t *p8_data = (uint8_t *)(u8_data_resp + 15);
								// 	Below function is for validation of Data reception and it's filling in
								// 	the files
								// 	For Actual Data storage section we need to put data into the shared objects
								// 	using the integer pointer and increment that integer pointer
								// 	Only the data section except Header, Footer and other attributes will be stored in
								//	the Data file so increment the pointer accordingly
								// 	Check how many data samples will be received in one Data frame.
								//	Total Data bytes = 3982
								// 	Total Data samples for Analog Board : 1 Samples = 3 bytes : 3982/3 = 1327
								// 	until now the testing was done with 2 bytes data but in actual scenario we need
								// 	to test the data filling with 3 bytes for both Analog and Digital boards and
								// 	According to that the logic should be implemented

//								printf("local pointer %p\r\n",p32_data);
								fprintf(log_slot_2,"local pointer %p\r\n",p32_data);

								for(u32_count = 0 ;u32_count < /*3981*/3981 ; u32_count = u32_count + 3 /*2*/)
								{
									i32_data = 0;

									i32_data = 	(p8_data[u32_count + 2] << 16) |
												(p8_data[u32_count + 1] << 8)|
												p8_data[u32_count];
									*(uint32_t *)p32_data = i32_data;
									p32_data++;
								}

//								printf("DB frame fill complete\r\n");
								//	Increment the shared object pointer after writing the 3981 bytes data
								// 	and update the particular pointer
								p32_data_shm[p16_d[6]-1] = p32_data;

//								printf("p32_data %p\r\n",p32_data);
								fprintf(log_slot_2,"p32_data %p\r\n",p32_data);

								//
								//						fseek(ptr_data,0,SEEK_CUR);
//								for(uint16_t i = 0 ; i < /*(u16_frame_size/1)*/ (i_read_count/2); /*i = i+4*/i++)
//								{
//									fprintf(ptr_data,"%d,",p16_d[i]);
//									//usleep(100);
//								}
								//fprintf(ptr_data,"\r\n");

								if(u16_iteration == CHANNEL_1_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_1_ITERATIONS)
									{
										/*	if the iteration reach the 76 then first channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	first channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[0]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_2_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_2_ITERATIONS)
									{
										/*	if the iteration reach the 152 then Second channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	Second channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[1]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_3_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_3_ITERATIONS)
									{
										/*	if the iteration reach the 228 then Third channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	Third channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[2]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_4_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_4_ITERATIONS)
									{
										/*	if the iteration reach the u16_max_iter then fourth channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	fourth channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[3]);
										sem_post(sem_write_socket);
									}
								}

								u16_iteration++;
							}
							else
							{
								/*	If Any Error Occurs then send the Same Data request again
								 * 	if the same Data request is send for consecutive 3 times then
								 * 	give Error that Data request Error to High level software
								 * 	and stop data fetching process
								 */
								printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
								fprintf(log_slot_2,"DB %d DR Frame Validation Error\r\n",u16_boards);

								u8_req_count++;
								// printf("frame Err %x\r\n",u8_ret);
								if(u8_req_count > 2)
								{
									cards_t[u16_boards].u8_ack_error = e_ERR;

									/*	send the Error response of activity on the socket */
									sem_wait(sem_write_socket);
									socket_resp(e_DATA_REC_ERROR,ERROR);
									sem_post(sem_write_socket);

									/*	when any of the data request doens't receive data for 3 consecutive
									 * 	iterations then break the current for loop of data fetching of current board
									 * 	and go to next Daughter Card for data fetch activity
									 */
									u8_req_count = 0;
									break;

									u16_iteration++;

								}
								u8_ret = 0;
							}
						}
						else
						{
							u8_req_count++;
							printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
							// printf("data rd err\r\n");
							fprintf(log_slot_2,"DB %d DR Read Count Error\r\n",u16_boards);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								/*	send the Error response of activity on the socket */
								sem_wait(sem_write_socket);
								socket_resp(e_DATA_REC_ERROR,ERROR);
								sem_post(sem_write_socket);

								/*	when any of the data request doens't receive data for 3 consecutive
								 * 	iterations then break the current for loop of data fetching of current board
								 * 	and go to next Daughter Card for data fetch activity
								 */
								u8_req_count = 0;
								break;

								u16_iteration++;

							}
						}
					}
					u16_boards++;
				}
			}
			system("sync");
			if(		(cards_t[DB_1].u8_ack_error == e_ERR)
					& (cards_t[DB_2].u8_ack_error == e_ERR)
					& (cards_t[DB_3].u8_ack_error == e_ERR) )
			{
				printf("Slot 2 data sampling failed due to Errors\r\n");
			}
			else
			{
				printf("slot 2 sampling complete\r\n");
			}
			fclose(log_slot_2);

			break;
		}

		//sem_post(sem_Slot_1);
	}
}

/*------------------------- Communication Code with Slot 2 Ends -------------------------------*/

/* --------------------------------------------------------------------------------------------*/

/*------------------------- Thread for communication with Slot 3 ------------------------------*/

void *communication_slot_3()
{
	/* 	Cards --> 1,2,3 */
	uint8_t 	u8_data_resp[4004] = {0};
	uint8_t 	u8_c_buff[MAX_LEN] = {0};
	int 		i_read_count,i_ret = 0;
	int32_t 	*p32_data = NULL;
	int32_t 	*p32_data_shm[4] ;
	int32_t 	i32_data = 0;

	uint8_t 	u8_state = e_REQ_DATA;
	uint8_t		u8_ret = 0,u8_req_count = 0;
	uint16_t 	u16_iteration = 0,u16_max_iter = 0;
	uint16_t 	u16_boards 	= 0;

	uint32_t 	u32_count = 0;

	FILE *log_slot_3;

	int i_sem_ret;

	log_slot_3 = fopen("log_slot_3.txt","w+");
	if(log_slot_3 == NULL)
	{
		perror("");
		exit(1);
	}

	printf("communication_slot_3 t thread\r\n");

	while(1)
	{

		printf("waiting for sem 3\r\n");
		i_sem_ret = sem_wait(sem_Slot_3);
		printf("semphore 3 success\r\n");

		/* 	There are 2 types of tests
		 * 	1. Hardware Config
		 * 	2. User Tests
		 */
		u8_state = u8_req_configured;

		switch(u8_state)
		{
		//		custom_uart_fifo_reset(p32_axi_uart_1);
		case e_REQ_HANDSHAKE:
			gpio_rs485_tx_en();
			usleep(5);

			/* 	below code is to make the handshake requests modular for 3 Daughter boards
			 * 	otherwise code will be duplicated for 3 boards and number of lines will
			 * 	increase.
			 */
			u8_req_count = 0;
			for(u16_iteration = 1 ; u16_iteration <= 3 ; )
			{
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= u16_iteration;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.u16_footer			= 0x5555;
				printf("Req Handshake\r\n");
				fprintf(log_slot_3,"DB %d HSR\r\n",u16_iteration);

				custom_uart_fifo_reset(p32_axi_uart_3);
				frame_req_slot_3(&frame_t,i_ret);
				memset(u8_c_buff,0,MAX_LEN);
				usleep(1000);

				i_read_count = custom_uart_3_read((uint8_t*)(u8_c_buff),56);
				printf("rd %d\r\n",i_read_count);
				fprintf(log_slot_3,"rd %d\r\n",i_read_count);

				/*	comment the below line of after testing will be complete */
//				i_read_count = 2;
				if(i_read_count > 0)
				{
					uint16_t *p16_d_1 = (u8_c_buff + 1);
//					for(u16_count = 0 ; u16_count < 28 ; u16_count++)
//					{
//						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
//					}
					u8_ret = frame_respone_validate((p16_d_1),i_read_count);
					if(u8_ret == 0)
					{
						if(p16_d_1[2] == 0x5C5C)
						{
//							u8_state = e_REQ_TEST_SETTINGS;

							/*	TODO : send message over socket for present data sampling channels
							 * 			present in the particular Daughter Board
							 */

							printf("Handshake Response\r\n");
							fprintf(log_slot_3,"DB %d HSR Success\r\n",u16_iteration);
//							set_channels(p16_d_1[5]);
							u16_iteration++;
						}
						else
						{
							printf("Response Error %x\r\n",p16_d_1[5]);
							fprintf(log_slot_3,"DB %d HSR DB Response Error\r\n",u16_iteration);
						}
					}
					else
					{
						/* 	set the flag to send the request again to the
						 * 	corresponding Daughter board with current activity
						 */
						u8_req_count++;
//						u8_state = e_REQ_HANDSHAKE;
						printf("DB %d\r\n",u16_iteration);
						fprintf(log_slot_3,"DB %d HSR Frame Validation Error %d\r\n",u16_iteration,u8_ret);
						if(u8_req_count > 2)
						{
							/*	if consecutive request count exceeds 3 iterations then
							 * 	send the message to High level software that particular
							 * 	channels of Corresponding Daughter Boards are not
							 * 	responding to Handshake request and give message to user
							 */
							printf("Error in Communication with %d\r\n",u16_iteration);

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_iteration++;
							u8_req_count = 0;
						}

					}
				}
				else
				{
					u8_req_count++;
					fprintf(log_slot_3,"DB %d HSR Frame Validation Error %d\r\n",u16_iteration,i_read_count);
					if(u8_req_count > 2)
					{
						/*	if consecutive request count exceeds 3 iterations then
						 * 	send the message to High level software that particular
						 * 	channels of Corresponding Daughter Boards are not
						 * 	responding to Handshake request and give message to user
						 */
						printf("Error in Communication with %d\r\n",u16_iteration);
						/*	send the Error response of activity on the socket */
						sem_wait(sem_write_socket);
						socket_resp(u8_test_configured,ERROR);
						sem_post(sem_write_socket);

						u16_iteration++;
						u8_req_count = 0;
					}
				}
			}

			break;
		case e_REQ_TEST_SETTINGS :
			//printf("th_1 e_REQ_CONFIG\r\n");
			gpio_rs485_tx_en();
			usleep(5);
			u8_req_count = 0;
			for(u16_boards = DB_7 ; u16_boards <= DB_9 ; /*u16_boards++*/)
			{
				/*	Daughter Board 1 send command and receive response */
				if(cards_t[u16_boards].u8_test == e_SELECTED)
				{
					frame_t.u16_header			= 0xAAAA;
					frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
					frame_t.u16_activity		= 0x0002;
//					frame_t.u16_activity		=
					frame_t.u16_identifier		= 0x5B5B;
					frame_t.u16_payload			= 22;
					frame_t.u16_footer			= 0x5555;

					/*	Make provision to copy the Payload data using the structure copy
					 * 	as "cards_t" structure will have the information regarding the parameters
					 * 	that will be sent to Daughter board
					 */
					frame_t.p16_data[2]			= 0xAABB;
					frame_t.p16_data[3]			= 0xAACC;


						printf("DB %d Test Param Req\r\n",u16_boards);
						fprintf(log_slot_3,"DB %d TPR\r\n",u16_boards);

						custom_uart_fifo_reset(p32_axi_uart_3);
						frame_req_slot_3(&frame_t,i_ret);
						memset(u8_c_buff,0,MAX_LEN);

					usleep(5000);

					i_read_count = custom_uart_3_read((uint8_t*)(u8_c_buff),56);
					printf("rd %d\r\n",i_read_count);
					fprintf(log_slot_3,"rd %d\r\n",i_read_count);

					/*	purposely making "i_read_count" greater than "0" for testing after
					 * 	testing remove the below line : HT
					 */
					//i_read_count = 2;
					if(i_read_count > 0)
					{
						uint16_t *p16_d_1 = (u8_c_buff + 1);

						//						for(u16_count = 0 ; u16_count < 28 ; u16_count++)
						//						{
						//							printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
						//						}

						u8_ret = frame_respone_validate((p16_d_1),i_read_count);
						if(u8_ret == 0)
						{
							if(p16_d_1[2] == 0x5C5C)
							{
//								for(int i = 0; i < i_read_count/2 ; i++)
//								{
//									fprintf(ptr_slot_1_config,"%d,",p16_d_1[i]);
//								}
//								fprintf(ptr_slot_1_config,"\r\n");
								fprintf(log_slot_3,"DB %d TPR Success\r\n",u16_boards);
								u16_boards++;
							}
							else
							{
								printf("Response Error %x\r\n",p16_d_1[5]);
								fprintf(log_slot_3,"DB %d TPR DB Response Error\r\n",u16_boards);
							}
						}
						else
						{
							/* 	set the flag to send the request again to the
							 * 	corresponding Daughter board with current activity
							 */
							u8_req_count++;
							printf("fame validation failed with Errors %x\r\n",u8_ret);
							fprintf(log_slot_3,"DB %d TPR Frame Validation Error %d\r\n",u16_boards,u8_ret);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								sem_wait(sem_write_socket);
								socket_resp(u8_test_configured,ERROR);
								sem_post(sem_write_socket);

								u16_boards++;
								u8_req_count = 0;
							}
						}
					}
					else
					{
						u8_req_count++;
						printf("data read error\r\n");
						fprintf(log_slot_3,"DB %d TPR Data Read Error %d\r\n",u16_boards,i_read_count);
						if(u8_req_count > 2)
						{
							cards_t[u16_boards].u8_ack_error = e_ERR;

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_boards++;
							u8_req_count = 0;
						}
					}
				}
				else
				{
					u16_boards++;
				}
			}


			printf("Test requests complete\r\n");
			system("sync");

		case e_OPTO_WAIT :

			printf("slot_3 e_OPTO_WAIT\r\n");
			sem_wait(sem_opto_wait);
			if(u8_opto_check == 0)
			{
				opto_wait();
			}
			else
			{

			}
			sem_post(sem_opto_wait);


		case e_REQ_DATA :
			gpio_rs485_tx_en();
			usleep(50);

			// 	Below code is for testing purpose comment it after testing is complete
//			cards_t[0].u8_ack_error = e_ACK;
//			cards_t[1].u8_ack_error = e_ACK;
//			cards_t[2].u8_ack_error = e_ACK;

			cards_t[0].u8_board = e_Analog_Baord;
			cards_t[1].u8_board = e_Analog_Baord;
			cards_t[2].u8_board = e_Analog_Baord;

			//	Above testing code Ends here

			u8_req_count = 0;

			printf("Data request Starts\r\n");
			fprintf(log_slot_3,"Data request Starts\r\n");

			for(u16_boards = DB_7 ; u16_boards <= DB_9 ;)
			{

				if(cards_t[u16_boards].u8_ack_error == e_ERR)
				{
					printf("Board %d has Comm Error\r\n",u16_boards);
					fprintf(log_slot_3,"DB %d has Comm Error\r\n",u16_boards);
					u16_boards++;
				}
				else
				{
					/* 	send the data request to board which has received correct
					 * 	test parameters of the current test selected by user
					 */
					p32_data_shm[0] = cards_t[u16_boards].p_data_files[0];
//					printf("p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);
					fprintf(log_slot_3,"p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);


					p32_data_shm[1] = cards_t[u16_boards].p_data_files[1];
//					printf("p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);
					fprintf(log_slot_3,"p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);


					p32_data_shm[2] = cards_t[u16_boards].p_data_files[2];
//					printf("p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);
					fprintf(log_slot_3,"p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);


					p32_data_shm[3] = cards_t[u16_boards].p_data_files[3];
//					printf("p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);
					fprintf(log_slot_3,"p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);


					if(cards_t[u16_boards].u8_board == e_Digital_Board)
					{
						u16_max_iter = MAX_DI_ITERATIONS;
					}
					else if(cards_t[u16_boards].u8_board == e_Analog_Baord)
					{
						u16_max_iter = MAX_AI_ITERATIONS;
					}

					for(u16_iteration = 1 ; u16_iteration <= u16_max_iter; /*u6_iteration++*/)
					{
//						sleep(1);
						//printf("DB : send CMD\r\n");
						frame_t.u16_header			= 0xAAAA;
						frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
						frame_t.u16_activity		= 0x0003;
						frame_t.u16_identifier		= 0x5B5B;
						frame_t.u16_payload			= 22;
						frame_t.p16_data[0]			= u16_iteration;
						frame_t.u16_footer			= 0x5555;

						/*	logic implemented for detecting the channels : specifically for
						 * 	Analog channels but it can be implemented for Daughter board also
						 */
						if(u16_iteration <= CHANNEL_1_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_1;
						}
						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_2;
						}
						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
						{
							frame_t.p16_data[1]		= CHANNEL_3;
						}
						else
						{
							frame_t.p16_data[1]		= CHANNEL_4;
						}

						custom_uart_fifo_reset(p32_axi_uart_3);
						memset(u8_data_resp,0,4004);

						fprintf(log_slot_3,"DB %d Req %d iter\r\n",u16_boards,u16_iteration);
						frame_req_slot_3(&frame_t,0);

						usleep(10000);

						i_read_count = custom_uart_3_read((uint8_t*)(u8_data_resp),MAX_LEN);
						fprintf(log_slot_3,"rd %d\r\n",i_read_count);
						//						printf("rd %d\r\n",i_read_count);
						uint16_t *p16_d = u8_data_resp + 1;

						/*	TODO: below code of "if" and "else" statements is for testing purpose
						 * 	comment it after testing is complete : HT
						 */
//						if(u16_iteration <= CHANNEL_1_ITERATIONS)
//						{
//							p16_d[6] = 1;
//						}
//						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
//						{
//							p16_d[6] = 2;
//						}
//						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
//						{
//							p16_d[6] = 3;
//						}
//						else
//						{
//							p16_d[6] = 4;
//						}

//						for(int i = 0 ; i < 100 ; i++)
//						{
//							printf("%d = %x\r\n",i,p16_d[i]);
//						}
//						printf("\r\n");
//						printf("rd %d\r\n",i_read_count);

//						uint16_t *p16_d = u8_data_resp + 1;
//						while(1);

						//	TODO: Below line is added for testing purpose comment it after
						// 	After testing is complete
//						i_read_count = 2;

						if(i_read_count > 0)
						{
							//printf("DB Resp frame validate\r\n");
							u8_ret = frame_respone_validate((p16_d),i_read_count);

							//	TODO: Below line is added for testing purpose comment it after
							// 	After testing is complete
//							u8_ret = 0;

							if(u8_ret == 0)
							{
								/*	check which channel data is arrived in Uart buffer and store that
								 * 	data into respective data storing files
								 */
								//ptr_data = ptr_ch_1;
//								printf("DB Resp Data fill\r\n");
//								p16_d[6] = 1
								if(p16_d[6] == 1)
								{
									p32_data = p32_data_shm[0];
//									printf("p32_data_shm[0] %p\r\n",p32_data_shm[0]);
									fprintf(log_slot_3,"p32_data_shm[0] %p\r\n",p32_data_shm[0]);

								}
								else if(p16_d[6] == 2)
								{
									p32_data = p32_data_shm[1];
//									printf("p32_data_shm[1] %p\r\n",p32_data_shm[1]);
									fprintf(log_slot_3,"p32_data_shm[1] %p\r\n",p32_data_shm[1]);


								}
								else if(p16_d[6] == 3)
								{
									p32_data = p32_data_shm[2];
//									printf("p32_data_shm[2] %p\r\n",p32_data_shm[2]);
									fprintf(log_slot_3,"p32_data_shm[2] %p\r\n",p32_data_shm[2]);

								}
								else if(p16_d[6] == 4)
								{
									p32_data = p32_data_shm[3];
//									printf("p32_data_shm[3] %p\r\n",p32_data_shm[3]);
									fprintf(log_slot_3,"p32_data_shm[3] %p\r\n",p32_data_shm[3]);


								}

								//	TODO : Need to implement the logic if no Error response received
								//  		while receiving the complete 76 iterations of one channel
								//			then send the response of complete data of one channel
								//			received to UI using the socket communications
								//			Check the method to check if Error generated for any iteration.

								//printf("DB file pointers attached\r\n");
								//  change the pointer address to point to actual data by adding the
								// 	the additional 6 bytes address

								uint8_t *p8_data = (uint8_t *)(u8_data_resp + 15);
								// 	Below function is for validation of Data reception and it's filling in
								// 	the files
								// 	For Actual Data storage section we need to put data into the shared objects
								// 	using the integer pointer and increment that integer pointer
								// 	Only the data section except Header, Footer and other attributes will be stored in
								//	the Data file so increment the pointer accordingly
								// 	Check how many data samples will be received in one Data frame.
								//	Total Data bytes = 3982
								// 	Total Data samples for Analog Board : 1 Samples = 3 bytes : 3982/3 = 1327
								// 	until now the testing was done with 2 bytes data but in actual scenario we need
								// 	to test the data filling with 3 bytes for both Analog and Digital boards and
								// 	According to that the logic should be implemented

//								printf("local pointer %p\r\n",p32_data);
								//fprintf(log_slot_3,"local pointer %p\r\n",p32_data);

								for(u32_count = 0 ;u32_count < /*3981*/3981 ; u32_count = u32_count + 3 /*2*/)
								{
									i32_data = 0;

									i32_data = 	(p8_data[u32_count + 2] << 16) |
												(p8_data[u32_count + 1] << 8)|
												p8_data[u32_count];
									*(uint32_t *)p32_data = i32_data;
									p32_data++;
								}

//								printf("DB frame fill complete\r\n");
								//	Increment the shared object pointer after writing the 3981 bytes data
								// 	and update the particular pointer
								p32_data_shm[p16_d[6]-1] = p32_data;

//								printf("p32_data %p\r\n",p32_data);
								//fprintf(log_slot_3,"p32_data %p\r\n",p32_data);

								//
								//						fseek(ptr_data,0,SEEK_CUR);
//								for(uint16_t i = 0 ; i < /*(u16_frame_size/1)*/ (i_read_count/2); /*i = i+4*/i++)
//								{
//									fprintf(ptr_data,"%d,",p16_d[i]);
//									//usleep(100);
//								}
								//fprintf(ptr_data,"\r\n");

								if(u16_iteration == CHANNEL_1_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_1_ITERATIONS)
									{
										/*	if the iteration reach the 76 then first channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	first channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[0]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_2_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_2_ITERATIONS)
									{
										/*	if the iteration reach the 152 then Second channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	Second channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[1]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_3_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_3_ITERATIONS)
									{
										/*	if the iteration reach the 228 then Third channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	Third channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[2]);
										sem_post(sem_write_socket);
									}
								}
								else if(u16_iteration == CHANNEL_4_ITERATIONS)
								{
									if(u16_iteration == CHANNEL_4_ITERATIONS)
									{
										/*	if the iteration reach the u16_max_iter then fourth channel data read from the
										 * 	Daughter board so send message over SOCKET for completion data fetch of
										 * 	fourth channel
										 */
										sem_wait(sem_write_socket);
										channel_data_rec_resp(cards_t[u16_boards].u8_channel[3]);
										sem_post(sem_write_socket);
									}
								}

								u16_iteration++;
							}
							else
							{
								/*	If Any Error Occurs then send the Same Data request again
								 * 	if the same Data request is send for consecutive 3 times then
								 * 	give Error that Data request Error to High level software
								 * 	and stop data fetching process
								 */
//								printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
								fprintf(log_slot_3,"DB %d DR Frame Validation Error\r\n",u16_boards);

								u8_req_count++;
//								printf("frame Err %x\r\n",u8_ret);
								if(u8_req_count > 2)
								{
									cards_t[u16_boards].u8_ack_error = e_ERR;

									/*	send the Error response of activity on the socket */
									sem_wait(sem_write_socket);
									socket_resp(e_DATA_REC_ERROR,ERROR);
									sem_post(sem_write_socket);

									/*	when any of the data request doens't receive data for 3 consecutive
									 * 	iterations then break the current for loop of data fetching of current board
									 * 	and go to next Daughter Card for data fetch activity
									 */
									u8_req_count = 0;
									break;

									u16_iteration++;

								}
								u8_ret = 0;
							}
						}
						else
						{
							u8_req_count++;
							printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
//							printf("data rd err\r\n");
							fprintf(log_slot_3,"DB %d DR Read Count Error\r\n",u16_boards);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								/*	send the Error response of activity on the socket */
								sem_wait(sem_write_socket);
								socket_resp(e_DATA_REC_ERROR,ERROR);
								sem_post(sem_write_socket);

								/*	when any of the data request doens't receive data for 3 consecutive
								 * 	iterations then break the current for loop of data fetching of current board
								 * 	and go to next Daughter Card for data fetch activity
								 */
								u8_req_count = 0;
								break;

								u16_iteration++;

							}
						}
					}
					u16_boards++;
				}
			}

			fclose(log_slot_3);
			system("sync");
			if(		(cards_t[DB_1].u8_ack_error == e_ERR)
					& (cards_t[DB_2].u8_ack_error == e_ERR)
					& (cards_t[DB_3].u8_ack_error == e_ERR) )
			{
				printf("Slot 3 data sampling failed due to Errors\r\n");
			}
			else
			{
				printf("slot 3 sampling complete\r\n");
			}


			break;
		}

		//sem_post(sem_Slot_1);
	}
}

/*------------------------- Communication Code with Slot 3 Ends -------------------------------*/

/* --------------------------------------------------------------------------------------------*/

/*------------------------- Thread for communication with Slot 4 ------------------------------*/

void *communication_slot_4()
{
	/* 	Cards --> 1,2,3 */
	uint8_t 	u8_data_resp[4004] = {0};
	uint8_t 	u8_c_buff[MAX_LEN] = {0};
	long int 		i_read_count,i_ret = 0;
	int32_t 	*p32_data = NULL;
	int32_t 	*p32_data_shm[4] ;
	int32_t 	i32_data = 0;

	uint8_t 	u8_state = e_REQ_DATA;
	uint8_t		u8_ret = 0,u8_req_count = 0;
	uint16_t 	u16_iteration = 0,u16_max_iter = 0;
	uint16_t 	u16_boards 	= 0;

	int i_status = -1;

	uint32_t 	u32_count = 0;

	FILE *log_slot_4;

	int i_sem_ret;

	log_slot_4 = fopen("log_slot_4.txt","w+");
	if(log_slot_4 == NULL)
	{
		perror("");
		exit(1);
	}

	printf("communication_slot_4 t thread\r\n");

	while(1)
	{

		printf("waiting for sem 4\r\n");
		i_sem_ret = sem_wait(sem_Slot_4);
		printf("semphore 4 success\r\n");

		/* 	There are 2 types of tests
		 * 	1. Hardware Config
		 * 	2. User Tests
		 */
		u8_state = u8_req_configured;
//		u8_state = e_OPTO_WAIT;

		switch(u8_state)
		{
		//		custom_uart_fifo_reset(p32_axi_uart_1);
		case e_REQ_HANDSHAKE:
			gpio_rs485_tx_en();
			usleep(5);

			/* 	below code is to make the handshake requests modular for 3 Daughter boards
			 * 	otherwise code will be duplicated for 3 boards and number of lines will
			 * 	increase.
			 */
			u8_req_count = 0;
			for(u16_iteration = 0 ; u16_iteration < 1 ; )
			{
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= u16_iteration + 1;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.u16_footer			= 0x5555;
				printf("Req Handshake\r\n");

				custom_uart_fifo_reset(p32_axi_uart_4);
				frame_req_slot_4(&frame_t,i_ret);
				memset(u8_c_buff,0,MAX_LEN);
				usleep(10000);

//				printf("going on infinite loop\r\n");
//				while(1);
				i_read_count = custom_uart_4_read((uint8_t*)(u8_c_buff),56);

				for(int u16_count = 0 ; u16_count < 61 ; u16_count=u16_count+4)
				{
					printf("%x %x %x %x\r\n",u8_c_buff[u16_count]
											,u8_c_buff[u16_count+1]
										 	,u8_c_buff[u16_count+2]
											,u8_c_buff[u16_count+3]);
				}
//				printf("\r\n");
				printf("rd %d\r\n",i_read_count);

				/*	comment the below line of after testing will be complete */
//				i_read_count = 2;
				if(i_read_count > 0)
				{
					uint16_t *p16_d_1 = (u8_c_buff + 1);
//					for(uint16_t u16_count = 0 ; u16_count < 28 ; u16_count++)
//					{
//						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
//					}
					u8_ret = frame_respone_validate((p16_d_1),i_read_count);
					if(u8_ret == 0)
					{
						if(p16_d_1[2] == 0x5C5C)
						{
//							u8_state = e_REQ_TEST_SETTINGS;

							/*	TODO : send message over socket for present data sampling channels
							 * 			present in the particular Daughter Board
							 */

							printf("Handshake Response Success %d\r\n",u16_iteration);
							u8_req_count = 0;
							/*	TODO :Below line is commented for testing purpose
							 * 	uncomment it after testing is over : HT
							 */
							//set_channels(p16_d_1[5]);
							u16_iteration++;
						}
						else
						{
							printf("Response Error %x\r\n",p16_d_1[5]);
							u8_req_count++;
	//						u8_state = e_REQ_HANDSHAKE;
							printf("DB %d\r\n",u16_iteration);
							if(u8_req_count > 2)
							{

								printf("Error Handshake Comm DB %d\r\n",u16_iteration);

								/*	send the Error response of activity on the socket */
								sem_wait(sem_write_socket);
								socket_resp(u8_test_configured,ERROR);
								sem_post(sem_write_socket);

								u16_iteration++;
								u8_req_count = 0;
							}
						}
					}
					else
					{

						u8_req_count++;
//						u8_state = e_REQ_HANDSHAKE;
						printf("Invalidat frame %d\r\n",u8_ret);
						if(u8_req_count > 2)
						{
							printf("Error Handshake Comm DB %d\r\n",u16_iteration);

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_iteration++;
							u8_req_count = 0;
						}

					}
				}
				else
				{
					u8_req_count++;
					if(u8_req_count > 2)
					{

						printf("Error Handshake Comm DB %d\r\n",u16_iteration);
						/*	send the Error response of activity on the socket */
						sem_wait(sem_write_socket);
						socket_resp(u8_test_configured,ERROR);
						sem_post(sem_write_socket);

						u16_iteration++;
						u8_req_count = 0;
					}
				}
			}

			break;
		case e_REQ_TEST_SETTINGS :

			/*	Reset the "opto_check" flag before starting any test*/
			u8_opto_check = 0;

			gpio_rs485_tx_en();
			usleep(5);
			u8_req_count = 0;
			for(u16_boards = DB_10 ; u16_boards < DB_11/*u16_boards <= DB_11*/ ; /*u16_boards++*/)
			{
				/*	Daughter Board 1 send command and receive response */
				if(cards_t[u16_boards].u8_test == e_SELECTED)
				{
					frame_t.u16_header			= 0xAAAA;
					frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
//					frame_t.u16_activity		= 0x0002;
					frame_t.u16_activity		= u8_test_configured;
					frame_t.u16_identifier		= 0x5B5B;
					frame_t.u16_payload			= 22;
					frame_t.u16_footer			= 0x5555;

					/*	Make provision to copy the Payload data using the structure copy
					 * 	as "cards_t" structure will have the information regarding the parameters
					 * 	that will be sent to Daughter board
					 */
					frame_t.p16_data[2]			= 0xAABB;
					frame_t.p16_data[3]			= 0xAACC;

					// 	Below code is for testing the correct configuration of
					//	of the board and TX an RX are working properly with RS485
//					while(1)
//					{
						printf("DB %d Test Param Req\r\n",u16_boards);

						custom_uart_fifo_reset(p32_axi_uart_4);
						frame_req_slot_4(&frame_t,i_ret);
						memset(u8_c_buff,0,MAX_LEN);
//					}
					usleep(1000);

					i_read_count = custom_uart_4_read((uint8_t*)(u8_c_buff),56);
					printf("rd %d\r\n",i_read_count);

					for(int u16_count = 0 ; u16_count < 61 ; u16_count=u16_count+4)
					{
						printf("%x %x %x %x\r\n",u8_c_buff[u16_count]
												,u8_c_buff[u16_count+1]
											 	,u8_c_buff[u16_count+2]
												,u8_c_buff[u16_count+3]);
					}
//					printf("\r\n");

					/*	purposely making "i_read_count" greater than "0" for testing after
					 * 	testing remove the below line : HT
					 */
					//i_read_count = 2;
					if(i_read_count > 0)
					{
						uint16_t *p16_d_1 = (u8_c_buff + 1);

						u8_ret = frame_respone_validate((p16_d_1),i_read_count);
						if(u8_ret == 0)
						{
							if(p16_d_1[2] == 0x5C5C)
							{
								printf("Test Response Success DB %d\r\n",u16_boards);
								for(int i = 0; i < i_read_count/2 ; i++)
								{
//									fprintf(ptr_slot_1_config,"%d,",p16_d_1[i]);
								}

//								fprintf(ptr_slot_1_config,"\r\n");
								u8_req_count = 0;
								u16_boards++;
							}
							else
							{
								printf("Response Error %x\r\n",p16_d_1[5]);
								u8_req_count++;
								if(u8_req_count > 2)
								{
									cards_t[u16_boards].u8_ack_error = e_ERR;
									u16_boards++;
									u8_req_count = 0;
								}
							}
						}
						else
						{
							/* 	set the flag to send the request again to the
							 * 	corresponding Daughter board with current activity
							 */
							u8_req_count++;
							printf("fame validation failed with Errors %x\r\n",u8_ret);
							if(u8_req_count > 2)
							{
								cards_t[u16_boards].u8_ack_error = e_ERR;

								sem_wait(sem_write_socket);
								socket_resp(u8_test_configured,ERROR);
								sem_post(sem_write_socket);

								u16_boards++;
								u8_req_count = 0;
							}
						}
					}
					else
					{
						u8_req_count++;
						printf("data read error\r\n");
						if(u8_req_count > 2)
						{
							cards_t[u16_boards].u8_ack_error = e_ERR;

							/*	send the Error response of activity on the socket */
							sem_wait(sem_write_socket);
							socket_resp(u8_test_configured,ERROR);
							sem_post(sem_write_socket);

							u16_boards++;
							u8_req_count = 0;
						}
					}
				}
				else
				{
					u16_boards++;
				}
			}


			printf("Test requests complete\r\n");
			system("sync");

		case e_OPTO_WAIT :

//			printf("slot_4 e_OPTO_WAIT\r\n");

			/*	send the activity of opto wait for the Slot */
			frame_t.u16_header			= 0xAAAA;
			frame_t.u16_card_address 	= 1;
			frame_t.u16_activity		= 0x0004;
			frame_t.u16_identifier		= 0x5B5B;
			frame_t.u16_payload			= 22;
			frame_t.u16_footer			= 0x5555;

//			custom_uart_fifo_reset(p32_axi_uart_4);
//			frame_req_slot_4(&frame_t,i_ret);

//			sem_wait(sem_opto_wait);

//			if(u8_opto_check == 0)
//			{
//				printf("function opto wait\r\n");
//				fflush(stdout);
//				i_status = opto_wait();
//			}
//			else
//			{
//
//			}

			usleep(10000);
			i_status = 1;

			if(i_status == -1)
			{
				printf("Opto Reception failed\r\n");
				fflush(stdout);
				sem_wait(sem_write_socket);
				socket_resp(OPTO_RECEPTION_FAILED,ERROR);
				sem_post(sem_write_socket);
				break;
			}
			else
			{
				printf("starting the Data requests\r\n");
				fflush(stdout);
//				sleep(5);
				sem_wait(sem_write_socket);
				socket_resp(OPTO_RECEPTION_SUCCESS,ACKNOWLEDGE);
				sem_post(sem_write_socket);

				/*	here break should be implemented on the basis of which test is performed by the
				 * 	user
				 * 	if the test is of simple Open/Close the program execution should not go to
				 * 	Data fetching activity
				 * 	if other tests are configured by user then program execution should go to
				 * 	Data fetching activity
				 */
//				break;
			}

			/*	TODO : below line is added for testing the simple "Open" and "Close" operation
			 * 			comment it after testing is complete
			 */
//			break;
//			sem_post(sem_opto_wait);

		case e_REQ_DATA :
			gpio_rs485_tx_en();
			usleep(500000);


			/*	TODO:Below code is introduced for Demo purpose remove it after Demo is
			 * 	Over : HT
			 * 	In this code only the Static data files Activity code is sent to node js code
			 */

			/*	Demo code starts here : HT */
			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_MAIN_PIR);
			sem_post(sem_write_socket);

			usleep(100000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_AUX);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_COIL_1);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_COIL_2);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_COIL_3);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_COIL_4);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_COIL_5);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_COIL_6);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_TRAVEL_1);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_TRAVEL_2);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			channel_data_rec_resp(e_TRAVEL_3);
			sem_post(sem_write_socket);

			usleep(500000);

			sem_wait(sem_write_socket);
			socket_resp(0x26,ACKNOWLEDGE);
			sem_post(sem_write_socket);


			/*	Demo code Ends here : HT */

			/* 	TODO: Below code is actual production code and it is commented for Demo
			 * 	purpose. remove it after Demo is over : HT
			 */


			// 	Below code is for testing purpose comment it after testing is complete
//			cards_t[DB_10].u8_ack_error = e_ACK;
//			cards_t[DB_11].u8_ack_error = e_ACK;
//			cards_t[2].u8_ack_error = e_ACK;
//
//			cards_t[DB_10].u8_board = e_Analog_Baord;
//			cards_t[DB_11].u8_board = e_Analog_Baord;
//			cards_t[2].u8_board = e_Analog_Baord;
//
//			//	Above testing code Ends here
//
//			u8_req_count = 0;
//
////			printf("Data request Starts\r\n");
//
//			for(u16_boards = DB_10 ; u16_boards <= DB_11 ;)
//			{
//
//				if(cards_t[u16_boards].u8_ack_error == e_ERR)
//				{
//					printf("Board %d has Comm Error\r\n",u16_boards);
//					u16_boards++;
//				}
//				else
//				{
//					/* 	send the data request to board which has received correct
//					 * 	test parameters of the current test selected by user
//					 */
//					p32_data_shm[0] = cards_t[u16_boards].p_data_files[0];
////					printf("p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);
//					fprintf(log_slot_4,"p32_data_shm[0].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[0]);
//
//
//					p32_data_shm[1] = cards_t[u16_boards].p_data_files[1];
////					printf("p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);
//					fprintf(log_slot_4,"p32_data_shm[1].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[1]);
//
//
//					p32_data_shm[2] = cards_t[u16_boards].p_data_files[2];
////					printf("p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);
//					fprintf(log_slot_4,"p32_data_shm[2].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[2]);
//
//
//					p32_data_shm[3] = cards_t[u16_boards].p_data_files[3];
////					printf("p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);
//					fprintf(log_slot_4,"p32_data_shm[3].p_data_files %p\r\n",cards_t[u16_boards].p_data_files[3]);
//
//
//					if(cards_t[u16_boards].u8_board == e_Digital_Board)
//					{
//						u16_max_iter = MAX_DI_ITERATIONS;
//						fprintf(log_slot_4,"%d e_Digital_Board\r\n",u16_boards);
//					}
//					else if(cards_t[u16_boards].u8_board == e_Analog_Baord)
//					{
//						u16_max_iter = MAX_AI_ITERATIONS;
//						fprintf(log_slot_4,"%d e_Analog_Baord\r\n",u16_boards);
//					}
//
//					for(u16_iteration = 1 ; u16_iteration <= u16_max_iter; /*u6_iteration++*/)
//					{
////						sleep(1);
//						//printf("DB : send CMD\r\n");
//						frame_t.u16_header			= 0xAAAA;
//						frame_t.u16_card_address 	= cards_t[u16_boards].u8_card_address;
//						frame_t.u16_activity		= 0x0003;
//						frame_t.u16_identifier		= 0x5B5B;
//						frame_t.u16_payload			= 22;
//						frame_t.p16_data[0]			= u16_iteration;
//						frame_t.u16_footer			= 0x5555;
//
//						/*	logic implemented for detecting the channels : specifically for
//						 * 	Analog channels but it can be implemented for Daughter board also
//						 */
//						if(u16_iteration <= CHANNEL_1_ITERATIONS)
//						{
//							frame_t.p16_data[1]		= CHANNEL_1;
//						}
//						else if(u16_iteration <= CHANNEL_2_ITERATIONS)
//						{
//							frame_t.p16_data[1]		= CHANNEL_2;
//						}
//						else if(u16_iteration <= CHANNEL_3_ITERATIONS)
//						{
//							frame_t.p16_data[1]		= CHANNEL_3;
//						}
//						else
//						{
//							frame_t.p16_data[1]		= CHANNEL_4;
//						}
//
//						custom_uart_fifo_reset(p32_axi_uart_4);
//						memset(u8_data_resp,0,4004);
//
//						printf("DB %d Req %d iter\r\n",u16_boards,u16_iteration);
//
//						fprintf(log_slot_4,"DB %d Req %d iter\r\n",u16_boards,u16_iteration);
//						frame_req_slot_4(&frame_t,0);
//
//						usleep(3000);
//
//						i_read_count = custom_uart_4_read((uint8_t*)(u8_data_resp),MAX_LEN);
////						printf("rd %d iter %d\r\n",i_read_count,u16_iteration);
//						printf("rd %d\r\n",i_read_count);
//
//						uint16_t *p16_d = u8_data_resp + 1;
//
////						for(uint16_t u16_count = 0 ; u16_count < 20 ; u16_count++)
////						{
////							printf("%d = %x\r\n",u16_count,p16_d[u16_count]);
////						}
//
//
//						if(i_read_count > 0)
//						{
////							printf("DB Resp frame validate\r\n");
//							u8_ret = frame_respone_validate((p16_d),i_read_count);
//
//							//	TODO: Below line is added for testing purpose comment it after
//							// 	After testing is complete
////							u8_ret = 0;
//
//							if(u8_ret == 0)
//							{
//								/*	check which channel data is arrived in Uart buffer and store that
//								 * 	data into respective data storing files
//								 */
//								//ptr_data = ptr_ch_1;
////								printf("DB Resp Data fill\r\n");
////								p16_d[6] = 1
//								if(p16_d[6] == 1)
//								{
//									p32_data = p32_data_shm[0];
////									printf("p32_data_shm[0] %p\r\n",p32_data_shm[0]);
//									fprintf(log_slot_4,"p32_data_shm[0] %p\r\n",p32_data_shm[0]);
//
//								}
//								else if(p16_d[6] == 2)
//								{
//									p32_data = p32_data_shm[1];
////									printf("p32_data_shm[1] %p\r\n",p32_data_shm[1]);
//									fprintf(log_slot_4,"p32_data_shm[1] %p\r\n",p32_data_shm[1]);
//
//
//								}
//								else if(p16_d[6] == 3)
//								{
//									p32_data = p32_data_shm[2];
////									printf("p32_data_shm[2] %p\r\n",p32_data_shm[2]);
//									fprintf(log_slot_4,"p32_data_shm[2] %p\r\n",p32_data_shm[2]);
//
//								}
//								else if(p16_d[6] == 4)
//								{
//									p32_data = p32_data_shm[3];
////									printf("p32_data_shm[3] %p\r\n",p32_data_shm[3]);
//									fprintf(log_slot_4,"p32_data_shm[3] %p\r\n",p32_data_shm[3]);
//
//
//								}
//
//								//	TODO : Need to implement the logic if no Error response received
//								//  		while receiving the complete 76 iterations of one channel
//								//			then send the response of complete data of one channel
//								//			received to UI using the socket communications
//								//			Check the method to check if Error generated for any iteration.
//
////								printf("DB file pointers attached\r\n");
//								//  change the pointer address to point to actual data by adding the
//								// 	the additional 6 bytes address
//
//								uint8_t *p8_data = (uint8_t *)(u8_data_resp + 15);
//								// 	Below function is for validation of Data reception and it's filling in
//								// 	the files
//								// 	For Actual Data storage section we need to put data into the shared objects
//								// 	using the integer pointer and increment that integer pointer
//								// 	Only the data section except Header, Footer and other attributes will be stored in
//								//	the Data file so increment the pointer accordingly
//								// 	Check how many data samples will be received in one Data frame.
//								//	Total Data bytes = 3982
//								// 	Total Data samples for Analog Board : 1 Samples = 3 bytes : 3982/3 = 1327
//								// 	until now the testing was done with 2 bytes data but in actual scenario we need
//								// 	to test the data filling with 3 bytes for both Analog and Digital boards and
//								// 	According to that the logic should be implemented
//
////								printf("local pointer %p\r\n",p32_data);
//								fprintf(log_slot_4,"local pointer %p\r\n",p32_data);
//
//								for(u32_count = 0 ;u32_count < /*3981*/3981 ; u32_count = u32_count + 3 /*2*/)
//								{
//									i32_data = 0;
//
//									i32_data = 	(p8_data[u32_count + 2] << 16) |
//												(p8_data[u32_count + 1] << 8)|
//												p8_data[u32_count];
//									*(uint32_t *)p32_data = i32_data;
//									p32_data++;
//								}
//
////								printf("DB frame fill complete\r\n");
//								//	Increment the shared object pointer after writing the 3981 bytes data
//								// 	and update the particular pointer
//								p32_data_shm[p16_d[6]-1] = p32_data;
//
////								printf("p32_data %p\r\n",p32_data);
//								fprintf(log_slot_4,"p32_data %p\r\n",p32_data);
//
//								//
//								//						fseek(ptr_data,0,SEEK_CUR);
////								for(uint16_t i = 0 ; i < /*(u16_frame_size/1)*/ (i_read_count/2); /*i = i+4*/i++)
////								{
////									fprintf(ptr_data,"%d,",p16_d[i]);
////									//usleep(100);
////								}
//								//fprintf(ptr_data,"\r\n");
//
//								if(u16_iteration == CHANNEL_1_ITERATIONS)
//								{
//									if(u16_iteration == CHANNEL_1_ITERATIONS)
//									{
//										/*	if the iteration reach the 76 then first channel data read from the
//										 * 	Daughter board so send message over SOCKET for completion data fetch of
//										 * 	first channel
//										 */
//										sem_wait(sem_write_socket);
//										channel_data_rec_resp(cards_t[u16_boards].u8_channel[0]);
//										sem_post(sem_write_socket);
//									}
//								}
//								else if(u16_iteration == CHANNEL_2_ITERATIONS)
//								{
//									if(u16_iteration == CHANNEL_2_ITERATIONS)
//									{
//										/*	if the iteration reach the 152 then Second channel data read from the
//										 * 	Daughter board so send message over SOCKET for completion data fetch of
//										 * 	Second channel
//										 */
//										sem_wait(sem_write_socket);
//										channel_data_rec_resp(cards_t[u16_boards].u8_channel[1]);
//										sem_post(sem_write_socket);
//									}
//								}
//								else if(u16_iteration == CHANNEL_3_ITERATIONS)
//								{
//									if(u16_iteration == CHANNEL_3_ITERATIONS)
//									{
//										/*	if the iteration reach the 228 then Third channel data read from the
//										 * 	Daughter board so send message over SOCKET for completion data fetch of
//										 * 	Third channel
//										 */
//										sem_wait(sem_write_socket);
//										channel_data_rec_resp(cards_t[u16_boards].u8_channel[2]);
//										sem_post(sem_write_socket);
//									}
//								}
//								else if(u16_iteration == CHANNEL_4_ITERATIONS)
//								{
//									if(u16_iteration == CHANNEL_4_ITERATIONS)
//									{
//										/*	if the iteration reach the u16_max_iter then fourth channel data read from the
//										 * 	Daughter board so send message over SOCKET for completion data fetch of
//										 * 	fourth channel
//										 */
//										sem_wait(sem_write_socket);
//										channel_data_rec_resp(cards_t[u16_boards].u8_channel[3]);
//										sem_post(sem_write_socket);
//									}
//								}
//
//								u16_iteration++;
//							}
//							else
//							{
//								/*	If Any Error Occurs then send the Same Data request again
//								 * 	if the same Data request is send for consecutive 3 times then
//								 * 	give Error that Data request Error to High level software
//								 * 	and stop data fetching process
//								 */
//								printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
//
//								u8_req_count++;
//								printf("frame Err %x\r\n",u8_ret);
//								if(u8_req_count > 2)
//								{
//									cards_t[u16_boards].u8_ack_error = e_ERR;
//
//									/*	send the Error response of activity on the socket */
//									sem_wait(sem_write_socket);
//									socket_resp(e_DATA_REC_ERROR,ERROR);
//									sem_post(sem_write_socket);
//
//									/*	when any of the data request doens't receive data for 3 consecutive
//									 * 	iterations then break the current for loop of data fetching of current board
//									 * 	and go to next Daughter Card for data fetch activity
//									 */
//									u8_req_count = 0;
////									while(1);
//									break;
//
//									u16_iteration++;
//
//								}
//								u8_ret = 0;
//							}
//						}
//						else
//						{
//							u8_req_count++;
//							printf("DB %d Iter %d\r\n",u16_boards,u16_iteration);
//							printf("data rd err\r\n");
//							if(u8_req_count > 2)
//							{
//								cards_t[u16_boards].u8_ack_error = e_ERR;
//
//								/*	send the Error response of activity on the socket */
//								sem_wait(sem_write_socket);
//								socket_resp(e_DATA_REC_ERROR,ERROR);
//								sem_post(sem_write_socket);
//
//								/*	when any of the data request doens't receive data for 3 consecutive
//								 * 	iterations then break the current for loop of data fetching of current board
//								 * 	and go to next Daughter Card for data fetch activity
//								 */
//								u8_req_count = 0;
////								while(1);
//								break;
//
//								u16_iteration++;
//
//							}
//						}
//					}
//					u16_boards++;
//				}
//			}
//			fclose(log_slot_4);
//			system("sync");
//			if(		(cards_t[DB_9].u8_ack_error == e_ERR)
//					& (cards_t[DB_10].u8_ack_error == e_ERR))
//			{
//				printf("Slot 4 data sampling failed due to Errors\r\n");
//			}
//			else
//			{
//				printf("slot 4 sampling complete\r\n");
//			}

//			while(1);


			break;
		}

		//sem_post(sem_Slot_1);
	}
}

/*------------------------- Communication Code with Slot 4 Ends -------------------------------*/


/*------------------------- Communication Code with MB Unit and DCRMS Units Starts ------------*/

//void *communication_MB_DCRM()
//{
//	/* 	Cards --> 1,2,3 */
//	uint8_t 	u8_data_resp[4004] = {0};
//	uint8_t 	u8_c_buff[MAX_LEN] = {0};
//	long int 		i_read_count,i_ret = 0;
//	int32_t 	*p32_data = NULL;
//	int32_t 	*p32_data_shm[4] ;
//	int32_t 	i32_data = 0;
//
//	uint8_t 	u8_state = e_REQ_DATA;
//	uint8_t		u8_ret = 0,u8_req_count = 0;
//	uint16_t 	u16_iteration = 0,u16_max_iter = 0;
//	uint16_t 	u16_boards 	= 0;
//
//	uint32_t 	u32_count = 0;
//
//	FILE *log_MB_DCRM;
//
//	int i_sem_ret;
//
//	log_MB_DCRM = fopen("log_MB_DCRM.txt","w+");
//	if(log_MB_DCRM == NULL)
//	{
//		perror("");
//		exit(1);
//	}
//
//	printf("MB Unit & DCRM thread\r\n");
//
//	while(1)
//	{
//		printf("waiting for sem MB Unit\r\n");
//		i_sem_ret = sem_wait(sem_mb_dcrm);
//		printf("semphore MB Unit success\r\n");
//
//		/* 	There are 2 types of tests
//		 * 	1. Hardware Config
//		 * 	2. User Tests
//		 */
//		u8_state = u8_req_configured;
//
//		switch(u8_state)
//		{
//			/*	shift the handshake request in a single function */
////			case e_REQ_HANDSHAKE :
////			break;
//
//			case e_REQ_HANDSHAKE :
//				gpio_rs485_tx_en();
//				usleep(5);
//
//
//				u8_req_count = 0;
//				for(u16_iteration = 1 ; u16_iteration <= 2 ; )
//				{
//					frame_t.u16_header			= 0xAAAA;
//					frame_t.u16_card_address 	= u16_iteration;
//					frame_t.u16_activity		= 0x0001;
//					frame_t.u16_identifier		= 0x5B5B;
//					frame_t.u16_payload			= 22;
//					frame_t.u16_footer			= 0x5555;
//					printf("Req Handshake\r\n");
//					fprintf(log_MB_DCRM,"DB %d HSR\r\n",u16_iteration);
//
//					custom_uart_fifo_reset(p32_RS485_MB_UNIT);
//					frame_req_MB_DCRM_unit(&frame_t,i_ret);
//
//					memset(u8_c_buff,0,MAX_LEN);
//					usleep(60000);
//
//					i_read_count = custom_uart_RS485_read((uint8_t*)(u8_c_buff),56);
//					printf("rd %d\r\n",i_read_count);
//					fprintf(log_MB_DCRM,"rd %d\r\n",i_read_count);
//
//					/*	comment the below line of after testing will be complete */
//	//				i_read_count = 2;
//					if(i_read_count > 0)
//					{
//						uint16_t *p16_d_1 = (u8_c_buff + 1);
//	//					for(u16_count = 0 ; u16_count < 28 ; u16_count++)
//	//					{
//	//						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
//	//					}
//						u8_ret = frame_respone_validate((p16_d_1),i_read_count);
//						if(u8_ret == 0)
//						{
//							if(p16_d_1[2] == 0x5C5C)
//							{
//	//							u8_state = e_REQ_TEST_SETTINGS;
//
//								/*	TODO : send message over socket for present data sampling channels
//								 * 			present in the particular Daughter Board
//								 */
//
//								printf("Handshake Response\r\n");
//								fprintf(log_MB_DCRM,"DB %d HSR Success\r\n",u16_iteration);
//								set_channels(p16_d_1[5]);
//								u16_iteration++;
//							}
//							else
//							{
//								printf("Response Error %x\r\n",p16_d_1[5]);
//								fprintf(log_MB_DCRM,"DB %d HSR DB Response Error\r\n",u16_iteration);
//							}
//						}
//						else
//						{
//							/* 	set the flag to send the request again to the
//							 * 	corresponding Daughter board with current activity
//							 */
//							u8_req_count++;
//	//						u8_state = e_REQ_HANDSHAKE;
//							printf("DB %d\r\n",u16_iteration);
//							fprintf(log_MB_DCRM,"DB %d HSR Frame Validation Error %d\r\n",u16_iteration,u8_ret);
//							if(u8_req_count > 2)
//							{
//								/*	if consecutive request count exceeds 3 iterations then
//								 * 	send the message to High level software that particular
//								 * 	channels of Corresponding Daughter Boards are not
//								 * 	responding to Handshake request and give message to user
//								 */
//								printf("Error in Communication with %d\r\n",u16_iteration);
//
//								/*	send the Error response of activity on the socket */
//								sem_wait(sem_write_socket);
//								socket_resp(u8_test_configured,ERROR);
//								sem_post(sem_write_socket);
//
//								u16_iteration++;
//								u8_req_count = 0;
//							}
//
//						}
//					}
//					else
//					{
//						u8_req_count++;
//						fprintf(log_MB_DCRM,"DB %d HSR Data Read Error %d\r\n",u16_iteration,i_read_count);
//						if(u8_req_count > 2)
//						{
//							/*	if consecutive request count exceeds 3 iterations then
//							 * 	send the message to High level software that particular
//							 * 	channels of Corresponding Daughter Boards are not
//							 * 	responding to Handshake request and give message to user
//							 */
//							printf("Error in Communication with %d\r\n",u16_iteration);
//							/*	send the Error response of activity on the socket */
//							sem_wait(sem_write_socket);
//							socket_resp(u8_test_configured,ERROR);
//							sem_post(sem_write_socket);
//
//							u16_iteration++;
//							u8_req_count = 0;
//						}
//					}
//				}
//
//				break;
//
//			case e_REQ_TEST_SETTINGS :
//
//				break;
//
//			case e_OPTO_WAIT :
//
//				break;
//
//			case e_REQ_DATA :
//
//				break;
//			break;
//		}
//	}
//}

/*------------------------- Communication Code with MB Unit and DCRMS Units Ends --------------*/

/* --------------------------------------------------------------------------------------------*/

void frame_req_slot_1(frame_info *pt_frame,uint16_t u16_error)
{
	uint16_t u16_data[30] = {0};

	u16_data[0] 	= pt_frame->u16_header;
	u16_data[1]		= pt_frame->u16_card_address;
	//	if(u16_error != e_NO_ERR)
	//	{
	//		u16_data[2]	= 0x8000 | pt_frame->u16_identifier;
	//		//printf("identifier %x\r\n",u16_data[2]);
	//	}
	//	else
	//	{
	u16_data[2] = pt_frame->u16_identifier;
	//printf("identifier %x\r\n",u16_data[2]);
	//	}
	u16_data[3]		= pt_frame->u16_activity;
	u16_data[4]		= pt_frame->u16_payload;

	if(u16_error != e_NO_ERR)
	{
		u16_data[5] = u16_error;
	}
	else
	{
		for(uint8_t u8_count = 5 ; u8_count < 26 ; u8_count++)
		{
			u16_data[u8_count] = pt_frame->p16_data[u8_count - 5];
		}
	}
	u16_data[26] = 0xBBBB;//pt_frame->u16_checksum;
	u16_data[27] = pt_frame->u16_footer;

	//	u16_data[7]	= 0x05;
	//	u16_data[8] = 0x06;
	//	u16_data[9] = 0x07;
	//	u16_data[10]= 0x08;


	/*	Below code is for testing purpose comment it after testing is over */
//				for(int i = 0 ; i < 30 ; i++)
//				{
//					printf("data[%d] = %x\r\n",i,u16_data[i]);
//					//u16_data[i] = 0x5555;
//				}



	custom_uart_1_write((uint8_t *)u16_data,58);
	//write(uart1_fd,(void *)u16_data,56);
	//printf("S_1: Req\r\n");

}

void frame_req_slot_2(frame_info *pt_frame,uint16_t u16_error)
{
	uint16_t u16_data[28] = {0};

	u16_data[0] 	= pt_frame->u16_header;
	u16_data[1]		= pt_frame->u16_card_address;
	if(u16_error != e_NO_ERR)
	{
		u16_data[2]	= 0x8000 | pt_frame->u16_identifier;
		//printf("identifier %x\r\n",u16_data[2]);
	}
	else
	{
		u16_data[2] = pt_frame->u16_identifier;
		//printf("identifier %x\r\n",u16_data[2]);
	}
	u16_data[3]		= pt_frame->u16_activity;
	u16_data[4]		= pt_frame->u16_payload;

	if(u16_error != e_NO_ERR)
	{
		u16_data[5] = u16_error;
	}
	else
	{
		for(uint8_t u8_count = 5 ; u8_count < 26 ; u8_count++)
		{
			u16_data[u8_count] = pt_frame->p16_data[u8_count - 5];
		}
	}
	u16_data[26] = pt_frame->u16_checksum;
	u16_data[27] = pt_frame->u16_footer;

	custom_uart_2_write((uint8_t *)u16_data,58);
	//write(uart2_fd,(void *)u16_data,56);

//	printf("S_2: Req\r\n");

}

void frame_req_slot_3(frame_info *pt_frame,uint16_t u16_error)
{
	uint16_t u16_data[28] = {0};

	u16_data[0] 	= pt_frame->u16_header;
	u16_data[1]		= pt_frame->u16_card_address;
	if(u16_error != e_NO_ERR)
	{
		u16_data[2]	= 0x8000 | pt_frame->u16_identifier;
		//printf("identifier %x\r\n",u16_data[2]);
	}
	else
	{
		u16_data[2] = pt_frame->u16_identifier;
		//printf("identifier %x\r\n",u16_data[2]);
	}
	u16_data[3]		= pt_frame->u16_activity;
	u16_data[4]		= pt_frame->u16_payload;

	if(u16_error != e_NO_ERR)
	{
		u16_data[5] = u16_error;
	}
	else
	{
		for(uint8_t u8_count = 5 ; u8_count < 26 ; u8_count++)
		{
			u16_data[u8_count] = pt_frame->p16_data[u8_count - 5];
		}
	}
	u16_data[26] = pt_frame->u16_checksum;
	u16_data[27] = pt_frame->u16_footer;

	custom_uart_3_write((uint8_t *)u16_data,58);
	//write(uart2_fd,(void *)u16_data,56);

//	printf("S_2: Req\r\n");
}

void frame_req_slot_4(frame_info *pt_frame,uint16_t u16_error)
{
	uint16_t u16_data[28] = {0};

	u16_data[0] 	= pt_frame->u16_header;
	u16_data[1]		= pt_frame->u16_card_address;
	if(u16_error != e_NO_ERR)
	{
		u16_data[2]	= 0x8000 | pt_frame->u16_identifier;
		//printf("identifier %x\r\n",u16_data[2]);
	}
	else
	{
		u16_data[2] = pt_frame->u16_identifier;
		//printf("identifier %x\r\n",u16_data[2]);
	}
	u16_data[3]		= pt_frame->u16_activity;
	u16_data[4]		= pt_frame->u16_payload;

	if(u16_error != e_NO_ERR)
	{
		u16_data[5] = u16_error;
	}
	else
	{
		for(uint8_t u8_count = 5 ; u8_count < 26 ; u8_count++)
		{
			u16_data[u8_count] = pt_frame->p16_data[u8_count - 5];
		}
	}

		u16_data[7]	= 0x05;
		u16_data[8] = 0x06;
		u16_data[9] = 0x07;
		u16_data[10]= 0xDD;
		u16_data[11]= 0x09;
		u16_data[12]= 0x0A;
		u16_data[13]= 0x0B;
		u16_data[14]= 0x0C;
		u16_data[15]= 0x0D;
		u16_data[16]= 0xBB;


	u16_data[26] = pt_frame->u16_checksum;
	u16_data[27] = pt_frame->u16_footer;

//	/*	Below code is for testing purpose comment it after testing is over */
//	for(int i = 0 ; i < 30 ; i++)
//	{
//		printf("data[%d] = %x\r\n",i,u16_data[i]);
//		//u16_data[i] = 0x5555;
//	}

	custom_uart_4_write((uint8_t *)u16_data,58);
	//write(uart2_fd,(void *)u16_data,56);

//	printf("S_2: Req\r\n");
//	printf("\r\n");
}

void frame_req_MB_DCRM_unit(frame_info *pt_frame,uint16_t u16_error)
{
	uint16_t u16_data[30] = {0};

	u16_data[0] 	= pt_frame->u16_header;
	u16_data[1]		= pt_frame->u16_card_address;
	//	if(u16_error != e_NO_ERR)
	//	{
	//		u16_data[2]	= 0x8000 | pt_frame->u16_identifier;
	//		//printf("identifier %x\r\n",u16_data[2]);
	//	}
	//	else
	//	{
	u16_data[2] = pt_frame->u16_identifier;
	//printf("identifier %x\r\n",u16_data[2]);
	//	}
	u16_data[3]		= pt_frame->u16_activity;
	u16_data[4]		= pt_frame->u16_payload;

	if(u16_error != e_NO_ERR)
	{
		u16_data[5] = u16_error;
	}
	else
	{
		for(uint8_t u8_count = 5 ; u8_count < 26 ; u8_count++)
		{
			u16_data[u8_count] = pt_frame->p16_data[u8_count - 5];
		}
	}
	u16_data[26] = 0xBBBB;//pt_frame->u16_checksum;
	u16_data[27] = pt_frame->u16_footer;

	//	u16_data[7]	= 0x05;
	//	u16_data[8] = 0x06;
	//	u16_data[9] = 0x07;
	//	u16_data[10]= 0x08;


	/*	Below code is for testing purpose comment it after testing is over */
//				for(int i = 0 ; i < 30 ; i++)
//				{
//					printf("data[%d] = %x\r\n",i,u16_data[i]);
//					//u16_data[i] = 0x5555;
//				}



	custom_uart_RS485_write((uint8_t *)u16_data,58);
	//write(uart1_fd,(void *)u16_data,56);
	//printf("S_1: Req\r\n");

}


uint16_t frame_respone_validate(uint16_t *p_frame,uint32_t u32_len)
{
//	printf("DB frame validation\r\n");
	uint16_t u16_checksum = 0;
	uint16_t u16_errors = 0;

	if(p_frame[HEADER] != 0xAAAA)
		u16_errors |=  e_ERR_HEADER;

	if((p_frame[CARD_ADDRESS] < 1) && (p_frame[CARD_ADDRESS] > 12))
		u16_errors |= e_ERR_CARD_ADDRESS;

	if((p_frame[FRAME_IDENTIFIER] != 0x5C5C) && (p_frame[FRAME_IDENTIFIER] != 0x5D5D))
		u16_errors |= e_ERR_FRAME_IDENTIFIER;

	if((p_frame[ACTIVITY_CODE] < 0) && (p_frame[ACTIVITY_CODE] > 255))
		u16_errors |= e_ERR_ACTIVITY_CDOE;

	//	u16_checksum = checksum_calculate(p_frame,u32_len);
	//	if(p_frame[FRAME_IDENTIFIER] == 0x72)
	//	{
	//		if(p_frame[D_CHECKSUM] != u16_checksum)
	//			u16_errors |= e_ERR_CHECKSUM;
	//	}
	//	else
	//	{
	//		if(p_frame[CC_CHECKSUM] != u16_checksum)
	//			u16_errors |= e_ERR_CHECKSUM;
	//	}

	if(u16_errors > 0)
		return u16_errors;
	else
		return e_NO_ERR;

}

uint16_t farme_checksum(uint16_t* data_frame,uint16_t length_of_frame)
{
	uint32_t sum = 0;
	for(int temp=0;temp<(length_of_frame-2);temp++)
	{
		sum += *(data_frame + temp);
	}
	return sum;
}

void init_card_info(void)
{
	//card_info cards_t[11] = {
	//		{.u8_slot = 0x01,.u8_card_address = 0x01},
	//		{.u8_slot = 0x01,.u8_card_address = 0x02},
	//		{.u8_slot = 0x01,.u8_card_address = 0x03},
	//		{.u8_slot = 0x02,.u8_card_address = 0x04},
	//		{.u8_slot = 0x02,.u8_card_address = 0x05},
	//		{.u8_slot = 0x02,.u8_card_address = 0x06},
	//		{.u8_slot = 0x03,.u8_card_address = 0x07},
	//		{.u8_slot = 0x03,.u8_card_address = 0x08},
	//		{.u8_slot = 0x03,.u8_card_address = 0x09},
	//		{.u8_slot = 0x04,.u8_card_address = 0x0A},
	//		{.u8_slot = 0x04,.u8_card_address = 0x0B},
	//};

	printf("Cards initialization\r\n");
	frame_t.u16_header = 0xAAAA;
	frame_t.u16_footer = 0x5555;

	cards_t[e_CARD_1].u8_slot = 0x01;
	cards_t[e_CARD_2].u8_slot = 0x01;
	cards_t[e_CARD_3].u8_slot = 0x01;
	cards_t[e_CARD_1].u8_board = e_Analog_Baord;

	/*------------------------- Card 1 config -------------------------------*/

	cards_t[e_CARD_1].p_data_files[0] = u_data_ptr.st_data_shm.p_ana_ch_1;
	printf("p_ana_ch_1 %p\r\n",u_data_ptr.st_data_shm.p_ana_ch_1);

	cards_t[e_CARD_1].p_data_files[1] = u_data_ptr.st_data_shm.p_ana_ch_2;
	cards_t[e_CARD_1].p_data_files[2] = u_data_ptr.st_data_shm.p_ana_ch_3;
	cards_t[e_CARD_1].p_data_files[3] = u_data_ptr.st_data_shm.p_ana_ch_4;

	cards_t[e_CARD_1].u8_channel[0] = e_ANA_CH_1;
	cards_t[e_CARD_1].u8_channel[1] = e_ANA_CH_2;
	cards_t[e_CARD_1].u8_channel[2] = e_ANA_CH_3;
	cards_t[e_CARD_1].u8_channel[3] = e_ANA_CH_4;

	/*------------------------- Card 2 config -------------------------------*/
	cards_t[e_CARD_2].u8_board = e_Analog_Baord;
	cards_t[e_CARD_2].p_data_files[0] = u_data_ptr.st_data_shm.p_ana_ch_5;
	cards_t[e_CARD_2].p_data_files[1] = u_data_ptr.st_data_shm.p_ana_ch_6;
	cards_t[e_CARD_2].p_data_files[2] = u_data_ptr.st_data_shm.p_ana_ch_7;
	cards_t[e_CARD_2].p_data_files[3] = u_data_ptr.st_data_shm.p_ana_ch_8;

	cards_t[e_CARD_2].u8_channel[0] = e_ANA_CH_5;
	cards_t[e_CARD_2].u8_channel[1] = e_ANA_CH_6;
	cards_t[e_CARD_2].u8_channel[2] = e_ANA_CH_7;
	cards_t[e_CARD_2].u8_channel[3] = e_ANA_CH_8;

	/*------------------------- Card 3 config -------------------------------*/
	cards_t[e_CARD_3].u8_board = e_Analog_Baord;
	cards_t[e_CARD_3].p_data_files[0] = u_data_ptr.st_data_shm.p_ana_ch_9;
	cards_t[e_CARD_3].p_data_files[1] = u_data_ptr.st_data_shm.p_ana_ch_10;
	cards_t[e_CARD_3].p_data_files[2] = u_data_ptr.st_data_shm.p_ana_ch_11;
	cards_t[e_CARD_3].p_data_files[3] = u_data_ptr.st_data_shm.p_ana_ch_12;

	cards_t[e_CARD_3].u8_channel[0] = e_ANA_CH_9;
	cards_t[e_CARD_3].u8_channel[1] = e_ANA_CH_10;
	cards_t[e_CARD_3].u8_channel[2] = e_ANA_CH_11;
	cards_t[e_CARD_3].u8_channel[3] = e_ANA_CH_12;


	cards_t[e_CARD_4].u8_slot = 0x02;
	cards_t[e_CARD_5].u8_slot = 0x02;
	cards_t[e_CARD_6].u8_slot = 0x02;

	cards_t[e_CARD_7].u8_slot = 0x03;
	cards_t[e_CARD_8].u8_slot = 0x03;
	cards_t[e_CARD_9].u8_slot = 0x03;

	cards_t[e_CARD_10].u8_slot = 0x04;
	cards_t[e_CARD_11].u8_slot = 0x04;

	cards_t[e_CARD_1].u8_card_address = 1;
	cards_t[e_CARD_2].u8_card_address = 2;
	cards_t[e_CARD_3].u8_card_address = 3;

	cards_t[e_CARD_4].u8_card_address = 4;
	cards_t[e_CARD_5].u8_card_address = 5;
	cards_t[e_CARD_6].u8_card_address = 6;

	cards_t[e_CARD_7].u8_card_address = 7;
	cards_t[e_CARD_8].u8_card_address = 8;
	cards_t[e_CARD_9].u8_card_address = 9;

	cards_t[e_CARD_10].u8_card_address = 01;

	cards_t[e_CARD_10].p_data_files[0] = u_data_ptr.st_data_shm.p_travel_1;
	cards_t[e_CARD_10].p_data_files[1] = u_data_ptr.st_data_shm.p_travel_2;
	cards_t[e_CARD_10].p_data_files[2] = u_data_ptr.st_data_shm.p_travel_3;
	cards_t[e_CARD_10].p_data_files[3] = u_data_ptr.st_data_shm.p_travel_4;

	cards_t[e_CARD_10].u8_channel[0] = e_TRAVEL_1;
	cards_t[e_CARD_10].u8_channel[1] = e_TRAVEL_2;
	cards_t[e_CARD_10].u8_channel[2] = e_TRAVEL_3;
	cards_t[e_CARD_10].u8_channel[3] = e_TRAVEL_4;


	cards_t[e_CARD_11].u8_card_address = 02;

	printf("Cards initialized\r\n");
}

void create_data_files()
{
	int i_segment_size = 450000;
//	char *p_coil_1 = "/coil_current_1";
//	char *p_coil_2 = "/coil_current_2";
//	char *p_coil_3 = "/coil_current_3";
//	char *p_coil_4 = "/coil_current_4";
//	char *p_coil_5 = "/coil_current_5";
//	char *p_coil_6 = "/coil_current_6";
//
//	char *p_main_pir = "/main_pir_contact";
//	char *p_aux = "/auxilary_contact";

	uint32_t u32_count = 0;
	char data_shm_objects[50][30] = {	"/coil_current_1",
										"/coil_current_2",
										"/coil_current_3",
										"/coil_current_4",
										"/coil_current_5",
										"/coil_current_6",
										"/sdcrm_1_v_1",
										"/sdcrm_2_v_1",
										"/sdcrm_3_v_1",
										"/sdcrm_4_v_1",
										"/sdcrm_5_v_1",
										"/sdcrm_6_v_1",
										"/sdcrm_1_v_2",
										"/sdcrm_2_v_2",
										"/sdcrm_3_v_2",
										"/sdcrm_4_v_2",
										"/sdcrm_5_v_2",
										"/sdcrm_6_v_2",
										"/sdcrm_1_c_1",
										"/sdcrm_2_c_1",
										"/sdcrm_3_c_1",
										"/sdcrm_4_c_1",
										"/sdcrm_5_c_1",
										"/sdcrm_6_c_1",
										"/sdcrm_1_c_2",
										"/sdcrm_2_c_2",
										"/sdcrm_3_c_2",
										"/sdcrm_4_c_2",
										"/sdcrm_5_c_2",
										"/sdcrm_6_c_2",
										"/travel_1",
										"/travel_2",
										"/travel_3",
										"/travel_4",
										"/travel_5",
										"/travel_6",
										"/analog_channel_1",
										"/analog_channel_2",
										"/analog_channel_3",
										"/analog_channel_4",
										"/analog_channel_5",
										"/analog_channel_6",
										"/analog_channel_7",
										"/analog_channel_8",
										"/analog_channel_9",
										"/analog_channel_10",
										"/analog_channel_11",
										"/analog_channel_12",
										"/main_pir_contact",
										"/auxilary_contact"
	};

	/*	creating shared memory objects */
	for(u32_count = 0 ; u32_count < 50 ; u32_count++)
	{
		int fd = shm_open(data_shm_objects[u32_count], O_CREAT | O_RDWR, 0666);
		if(fd == -1)
		{
			printf("shm object Error %s\r\n",data_shm_objects[u32_count]);
		}

		ftruncate(fd,i_segment_size);

	    void *mem = mmap(NULL, i_segment_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	    if (mem == MAP_FAILED) {
	    	printf("memory map Error %s",data_shm_objects[u32_count]);
	        //return -1;
	        while(1);
	    }

	    u_data_ptr.p_data_shm[u32_count] = (int32_t*)mem;

//	    printf("mem %p\r\n",mem);
//	    printf("shm oject %s at %p\r\n",data_shm_objects[u32_count],u_data_ptr.p_data_shm[u32_count]);

	}

//	printf("%s = %p\r\n",data_shm_objects[0],u_data_ptr.st_data_shm.p_coil_1);
//	printf("%s = %p\r\n",data_shm_objects[1],u_data_ptr.st_data_shm.p_coil_2);
//	printf("%s = %p\r\n",data_shm_objects[2],u_data_ptr.st_data_shm.p_coil_3);
//	printf("%s = %p\r\n",data_shm_objects[3],u_data_ptr.st_data_shm.p_coil_4);
//	printf("%s = %p\r\n",data_shm_objects[4],u_data_ptr.st_data_shm.p_coil_5);
//	printf("%s = %p\r\n",data_shm_objects[5],u_data_ptr.st_data_shm.p_coil_6);

	printf("data files creation complete\r\n");

}


//	Below function is to set the Channel numbers and their respective shared memory
// 	objects with data structure for particular Daughter Card in slot

void set_channels(uint8_t u8_card_address)
{
	switch(u8_card_address)
	{

		//	change the channel shared objects as per the requirements from the
		//
		case e_CARD_1:
			printf("Card address: 01\r\n");
			printf("p_ana_ch_1 %p\r\n",u_data_ptr.st_data_shm.p_ana_ch_1);
			cards_t[e_CARD_1].p_data_files[0] = u_data_ptr.st_data_shm.p_ana_ch_1;
			cards_t[e_CARD_1].p_data_files[1] = u_data_ptr.st_data_shm.p_ana_ch_2;
			cards_t[e_CARD_1].p_data_files[2] = u_data_ptr.st_data_shm.p_ana_ch_3;
			cards_t[e_CARD_1].p_data_files[3] = u_data_ptr.st_data_shm.p_ana_ch_4;

			break;
		case e_CARD_2:
			printf("Card address: 02\r\n");
			cards_t[e_CARD_2].p_data_files[0] = u_data_ptr.st_data_shm.p_ana_ch_5;
			cards_t[e_CARD_2].p_data_files[1] = u_data_ptr.st_data_shm.p_ana_ch_6;
			cards_t[e_CARD_2].p_data_files[2] = u_data_ptr.st_data_shm.p_ana_ch_7;
			cards_t[e_CARD_2].p_data_files[3] = u_data_ptr.st_data_shm.p_ana_ch_8;

			break;
		case e_CARD_3:
			printf("Card address: 03\r\n");
			cards_t[e_CARD_3].p_data_files[0] = u_data_ptr.st_data_shm.p_ana_ch_9;
			cards_t[e_CARD_3].p_data_files[1] = u_data_ptr.st_data_shm.p_ana_ch_10;
			cards_t[e_CARD_3].p_data_files[2] = u_data_ptr.st_data_shm.p_ana_ch_11;
			cards_t[e_CARD_3].p_data_files[3] = u_data_ptr.st_data_shm.p_ana_ch_12;

			break;
		case e_CARD_4:
			printf("Card address: 04\r\n");

			break;
		case e_CARD_5:
			printf("Card address: 05\r\n");

			break;
		case e_CARD_6:
			printf("Card address: 06\r\n");

			break;
		case e_CARD_7:
			printf("Card address: 07\r\n");

			break;
		case e_CARD_8:
			printf("Card address: 08\r\n");

			break;
		case e_CARD_9:
			printf("Card address: 09\r\n");

			break;
		case e_CARD_10:
			printf("Card address: 10\r\n");

			break;
		case e_CARD_11:
			printf("Card address: 11\r\n");

			break;
		default :
			printf("Wrong Card Address\r\n");
			break;
	}
}


void frame_arrange(uint8_t *p8_req,uint8_t *p8_frame)
{
    uint16_t count = 0 , len = 4000;
    while(*p8_req != 0xAA)
        p8_req++;
    for(count = 0; count < len ; count++)
    {
        p8_frame[count] = p8_req[count];
    }
}

void socket_resp(uint16_t u16_activity, uint16_t u16_ack_err)
{
	uint16_t 	u16_sock_tx[6] = {0xBBBB,0,0,0,0,0x5555};

	/*	send the Error response of activity on the socket */
	u16_sock_tx[ACTIVITY] = u16_activity;

	/*	setting the Error bytes in the socket request */
	u16_sock_tx[ACK_ERR] = u16_ack_err;

	/* TODO :below code is added for testing purpose remove after testing : HT*/
//	uint8_t u16_sock_tx[] = "Error";
	for(int count = 0 ; count < 6 ; count++)
	{
		printf("socket[%d] = %x\r\n",count,u16_sock_tx[count]);
		fflush(stdout);
	}

	write(rec_fd,u16_sock_tx,/*7*/sizeof(u16_sock_tx));

}

void channel_data_rec_resp(uint8_t u8_data_file)
{
	/* 	For the reference of Activity Codes please refer the Google Sheet prepared
	 * 	"HL_LL_SOC_COMM_ACTIVITIES"
	 * 	Link : https://docs.google.com/spreadsheets/d/1QftMSRNq4J1zHj5oUTjRXPLYA1rD6ilGQodx-Y-HAX8/edit?gid=220058638#gid=220058638
	 */

	uint16_t u16_activity = 0;
	switch(u8_data_file)
	{
	case e_COIL_1:
		u16_activity = 0x3E;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_COIL_2:
		u16_activity = 0x3F;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_COIL_3:
		u16_activity = 0x40;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_COIL_4:
		u16_activity = 0x41;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_COIL_5:
		u16_activity = 0x42;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_COIL_6:
		u16_activity = 0x43;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;


	case e_SDCRM_1_V_1:
		u16_activity = 0x32;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_2_V_1:
		u16_activity = 0x33;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_3_V_1:
		u16_activity = 0x34;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_4_V_1:
		u16_activity = 0x35;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_5_V_1:
		u16_activity = 0x36;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_6_V_1:
		u16_activity = 0x37;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_1_C_1:
		u16_activity = 0x38;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_2_C_1:
		u16_activity = 0x39;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_3_C_1:
		u16_activity = 0x3A;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_4_C_1:
		u16_activity = 0x3B;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_5_C_1:
		u16_activity = 0x3C;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_6_C_1:
		u16_activity = 0x3D;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_SDCRM_1_V_2 :
		u16_activity = 0x83;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case	e_SDCRM_2_V_2:
		u16_activity = 0x84;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case	e_SDCRM_3_V_2:
		u16_activity = 0x85;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case	e_SDCRM_4_V_2:
		u16_activity = 0x86;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case	e_SDCRM_5_V_2:
		u16_activity = 0x87;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case	e_SDCRM_6_V_2:
		u16_activity = 0x88;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;


	case e_SDCRM_1_C_2:
		u16_activity = 0x89;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case e_SDCRM_2_C_2:
		u16_activity = 0x8A;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case e_SDCRM_3_C_2:
		u16_activity = 0x8B;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case e_SDCRM_4_C_2:
		u16_activity = 0x8C;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case e_SDCRM_5_C_2:
		u16_activity = 0x8D;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;
	case e_SDCRM_6_C_2:
		u16_activity = 0x8E;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_TRAVEL_1:
		u16_activity = 0x68;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_TRAVEL_2:
		u16_activity = 0x69;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_TRAVEL_3:
		u16_activity = 0x6A;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_TRAVEL_4:
		u16_activity = 0x6B;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_TRAVEL_5:
		u16_activity = 0x6C;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_TRAVEL_6:
		u16_activity = 0x6D;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_1:
		u16_activity = 0x6E;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_2:
		u16_activity = 0x6F;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_3:
		u16_activity = 0x70;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_4:
		u16_activity = 0x71;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_5:
		u16_activity = 0x72;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_6:
		u16_activity = 0x73;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_7:
		u16_activity = 0x74;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_8:
		u16_activity = 0x75;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_9:
		u16_activity = 0x76;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_10:
		u16_activity = 0x77;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_11:
		u16_activity = 0x78;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_ANA_CH_12:
		u16_activity = 0x79;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_MAIN_PIR:
		u16_activity = 0x5D;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	case e_AUX:
		u16_activity = 0x5C;
		socket_resp(u16_activity,ACKNOWLEDGE);
		break;

	}
}


int opto_wait()
{
	int i_status = -1;

	uint8_t u8_opto_sig[4] = {'O','O','O','O'};
	uint8_t u8_opto_received = 0;
	uint8_t u8_opto_success = 0;
	uint8_t u8_rx_data[2] = {0};

	int i_read_count = 0;

	uint16_t u16_msg_norm[28]= {
			0xAAAA,
			0x5555
	};
	/* 	TODO : confirm the method of detecting the OPTO Trigger
	 * 	1. whether it is interrupt based
	 * 	2. Uart based : single character singal
	 * 	there should be method to break the infinite loop receiving the Opto Trigger
	 * 	signal : timer interrupt/exception
	 */

	/*	set the "opto checking" global flag */
	u8_opto_check = 1;

	/*	wait for "Opto Trigger" for specified period of time */
	/*	set the flag "u8_opto_received" after receiving the correct Opto trigger */

	/*	wait for opto on the "Custom_Uart" which will be used for communicating with
	 * 	MB unit with Baud_rate upto 5-10 Mhz here also the timer section should be
	 * 	implemented of about 15 sec as if No "opto" signal is received in that 15 secs
	 * 	then CPU Board should send the Broad Cast message of Normal state to every slot
	 * 	and MB unit.
	 */
	printf("receiving opto\r\n");
	while(u8_opto_received != 1)
	{
		custom_uart_fifo_reset(p32_axi_uart_4);
		i_read_count = custom_uart_4_read(u8_rx_data,2);
		if(u8_rx_data[0] == 'O' || u8_rx_data[1] == 'O')
		{
			u8_opto_received = 1;
			u8_opto_success = 1;

			i_status = 0;
		}
		else if(u8_rx_data[0] == 'F' || u8_rx_data[1] == 'F')
		{
			u8_opto_received = 1;
			u8_opto_success = 0;

			i_status = -1 ;
		}
		else
		{
			u8_opto_received = 0;
		}
	}


	/*	After receiving the Opto Trigger signal send the broad cast signal to all
	 * 	Daughter Boards for start sampling for configured test
	 * 	1.	Send broad Cast message to slot 1.
	 * 	2. 	Send broad Cast message to slot 2.
	 * 	3. 	Send broad Cast message to slot 3.
	 * 	4.	Send broad Cast message to slot 4.
	 */
	if(u8_opto_success == 1)
	{
		custom_uart_1_write(u8_opto_sig,4);
		custom_uart_2_write(u8_opto_sig,4);
		custom_uart_3_write(u8_opto_sig,4);
		custom_uart_4_write(u8_opto_sig,4);

		printf("opto received\r\n");
		printf("Opto reception Command sent\r\n");
		/*	also send the broad cast message to all DCRM's using only one message */
	}
	else
	{

		printf("opto not received\r\n");
	}

	return i_status;
}


int config_hardware(uint8_t u8_slot,uint8_t u8_card_address)
{
	/*	there are total 6 slots
	 * 	Slot_1
	 * 	Slot_2
	 * 	Slot_3
	 * 	Slot_4
	 * 	MB_Unit
	 * 	DCRM_Units
	 */
	int i_status = -1,i_read_count = 0;
	uint8_t u8_c_buff[HDS_ARR_LEN] = {0};
	uint8_t u8_ret = 0;

	frame_t.u16_header			= 0xAAAA;
	frame_t.u16_card_address 	= u8_card_address;
	frame_t.u16_activity		= 0x0001;			// Handshake activity
	frame_t.u16_identifier		= 0x5B5B;
	frame_t.u16_payload			= 22;
	frame_t.u16_footer			= 0x5555;

	if(u8_slot == SLOT_1)
	{
		printf("sending command slot 1 %d\r\n",u8_card_address);
		custom_uart_fifo_reset(p32_axi_uart_1);
		frame_req_slot_1(&frame_t,e_NO_ERR);
		memset(u8_c_buff,0,HDS_ARR_LEN);
		usleep(5000);

		i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),HDS_ARR_LEN);
		printf("rd %d\r\n",i_read_count);
		if(i_read_count > 0)
		{
			uint16_t *p16_d_1 = (u8_c_buff + 1);
			u8_ret = frame_respone_validate((p16_d_1),i_read_count);
			if(u8_ret == 0)
			{
				if(p16_d_1[2] == 0x5C5C)
				{
					printf("Card Address %d HDR success\r\n",u8_card_address);
					return u8_card_address;
				}
				else
				{
					printf("Card Address %d HDR Error\r\n",u8_card_address);
					return i_status = -1;
				}
			}
		}
		else
		{
			printf("Slot %d Data read Error\r\n",SLOT_1);
			return i_status = -1;
		}
	}
	else if(u8_slot == SLOT_2)
	{
		printf("sending command slot 2 %d\r\n",u8_card_address);
		custom_uart_fifo_reset(p32_axi_uart_2);
		frame_req_slot_2(&frame_t,e_NO_ERR);
		memset(u8_c_buff,0,HDS_ARR_LEN);
		usleep(5000);

		i_read_count = custom_uart_2_read((uint8_t*)(u8_c_buff),HDS_ARR_LEN);
		printf("rd %d\r\n",i_read_count);

		if(i_read_count > 0)
		{
			uint16_t *p16_d_1 = (u8_c_buff + 1);
			u8_ret = frame_respone_validate((p16_d_1),i_read_count);
			if(u8_ret == 0)
			{
				if(p16_d_1[2] == 0x5C5C)
				{
					printf("Card Address %d HDR success\r\n",u8_card_address);
					return u8_card_address;
				}
				else
				{
					printf("Card Address %d HDR Error\r\n",u8_card_address);
					return i_status = -1;
				}
			}
		}
		else
		{
			printf("Slot %d Data read Error\r\n",SLOT_2);
			return i_status = -1;
		}
	}
	else if(u8_slot == SLOT_3)
	{
		printf("sending command slot 3 %d\r\n",u8_card_address);
		custom_uart_fifo_reset(p32_axi_uart_3);
		frame_req_slot_3(&frame_t,e_NO_ERR);
		memset(u8_c_buff,0,HDS_ARR_LEN);
		usleep(5000);

		i_read_count = custom_uart_3_read((uint8_t*)(u8_c_buff),HDS_ARR_LEN);
		printf("rd %d\r\n",i_read_count);

		if(i_read_count > 0)
		{
			uint16_t *p16_d_1 = (u8_c_buff + 1);
			u8_ret = frame_respone_validate((p16_d_1),i_read_count);
			if(u8_ret == 0)
			{
				if(p16_d_1[2] == 0x5C5C)
				{
					printf("Card Address %d HDR success\r\n",u8_card_address);
					return u8_card_address;
				}
				else
				{
					printf("Card Address %d HDR Error\r\n",u8_card_address);
					return i_status = -1;
				}
			}
		}
		else
		{
			printf("Slot %d Data read Error\r\n",SLOT_3);
			return i_status = -1;
		}
	}
	else if(u8_slot == SLOT_4)
	{
		printf("sending command slot 4 %d\r\n",u8_card_address);
		custom_uart_fifo_reset(p32_axi_uart_4);
		frame_req_slot_4(&frame_t,e_NO_ERR);
		memset(u8_c_buff,0,HDS_ARR_LEN);
		usleep(5000);

		i_read_count = custom_uart_4_read((uint8_t*)(u8_c_buff),HDS_ARR_LEN);
		printf("rd %d\r\n",i_read_count);

		if(i_read_count > 0)
		{
			uint16_t *p16_d_1 = (u8_c_buff + 1);
			u8_ret = frame_respone_validate((p16_d_1),i_read_count);
			if(u8_ret == 0)
			{
				if(p16_d_1[2] == 0x5C5C)
				{
					printf("Card Address %d HDR success\r\n",u8_card_address);
					return u8_card_address;
				}
				else
				{
					printf("Card Address %d HDR Error\r\n",u8_card_address);
					return i_status = -1;
				}
			}
		}
		else
		{
			printf("Slot %d Data read Error\r\n",SLOT_4);
			return i_status = -1;
		}
	}
	else if(u8_slot == MB_UNIT)
	{

	}
	else if(u8_slot == DCRM_1)
	{
		/*	for MB units we need to send two frames
		 * 	1. To MB unit for configuring the RS485 channel of DCRM
		 * 	2. Handshake request to DCRM unit
		 */
	}
	else if(u8_slot == DCRM_2)
	{

	}
	else if(u8_slot == DCRM_3)
	{

	}
	else if(u8_slot == DCRM_4)
	{

	}
	else if(u8_slot == DCRM_5)
	{

	}
	else if(u8_slot == DCRM_6)
	{

	}


	return i_status;


}


void system_configuration()
{
	/*	send handshake request to all Daughter Boards present in the
	 * 	system from
	 * 	Slot 1
	 * 	Slot 2
	 * 	Slot 3
	 * 	Slot 4
	 * 	RS485 1
	 * 	RS485 2
	 */

	uint32_t 	u32_config_reg_1 = 0,
				u32_config_reg_2 = 0,
				u32_config_reg_3 = 0,
				u32_config_reg_4 = 0;


	uint8_t		u8_slot_cards[8] 	= {1,2,3,4,12,13,14,15};
	uint8_t 	u8_MB_unit_cards[7]	= {5,6,7,8,9,10,11};

	char *name = "/dev/shm/hardware_config.txt";


	/*	Integrate this function into Read socket thread so when user asks
	 * 	for Hardware present in the system this only one function will be called
	 * 	instead of all other threads
	 */

	/* 	Open the system */
	FILE *hardware_config;

	hardware_config = fopen(name,"w+");
	if(hardware_config == NULL)
	{
		perror("");
		exit(1);
	}

	/*	Implement logic for reading the handshake requests from All data
	 * 	channels
	 */

	// 	Below logic is for detecting the Card in a 4 slots only
	//	check the logic using "online C compiler"
//	int main() {
//
//	    int count_1 = 0,count_2 = 0;
//	    int iter = 0;
//
//	    int array[8] = {1,2,3,4,12,13,14,15};
//
//	    int slot_1[6] = {0,3,0,0,0,1};
//	    int slot_2[6] = {4,0,0,0,0,12};
//	    int slot_3[6] = {0,0,14,2,0,0};
//	    int slot_4[6] = {0,0,0,13,15,0};
//
//
//	    for(count_1 = 0 ; count_1 < 8 ; count_1++)
//	    {
//	        for(count_2 = 0 ; count_2 < 6 ; count_2++)
//	        {
//	            if(array[count_1] == slot_1[count_2])
//	            {
//	                printf("slot_1 count_2 %d data %d\r\n",count_2 + 1,array[count_1]);
//	                iter++;
//	            }
//	            else if(array[count_1] == slot_2[count_2])
//	            {
//	                printf("slot_2 count_2 %d data %d\r\n",count_2,array[count_1]);
//	                iter++;
//	            }
//	            else if(array[count_1] == slot_3[count_2])
//	            {
//	                printf("slot_3 count_2 %d data %d\r\n",count_2,array[count_1]);
//	                iter++;
//	            }
//	            else if(array[count_1] == slot_4[count_2])
//	            {
//	                printf("slot_4 count_2 %d data %d\r\n",count_2,array[count_1]);
//	                iter++;
//	            }
//	        }
//	    }
//	    printf("total iterations %d\r\n",iter);
//
//	}

	/*	Below code if for extracting the Card addresses 4 slots from Back plane */
	for(uint8_t u8_count = 0 ; u8_count < 8 ; u8_count++)
	{
		for(uint8_t u8_req = 0; u8_req < 6 ; u8_req++)
		{
			if(u8_slot_cards[u8_count] == config_hardware(SLOT_1,u8_slot_cards[u8_count]))
			{
				printf("slot 1 Card Address %d\r\n",u8_slot_cards[u8_count]);
			}
			else if(u8_slot_cards[u8_count] == config_hardware(SLOT_2,u8_slot_cards[u8_count]))
			{
				printf("slot 2 Card Address %d\r\n",u8_slot_cards[u8_count]);
			}
			else if(u8_slot_cards[u8_count] == config_hardware(SLOT_3,u8_slot_cards[u8_count]))
			{
				printf("slot 3 Card Address %d\r\n",u8_slot_cards[u8_count]);
			}
			else if(u8_slot_cards[u8_count] == config_hardware(SLOT_4,u8_slot_cards[u8_count]))
			{
				printf("slot 4 Card Address %d\r\n",u8_slot_cards[u8_count]);
			}
		}

	}



	fprintf(hardware_config,"u32_config_reg_1 %X\r\n",u32_config_reg_1);
	fprintf(hardware_config,"u32_config_reg_2 %X\r\n",u32_config_reg_2);
	fprintf(hardware_config,"u32_config_reg_3 %X\r\n",u32_config_reg_3);
	fprintf(hardware_config,"u32_config_reg_4 %X\r\n",u32_config_reg_4);

	fclose(hardware_config);
	system("sync");

	/*	send the message on socket to Higher Level application for
	 *  completion of Hardware config using Handshake request
	 */
	sem_wait(sem_write_socket);
	socket_resp(e_HARDWARE_CONFIG,ACKNOWLEDGE);
	sem_post(sem_write_socket);
}

/*	TODO: below function is developed for : hardware configuration part
 * 			it sets which sampling Daughter Board is present in the complete
 * 			system
 */
void dummy_hardware_config()
{
	FILE *config_HW;

	config_HW = fopen("/dev/shm/hardware_config.txt","w+");
	if(config_HW == NULL)
	{
		perror("");
		exit(1);
	}

	fprintf(config_HW,"Travel-CH1-CH3=%d\r\n",1);
	fprintf(config_HW,"Travel-CH4-CH6=%d\r\n",1);

	fprintf(config_HW,"Coil-Current-CH1-CH3=%d\r\n",1);
	fprintf(config_HW,"Coil-Current-CH4-CH6=%d\r\n",1);

	fprintf(config_HW,"MB-CPU=%d\r\n",1);

	fprintf(config_HW,"DCRM-1=%d\r\n",1);
	fprintf(config_HW,"DCRM-2=%d\r\n",1);
	fprintf(config_HW,"DCRM-3=%d\r\n",1);
	fprintf(config_HW,"DCRM-4=%d\r\n",1);
	fprintf(config_HW,"DCRM-5=%d\r\n",1);
	fprintf(config_HW,"DCRM-6=%d\r\n",1);

	fprintf(config_HW,"Analog-CH1-CH3=%d\r\n",1);
	fprintf(config_HW,"Analog-CH4-CH=%d\r\n",1);
	fprintf(config_HW,"Analog-CH7-CH9=%d\r\n",1);
	fprintf(config_HW,"Analog-CH10-CH12=%d\r\n",1);

	usleep(500000);

	sem_wait(sem_write_socket);
	socket_resp(e_HARDWARE_CONFIG,ACKNOWLEDGE);
	sem_post(sem_write_socket);

	fclose(config_HW);
	system("sync");
}


/*	Below code is for calculating the iterations according to
 * 	Sampling speed configured
 */

//#include <stdio.h>
//
//// 1.	100 ksps 	data : 300 K
//// 2.	50  ksps 	data : 150 K
//// 3.	20  ksps	data : 60  K
//// 4.	10  ksps	data : 30  K
//// 5.	5   ksps	data : 15  K
//// 6. 	1   ksps	data : 3   K
//// 7.   	500 sps		data : 1.5 K
//// 8.	200 sps		data : 600 Bytes
//// 9.	100 sps		data : 300 Bytes
//
//int main() {
//    unsigned int samples[10] = {
//        300000,
//        150000,
//        60000,
//        30000,
//        15000,
//        3000,
//        1500,
//        600,
//        300
//    };
//
//    int count = 0;
//    unsigned short quotient = 0 ;
//    unsigned short reminder = 0 ;
//
//    for(count = 0 ; count < 9 ; count++)
//    {
//        quotient = samples[count] / 3981;
//        if(0 == (reminder = samples[count] % 3981))
//        {
//            printf("iterations %d\r\n",quotient);
//        }
//        else
//        {
//             printf("iterations %d\r\n",quotient + 1);
//        }
//    }
//
//    return 0;
//}
