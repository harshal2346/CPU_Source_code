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

#define		SOC_SERVER_IP		"10.42.0.220"
//#define		SOC_SERVER_IP		"192.168.1.10"
#define		SOC_PORT			"8000"

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

sem_t 	*sem_read_socket,
		*sem_Slot_1,
		*sem_Slot_2,
		*sem_Slot_3,
		*sem_Slot_4;





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

/**************************** FUNCTION PROTOTYPES *******************/
uint16_t frame_respone_validate(uint16_t *p_frame,uint32_t u32_len);
void frame_req_slot_1(frame_info *pt_frame,uint16_t u16_error);
void frame_req_slot_2(frame_info *pt_frame,uint16_t u16_error);
void frame_req_slot_3(frame_info *pt_frame,uint16_t u16_error);
void frame_req_slot_4(frame_info *pt_frame,uint16_t u16_error);
uint16_t farme_checksum(uint16_t* data_frame,uint16_t length_of_frame);
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
		while(1);
	}
	printf("SOC : waiting for connection\r\n");

	flag = fcntl(sockfd, F_GETFL);
	if (fcntl(sockfd, F_SETFL, flag | FASYNC /*| O_NONBLOCK*/) == -1)
		perror("fcntl(F_SETFL)");

//	system("node /home/root/server/app.js &");
////
//	printf("node server started\r\n");

	rec_fd = accept(sockfd, (struct sockaddr*)&serv_addr, &serv_addr);
	if(rec_fd > 0)
	{
		printf("accept(): OK\n");
	}
	else
	{
		printf("SOC : Connection accepted\r\n");
	}
	//sleep(1);
	while(1)
	{

		i_read_count = read(rec_fd,u16_sock_rx,sizeof(u16_sock_rx));
		//printf("SOC : read_count %d\r\n",i_read_count);
		//i_read_count = 2;
		if(i_read_count > 0)
		{
			/*	take action on the various ID's received in the socket
			 * 	frame and unblock the threads
			 */
//			for(int i = 0 ; i < 6 ; i++)
//			{
//				printf("u16_sock_rx[%d] = %X\r\n",i,u16_sock_rx[i]);
//			}
//			printf("\r\n");
//
//			u16_state = u16_sock_rx[1];
////			u8_socket_frame_decode(u8_soc_frame);
//
//			/*	Currently for testing purpose we will use only one
//			 * 	slot for communication with only one semaphore -> lock and unlock
//			 */
			usleep(1000);
//			database_fetch_system_settings();
//			database_fetch_coil_current_ch_info();
//			database_fetch_travel_ch_info();
//			database_fetch_analog_ch_info();
//			while(1);
//			switch(u16_state)
//			{
//			case e_SCRM :
//				printf("Test : e_SCRM\r\n");
//				u16_sock_tx[1] = e_SCRM;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_MAIN :
//				printf("Test : e_TIMING_MAIN\r\n");
//				u16_sock_tx[1] = e_TIMING_MAIN;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_AUXILIARY :
//				printf("Test : e_TIMING_AUXILIARY\r\n");
//				u16_sock_tx[1] = e_TIMING_AUXILIARY;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_PIR :
//				printf("Test : e_TIMING_PIR\r\n");
//				u16_sock_tx[1] = e_TIMING_PIR;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_SIMPLE_OPEN :
//				printf("Test : e_SIMPLE_OPEN\r\n");
//				u16_sock_tx[1] = e_SIMPLE_OPEN;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_SIMPLE_CLOSE :
//				printf("Test : e_SIMPLE_CLOSE\r\n");
//				u16_sock_tx[1] = e_SIMPLE_CLOSE;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_DCRM_OPEN :
//				printf("Test : e_DCRM_OPEN\r\n");
//				u16_sock_tx[1] = e_DCRM_OPEN;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_DCRM_CO :
//				printf("Test : e_DCRM_CO\r\n");
//				u16_sock_tx[1] = e_DCRM_CO;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_OPEN :
//				printf("Test : e_TIMING_OPEN\r\n");
//				u16_sock_tx[1] = e_TIMING_OPEN;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_CLOSE :
//				printf("Test : e_TIMING_CLOSE\r\n");
//				u16_sock_tx[1] = e_TIMING_CLOSE;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_CO :
//				printf("Test : e_TIMING_CO\r\n");
//				u16_sock_tx[1] = e_TIMING_CO;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_OC :
//				printf("Test : e_TIMING_OC\r\n");
//				u16_sock_tx[1] = e_TIMING_OC;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_OCO :
//				printf("Test : e_TIMING_OCO\r\n");
//				u16_sock_tx[1] = e_TIMING_OCO;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_TIMING_O_CO_CO :
//				printf("Test : e_TIMING_O_CO_CO\r\n");
//				u16_sock_tx[1] = e_TIMING_O_CO_CO;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_FIRST_TRIP :
//				printf("Test : e_FIRST_TRIP\r\n");
//				u16_sock_tx[1] = e_FIRST_TRIP;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			case e_MOTOR_CURRENT :
//				printf("Test : e_MOTOR_CURRENT\r\n");
//				u16_sock_tx[1] = e_MOTOR_CURRENT;
//				u16_sock_tx[2] = 0xA5A5;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//
//			default :
//				printf("Wrong Test ID\r\n");
//				u16_sock_tx[1] = u16_state;
//				u16_sock_tx[2] = 0xFFFF;
//				write(rec_fd,u16_sock_tx,sizeof(u16_sock_rx));
//				break;
//			}
			//printf("SOC : command received\r\n");
			sem_post(sem_Slot_1);
			//sem_post(sem_Slot_2);
			//sem_post(sem_Slot_3);
			//sem_post(sem_Slot_4);
			//sched_yield();
		}
	}
}


