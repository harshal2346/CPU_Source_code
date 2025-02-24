/*
 * Copyright (c) 2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stddef.h>
#include <termios.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <errno.h>


#include <semaphore.h>
#include <fcntl.h>
#include "app.h"
#include "gpio.h"
#include "custom_uart.h"
#include "timer.h"


#define 	SEM_SLOT_1			"/sem_slot_1"
#define 	SEM_SLOT_2			"/sem_slot_2"
#define 	SEM_SLOT_3			"/sem_slot_3"
#define 	SEM_SLOT_4			"/sem_slot_4"
#define 	SEM_WRITE_SOCKET	"/write_socket"
#define 	SEM_OPTO_WAIT		"/sem_opto_wait"
#define 	SEM_MB_DCRM			"/sem_mb_dcrm"

void spi_test();
void uart_test();
void create_dummy_file();
void create_demoClose_shared_objects();
void create_demo_shared_objects();

int main()
{
	printf("Hello World\r\n");
	printf("test for InAS without HDMI\r\n");

	/* 	create the Semaphores and Threads here and starts
	 * 	use the main application only for idle purpose
	 * 	OR cleaning objects purpose.
	 */
	uint32_t u32_iter = 0;


	pthread_t 	t_id_read_socket,
	t_id_Slot_1,
	t_id_Slot_2,
	t_id_Slot_3,
	t_id_Slot_4;

	int 	i_ret;

	gpio_init();

	custom_uart_init();
	timer_init();

	// 	Below code is only used for testing purpose and comment it after testing is over
	uint8_t u8_data = 0x0A;
	uint8_t u8_tx[60] = {0};
	uint8_t u8_rx[60] = {0};
	uint8_t u8_iter = 0;

//	for(int count  = 0; count < 60 ; count++)
//	{
//		u8_tx[count] = 0xAA/*count*/;
//	}

//		printf("testing the LVDS Slot \r\n");
//		while(1)
//		{
//			gpio_slot_en(2,1);
	//		gpio_spare_en(2,1);
//			gpio_rs485_tx_en();
	//		gpio_rst_str(2,1);
	//		gpio_common(2,1);
//		}

//	printf("test_so created\r\n");
//	create_dummy_file();
//
//	while(1);


//	printf("spi_test\r\n");
//	while(1)
//	{
//		//
//					gpio_Led_en(1);
//					usleep(500000);
//					gpio_Led_en(0);
//					usleep(500000);
//		//			gpio_spi_test(1);
//		//			usleep(1);
//		//			gpio_spi_test(0);
//		//			usleep(1);
//					uart_test();
//					spi_test();
//		gpio_spare_en(2,1);
//				gpio_common(2,1);
//				}





//		for(int count  = 0; count < 60 ; count++)
//		{
//			u8_tx[count] = count/*0x56*//*0x55*//*0x10 +*/ /*u32_iter*/ /*(10 * u32_iter ) +*/ /*20 + count*/ ;
//		}

		/*--------------- testing slot 1 by loop back method ---------------*/
//				printf("Uart 1 test %d\r\n",u32_iter);
//				printf("test %d\r\n",u32_iter);
//				custom_uart_fifo_reset(p32_axi_uart_1);
//				custom_uart_1_write(u8_tx,60);
//				usleep(8000);
//				custom_uart_1_read(u8_rx,60);
//				for(int count = 0; count < 60 ; count++)
//				{
//					printf("%d = %d\r\n",count, u8_rx[count]);
//				}
//
//				printf("\r\n");
//				u32_iter++;
//				if(u32_iter > 3)
//				{
//					while(1);
//				}

		/*--------------- testing slot 2 by loop back method ---------------*/
//		printf("Uart 2 test %d\r\n",u32_iter);
//		custom_uart_fifo_reset(p32_axi_uart_2);
//		custom_uart_2_write(u8_tx,60);
//		usleep(8000);
//		custom_uart_2_read(u8_rx,60);
//		for(int count = 0; count < 60 ; count++)
//		{
//			//printf("%d = %d|",count, u8_rx[count]);
//			printf("%d = %d\r\n",count, u8_rx[count]);
//		}
//
//		printf("\r\n");
//		u32_iter++;
//		if(u32_iter > 3)
//		{
//			while(1);
//		}


		/*--------------- testing slot 3 by loop back method ---------------*/
//					printf("Uart 3 test %d\r\n",u32_iter);
//					custom_uart_fifo_reset(p32_axi_uart_3);
//					custom_uart_3_write(u8_tx,60);
//					usleep(8000);
//					custom_uart_3_read(u8_rx,60);
//					for(int count = 0; count < 60 ; count++)
//					{
//						//				//printf("%d = %d|",count, u8_rx[count]);
//										printf("%d = %d\r\n",count, u8_rx[count]);
//					}
//
//					printf("\r\n");
//					u32_iter++;
//					if(u32_iter > 3)
//					{
//						while(1);
//					}

		/*--------------- testing slot 4 by loop back method ---------------*/
//					printf("Uart 4 test %d\r\n",u32_iter);
//					custom_uart_fifo_reset(p32_axi_uart_4);
////					custom_uart_4_write(u8_tx,60);
//					usleep(80000);
//					custom_uart_4_read(u8_rx,60);
//					for(int count = 0; count < 60 ; count++)
//					{
//						//printf("%d = %d|",count, u8_rx[count]);
//						printf("%d = %d\r\n",count, u8_rx[count]);
//					}
//
//
//					printf("\r\n");
//					u32_iter++;
////					while(1);
//					if(u32_iter > 4)
//					{
//						while(1);
//					}

//	}


	//	Below is the actual code used for User application purpose
	sem_write_socket = sem_open(SEM_WRITE_SOCKET,O_CREAT,0660,1);
	if(sem_write_socket == SEM_FAILED)
	{
		perror("sem_read_socket :");
	}

	sem_Slot_1 = sem_open(SEM_SLOT_1,O_CREAT,0660,0);
	if(sem_Slot_1 == SEM_FAILED)
	{
		perror("sem_Slot_1 :");
	}

	sem_Slot_2 = sem_open(SEM_SLOT_2,O_CREAT,0660,0);
	if(sem_Slot_2 == SEM_FAILED)
	{
		perror("sem_Slot_2 :");
	}

	sem_Slot_3 = sem_open(SEM_SLOT_3,O_CREAT,0660,0);
	if(sem_Slot_3 == SEM_FAILED)
	{
		perror("sem_Slot_3 :");
	}

	sem_Slot_4 = sem_open(SEM_SLOT_4,O_CREAT,0660,0);
	if(sem_Slot_4 == SEM_FAILED)
	{
		perror("sem_Slot_4 :");
	}

	sem_opto_wait = sem_open(SEM_OPTO_WAIT,O_CREAT,0660,0);
	if(sem_opto_wait == SEM_FAILED)
	{
		perror("sem_opto_wait :");
	}

	sem_mb_dcrm = sem_open(SEM_MB_DCRM,O_CREAT,0660,0);
	if(sem_mb_dcrm == SEM_FAILED)
	{
		perror("sem_mb_dcrm :");
	}

	printf("Semaphores created\r\n");

	create_demo_shared_objects();
//	create_demoClose_shared_objects();

//	while(1);

//	create_data_files();

	init_card_info();

//	printf("node server started\r\n");
//
//system("node /home/root/new_files/server_03_01/server/app.js &");
//
//sleep(10);

	//    /* 	create threads necessary for the execution of all communication state-machines
	//     * 	for 4 slots.
	//     */
	i_ret = pthread_create(&t_id_read_socket,NULL,read_socket ,NULL);
	if(i_ret != 0)
	{
		perror("read_th");
	}

	i_ret = pthread_create(&t_id_Slot_1,NULL,communication_slot_1 ,NULL);
	if(i_ret != 0)
	{
		perror("Slot1_th");
	}

	printf("Threads created\r\n");
	i_ret = pthread_create(&t_id_Slot_2,NULL,communication_slot_2 ,NULL);
	if(i_ret != 0)
	{
		perror("Slot2_th");
	}

	i_ret = pthread_create(&t_id_Slot_3,NULL,communication_slot_3 ,NULL);
	if(i_ret != 0)
	{
		perror("Slot3_th");
	}

	i_ret = pthread_create(&t_id_Slot_4,NULL,communication_slot_4 ,NULL);
	if(i_ret != 0)
	{
		perror("Slot4_th");
	}

	/*	here program execution will not come to main program as all logic will be executed
	 * 	from threads
	 * 	check how program execution comes in main program after creating the required threads
	 * 	as it will help to clean up the buffers and memory files while program execution is ideal.
	 */
	while(1)
	{
		//sleep(2);
		//printf("%d %s\r\n", __LINE__,__FUNCTION__);
	}

	return 0;

}

