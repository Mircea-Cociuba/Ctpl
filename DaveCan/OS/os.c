/*
 * os.c
 *
 *  Created on: Jul 23, 2019
 *      Author: uia94406
 */

#include <DAVE.h>
#include "os.h"
#include "Can.h"
#include "Ctpl.h"
#include "appl.h"

#define ONE_MILI_SEC 1000U

/*OS timers*/
uint32_t TimerId_1ms;
uint32_t TimerId_10ms;
uint32_t TimerId_5ms;



void Os_Init(void)
{
	TimerId_1ms = (uint32_t)SYSTIMER_CreateTimer(ONE_MILI_SEC,SYSTIMER_MODE_PERIODIC,(void*)OS_1MS_Task,NULL);
	TimerId_5ms = (uint32_t)SYSTIMER_CreateTimer(ONE_MILI_SEC * 5u,SYSTIMER_MODE_PERIODIC,(void*)OS_5MS_Task,NULL);
	TimerId_10ms = (uint32_t)SYSTIMER_CreateTimer(ONE_MILI_SEC * 10u,SYSTIMER_MODE_PERIODIC,(void*)OS_10MS_Task,NULL);

	//timer is created successfully, now start/run software timer

	SYSTIMER_StartTimer(TimerId_1ms);
	SYSTIMER_StartTimer(TimerId_5ms);
	SYSTIMER_StartTimer(TimerId_10ms);
}


/* Function: Will be called every 1ms
 * input none
 * output none
 *
 * */
void OS_1MS_Task(void)
{
	Ctpl_MainFunction();
}

/* Function: Will be called every 5ms
 * input none
 * output none
 *
 * */
void OS_5MS_Task(void)
{
	Appl_MainFunction();
}

/* Function: Will be called every 10ms
 * input none
 * output none
 *
 * */
void OS_10MS_Task(void)
{
#if 0
	//just for DEMO! should be deleted when starting the real soft development
	static uint8_t ticker=0;
	uint8_t dat[8]={0,0,0,0, 0,0,0,0};

	dat[0]=ticker++;
	Can_SetTxData(&dat[0]);
	Can_Transmit();
#endif
}

