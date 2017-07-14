#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#define _CRT_SECURE_NO_WARNINGS	

#include <assert.h>
#include <includes.h>		
/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define MainTask_Prio 5
#define App1Task_Prio 7
#define App2Task_Prio 9
#define App3Task_Prio 10
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define MainTask_StkSize 1024
#define App1Task_StkSize 1024
#define App2Task_StkSize 1024
#define App3Task_StkSize 1024

#define OS_TASK_TMR_PRIO 65533
#define OS_IDLE_PRIO OS_TASK_IDLE_PRIO
#define OS_STAT_PRIO OS_TASK_STAT_PRIO


extern void MainTask(void *p_arg);
extern void App1Task(void *p_arg);
extern void App2Task(void *p_arg);
extern void App3Task(void *p_arg);

#endif