void spi_test()
{
	int fd_spi;
	int len;
	char send[10]	= "Karvi\r\n";
	char rec[10]	= {0};
	char spi_s_rec[20]	= {0};
	int iter = 0;

	fd_spi = open("/dev/spidev1.0",O_RDWR /*| O_NONBLOCK*/);
	if(fd_spi == -1)
	{
		perror("Error : SPI device open");
	}

	struct spi_ioc_transfer spi_master ;

	//	    for(int count = 0 ; count < 10 ; count++)
	//	    {
	//	    	send[count] = 0xAA;
	//	    }

	spi_master.tx_buf	= (unsigned long)send;
	spi_master.rx_buf 	= (unsigned long)rec;
	spi_master.len		= 10;
	spi_master.speed_hz	= 1000000;
	spi_master.bits_per_word	= 8;
	spi_master.delay_usecs		= 10000;

	while(1)
	{
		printf("iter %d\r\n",iter);
		int status = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &spi_master);
		if (status < 0) {
			perror("SPI: SPI_IOC_MESSAGE Failed");
			exit(1);
		}
		usleep(100000);
		iter++;
	}

}

void uart_test()
{
	int uart_fd, count ;
	unsigned int r_count;
	struct termios st_uart, st_uart_2;

	int iter = 0;

	char data = 0x01;
	char r_data = 0;

	char str[10] = "Karvi\r\n";
	char r_str[100] = {0};

	//uart_fd = open("/dev/ttyUL3",O_RDWR | O_NONBLOCK);

	//	"/dev/ttyUL2" -> RS232_1
	//	"/dev/ttyUL3" -> RS232_2
	//	"/dev/ttyUL1" -> EXTRA_UART



	uart_fd = open("/dev/ttyUL1",O_RDWR | O_NONBLOCK);
	if(uart_fd < 0)
	{
		perror("Error : file open Error\r\n");
		exit(1);
	}

	/*-------------------- Uart 1 termios init -----------------------*/

	tcgetattr(uart_fd, &st_uart);

	st_uart.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
	st_uart.c_oflag &= ~(IXON) ;
	st_uart.c_iflag = IGNPAR | ICRNL;
	st_uart_2.c_lflag &= ~(ICANON);

	st_uart.c_cc[VTIME] = 0;
	st_uart.c_cc[VMIN] = 1;

	tcflush(uart_fd,TCIFLUSH);
	tcsetattr(uart_fd, TCSANOW, &st_uart);

	while(1)
	{
		printf("iter %d\r\n",iter);
		write(uart_fd,(char *)str,10);
		usleep(10000);
//		r_count = read(uart_fd,(void*)r_str,10);
//		for(int count  = 0 ; count < 50 ; count++)
//		{
//			printf("%c\t",r_str[count]);
//		}
		printf("\r\n");
//		usleep(100000);
		iter++;
	}
	/*--------------------- Uart 1 read api --------------------------*/
	//r_count = read(uart_fd,(void*)r_str,50);
}

void create_dummy_file()
{
	char *name = "/home/root/server/test_so";

	int test_arr[100000] = {0};
	int read_arr[100000] = {0};

	for(int count = 0 ; count < 100000 ; count++)
	{
		test_arr[count] = count;
		read_arr[count] = 0x00;
	}

	FILE *test_so;
	test_so = fopen(name,"w+");
	if(test_so == NULL)
	{
		perror("");
		exit(1);
	}

	fwrite(test_arr,sizeof(test_arr),1,test_so);

	system("sync");

	rewind(test_so);

	fread(read_arr,sizeof(read_arr),1,test_so);

	for(int count = 0 ; count < 100000 ; count = count + 8)
	{
		printf("%d %d %d %d %d %d %d %d\r\n",read_arr[count],
											read_arr[count+1],
											read_arr[count+2],
											read_arr[count+3],
											read_arr[count+4],
											read_arr[count+5],
											read_arr[count+6],
											read_arr[count+7]);
	}

	fclose(test_so);

}

/*	TODO :	Below are the pointers of shared objects created for the demo purpose
 * 			remove them after Demo is over. Below files are created
 * 			1. 	Main-PIR
 * 			2.	Auxiliary Contact
 * 			3.	Coil_Current 1 to 6
 * 			4.	Travel 1 to 3
 */

void *mem_1;
void *aux;

void *coil_1;
void *coil_2;
void *coil_3;
void *coil_4;
void *coil_5;
void *coil_6;

void *travel_1;
void *travel_2;
void *travel_3;
void *travel_4;
void *travel_5;
void *travel_6;

void *dcrm1_v1;
void *dcrm2_v1;
void *dcrm3_v1;
void *dcrm4_v1;
void *dcrm5_v1;
void *dcrm6_v1;


void *dcrm1_c1;
void *dcrm2_c1;
void *dcrm3_c1;
void *dcrm4_c1;
void *dcrm5_c1;
void *dcrm6_c1;