void *communication_slot_1()
{
	/* 	Cards --> 1,2,3 */
	uint8_t 	u8_data_resp[4004] = {0};
	uint8_t 	u8_c_buff[MAX_LEN] = {0};
	int 		i_read_count,i_ret = 0;
	FILE		*ptr_data;
	uint8_t 	u8_state = e_REQ_HANDSHAKE;
	uint8_t		u8_ret = 0,u8_req_count = 0;
	uint16_t 	u16_count = 0,u6_iteration = 0;
	int i_sem_ret;

	ptr_ch_1 			= fopen("channel_1.csv","w+");
	ptr_ch_2 			= fopen("channel_2.csv","w+");
	ptr_ch_3 			= fopen("channel_3.csv","w+");
	ptr_ch_4 			= fopen("channel_4.csv","w+");
	ptr_slot_1_config	= fopen("slot_1_config.csv","w+");
	ptr_slot_2_config	= fopen("slot_2_config.csv","w+");
	ptr_slot_3_config	= fopen("slot_3_config.csv","w+");

	printf("communication_slot_1 t thread\r\n");
	while(1)
	{
		i_sem_ret = sem_wait(sem_Slot_1);
//		if(i_sem_ret != NULL)
//		{
//			perror("Th 1 :");
//			exit(1);
//		}
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
			for(u6_iteration = 1 ; u6_iteration <= 3 ; )
			{
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= u6_iteration;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.u16_footer			= 0x5555;
				printf("Req Handshake\r\n");

				//			while(1)
				//			{
				custom_uart_fifo_reset(p32_axi_uart_1);
				frame_req_slot_1(&frame_t,i_ret);
				memset(u8_c_buff,0,MAX_LEN);

				usleep(60000);
				//			}

				i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),56);
				printf("rd %d\r\n",i_read_count);
				if(i_read_count > 0)
				{
					uint16_t *p16_d_1 = (u8_c_buff + 1);

					for(u16_count = 0 ; u16_count < 28 ; u16_count++)
					{
						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
					}

					u8_ret = frame_respone_validate((p16_d_1),i_read_count);
					if(u8_ret == 0)
					{

						if(p16_d_1[2] == 0x5C5C)
						{
							u8_state = e_REQ_TEST_SETTINGS;
							printf("Handshake Response\r\n");
							u6_iteration++;
						}
						else
						{
							printf("Response Error %x\r\n",p16_d_1[5]);
						}
					}
					else
					{
						/* 	set the flag to send the request again to the
						 * 	corresponding Daughter board with current activity
						 */
						u8_req_count++;
						u8_state = e_REQ_HANDSHAKE;
						if(u8_req_count > 3)
						{
							/*	if consecutive request count exceeds 3 iterations then
							 * 	send the message to High level software that particular
							 * 	channels of Corresponding Daughter Boards are not
							 * 	responding to Handshake request and give message to user
							 */
							printf("Error in Communication with DB_1\r\n");
							u6_iteration++;
							u8_req_count = 0;
						}

					}
				}
			}
