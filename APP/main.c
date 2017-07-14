/*
*********************************************************************************************************
*												EMOS TEST CODE
* File    : main.c
* By      : tzaiyang
* Version : V2.0
*********************************************************************************************************
*/

#include "main.h"
/*
*********************************************************************************************************
*                                         Define stack of app tasks
*********************************************************************************************************
*/
OS_STK MainTask_Stk[MainTask_StkSize];
OS_STK App1Task_Stk[App1Task_StkSize];
OS_STK App2Task_Stk[App2Task_StkSize];
OS_STK App3Task_Stk[App3Task_StkSize];

int main(void)
{
	OSInit();																				/* Os kernel init */
	OSTaskCreate(MainTask, (void *)0, &MainTask_Stk[MainTask_StkSize-1], MainTask_Prio);	/* Create the main task */
	OSStart();																				/* Start sched */
	return 0;
}

/*
*********************************************************************************************************
*												Main Task
*********************************************************************************************************
*/
void MainTask(void *p_arg)
{
p_arg = p_arg;
OSStatInit();																				/* Statistic task init */
	while(1)
	{																						/* Create other tasks */
		OSTaskCreate(App1Task, (void *)0, &App1Task_Stk[App1Task_StkSize-1], App1Task_Prio);
		OSTaskCreate(App2Task, (void *)0, &App2Task_Stk[App2Task_StkSize-1], App2Task_Prio);
		OSTaskCreate(App3Task, (void *)0, &App3Task_Stk[App3Task_StkSize-1], App3Task_Prio);

		OSTimeDlyHMSM(0,0,1,0);																/* Task period is 1 second */
		//OSTaskSuspend(OS_PRIO_SELF);
		OSTaskDel(MainTask_Prio);															/* Delete main task */

	}
}

/*
*********************************************************************************************************
*                                         App1 Task
*********************************************************************************************************
*/
void App1Task(void *p_arg)
{
p_arg = p_arg;
	while(1)
	{
		printf("I am Task1!\n");
		OSTimeDlyHMSM(0, 0, 1, 0);															/* Task period is 1 second */
	}
}
/*
*********************************************************************************************************
*                                         App2 Task
*********************************************************************************************************
*/
void App2Task(void *p_arg)
{
p_arg = p_arg;
int count = 0;
	while(1)
	{
		printf("I am Task2!\n");
		OSTimeDlyHMSM(0, 0, 2, 0);															/* Task period is 1 second */
		count++;
		//OSTaskDel(OS_PRIO_SELF);															/* Delete taskself */
		if(count==1)OSTaskSuspend(App1Task_Prio);
		if(count==5)OSTaskResume(App1Task_Prio);
	}
}
/*
*********************************************************************************************************
*                                         App3 Task
*********************************************************************************************************
*/
void App3Task(void *p_arg)
{
	p_arg = p_arg;
	while (1)
	{
		printf("I am Task3!\n");
		
		OSTimeDlyHMSM(0, 0, 1, 0);															/* Task period is 1 second */
		//OSTaskDel(OS_PRIO_SELF);															/* Delete taskself */
	}
	
}