void create_demo_shared_objects()
{
	int size = 400000;
	/*	File names required for creating shared objects into /dev/shm :HT */
	char *p_main_pir = "/main_pir_contact";

	char *p_aux = "/auxilary_contact";

	/* 	shared name object for Coil current data */
	char *p_coil_1 = "/coil_current_1";
	char *p_coil_2 = "/coil_current_2";
	char *p_coil_3 = "/coil_current_3";
	char *p_coil_4 = "/coil_current_4";
	char *p_coil_5 = "/coil_current_5";
	char *p_coil_6 = "/coil_current_6";

	/*	shared name object for 6 channels of Travel data */
	char *p_travel_1 = "/travel_1";
	char *p_travel_2 = "/travel_2";
	char *p_travel_3 = "/travel_3";
	char *p_travel_4 = "/travel_4";
	char *p_travel_5 = "/travel_5";
	char *p_travel_6 = "/travel_6";

	/*	shared name objects for 12 channels of DCRM data */
	char *p_dcrm1_v1 = "/sdcrm_v1";
	char *p_dcrm2_v1 = "/sdcrm_v2";
	char *p_dcrm3_v1 = "/sdcrm_v3";
	char *p_dcrm4_v1 = "/sdcrm_v4";
	char *p_dcrm5_v1 = "/sdcrm_v5";
	char *p_dcrm6_v1 = "/sdcrm_v6";


	char *p_dcrm1_c1 = "/sdcrm_c1";
	char *p_dcrm2_c1 = "/sdcrm_c2";
	char *p_dcrm3_c1 = "/sdcrm_c3";
	char *p_dcrm4_c1 = "/sdcrm_c4";
	char *p_dcrm5_c1 = "/sdcrm_c5";
	char *p_dcrm6_c1 = "/sdcrm_c6";

//	/* 	file opened for the Main and PIR contact values reading */
//	fp = fopen("/home/root/Demo_data/Close/Main_PIR.csv","r");
//	if(fp == NULL)
//	{
//		printf("Error in opening Main_PIR file\r\n");
//	}
//
//	/* 	file opened for the Coil Current values reading */
//	fp_coil = fopen("/home/root/Demo_data/Close/Coil_current.csv","r");
//	if(fp_coil == NULL)
//	{
//		printf("Error in opening coil_current file\r\n");
//	}
//
//	fp_aux = fopen("/home/root/Demo_data/Close/Auxilary_cont.csv","r");
//	if(fp_aux == NULL)
//	{
//		printf("Error in opening auxilary contact file\r\n");
//	}
//
//	fp_travel = fopen("/home/root/Demo_data/Close/Travel.csv","r");
//	if(fp_travel == NULL)
//	{
//		printf("Error in opening Travel data file\r\n");
//	}

	/*------------------------ Shared object creation for Auxilary contact data -----------*/
	int fd_aux = shm_open(p_aux, O_CREAT | O_RDWR, 0666);
	if(fd_aux == -1)
	{
		perror("Error creating Auxilary contact memory file\r\n");
		while(1);
	}

	ftruncate(fd_aux, size);

    aux = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_aux, 0);
    if (aux == MAP_FAILED) {
        perror("Error mapping Auxilary shared memory file into memory");
        //return -1;
        while(1);
    }

    printf("Auxilary contact objects created\r\n");

	/*------------------------ file 1 creation code ------------------------------*/

    int fd_1 = shm_open(p_main_pir, O_CREAT | O_RDWR, 0666);
    if (fd_1 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_1, size);

    mem_1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_1, 0);
    if (mem_1 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    printf("Main PIR objects created\r\n");


    /*------------------------ shared object Coil_Current_1 creation code ------------------------------*/

    int fd_coil_1 = shm_open(p_coil_1, O_CREAT | O_RDWR, 0666);
    if (fd_coil_1 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_coil_1, size);

    coil_1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_1, 0);
    if (coil_1 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    /*------------------------ shared object Coil_Current_2 creation code ------------------------------*/

    int fd_coil_2 = shm_open(p_coil_2, O_CREAT | O_RDWR, 0666);
    if (fd_coil_2 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_coil_2, size);

    coil_2 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_2, 0);
    if (coil_2 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    /*------------------------ shared object Coil_Current_3 creation code ------------------------------*/

    int fd_coil_3 = shm_open(p_coil_3, O_CREAT | O_RDWR, 0666);
    if (fd_coil_3 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_coil_3, size);

    coil_3 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_3, 0);
    if (coil_3 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    /*------------------------ shared object Coil_Current_4 creation code ------------------------------*/

    int fd_coil_4 = shm_open(p_coil_4, O_CREAT | O_RDWR, 0666);
    if (fd_coil_4 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_coil_4, size);

    coil_4 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_4, 0);
    if (coil_4 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    /*------------------------ shared object Coil_Current_5 creation code ------------------------------*/

    int fd_coil_5 = shm_open(p_coil_5, O_CREAT | O_RDWR, 0666);
    if (fd_coil_5 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_coil_5, size);

    coil_5 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_5, 0);
    if (coil_5 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    /*------------------------ shared object Coil_Current_6 creation code ------------------------------*/

    int fd_coil_6 = shm_open(p_coil_6, O_CREAT | O_RDWR, 0666);
    if (fd_coil_6 == -1) {
        perror("Error creating shared memory file");
        //return -1;
        while(1);
    }

    ftruncate(fd_coil_6, size);

    coil_6 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_6, 0);
    if (coil_6 == MAP_FAILED) {
        perror("Error mapping shared memory file into memory");
        //return -1;
        while(1);
    }

    printf("Coil Current objects created\r\n");

    /*------------------------ shared Object Travel_1 creation code ------------------------------------*/
    int fd_travel_1 = shm_open(p_travel_1, O_CREAT | O_RDWR, 0666);
    if(fd_travel_1 == -1)
    {
    	perror("Error in creating shared memory file for travel_1\r\n");
    	while(1);
    }

    ftruncate(fd_travel_1,size);

    travel_1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_1, 0);
    if(travel_1 == -1)
    {
    	perror("Error in creating shared memory file for travel_1\r\n");
    	while(1);
    }

    /*------------------------ shared Object Travel_2 creation code ------------------------------------*/
    int fd_travel_2 = shm_open(p_travel_2, O_CREAT | O_RDWR, 0666);
    if(fd_travel_2 == -1)
    {
    	perror("Error in creating shared memory file for travel_2\r\n");
    	while(1);
    }

    ftruncate(fd_travel_2,size);

    travel_2 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_2, 0);
    if(travel_2 == -1)
    {
    	perror("Error in creating shared memory file for travel_2\r\n");
    	while(1);
    }

    /*------------------------ shared Object Travel_3 creation code ------------------------------------*/
    int fd_travel_3 = shm_open(p_travel_3, O_CREAT | O_RDWR, 0666);
    if(fd_travel_3 == -1)
    {
    	perror("Error in creating shared memory file for travel_3\r\n");
    	while(1);
    }

    ftruncate(fd_travel_3,size);

    travel_3 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_3, 0);
    if(travel_3 == -1)
    {
    	perror("Error in creating shared memory file for travel_3\r\n");
    	while(1);
    }

    /*------------------------ shared Object Travel_4 creation code ------------------------------------*/
    int fd_travel_4 = shm_open(p_travel_4, O_CREAT | O_RDWR, 0666);
    if(fd_travel_4 == -1)
    {
    	perror("Error in creating shared memory file for travel_4\r\n");
    	while(1);
    }

    ftruncate(fd_travel_4,size);

    travel_4 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_4, 0);
    if(travel_4 == -1)
    {
    	perror("Error in creating shared memory file for travel_4\r\n");
    	while(1);
    }

    /*------------------------ shared Object Travel_5 creation code ------------------------------------*/
    int fd_travel_5 = shm_open(p_travel_5, O_CREAT | O_RDWR, 0666);
    if(fd_travel_5 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_travel_5,size);

    travel_5 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_5, 0);
    if(travel_5 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*------------------------ shared Object Travel_5 creation code ------------------------------------*/
    int fd_travel_6 = shm_open(p_travel_6, O_CREAT | O_RDWR, 0666);
    if(fd_travel_6 == -1)
    {
    	perror("Error in creating shared memory file for travel_6\r\n");
    	while(1);
    }

    ftruncate(fd_travel_6,size);

    travel_6 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_6, 0);
    if(travel_6 == -1)
    {
    	perror("Error in creating shared memory file for travel_6\r\n");
    	while(1);
    }

    printf("Travel objects created\r\n");

    /*--------------------- DCRM 1 - 6 Voltage and Current files creation ---------*/
    /*--------------------- DCRM 1 - voltage 1 ------------------------------------*/

    int fd_dcrm1_v1 = shm_open(p_dcrm1_v1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm1_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm1_v1,size);

    dcrm1_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm1_v1, 0);
    if(dcrm1_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }


    /*--------------------- DCRM 2 - voltage 1 ------------------------------------*/

    int fd_dcrm2_v1 = shm_open(p_dcrm2_v1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm2_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm2_v1,size);

    dcrm2_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm2_v1, 0);
    if(dcrm2_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 3 - voltage 1 ------------------------------------*/

    int fd_dcrm3_v1 = shm_open(p_dcrm3_v1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm3_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm3_v1,size);

    dcrm3_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm3_v1, 0);
    if(dcrm3_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 4 - voltage 1 ------------------------------------*/

    int fd_dcrm4_v1 = shm_open(p_dcrm4_v1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm4_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm4_v1,size);

    dcrm4_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm4_v1, 0);
    if(dcrm4_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }


    /*--------------------- DCRM 5 - voltage 1 ------------------------------------*/

    int fd_dcrm5_v1 = shm_open(p_dcrm5_v1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm5_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm5_v1,size);

    dcrm5_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm5_v1, 0);
    if(dcrm5_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 6 - voltage 1 ------------------------------------*/

    int fd_dcrm6_v1 = shm_open(p_dcrm6_v1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm6_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm6_v1,size);

    dcrm6_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm6_v1, 0);
    if(dcrm6_v1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 1 - current 1 ------------------------------------*/

    int fd_dcrm1_c1 = shm_open(p_dcrm1_c1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm1_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm1_c1,size);

    dcrm1_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm1_c1, 0);
    if(dcrm1_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 2 - current 1 ------------------------------------*/

    int fd_dcrm2_c1 = shm_open(p_dcrm2_c1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm2_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm2_c1,size);

    dcrm2_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm2_c1, 0);
    if(dcrm2_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 3 - current 1 ------------------------------------*/

    int fd_dcrm3_c1 = shm_open(p_dcrm3_c1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm3_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm3_c1,size);

    dcrm3_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm3_c1, 0);
    if(dcrm3_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 4 - current 1 ------------------------------------*/

    int fd_dcrm4_c1 = shm_open(p_dcrm4_c1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm4_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm4_c1,size);

    dcrm4_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm4_c1, 0);
    if(dcrm4_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 5 - current 1 ------------------------------------*/

    int fd_dcrm5_c1 = shm_open(p_dcrm5_c1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm5_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm5_c1,size);

    dcrm5_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm5_c1, 0);
    if(dcrm5_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    /*--------------------- DCRM 6 - current 1 ------------------------------------*/

    int fd_dcrm6_c1 = shm_open(p_dcrm6_c1, O_CREAT | O_RDWR, 0666);
    if(fd_dcrm6_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    ftruncate(fd_dcrm6_c1,size);

    dcrm6_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm6_c1, 0);
    if(dcrm6_c1 == -1)
    {
    	perror("Error in creating shared memory file for travel_5\r\n");
    	while(1);
    }

    printf("DCRM objects created\r\n");

    printf("memory address %p\r\n",(char*)mem_1);
    printf("Auxilary address %p\r\n",(char*)aux);
    printf("coil_1 address %p\r\n",(char*)coil_1);
    printf("coil_2 address %p\r\n",(char*)coil_2);
    printf("coil_3 address %p\r\n",(char*)coil_3);
    printf("coil_4 address %p\r\n",(char*)coil_4);
    printf("coil_5 address %p\r\n",(char*)coil_5);
    printf("coil_6 address %p\r\n",(char*)coil_6);

    printf("travel_1 address %p\r\n",(char*)travel_1);
    printf("travel_2 address %p\r\n",(char*)travel_2);
    printf("travel_3 address %p\r\n",(char*)travel_3);


//    printf("shared objects created separately\r\n");

}