//			printf("infinite loop\r\n");
//			while(1);
			sleep(4);
			//break;
		case e_REQ_TEST_SETTINGS :
			//printf("th_1 e_REQ_CONFIG\r\n");
			gpio_rs485_tx_en();
			usleep(5);

//			for(u16_count = 1 ; u16_count < 4 ; u16_count++)
//			{
				//u16_count = 2;
				/*	Daughter Board 1 send command and receive response */
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= 0x0001;
				frame_t.u16_activity		= 0x0002;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.u16_footer			= 0x5555;

				frame_t.p16_data[2]			= 0xAABB;
				frame_t.p16_data[3]			= 0xAACC;
				printf("Req Test_settings\r\n");


				custom_uart_fifo_reset(p32_axi_uart_1);
				frame_req_slot_1(&frame_t,i_ret);
				memset(u8_c_buff,0,MAX_LEN);
				usleep(600000);

				i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),56);
				printf("rd %d\r\n",i_read_count);
				if(i_read_count > 0)
				{
					uint16_t *p16_d_1 = (u8_c_buff + 1);

					for(u16_count = 0 ; u16_count < 28 ; u16_count++)
					{
						printf("%d = %x\r\n",u16_count,p16_d_1[u16_count]);
					}
					u8_ret = frame_respone_validate((p16_d_1),i_read_count);

					if(u8_ret == 0)
					{
						if(p16_d_1[2] == 0x5C5C)
						{
							for(int i = 0; i < i_read_count/2 ; i++)
							{
								fprintf(ptr_slot_1_config,"%d,",p16_d_1[i]);
							}
							fprintf(ptr_slot_1_config,"\r\n");
						}
						else
						{
							printf("Response Error %x\r\n",p16_d_1[5]);
						}
					}
					else
					{
						/* 	set the flag to send the request again to the
						 * 	corresponding Daughter board with current activity
						 */
						printf("fame validation failed with Errors %x\r\n",u8_ret);
					}
				}
//				/*	Daughter Board 2 send command and receive response */
//				frame_t.u16_card_address 	= 0x0002;
//				frame_t.u16_activity		= 0x0002;
//				frame_t.u16_identifier		= 0x5B5B;
//				frame_t.u16_payload			= 22;
//				frame_t.p16_data[0]			= u16_count;
//				printf("Req 2\r\n");
//				////
//				custom_uart_fifo_reset(p32_axi_uart_1);
//
//				frame_req_slot_1(&frame_t,i_ret);
//				memset(u8_c_buff,0,MAX_LEN);
//				usleep(6000);
//				//
//				i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),57);
//				printf("rd %d\r\n",i_read_count);
//				if(i_read_count > 0)
//				{
//					uint16_t *p16_d_2 = (u8_c_buff + 1);
//					u8_ret = frame_respone_validate((p16_d_2),i_read_count);
//					if(u8_ret == 0)
//					{
//						for(int i = 0; i < i_read_count/2 ; i++)
//						{
//							fprintf(ptr_slot_2_config,"%d,",p16_d_2[i]);
//						}
//						fprintf(ptr_slot_2_config,"\r\n");
//					}
//					else
//					{
//						/* 	set the flag to send the request again to the
//						 * 	corresponding Daughter board with current activity
//						 */
//						printf("fame validation failed with Errors %x\r\n",u8_ret);
//					}
//				}
				/*	Daughter Board 3 send command and receive response */
