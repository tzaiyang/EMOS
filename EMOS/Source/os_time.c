/*
*********************************************************************************************************
*                                          The Real-Time Kernel
*                                             TIME MANAGEMENT
* File    : OS_TIME.C
* By      : Czhiming
* Version : V2.0
*********************************************************************************************************
*/
#ifndef  OS_MASTER_FILE
#include <emos.h>
#endif

/*
*********************************************************************************************************
*                                       DELAY TASK 'n' TICKS
*********************************************************************************************************
*/
void  OSTimeDly (INT32U ticks)
{
    INT8U      y;

    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        return;
    }
    if (OSLockNesting > 0u) {                    /* See if called with scheduler locked                */
        return;
    }
    if (ticks > 0u) {                            /* 0 means no delay!                                  */
        OS_ENTER_CRITICAL();
        y            =  OSTCBCur->OSTCBY;        /* Delay current task                                 */
        OSRdyTbl[y] &= (OS_PRIO)~OSTCBCur->OSTCBBitX;
        if (OSRdyTbl[y] == 0u) {
            OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
        }
        OSTCBCur->OSTCBDly = ticks;              /* Load ticks in TCB                                  */
        OS_EXIT_CRITICAL();
        OS_Sched();                              /* Find next task to run!                             */
    }
}

/*
*********************************************************************************************************
*                                     DELAY TASK FOR SPECIFIED TIME
*********************************************************************************************************
*/
#if OS_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM (INT8U   hours,
                      INT8U   minutes,
                      INT8U   seconds,
                      INT16U  ms)
{
    INT32U ticks;


    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        return (OS_ERR_TIME_DLY_ISR);
    }
    if (OSLockNesting > 0u) {                    /* See if called with scheduler locked                */
        return (OS_ERR_SCHED_LOCKED);
    }
#if OS_ARG_CHK_EN > 0u
    if (hours == 0u) {
        if (minutes == 0u) {
            if (seconds == 0u) {
                if (ms == 0u) {
                    return (OS_ERR_TIME_ZERO_DLY);
                }
            }
        }
    }
    if (minutes > 59u) {
        return (OS_ERR_TIME_INVALID_MINUTES);    /* Validate arguments to be within range              */
    }
    if (seconds > 59u) {
        return (OS_ERR_TIME_INVALID_SECONDS);
    }
    if (ms > 999u) {
        return (OS_ERR_TIME_INVALID_MS);
    }
#endif
                                                 /* Compute the total number of clock ticks required.. */
                                                 /* .. (rounded to the nearest tick)                   */
    ticks = ((INT32U)hours * 3600uL + (INT32U)minutes * 60uL + (INT32U)seconds) * OS_TICKS_PER_SEC
          + OS_TICKS_PER_SEC * ((INT32U)ms + 500uL / OS_TICKS_PER_SEC) / 1000uL;
    OSTimeDly(ticks);
    return (OS_ERR_NONE);
}
#endif

/*
*********************************************************************************************************
*                                         RESUME A DELAYED TASK
*********************************************************************************************************
*/

#if OS_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume (INT8U prio)
{
    OS_TCB    *ptcb;

    if (prio >= OS_LOWEST_PRIO) {
        return (OS_ERR_PRIO_INVALID);
    }
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];                                 /* Make sure that task exist            */
    if (ptcb == (OS_TCB *)0) {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);                        /* The task does not exist              */
    }
    if (ptcb == OS_TCB_RESERVED) {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);                        /* The task does not exist              */
    }
    if (ptcb->OSTCBDly == 0u) {                                /* See if task is delayed               */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TIME_NOT_DLY);                          /* Indicate that task was not delayed   */
    }

    ptcb->OSTCBDly = 0u;                                       /* Clear the time delay                 */
    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY) {
        ptcb->OSTCBStat     &= ~OS_STAT_PEND_ANY;              /* Yes, Clear status flag               */
        ptcb->OSTCBStatPend  =  OS_STAT_PEND_TO;               /* Indicate PEND timeout                */
    } else {
        ptcb->OSTCBStatPend  =  OS_STAT_PEND_OK;
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY) {  /* Is task suspended?                   */
        OSRdyGrp               |= ptcb->OSTCBBitY;             /* No,  Make ready                      */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OS_EXIT_CRITICAL();
        OS_Sched();                                            /* See if this is new highest priority  */
    } else {
        OS_EXIT_CRITICAL();                                    /* Task may be suspended                */
    }
    return (OS_ERR_NONE);
}
#endif

/*
*********************************************************************************************************
*                                         GET CURRENT SYSTEM TIME
*********************************************************************************************************
*/
#if OS_TIME_GET_SET_EN > 0u
INT32U  OSTimeGet (void)
{
    INT32U     ticks;

    OS_ENTER_CRITICAL();
    ticks = OSTime;
    OS_EXIT_CRITICAL();
    return (ticks);
}
#endif

/*
*********************************************************************************************************
*                                            SET SYSTEM CLOCK
*********************************************************************************************************
*/

#if OS_TIME_GET_SET_EN > 0u
void  OSTimeSet (INT32U ticks)
{
    OS_ENTER_CRITICAL();
    OSTime = ticks;
    OS_EXIT_CRITICAL();
}
#endif
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