void create_demoClose_shared_objects()
{


	FILE *fp,*fp_coil,*fp_aux,*fp_travel,*fp_dcrm;
	char raw_data[5000];
	char *token;

	int 	i_ret = 0;
	uint32_t u32_count = 0, u32_value = 0;

	int32_t i32_coil[6] = {0};

	int32_t i32_travel[6] = {0};

	int32_t i32_dcrm[12] = {0};

//	int size = 400000;

	/*	File names required for creating shared objects into /dev/shm :HT */
//	char *p_main_pir = "/main_pir_contact";

//	char *p_aux = "/auxilary_contact";

	/* 	shared name object for Coil current data */
//	char *p_coil_1 = "/coil_current_1";
//	char *p_coil_2 = "/coil_current_2";
//	char *p_coil_3 = "/coil_current_3";
//	char *p_coil_4 = "/coil_current_4";
//	char *p_coil_5 = "/coil_current_5";
//	char *p_coil_6 = "/coil_current_6";

	/*	shared name object for 6 channels of Travel data */
//	char *p_travel_1 = "/travel_1";
//	char *p_travel_2 = "/travel_2";
//	char *p_travel_3 = "/travel_3";
//	char *p_travel_4 = "/travel_4";
//	char *p_travel_5 = "/travel_5";
//	char *p_travel_6 = "/travel_6";

//	char *p_dcrm1_v1 = "/sdcrm_1_v_1";
//	char *p_dcrm2_v1 = "/sdcrm_2_v_1";
//	char *p_dcrm3_v1 = "/sdcrm_3_v_1";
//	char *p_dcrm4_v1 = "/sdcrm_4_v_1";
//	char *p_dcrm5_v1 = "/sdcrm_5_v_1";
//	char *p_dcrm6_v1 = "/sdcrm_6_v_1";

//		char *p_dcrm1_v1 = "/sdcrm_v1";
//		char *p_dcrm2_v1 = "/sdcrm_v2";
//		char *p_dcrm3_v1 = "/sdcrm_v3";
//		char *p_dcrm4_v1 = "/sdcrm_v4";
//		char *p_dcrm5_v1 = "/sdcrm_v5";
//		char *p_dcrm6_v1 = "/sdcrm_v6";
//
//
//	char *p_dcrm1_c1 = "/sdcrm_c1";
//	char *p_dcrm2_c1 = "/sdcrm_c2";
//	char *p_dcrm3_c1 = "/sdcrm_c3";
//	char *p_dcrm4_c1 = "/sdcrm_c4";
//	char *p_dcrm5_c1 = "/sdcrm_c5";
//	char *p_dcrm6_c1 = "/sdcrm_c6";




	/* 	file opened for the Main and PIR contact values reading */

	// "/home/root/Demo_data/Close"

	fp = fopen("/home/root/Demo_data/Close/Main_PIR.csv","r");
	if(fp == NULL)
	{
		printf("Error in opening Main_PIR file\r\n");
	}

	/* 	file opened for the Coil Current values reading */
	fp_coil = fopen("/home/root/Demo_data/Close/Coil_current.csv","r");
	if(fp_coil == NULL)
	{
		printf("Error in opening coil_current file\r\n");
	}

	fp_aux = fopen("/home/root/Demo_data/Close/Auxilary_cont.csv","r");
	if(fp_aux == NULL)
	{
		printf("Error in opening auxilary contact file\r\n");
	}

	fp_travel = fopen("/home/root/Demo_data/Close/Travel.csv","r");
	if(fp_travel == NULL)
	{
		printf("Error in opening Travel data file\r\n");
	}

//	fp_dcrm = fopen("/home/root/Demo_data/DCRM/DCRM.csv","r");
//	if(fp_dcrm == NULL)
//	{
//		printf("Error in opening DCRM data file\r\n");
//	}

//	/*------------------------ Shared object creation for Auxilary contact data -----------*/
//	int fd_aux = shm_open(p_aux, O_CREAT | O_RDWR, 0666);
//	if(fd_aux == -1)
//	{
//		perror("Error creating Auxilary contact memory file\r\n");
//		while(1);
//	}
//
//	ftruncate(fd_aux, size);
//
//    aux = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_aux, 0);
//    if (aux == MAP_FAILED) {
//        perror("Error mapping Auxilary shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//	/*------------------------ file 1 creation code ------------------------------*/
//
//    int fd_1 = shm_open(p_main_pir, O_CREAT | O_RDWR, 0666);
//    if (fd_1 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_1, size);
//
//    mem_1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_1, 0);
//    if (mem_1 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//
//    /*------------------------ shared object Coil_Current_1 creation code ------------------------------*/
//
//    int fd_coil_1 = shm_open(p_coil_1, O_CREAT | O_RDWR, 0666);
//    if (fd_coil_1 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_coil_1, size);
//
//    coil_1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_1, 0);
//    if (coil_1 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//    /*------------------------ shared object Coil_Current_2 creation code ------------------------------*/
//
//    int fd_coil_2 = shm_open(p_coil_2, O_CREAT | O_RDWR, 0666);
//    if (fd_coil_2 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_coil_2, size);
//
//    coil_2 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_2, 0);
//    if (coil_2 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//    /*------------------------ shared object Coil_Current_3 creation code ------------------------------*/
//
//    int fd_coil_3 = shm_open(p_coil_3, O_CREAT | O_RDWR, 0666);
//    if (fd_coil_3 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_coil_3, size);
//
//    coil_3 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_3, 0);
//    if (coil_3 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//    /*------------------------ shared object Coil_Current_4 creation code ------------------------------*/
//
//    int fd_coil_4 = shm_open(p_coil_4, O_CREAT | O_RDWR, 0666);
//    if (fd_coil_4 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_coil_4, size);
//
//    coil_4 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_4, 0);
//    if (coil_4 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//    /*------------------------ shared object Coil_Current_5 creation code ------------------------------*/
//
//    int fd_coil_5 = shm_open(p_coil_5, O_CREAT | O_RDWR, 0666);
//    if (fd_coil_5 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_coil_5, size);
//
//    coil_5 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_5, 0);
//    if (coil_5 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//    /*------------------------ shared object Coil_Current_6 creation code ------------------------------*/
//
//    int fd_coil_6 = shm_open(p_coil_6, O_CREAT | O_RDWR, 0666);
//    if (fd_coil_6 == -1) {
//        perror("Error creating shared memory file");
//        //return -1;
//        while(1);
//    }
//
//    ftruncate(fd_coil_6, size);
//
//    coil_6 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_coil_6, 0);
//    if (coil_6 == MAP_FAILED) {
//        perror("Error mapping shared memory file into memory");
//        //return -1;
//        while(1);
//    }
//
//    /*------------------------ shared Object Travel_1 creation code ------------------------------------*/
//    int fd_travel_1 = shm_open(p_travel_1, O_CREAT | O_RDWR, 0666);
//    if(fd_travel_1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_1\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_travel_1,size);
//
//    travel_1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_1, 0);
//    if(travel_1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_1\r\n");
//    	while(1);
//    }
//
//    /*------------------------ shared Object Travel_2 creation code ------------------------------------*/
//    int fd_travel_2 = shm_open(p_travel_2, O_CREAT | O_RDWR, 0666);
//    if(fd_travel_2 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_2\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_travel_2,size);
//
//    travel_2 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_2, 0);
//    if(travel_2 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_2\r\n");
//    	while(1);
//    }
//
//    /*------------------------ shared Object Travel_3 creation code ------------------------------------*/
//    int fd_travel_3 = shm_open(p_travel_3, O_CREAT | O_RDWR, 0666);
//    if(fd_travel_3 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_3\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_travel_3,size);
//
//    travel_3 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_3, 0);
//    if(travel_3 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_3\r\n");
//    	while(1);
//    }
//
//    /*------------------------ shared Object Travel_4 creation code ------------------------------------*/
//    int fd_travel_4 = shm_open(p_travel_4, O_CREAT | O_RDWR, 0666);
//    if(fd_travel_4 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_4\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_travel_4,size);
//
//    travel_4 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_4, 0);
//    if(travel_4 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_4\r\n");
//    	while(1);
//    }
//
//    /*------------------------ shared Object Travel_5 creation code ------------------------------------*/
//    int fd_travel_5 = shm_open(p_travel_5, O_CREAT | O_RDWR, 0666);
//    if(fd_travel_5 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_travel_5,size);
//
//    travel_5 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_5, 0);
//    if(travel_5 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*------------------------ shared Object Travel_5 creation code ------------------------------------*/
//    int fd_travel_6 = shm_open(p_travel_6, O_CREAT | O_RDWR, 0666);
//    if(fd_travel_6 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_6\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_travel_6,size);
//
//    travel_6 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_travel_6, 0);
//    if(travel_6 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_6\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 1 - 6 Voltage and Current files creation ---------*/
//    /*--------------------- DCRM 1 - voltage 1 ------------------------------------*/
//
//    int fd_dcrm1_v1 = shm_open(p_dcrm1_v1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm1_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm1_v1,size);
//
//    dcrm1_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm1_v1, 0);
//    if(dcrm1_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//
//    /*--------------------- DCRM 2 - voltage 1 ------------------------------------*/
//
//    int fd_dcrm2_v1 = shm_open(p_dcrm2_v1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm2_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm2_v1,size);
//
//    dcrm2_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm2_v1, 0);
//    if(dcrm2_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 3 - voltage 1 ------------------------------------*/
//
//    int fd_dcrm3_v1 = shm_open(p_dcrm3_v1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm3_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm3_v1,size);
//
//    dcrm3_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm3_v1, 0);
//    if(dcrm3_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 4 - voltage 1 ------------------------------------*/
//
//    int fd_dcrm4_v1 = shm_open(p_dcrm4_v1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm4_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm4_v1,size);
//
//    dcrm4_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm4_v1, 0);
//    if(dcrm4_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//
//    /*--------------------- DCRM 5 - voltage 1 ------------------------------------*/
//
//    int fd_dcrm5_v1 = shm_open(p_dcrm5_v1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm5_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm5_v1,size);
//
//    dcrm5_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm5_v1, 0);
//    if(dcrm5_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 6 - voltage 1 ------------------------------------*/
//
//    int fd_dcrm6_v1 = shm_open(p_dcrm6_v1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm6_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm6_v1,size);
//
//    dcrm6_v1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm6_v1, 0);
//    if(dcrm6_v1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 1 - current 1 ------------------------------------*/
//
//    int fd_dcrm1_c1 = shm_open(p_dcrm1_c1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm1_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm1_c1,size);
//
//    dcrm1_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm1_c1, 0);
//    if(dcrm1_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 2 - current 1 ------------------------------------*/
//
//    int fd_dcrm2_c1 = shm_open(p_dcrm2_c1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm2_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm2_c1,size);
//
//    dcrm2_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm2_c1, 0);
//    if(dcrm2_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 3 - current 1 ------------------------------------*/
//
//    int fd_dcrm3_c1 = shm_open(p_dcrm3_c1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm3_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm3_c1,size);
//
//    dcrm3_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm3_c1, 0);
//    if(dcrm3_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 4 - current 1 ------------------------------------*/
//
//    int fd_dcrm4_c1 = shm_open(p_dcrm4_c1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm4_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm4_c1,size);
//
//    dcrm4_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm4_c1, 0);
//    if(dcrm4_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 5 - current 1 ------------------------------------*/
//
//    int fd_dcrm5_c1 = shm_open(p_dcrm5_c1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm5_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm5_c1,size);
//
//    dcrm5_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm5_c1, 0);
//    if(dcrm5_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    /*--------------------- DCRM 6 - current 1 ------------------------------------*/
//
//    int fd_dcrm6_c1 = shm_open(p_dcrm6_c1, O_CREAT | O_RDWR, 0666);
//    if(fd_dcrm6_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }
//
//    ftruncate(fd_dcrm6_c1,size);
//
//    dcrm6_c1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dcrm6_c1, 0);
//    if(dcrm6_c1 == -1)
//    {
//    	perror("Error in creating shared memory file for travel_5\r\n");
//    	while(1);
//    }

//    printf("DCRM shared objects created\r\n");


//    printf("travel_4 address %p\r\n",(char*)travel_4);
//    printf("travel_5 address %p\r\n",(char*)travel_5);
//    printf("travel_6 address %p\r\n",(char*)travel_6);




    int32_t* data_1 = (int32_t)mem_1;
    int32_t* i_aux = (int32_t*)aux;
    int32_t* i_coil_1 = (int32_t*)coil_1;
    int32_t* i_coil_2 = (int32_t*)coil_2;
    int32_t* i_coil_3 = (int32_t*)coil_3;
    int32_t* i_coil_4 = (int32_t*)coil_4;
    int32_t* i_coil_5 = (int32_t*)coil_5;
    int32_t* i_coil_6 = (int32_t*)coil_6;

    int32_t* i_travel_1 = (int32_t*)travel_1;
    int32_t* i_travel_2 = (int32_t*)travel_2;
    int32_t* i_travel_3 = (int32_t*)travel_3;
    int32_t* i_travel_4 = (int32_t*)travel_4;
    int32_t* i_travel_5 = (int32_t*)travel_5;
    int32_t* i_travel_6 = (int32_t*)travel_6;

//    int32_t* i_dcrm1_v1 = (int32_t*)dcrm1_v1;
//    int32_t* i_dcrm2_v1 = (int32_t*)dcrm2_v1;
//    int32_t* i_dcrm3_v1 = (int32_t*)dcrm3_v1;
//    int32_t* i_dcrm4_v1 = (int32_t*)dcrm4_v1;
//    int32_t* i_dcrm5_v1 = (int32_t*)dcrm5_v1;
//    int32_t* i_dcrm6_v1 = (int32_t*)dcrm6_v1;
//
//    int32_t* i_dcrm1_c1 = (int32_t*)dcrm1_c1;
//    int32_t* i_dcrm2_c1 = (int32_t*)dcrm2_c1;
//    int32_t* i_dcrm3_c1 = (int32_t*)dcrm3_c1;
//    int32_t* i_dcrm4_c1 = (int32_t*)dcrm4_c1;
//    int32_t* i_dcrm5_c1 = (int32_t*)dcrm5_c1;
//    int32_t* i_dcrm6_c1 = (int32_t*)dcrm6_c1;





    //float *data_1 = (float *)mem_1;
    //strcpy((char*)mem,"hello world\r\n");
    //sprintf(data, "Hello, world!");


    /* 	Below code is for extracting the Main and PIR contact values from .csv file */
	while(feof(fp) != 1)
	{
		fgets(raw_data,5000,fp);
		token = strtok(raw_data,",");

		while(token != NULL)
		{
//			if(u32_count>= 2)
//			{
				i_ret = strncmp(token,"0",1);
				if(i_ret == 0)
				{
					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (0 << 0));
				}
				else
				{

					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (1 << 0));
				}
//			}
			token = strtok(NULL,",");
		}