//				frame_t.u16_card_address 	= 0x0003;
//				frame_t.u16_activity		= 0x0002;
//				frame_t.u16_identifier		= 0x5B5B;
//				frame_t.u16_payload			= 22;
//				frame_t.p16_data[0]			= u16_count;
//				printf("Req 3\r\n");
//				////
//				custom_uart_fifo_reset(p32_axi_uart_1);
//
//				frame_req_slot_1(&frame_t,i_ret);
//				memset(u8_c_buff,0,MAX_LEN);
//				usleep(6000);
//				//
//				i_read_count = custom_uart_1_read((uint8_t*)(u8_c_buff),57);
//				printf("rd %d\r\n",i_read_count);
//				if(i_read_count > 0)
//				{
//					uint16_t *p16_d_3 = (u8_c_buff + 1);
//					u8_ret = frame_respone_validate((p16_d_3),i_read_count);
//					if(u8_ret == 0)
//					{
//						for(int i = 0; i < i_read_count/2 ; i++)
//						{
//							fprintf(ptr_slot_3_config,"%d,",p16_d_3[i]);
//						}
//						fprintf(ptr_slot_3_config,"\r\n");
//					}
//					else
//					{
//						/* 	set the flag to send the request again to the
//						 * 	corresponding Daughter board with current activity
//						 */
//						printf("fame validation failed with Errors %x\r\n",u8_ret);
//					}
//				}
//			}
			system("sync");
			fclose(ptr_slot_1_config);
			fclose(ptr_slot_2_config);
			fclose(ptr_slot_3_config);
			u8_state = e_REQ_DATA;

//			printf("infinite loop after Test setting\r\n");
//			while(1);

//			printf("thread 1 : infinite loop\r\n");
//			while(1);
			//break;

			sleep(3);
		case e_REQ_DATA :
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

			for(int count = 1 ; count <= 75; count++)
			{
				//printf("DB : send CMD\r\n");
				frame_t.u16_header			= 0xAAAA;
				frame_t.u16_card_address 	= 0x0001;
				frame_t.u16_activity		= 0x0003;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.p16_data[0]			= count;
				frame_t.u16_footer			= 0x5555;

				custom_uart_fifo_reset(p32_axi_uart_1);
				memset(u8_data_resp,0,4004);
				frame_req_slot_1(&frame_t,0);

				usleep(6000);

				i_read_count = custom_uart_1_read((uint8_t*)(u8_data_resp),MAX_LEN);
				printf("rd %d\r\n",i_read_count);
//				i_read_count = read(uart1_fd,(void *)u8_data_resp,4000);

				uint16_t *p16_d = u8_data_resp + 1;
//				for(u16_count = 0 ; u16_count < (i_read_count/2) ; u16_count++)
//				{
//					printf("%d\t%d\r\n",u16_count,p16_d[u16_count]);
//				}
//				printf("\r\n");
				if(i_read_count > 0)
				{
					//printf("DB Resp %d\r\n",count);


					u8_ret = frame_respone_validate((p16_d),i_read_count);
					if(u8_ret == 0)
					{
					/*	check which channel data is arrived in Uart buffer and store that
					 * 	data into respective data storing files
					 */
						ptr_data = ptr_ch_1;
//						if(p16_d[6] == 1)
//						{
//							ptr_data = ptr_ch_1;
//						}
//						else if(p16_d[6] == 2)
//						{
//							ptr_data = ptr_ch_2;
//						}
//						else if(p16_d[6] == 3)
//						{
//							ptr_data = ptr_ch_3;
//						}
//						else if(p16_d[6] == 4)
//						{
//							ptr_data = ptr_ch_4;
//						}
//
//						fseek(ptr_data,0,SEEK_CUR);



						for(uint16_t i = 0 ; i < /*(u16_frame_size/1)*/ (i_read_count/2); /*i = i+4*/i++)
						{
							fprintf(ptr_data,"%d,",p16_d[i]);
							//usleep(100);
						}
						fprintf(ptr_data,"\r\n");
					}
					else
					{
						/*	If Any Error Occurs then send the Same Data request again
						 * 	if the same Data request is send for consecutive 3 times then
						 * 	give Error that Data request Error to High level software
						 * 	and stop data fetching process
						 */
						printf("Iteration %d\r\n",count);
						printf("fame validation failed with Errors %x\r\n",u8_ret);
						u8_ret = 0;
					}
				}
			}
			system("sync");
			fclose(ptr_ch_1);
			fclose(ptr_ch_2);
			fclose(ptr_ch_3);
			fclose(ptr_ch_4);
			//fclose(err_ptr);
			printf("file closed\r\n");
			while(1);
			break;
		}

		//sem_post(sem_Slot_1);
	}
}

