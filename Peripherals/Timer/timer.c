/*
 * timer.c
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
#include <ucontext.h>
#include <sys/time.h>
       #include <signal.h>
       #include <time.h>


/**************************** EXTERN VARIABLES **********************/


/**************************** MACROS ********************************/


/**************************** TYPEDEFS ******************************/


/**************************** STATIC VARIABLES **********************/


/**************************** GLOBAL VARIABLES **********************/
timer_t	st_timerid_1,
		st_timerid_2,
		st_timerid_3,
		st_timerid_4,
		st_timerid_RS485;

volatile uint8_t 	u8_timer1_expired = 0,
					u8_timer2_expired = 0,
					u8_timer3_expired = 0,
					u8_timer4_expired = 0,
					u8_timer_RS485_expired = 0;

/**************************** FUNCTION PROTOTYPES *******************/


/**************************** FUNCTIONS *****************************/

void posix_timer_1_handler(int sig, siginfo_t *si, void *uc)
{
	timer_t *timer_id;

	timer_id = si->si_value.sival_ptr;

	//printf("Timer_1_id %d in signal handler\r\n",*timer_id);
    //printf("Caught signal %d\n", sig);
    //print_siginfo(si);

    if(*timer_id == st_timerid_1)
    {
    	printf("timer 1 expired\r\n");
    	u8_timer1_expired = 1;
    }

    if(*timer_id == st_timerid_2)
    {
    	printf("timer 2 expired\r\n");
    	u8_timer2_expired  = 1;
    }

    if(*timer_id == st_timerid_3)
    {
    	printf("timer 3 expired\r\n");
    	u8_timer3_expired = 1;
    }

    if(*timer_id == st_timerid_4)
    {
    	printf("timer 4 expired\r\n");
    	u8_timer4_expired = 1;
    }

    if(*timer_id == st_timerid_RS485)
    {
    	printf("timer RS485 expired\r\n");
    	u8_timer_RS485_expired = 1;
    }
}


void timer_init()
{
	struct itimerspec 	/*st_timer_specs_1,*/
						st_timer_specs_2,
						st_timer_specs_3,
						st_timer_specs_4;

	struct sigaction 	st_sa_1,
						st_sa_2,
						st_sa_3,
						st_sa_4,
						st_sa_RS485;

	struct sigevent		st_sev_1,
						st_sev_2,
						st_sev_3,
						st_sev_4,
						st_sev_RS485;

    /* ------------ create timer 1--------------------*/
	st_sev_1.sigev_notify = SIGEV_SIGNAL;
	st_sev_1.sigev_signo = SIGRTMIN;
	st_sev_1.sigev_value.sival_ptr = &st_timerid_1;
    if (timer_create(CLOCK_REALTIME, &st_sev_1, &st_timerid_1) == -1)
    	perror("timer_1_create");

    printf("timer_id  1 %d\r\n",st_timerid_1);

    /* ------------ Set timer 1-----------------------*/
//    st_timer_specs_1.it_value.tv_sec = 4;
//    st_timer_specs_1.it_value.tv_nsec = 0;
//    st_timer_specs_1.it_interval.tv_sec = 0;
//    st_timer_specs_1.it_interval.tv_nsec = 0;
//
//    if (timer_settime(&st_timerid_1, 0, &st_timer_specs_1, NULL) == -1)
//    	perror("timer_1_settime");

    st_sa_1.sa_flags = SA_SIGINFO;
    st_sa_1.sa_sigaction = posix_timer_1_handler;
    sigemptyset(&st_sa_1.sa_mask);
    if (sigaction(SIGRTMIN, &st_sa_1, NULL) == -1)
        perror(": \r\n");

    /*------------------------------------------------------------------*/

    /* ------------ create timer 2--------------------*/
	st_sev_2.sigev_notify = SIGEV_SIGNAL;
	st_sev_2.sigev_signo = SIGRTMIN;
	st_sev_2.sigev_value.sival_ptr = &st_timerid_2;
    if (timer_create(CLOCK_REALTIME, &st_sev_2, &st_timerid_2) == -1)
    	perror("timer_2_create");

    printf("timer_id  2 %d\r\n",st_timerid_2);

    /* ------------ Set timer 2-----------------------*/
//    st_timer_specs_2.it_value.tv_sec = 4;
//    st_timer_specs_2.it_value.tv_nsec = 0;
//    st_timer_specs_2.it_interval.tv_sec = 0;
//    st_timer_specs_2.it_interval.tv_nsec = 0;
//
//    if (timer_settime(&st_timerid_2, 0, &st_timer_specs_2, NULL) == -1)
//    	perror("timer_2_settime");

    st_sa_2.sa_flags = SA_SIGINFO;
    st_sa_2.sa_sigaction = posix_timer_1_handler;
    sigemptyset(&st_sa_2.sa_mask);
    if (sigaction(SIGRTMIN, &st_sa_2, NULL) == -1)
        perror(": \r\n");

    /* ------------ create timer 3--------------------*/
	st_sev_3.sigev_notify = SIGEV_SIGNAL;
	st_sev_3.sigev_signo = SIGRTMIN;
	st_sev_3.sigev_value.sival_ptr = &st_timerid_3;
    if (timer_create(CLOCK_REALTIME, &st_sev_3, &st_timerid_3) == -1)
    	perror("timer_3_create");

    printf("timer_id  3 %d\r\n",st_timerid_3);

    st_sa_3.sa_flags = SA_SIGINFO;
    st_sa_3.sa_sigaction = posix_timer_1_handler;
    sigemptyset(&st_sa_3.sa_mask);
    if (sigaction(SIGRTMIN, &st_sa_3, NULL) == -1)
        perror(": \r\n");

    /* ------------ create timer 4--------------------*/
	st_sev_4.sigev_notify = SIGEV_SIGNAL;
	st_sev_4.sigev_signo = SIGRTMIN;
	st_sev_4.sigev_value.sival_ptr = &st_timerid_4;
    if (timer_create(CLOCK_REALTIME, &st_sev_4, &st_timerid_4) == -1)
    	perror("timer_4_create");

    printf("timer_id  4 %d\r\n",st_timerid_4);

    st_sa_4.sa_flags = SA_SIGINFO;
    st_sa_4.sa_sigaction = posix_timer_1_handler;
    sigemptyset(&st_sa_4.sa_mask);
    if (sigaction(SIGRTMIN, &st_sa_4, NULL) == -1)
        perror(": \r\n");

    /* ------------ create timer for RS485 ------------------*/

    st_sev_RS485.sigev_notify = SIGEV_SIGNAL;
    st_sev_RS485.sigev_signo = SIGRTMIN;
    st_sev_RS485.sigev_value.sival_ptr = &st_timerid_RS485;
    if (timer_create(CLOCK_REALTIME, &st_sev_RS485, &st_timerid_RS485) == -1)
    	perror("timer_RS485_create");

    printf("timer_id  RS485 %d\r\n",st_timerid_RS485);

    st_sa_RS485.sa_flags = SA_SIGINFO;
    st_sa_RS485.sa_sigaction = posix_timer_1_handler;
    sigemptyset(&st_sa_RS485.sa_mask);
    if (sigaction(SIGRTMIN, &st_sa_RS485, NULL) == -1)
        perror(": \r\n");

}