//		if(u32_count > 300 & u32_count < 600)
//		{
//			printf("%d\t %x\r\n",u32_count,u32_value);
//		}

		u32_count++;
		*(int32_t *)data_1 = u32_value;
		data_1++;
		u32_value = 0;
	}

	printf("Main PIR object created\r\n");

	/*	Below code is for extracting the Auxilary Contact values from the .csv file */
	u32_count = 0;
	u32_value = 0;
	while(feof(fp_aux) != 1)
	{
		fgets(raw_data,5000,fp_aux);
		token = strtok(raw_data,",");

		while(token != NULL)
		{
//			if(u32_count>= 2)
//			{
				i_ret = strncmp(token,"0",1);
				if(i_ret == 0)
				{
					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (0 << 0));
				}
				else
				{

					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (1 << 0));
				}
//			}
			token = strtok(NULL,",");
		}


//		if(u32_count < 100)
//		{
//			printf("%d\t %x\r\n",u32_count,u32_value);
//		}

		u32_count++;
		*(int32_t *)i_aux = u32_value;
		i_aux++;
		u32_value = 0;
	}

	printf("count %d\r\n",u32_count);


	u32_count = 0;
    while(fgets(raw_data,5000,fp_coil)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
//    	    		printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_coil[0] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[1] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[2] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[3] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[4] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[5] = strtol(token, NULL, 10);
    			//token = strtok(NULL,",");

    			*(int32_t *)i_coil_1 = i32_coil[0];
    			*(int32_t *)i_coil_2 = i32_coil[1];
    			*(int32_t *)i_coil_3 = i32_coil[2];
    			*(int32_t *)i_coil_4 = i32_coil[3];
    			*(int32_t *)i_coil_5 = i32_coil[4];
    			*(int32_t *)i_coil_6 = i32_coil[5];

    			i_coil_1++;
				i_coil_2++;
				i_coil_3++;
				i_coil_4++;
				i_coil_5++;
				i_coil_6++;

//    			if(u32_count>= 1000)
//    			{
//    				printf("count %d\r\n",u32_count);
//    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }

    printf("count %d\r\n",u32_count);

	u32_count = 0;
    while(fgets(raw_data,5000,fp_travel)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
    	    		//printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_travel[0] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_travel[1] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_travel[2] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[3] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[4] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[5] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");



    			*(int32_t *)i_travel_1 = i32_travel[0];
    			*(int32_t *)i_travel_2 = i32_travel[1];
    			*(int32_t *)i_travel_3 = i32_travel[2];
//    			*(int32_t *)i_travel_4 = i32_travel[3];
//    			*(int32_t *)i_travel_5 = i32_travel[4];
//    			*(int32_t *)i_travel_6 = i32_travel[5];

    			i_travel_1++;
    			i_travel_2++;
    			i_travel_3++;
//    			i_travel_4++;
//    			i_travel_5++;
//    			i_travel_6++;

//    			if(u32_count<= 100)
//    			{
////    				printf("count %d\r\n",u32_count);
////    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//
//    				printf("count %d\r\n",u32_count);
//    				printf("tc1 %d\tc2 %d\tc2 %d\r\n",i32_travel[0],i32_travel[1],i32_travel[2]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }

    printf("count %d\r\n",u32_count);

    /* 	data of DCRM will be in the form d1_v1,d1_c1,d2_v1,d2_c2...... like that */

//	u32_count = 0;
//    while(fgets(raw_data,5000,fp_dcrm)!= NULL)
////    while(u32_count < 10)
//    {
//    	//fgets(raw_data,5000,fp_coil);
//    	token = strtok(raw_data,",");
//    	    		//printf("%s\r\n",token);
//    	    		//token = strtok(NULL,",");
//    	while(token != NULL)
//    	{
////    		if(u32_count >= 2)
////    		{
//    			i32_dcrm[0] = strtol(token, NULL, 10);	// d1_v1
//    			token = strtok(NULL,",");
//    			i32_dcrm[1] = strtol(token, NULL, 10);	// d1_c1
//    			token = strtok(NULL,",");
//    			i32_dcrm[2] = strtol(token, NULL, 10);	// d2_v1
//    			token = strtok(NULL,",");
//    			i32_dcrm[3] = strtol(token, NULL, 10);	// d2_c1
//    			token = strtok(NULL,",");
//    			i32_dcrm[4] = strtol(token, NULL, 10);	// d3_v1
//    			token = strtok(NULL,",");
//    			i32_dcrm[5] = strtol(token, NULL, 10);	// d3_c1
//    			token = strtok(NULL,",");
//    			i32_dcrm[6] = strtol(token, NULL, 10);	// d4_v1
//    			token = strtok(NULL,",");
//    			i32_dcrm[7] = strtol(token, NULL, 10);	// d4_c1
//    			token = strtok(NULL,",");
//    			i32_dcrm[8] = strtol(token, NULL, 10);	// d5_v1
//    			token = strtok(NULL,",");
//    			i32_dcrm[9] = strtol(token, NULL, 10);	// d5_c1
//    			token = strtok(NULL,",");
//    			i32_dcrm[10] = strtol(token, NULL, 10);	// d6_v1
//    			token = strtok(NULL,",");
//    			i32_dcrm[11] = strtol(token, NULL, 10);	// d6_c1
//    			token = strtok(NULL,",");
//
//    			*(int32_t *)i_dcrm1_v1 = i32_dcrm[0];
//    			*(int32_t *)i_dcrm1_c1 = i32_dcrm[1];
//
//    			*(int32_t *)i_dcrm2_v1 = i32_dcrm[2];
//    			*(int32_t *)i_dcrm2_c1 = i32_dcrm[3];
//
//    			*(int32_t *)i_dcrm3_v1 = i32_dcrm[4];
//    			*(int32_t *)i_dcrm3_c1 = i32_dcrm[5];
//
//    			*(int32_t *)i_dcrm4_v1 = i32_dcrm[6];
//    			*(int32_t *)i_dcrm4_c1 = i32_dcrm[7];
//
//    			*(int32_t *)i_dcrm5_v1 = i32_dcrm[8];
//    			*(int32_t *)i_dcrm5_c1 = i32_dcrm[9];
//
//    			*(int32_t *)i_dcrm6_v1 = i32_dcrm[10];
//    			*(int32_t *)i_dcrm6_c1 = i32_dcrm[11];
//
//    			i_dcrm1_v1++;
//    			i_dcrm2_v1++;
//    			i_dcrm3_v1++;
//    			i_dcrm4_v1++;
//    			i_dcrm5_v1++;
//    			i_dcrm6_v1++;
//
//    			i_dcrm1_c1++;
//    			i_dcrm2_c1++;
//    			i_dcrm3_c1++;
//    			i_dcrm4_c1++;
//    			i_dcrm5_c1++;
//    			i_dcrm6_c1++;
//
////    			if(u32_count<= 100)
////    			{
//////    				printf("count %d\r\n",u32_count);
//////    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
////
////    				printf("count %d\r\n",u32_count);
////    				printf("tc1 %d\tc2 %d\tc2 %d\r\n",i32_travel[0],i32_travel[1],i32_travel[2]);
////    				//printf("%s\r\n",token);
////    			}
////    		}
//    		token = strtok(NULL,",");
//    	}
//    	u32_count++;
//    }


//    printf("count %d\r\n",u32_count);
//    printf(".csv read complete\r\n");



//    for(int i = 0 ; i < 100000 ; i++)
//    {
////    	*(int32_t *)data_1 = 0x00000000 + i;
//    	*(int32_t *)data_1 = 50000 + u32_channel;
//
//    	/* below function is used to generate the random numbers of 24-bit for testing
//    	 * purpose : HT
//    	 */
//    	//*(int32_t *)data_1 = (0x00FFFFFF & rand());
//    	//*(float *)data_1 = randomNum((u32_channel * 2 - 1),(u32_channel * 2));
//
//    	data_1++;
//    }

//    if (munmap(mem_1, size) == -1) {
//        perror("Error unmapping shared memory file from memory");
//        return -1;
//    }

    // Close the shared memory file
//    if (close(fd_1) == -1) {
//        perror("Error closing shared memory file");
//        return -1;
//    }

    printf("Close CMD data updated\r\n");
//    printf("Shared memory file created\r\n");
    //printf("u32_count %d\r\n",u32_count);

    system("sync");

    fclose(fp);
    fclose(fp_travel);
    fclose(fp_aux);
    fclose(fp_coil);

}


void update_dummyOpen_shared_objects()
{
	FILE *fp,*fp_coil,*fp_aux,*fp_travel;
	char raw_data[5000];
	char *token;

	int 	i_ret = 0;
	uint32_t u32_count = 0, u32_value = 0;

	int32_t i32_coil[6] = {0};

	int32_t i32_travel[6] = {0};

	// "/home/root/Demo_data/Open"


	fp = fopen("/home/root/Demo_data/Open/Main_PIR.csv","r");
	if(fp == NULL)
	{
		printf("Error in opening Main_PIR file\r\n");
	}

	/* 	file opened for the Coil Current values reading */
	fp_coil = fopen("/home/root/Demo_data/Open/Coil_current.csv","r");
	if(fp_coil == NULL)
	{
		printf("Error in opening coil_current file\r\n");
	}

	fp_aux = fopen("/home/root/Demo_data/Open/Auxilary_cont.csv","r");
	if(fp_aux == NULL)
	{
		printf("Error in opening auxilary contact file\r\n");
	}

	fp_travel = fopen("/home/root/Demo_data/Open/Travel.csv","r");
	if(fp_travel == NULL)
	{
		printf("Error in opening Travel data file\r\n");
	}

    int32_t* data_1 = (int32_t)mem_1;
    int32_t* i_aux = (int32_t*)aux;
    int32_t* i_coil_1 = (int32_t*)coil_1;
    int32_t* i_coil_2 = (int32_t*)coil_2;
    int32_t* i_coil_3 = (int32_t*)coil_3;
    int32_t* i_coil_4 = (int32_t*)coil_4;
    int32_t* i_coil_5 = (int32_t*)coil_5;
    int32_t* i_coil_6 = (int32_t*)coil_6;

    int32_t* i_travel_1 = (int32_t*)travel_1;
    int32_t* i_travel_2 = (int32_t*)travel_2;
    int32_t* i_travel_3 = (int32_t*)travel_3;
//    int32_t* i_travel_4 = (int32_t*)travel_4;
//    int32_t* i_travel_5 = (int32_t*)travel_5;
//    int32_t* i_travel_6 = (int32_t*)travel_6;

    //float *data_1 = (float *)mem_1;
    //strcpy((char*)mem,"hello world\r\n");
    //sprintf(data, "Hello, world!");


    /* 	Below code is for extracting the Main and PIR contact values from .csv file */
	while(feof(fp) != 1)
	{
		fgets(raw_data,5000,fp);
		token = strtok(raw_data,",");

		while(token != NULL)
		{
//			if(u32_count>= 2)
//			{
				i_ret = strncmp(token,"0",1);
				if(i_ret == 0)
				{
					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (0 << 0));
				}
				else
				{

					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (1 << 0));
				}
//			}
			token = strtok(NULL,",");
		}

		//printf("%d\t %x\r\n",u32_count,u32_value);
		u32_count++;
		*(int32_t *)data_1 = u32_value;
		data_1++;
		u32_value = 0;
	}

	printf("count %d\r\n",u32_count);
	printf("Main PIR object created\r\n");

	/*	Below code is for extracting the Auxilary Contact values from the .csv file */
	u32_count = 0;
	u32_value = 0;
	while(feof(fp_aux) != 1)
	{
		fgets(raw_data,5000,fp_aux);
		token = strtok(raw_data,",");

		while(token != NULL)
		{
//			if(u32_count>= 2)
//			{
				i_ret = strncmp(token,"0",1);
				if(i_ret == 0)
				{
					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (0 << 0));
				}
				else
				{

					u32_value =  u32_value << 1;
					u32_value =  (u32_value | (1 << 0));
				}
//			}
			token = strtok(NULL,",");
		}