void *communication_slot_2()
{
	uint8_t 	u8_data_resp[4000] = {0};
	uint8_t 	u8_c_buff[56] = {0};
	int 		i_read_count,i_ret = 0;
	FILE		*ptr_data;
	uint8_t 	u8_ret,u8_req_count = 0;
	uint8_t 	u8_state = e_REQ_TEST_SETTINGS;
	uint16_t 	u16_count = 0;
	int 	i_sem_ret;

	ptr_ch_5 			= fopen("channel_5.csv","w+");
	ptr_ch_6 			= fopen("channel_6.csv","w+");
	ptr_ch_7 			= fopen("channel_7.csv","w+");
	ptr_ch_8 			= fopen("channel_8.csv","w+");

	printf("Slot 2\r\n");
	while(1)
	{
		i_sem_ret = sem_wait(sem_Slot_2);
		if(i_sem_ret != 0)
		{
			perror("Th 2 :");
			exit(1);
		}
		switch(u8_state)
		{

		case e_REQ_HANDSHAKE:

			frame_t.u16_card_address 	= 0x0001;
			frame_t.u16_activity		= 0x0001;
			frame_t.u16_identifier		= 0x5B5B;
			frame_t.u16_payload			= 22;
			printf("Req 1\r\n");

			custom_uart_fifo_reset(p32_axi_uart_1);
			frame_req_slot_2(&frame_t,i_ret);
			memset(u8_c_buff,0,MAX_LEN);
			usleep(6000);

			i_read_count = custom_uart_2_read((uint8_t*)(u8_c_buff),MAX_LEN);
			printf("rd %d\r\n",i_read_count);
			if(i_read_count > 0)
			{
				uint16_t *p16_d_1 = (u8_c_buff + 1);
				u8_ret = frame_respone_validate((p16_d_1),i_read_count);
				if(u8_ret == 0)
				{
					u8_state = e_REQ_TEST_SETTINGS;
//					for(int i = 0; i < i_read_count/2 ; i++)
//					{
//						fprintf(ptr_slot_1_config,"%d,",p16_d_1[i]);
//					}
//					fprintf(ptr_slot_1_config,"\r\n");
				}
				else
				{
					/* 	set the flag to send the request again to the
					 * 	corresponding Daughter board with current activity
					 */
					u8_req_count++;
					u8_state = e_REQ_HANDSHAKE;
					if(u8_req_count > 3)
					{
						printf("Error in Communication with DB_1\r\n");
					}
				}
			}

		case e_REQ_TEST_SETTINGS :
			for(u16_count = 0 ; u16_count < 4 ; u16_count++)
			{
				frame_t.u16_card_address 	= 0x0001;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				//printf("Req Config\r\n");
				frame_req_slot_2(&frame_t,i_ret);

				usleep(500000);

//				i_read_count  = read(uart2_fd,(void*)u8_c_buff,56);
				//i_ret = frame_respone_validate((uint16_t *)u8_c_buff,sizeof(u8_c_buff));
				printf("read count %d\r\n",i_read_count);
				//printf("Error %d\r\n",i_ret);
				uint16_t *p16_d = u8_c_buff;
				for(int i = 0; i < i_read_count/2 ; i++)
				{
					fprintf(ptr_slot_2_config,"%d,",p16_d[i]);
				}

				fprintf(ptr_slot_2_config,"\r\n");
			}
			fclose(ptr_slot_2_config);
			u8_state = e_REQ_DATA;
			//break;

		case e_REQ_DATA :
			/*	Create Data file here to store the complete data of 1.2 MB data of 4 channels.
			 * 	Need to create logic for storing separate files for each channels.
			 */

			for(int count = 1 ; count <= 2; count++)
			{
				//printf("DB : send CMD\r\n");
				frame_t.u16_card_address 	= 0x0001;
				frame_t.u16_activity		= 0x0001;
				frame_t.u16_identifier		= 0x5B5B;
				frame_t.u16_payload			= 22;
				frame_t.p16_data[0]			= count;
				frame_req_slot_2(&frame_t,0);

				usleep(5000);

//				i_read_count = read(uart2_fd,(void *)u8_data_resp,4000);

				printf("S_2 rd %d it %d\r\n",i_read_count,count);
				if(i_read_count > 0)
				{
					//printf("DB Resp %d\r\n",count);
					uint16_t *p16_d = u8_data_resp;

					/*	check which channel data is arrived in Uart buffer and store that
					 * 	data into respective data storing files
					 */
					if(p16_d[6] == 1)
					{
						ptr_data = ptr_ch_5;
					}
					else if(p16_d[6] == 2)
					{
						ptr_data = ptr_ch_6;
					}
					else if(p16_d[6] == 3)
					{
						ptr_data = ptr_ch_7;
					}
					else if(p16_d[6] == 4)
					{
						ptr_data = ptr_ch_8;
					}

					fseek(ptr_data,0,SEEK_CUR);

					for(uint16_t i = 0 ; i < /*(u16_frame_size/1)*/ (i_read_count/2); /*i = i+4*/i++)
					{
						fprintf(ptr_data,"%d,",p16_d[i]);
						//usleep(100);
					}
					fprintf(ptr_data,"\r\n");
				}
			}
			system("sync");
			fclose(ptr_ch_5);
			fclose(ptr_ch_6);
			fclose(ptr_ch_7);
			fclose(ptr_ch_8);
			//fclose(err_ptr);
			printf("file closed\r\n");
			break;
		}

		//sem_post(sem_Slot_1);
	}
}