//		if(u32_count < 500)
//		{
//			printf("%d\t %x\r\n",u32_count,u32_value);
//		}

		u32_count++;
		*(int32_t *)i_aux = u32_value;
		i_aux++;
		u32_value = 0;
	}

	printf("count %d\r\n",u32_count);

	u32_count = 0;
    while(fgets(raw_data,5000,fp_coil)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
//    	    		printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_coil[0] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[1] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[2] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[3] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[4] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[5] = strtol(token, NULL, 10);
    			//token = strtok(NULL,",");

    			*(int32_t *)i_coil_1 = i32_coil[0];
    			*(int32_t *)i_coil_2 = i32_coil[1];
    			*(int32_t *)i_coil_3 = i32_coil[2];
    			*(int32_t *)i_coil_4 = i32_coil[3];
    			*(int32_t *)i_coil_5 = i32_coil[4];
    			*(int32_t *)i_coil_6 = i32_coil[5];

    			i_coil_1++;
				i_coil_2++;
				i_coil_3++;
				i_coil_4++;
				i_coil_5++;
				i_coil_6++;

//    			if(u32_count>= 1000)
//    			{
//    				printf("count %d\r\n",u32_count);
//    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }

    printf("count %d\r\n",u32_count);

	u32_count = 0;
    while(fgets(raw_data,5000,fp_travel)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
    	    		//printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_travel[0] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_travel[1] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_travel[2] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[3] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[4] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[5] = strtol(token, NULL, 10);
    			//token = strtok(NULL,",");

    			*(int32_t *)i_travel_1 = i32_travel[0];
    			*(int32_t *)i_travel_2 = i32_travel[1];
    			*(int32_t *)i_travel_3 = i32_travel[2];
//    			*(int32_t *)i_travel_4 = i32_travel[3];
//    			*(int32_t *)i_travel_5 = i32_travel[4];
//    			*(int32_t *)i_travel_6 = i32_travel[5];

    			i_travel_1++;
    			i_travel_2++;
    			i_travel_3++;
//    			i_travel_4++;
//    			i_travel_5++;
//    			i_travel_6++;

//    			if(u32_count>= 1000)
//    			{
////    				printf("count %d\r\n",u32_count);
////    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//
//    				printf("count %d\r\n",u32_count);
//    				printf("tc1 %d\tc2 %d\tc2 %d\r\n",i32_travel[0],i32_travel[1],i32_travel[2]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }


    printf("count %d\r\n",u32_count);
    printf("Open CMD data updated\r\n");
    //printf(".csv read complete\r\n");
}

void update_dummyDCRM_shared_objects()
{
	FILE *fp_dcrm,*fp_coil,*fp_travel;
	char raw_data[5000];
	char *token;

	int32_t i32_dcrm[12] = {0},
			i32_travel[6] = {0},
			i32_coil[6] = {0};

	uint32_t u32_count = 0;


    int32_t* i_dcrm1_v1 = (int32_t*)dcrm1_v1;
    int32_t* i_dcrm2_v1 = (int32_t*)dcrm2_v1;
    int32_t* i_dcrm3_v1 = (int32_t*)dcrm3_v1;
    int32_t* i_dcrm4_v1 = (int32_t*)dcrm4_v1;
    int32_t* i_dcrm5_v1 = (int32_t*)dcrm5_v1;
    int32_t* i_dcrm6_v1 = (int32_t*)dcrm6_v1;

    int32_t* i_dcrm1_c1 = (int32_t*)dcrm1_c1;
    int32_t* i_dcrm2_c1 = (int32_t*)dcrm2_c1;
    int32_t* i_dcrm3_c1 = (int32_t*)dcrm3_c1;
    int32_t* i_dcrm4_c1 = (int32_t*)dcrm4_c1;
    int32_t* i_dcrm5_c1 = (int32_t*)dcrm5_c1;
    int32_t* i_dcrm6_c1 = (int32_t*)dcrm6_c1;

    int32_t* i_coil_1 = (int32_t*)coil_1;
    int32_t* i_coil_2 = (int32_t*)coil_2;
    int32_t* i_coil_3 = (int32_t*)coil_3;
    int32_t* i_coil_4 = (int32_t*)coil_4;
    int32_t* i_coil_5 = (int32_t*)coil_5;
    int32_t* i_coil_6 = (int32_t*)coil_6;

    int32_t* i_travel_1 = (int32_t*)travel_1;
    int32_t* i_travel_2 = (int32_t*)travel_2;
    int32_t* i_travel_3 = (int32_t*)travel_3;
    int32_t* i_travel_4 = (int32_t*)travel_4;
    int32_t* i_travel_5 = (int32_t*)travel_5;
    int32_t* i_travel_6 = (int32_t*)travel_6;

//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_coil_1 = 0x00000000;
//    	i_coil_1++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_coil_2 = 0x00000000;
//    	i_coil_2++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_coil_3 = 0x00000000;
//    	i_coil_3++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_coil_4 = 0x00000000;
//    	i_coil_4++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_coil_5 = 0x00000000;
//    	i_coil_5++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_coil_6 = 0x00000000;
//    	i_coil_6++;
//    }
//
//
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_travel_1 = 0x00000000;
//    	i_travel_1++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_travel_2 = 0x00000000;
//    	i_travel_2++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_travel_3 = 0x00000000;
//    	i_travel_3++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_travel_4 = 0x00000000;
//    	i_travel_4++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_travel_5 = 0x00000000;
//    	i_travel_5++;
//    }
//    for(int count = 0 ; count < 8000 ; count++)
//    {
//    	*(int32_t*)i_travel_6 = 0x00000000;
//    	i_travel_6++;
//    }

//    i_coil_1 = (int32_t*)coil_1;
//    i_coil_2 = (int32_t*)coil_2;
//    i_coil_3 = (int32_t*)coil_3;
//    i_coil_4 = (int32_t*)coil_4;
//    i_coil_5 = (int32_t*)coil_5;
//    i_coil_6 = (int32_t*)coil_6;
//
//    i_travel_1 = (int32_t*)travel_1;
//    i_travel_2 = (int32_t*)travel_2;
//    i_travel_3 = (int32_t*)travel_3;
//    i_travel_4 = (int32_t*)travel_4;
//    i_travel_5 = (int32_t*)travel_5;
//    i_travel_6 = (int32_t*)travel_6;


	fp_dcrm = fopen("/home/root/Demo_data/DCRM/DCRM.csv","r");
	if(fp_dcrm == NULL)
	{
		printf("Error in opening DCRM data file\r\n");
	}

	fp_coil = fopen("/home/root/Demo_data/DCRM/Coil_current.csv","r");
	if(fp_coil == NULL)
	{
		printf("Error in opening coil_current file\r\n");
	}

	fp_travel = fopen("/home/root/Demo_data/DCRM/Travel.csv","r");
	if(fp_travel == NULL)
	{
		printf("Error in opening Travel data file\r\n");
	}

    /* 	data of DCRM will be in the form d1_v1,d1_c1,d2_v1,d2_c2...... like that */

	u32_count = 0;
    while(fgets(raw_data,5000,fp_dcrm)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
    	    		//printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_dcrm[0] = strtol(token, NULL, 10);	// d1_v1
    			token = strtok(NULL,",");
    			i32_dcrm[1] = strtol(token, NULL, 10);	// d1_c1
    			token = strtok(NULL,",");
    			i32_dcrm[2] = strtol(token, NULL, 10);	// d2_v1
    			token = strtok(NULL,",");
    			i32_dcrm[3] = strtol(token, NULL, 10);	// d2_c1
    			token = strtok(NULL,",");
    			i32_dcrm[4] = strtol(token, NULL, 10);	// d3_v1
    			token = strtok(NULL,",");
    			i32_dcrm[5] = strtol(token, NULL, 10);	// d3_c1
    			token = strtok(NULL,",");
    			i32_dcrm[6] = strtol(token, NULL, 10);	// d4_v1
    			token = strtok(NULL,",");
    			i32_dcrm[7] = strtol(token, NULL, 10);	// d4_c1
    			token = strtok(NULL,",");
    			i32_dcrm[8] = strtol(token, NULL, 10);	// d5_v1
    			token = strtok(NULL,",");
    			i32_dcrm[9] = strtol(token, NULL, 10);	// d5_c1
    			token = strtok(NULL,",");
    			i32_dcrm[10] = strtol(token, NULL, 10);	// d6_v1
    			token = strtok(NULL,",");
    			i32_dcrm[11] = strtol(token, NULL, 10);	// d6_c1
//    			token = strtok(NULL,",");

    			*(int32_t *)i_dcrm1_v1 = i32_dcrm[0];
    			*(int32_t *)i_dcrm1_c1 = i32_dcrm[1];

    			*(int32_t *)i_dcrm2_v1 = i32_dcrm[2];
    			*(int32_t *)i_dcrm2_c1 = i32_dcrm[3];

    			*(int32_t *)i_dcrm3_v1 = i32_dcrm[4];
    			*(int32_t *)i_dcrm3_c1 = i32_dcrm[5];

    			*(int32_t *)i_dcrm4_v1 = i32_dcrm[6];
    			*(int32_t *)i_dcrm4_c1 = i32_dcrm[7];

    			*(int32_t *)i_dcrm5_v1 = i32_dcrm[8];
    			*(int32_t *)i_dcrm5_c1 = i32_dcrm[9];

    			*(int32_t *)i_dcrm6_v1 = i32_dcrm[10];
    			*(int32_t *)i_dcrm6_c1 = i32_dcrm[11];

    			i_dcrm1_v1++;
    			i_dcrm2_v1++;
    			i_dcrm3_v1++;
    			i_dcrm4_v1++;
    			i_dcrm5_v1++;
    			i_dcrm6_v1++;

    			i_dcrm1_c1++;
    			i_dcrm2_c1++;
    			i_dcrm3_c1++;
    			i_dcrm4_c1++;
    			i_dcrm5_c1++;
    			i_dcrm6_c1++;

//    			if(u32_count<= 100)
//    			{
////    				printf("count %d\r\n",u32_count);
////    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//
//    				printf("count %d\r\n",u32_count);
//    				printf("tc1 %d\tc2 %d\tc2 %d\r\n",i32_travel[0],i32_travel[1],i32_travel[2]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }

    printf("count %d\r\n",u32_count);

	u32_count = 0;
    while(fgets(raw_data,5000,fp_travel)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
    	    		//printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_travel[0] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_travel[1] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_travel[2] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[3] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[4] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");
//    			i32_travel[5] = strtol(token, NULL, 10);
//    			token = strtok(NULL,",");

    			*(int32_t *)i_travel_1 = i32_travel[0];
    			*(int32_t *)i_travel_2 = i32_travel[1];
    			*(int32_t *)i_travel_3 = i32_travel[2];
//    			*(int32_t *)i_travel_4 = i32_travel[3];
//    			*(int32_t *)i_travel_5 = i32_travel[4];
//    			*(int32_t *)i_travel_6 = i32_travel[5];

    			i_travel_1++;
    			i_travel_2++;
    			i_travel_3++;
//    			i_travel_4++;
//    			i_travel_5++;
//    			i_travel_6++;

//    			if(u32_count<= 100)
//    			{
////    				printf("count %d\r\n",u32_count);
////    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//
//    				printf("count %d\r\n",u32_count);
//    				printf("tc1 %d\tc2 %d\tc2 %d\r\n",i32_travel[0],i32_travel[1],i32_travel[2]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }

    printf("count %d\r\n",u32_count);

	u32_count = 0;
    while(fgets(raw_data,5000,fp_coil)!= NULL)
//    while(u32_count < 10)
    {
    	//fgets(raw_data,5000,fp_coil);
    	token = strtok(raw_data,",");
//    	    		printf("%s\r\n",token);
    	    		//token = strtok(NULL,",");
    	while(token != NULL)
    	{
//    		if(u32_count >= 2)
//    		{
    			i32_coil[0] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[1] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[2] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[3] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[4] = strtol(token, NULL, 10);
    			token = strtok(NULL,",");
    			i32_coil[5] = strtol(token, NULL, 10);
    			//token = strtok(NULL,",");

    			*(int32_t *)i_coil_1 = i32_coil[0];
    			*(int32_t *)i_coil_2 = i32_coil[1];
    			*(int32_t *)i_coil_3 = i32_coil[2];
    			*(int32_t *)i_coil_4 = i32_coil[3];
    			*(int32_t *)i_coil_5 = i32_coil[4];
    			*(int32_t *)i_coil_6 = i32_coil[5];

    			i_coil_1++;
				i_coil_2++;
				i_coil_3++;
				i_coil_4++;
				i_coil_5++;
				i_coil_6++;

//    			if(u32_count>= 1000)
//    			{
//    				printf("count %d\r\n",u32_count);
//    				printf("c1 %d\tc2 %d\tc2 %d\tc4 %d\tc5 %d\tc6 %d\r\n",i32_coil[0],i32_coil[1],i32_coil[2],i32_coil[3],i32_coil[4],i32_coil[5]);
//    				//printf("%s\r\n",token);
//    			}
//    		}
    		token = strtok(NULL,",");
    	}
    	u32_count++;
    }

    printf("count %d\r\n",u32_count);

    fclose(fp_travel);
    fclose(fp_dcrm);
    fclose(fp_coil);

    system("sync");

    printf("DCRM CMD data updated\r\n");

}