void *communication_slot_3()
{
	/* 	Cards --> 7,8,9 */
	printf("communication_slot_3 t thread\r\n");
	while(1)
	{
		sem_wait(sem_Slot_3);
	}
}

void *communication_slot_4()
{
	/* 	Cards --> 10,11 */
	printf("communication_slot_4 t thread\r\n");
	while(1)
	{
		sem_wait(sem_Slot_4);
	}
}


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
//			for(int i = 0 ; i < 30 ; i++)
//			{
//				//printf("data[%d] = %x\r\n",i,u16_data[i]);
//				u16_data[i] = 0xFF00;
//			}

	custom_uart_1_write((uint8_t *)u16_data,58);
	//write(uart1_fd,(void *)u16_data,56);
	//		char data = 0xAA;
	//		write(uart2_fd,(void *)&data,1);


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

	custom_uart_2_write((uint8_t *)u16_data,56);
	//write(uart2_fd,(void *)u16_data,56);

	printf("S_2: Req\r\n");

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

	//custom_uart_3_write((uint8_t *)u16_data,56);
	//write(uart2_fd,(void *)u16_data,56);

	printf("S_2: Req\r\n");
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
	u16_data[26] = pt_frame->u16_checksum;
	u16_data[27] = pt_frame->u16_footer;

	//custom_uart_4_write((uint8_t *)u16_data,56);
	//write(uart2_fd,(void *)u16_data,56);

	printf("S_2: Req\r\n");
}

uint16_t frame_respone_validate(uint16_t *p_frame,uint32_t u32_len)
{
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

void init_card_info()
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

	cards_t[e_CARD_1].u8_slot = 0x01;
	cards_t[e_CARD_2].u8_slot = 0x01;
	cards_t[e_CARD_3].u8_slot = 0x01;

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

	cards_t[e_CARD_10].u8_card_address = 10;
	cards_t[e_CARD_11].u8_card_address = 11;

}
