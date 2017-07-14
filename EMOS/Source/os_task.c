/*
*********************************************************************************************************
*                                          The Real-Time Kernel
*                                            TASK MANAGEMENT
*
* File    : OS_TASK.C
* By      : Wqinghong
* Version : V2.0
*********************************************************************************************************
*/

#include <emos.h>

/*
*********************************************************************************************************
*                                            CREATE A TASK
*********************************************************************************************************
*/
INT8U  OSTaskCreate (void   (*task)(void *p_arg),
                     void    *p_arg,
                     OS_STK  *ptos,
                     INT8U    prio)
{
    OS_STK    *psp;
    INT8U      err;

    if (prio > OS_LOWEST_PRIO) {             /* Make sure priority is within allowable range           */
        return (OS_ERR_PRIO_INVALID);
    }

    OS_ENTER_CRITICAL();
    if (OSIntNesting > 0u) {                 /* Make sure we don't create the task from within an ISR  */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_CREATE_ISR);
    }
    if (OSTCBPrioTbl[prio] == (OS_TCB *)0) { /* Make sure task doesn't already exist at this priority  */
        OSTCBPrioTbl[prio] = OS_TCB_RESERVED;/* Reserve the priority to prevent others from doing ...  */
                                             /* ... the same thing until task is created.              */
        OS_EXIT_CRITICAL();
        psp = OSTaskStkInit(task, p_arg, ptos, 0u);             /* Initialize the task's stack         */
        err = OS_TCBInit(prio, psp, (OS_STK *)0, 0u, 0u, (void *)0, 0u);
        if (err == OS_ERR_NONE) {
            if (OSRunning == OS_TRUE) {      /* Find highest priority task if multitasking has started */
                OS_Sched();
            }
        } else {
            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB *)0;/* Make this priority available to others                 */
            OS_EXIT_CRITICAL();
        }
        return (err);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_PRIO_EXIST);
}

/*
*********************************************************************************************************
*                                            DELETE A TASK
*********************************************************************************************************
*/
#if OS_TASK_DEL_EN > 0u
INT8U  OSTaskDel (INT8U prio)
{
    OS_TCB       *ptcb;

    if (OSIntNesting > 0u) {                            /* See if trying to delete from ISR            */
        return (OS_ERR_TASK_DEL_ISR);
    }
    if (prio == OS_TASK_IDLE_PRIO) {                    /* Not allowed to delete idle task             */
        return (OS_ERR_TASK_DEL_IDLE);
    }
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO) {                       /* Task priority valid ?                       */
        if (prio != OS_PRIO_SELF) {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif

/*$PAGE*/
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                         /* See if requesting to delete self            */
        prio = OSTCBCur->OSTCBPrio;                     /* Set priority to delete to current           */
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (OS_TCB *)0) {                          /* Task to delete must exist                   */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == OS_TCB_RESERVED) {                      /* Must not be assigned to Mutex               */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_DEL);
    }

    OSRdyTbl[ptcb->OSTCBY] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[ptcb->OSTCBY] == 0u) {                 /* Make task not ready                         */
        OSRdyGrp           &= (OS_PRIO)~ptcb->OSTCBBitY;
    }

#if (OS_EVENT_EN)
    if (ptcb->OSTCBEventPtr != (OS_EVENT *)0) {
        OS_EventTaskRemove(ptcb, ptcb->OSTCBEventPtr);  /* Remove this task from any event   wait list */
    }
#if (OS_EVENT_MULTI_EN > 0u)
    if (ptcb->OSTCBEventMultiPtr != (OS_EVENT **)0) {   /* Remove this task from any events' wait lists*/
        OS_EventTaskRemoveMulti(ptcb, ptcb->OSTCBEventMultiPtr);
    }
#endif
#endif

    ptcb->OSTCBDly      = 0u;                           /* Prevent OSTimeTick() from updating          */
    ptcb->OSTCBStat     = OS_STAT_RDY;                  /* Prevent task from being resumed             */
    ptcb->OSTCBStatPend = OS_STAT_PEND_OK;
    if (OSLockNesting < 255u) {                         /* Make sure we don't context switch           */
        OSLockNesting++;
    }
    OS_EXIT_CRITICAL();                                 /* Enabling INT. ignores next instruc.         */
     
    OS_ENTER_CRITICAL();                                /* ... disabled HERE!                          */
    if (OSLockNesting > 0u) {                           /* Remove context switch lock                  */
        OSLockNesting--;
    }
    
    OSTaskCtr--;                                        /* One less task being managed                 */
    OSTCBPrioTbl[prio] = (OS_TCB *)0;                   /* Clear old priority entry                    */
    if (ptcb->OSTCBPrev == (OS_TCB *)0) {               /* Remove from TCB chain                       */
        ptcb->OSTCBNext->OSTCBPrev = (OS_TCB *)0;
        OSTCBList                  = ptcb->OSTCBNext;
    } else {
        ptcb->OSTCBPrev->OSTCBNext = ptcb->OSTCBNext;
        ptcb->OSTCBNext->OSTCBPrev = ptcb->OSTCBPrev;
    }
    ptcb->OSTCBNext     = OSTCBFreeList;                /* Return TCB to free TCB list                 */
    OSTCBFreeList       = ptcb;
#if OS_TASK_NAME_EN > 0u
    ptcb->OSTCBTaskName = (INT8U *)(void *)"?";
#endif
    OS_EXIT_CRITICAL();
    if (OSRunning == OS_TRUE) {
        OS_Sched();                                     /* Find new highest priority task              */
    }
    return (OS_ERR_NONE);
}
#endif

/*
*********************************************************************************************************
*                                    REQUEST THAT A TASK DELETE ITSELF
*********************************************************************************************************
*/
/*$PAGE*/
#if OS_TASK_DEL_EN > 0u
INT8U  OSTaskDelReq (INT8U prio)
{
    INT8U      stat;
    OS_TCB    *ptcb;

    if (prio == OS_TASK_IDLE_PRIO) {                            /* Not allowed to delete idle task     */
        return (OS_ERR_TASK_DEL_IDLE);
    }
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO) {                               /* Task priority valid ?               */
        if (prio != OS_PRIO_SELF) {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    if (prio == OS_PRIO_SELF) {                                 /* See if a task is requesting to ...  */
        OS_ENTER_CRITICAL();                                    /* ... this task to delete itself      */
        stat = OSTCBCur->OSTCBDelReq;                           /* Return request status to caller     */
        OS_EXIT_CRITICAL();
        return (stat);
    }
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (OS_TCB *)0) {                                  /* Task to delete must exist           */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);                         /* Task must already be deleted        */
    }
    if (ptcb == OS_TCB_RESERVED) {                              /* Must NOT be assigned to a Mutex     */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_DEL);
    }
    ptcb->OSTCBDelReq = OS_ERR_TASK_DEL_REQ;                    /* Set flag indicating task to be DEL. */
    OS_EXIT_CRITICAL();
    return (OS_ERR_NONE);
}
#endif


/*
*********************************************************************************************************
*                                        RESUME A SUSPENDED TASK
*********************************************************************************************************
*/

#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskResume (INT8U prio)
{
    OS_TCB    *ptcb;

#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO) {                             /* Make sure task priority is valid      */
        return (OS_ERR_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (OS_TCB *)0) {                                /* Task to suspend must exist            */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_RESUME_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED) {                            /* See if assigned to Mutex              */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) != OS_STAT_RDY) { /* Task must be suspended                */
        ptcb->OSTCBStat &= (INT8U)~(INT8U)OS_STAT_SUSPEND;    /* Remove suspension                     */
        if (ptcb->OSTCBStat == OS_STAT_RDY) {                 /* See if task is now ready              */
            if (ptcb->OSTCBDly == 0u) {
                OSRdyGrp               |= ptcb->OSTCBBitY;    /* Yes, Make task ready to run           */
                OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                OS_EXIT_CRITICAL();
                if (OSRunning == OS_TRUE) {
                    OS_Sched();                               /* Find new highest priority task        */
                }
            } else {
                OS_EXIT_CRITICAL();
            }
        } else {                                              /* Must be pending on event              */
            OS_EXIT_CRITICAL();
        }
        return (OS_ERR_NONE);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_TASK_NOT_SUSPENDED);
}
#endif

/*
*********************************************************************************************************
*                                            SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if the
*              priority passed to OSTaskSuspend() is the priority of the calling task or OS_PRIO_SELF.
*********************************************************************************************************
*/
#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskSuspend (INT8U prio)
{
    BOOLEAN    self;
    OS_TCB    *ptcb;
    INT8U      y;

#if OS_ARG_CHK_EN > 0u
    if (prio == OS_TASK_IDLE_PRIO) {                            /* Not allowed to suspend idle task    */
        return (OS_ERR_TASK_SUSPEND_IDLE);
    }
    if (prio >= OS_LOWEST_PRIO) {                               /* Task priority valid ?               */
        if (prio != OS_PRIO_SELF) {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                                 /* See if suspend SELF                 */
        prio = OSTCBCur->OSTCBPrio;
        self = OS_TRUE;
    } else if (prio == OSTCBCur->OSTCBPrio) {                   /* See if suspending self              */
        self = OS_TRUE;
    } else {
        self = OS_FALSE;                                        /* No suspending another task          */
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (OS_TCB *)0) {                                  /* Task to suspend must exist          */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_SUSPEND_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED) {                              /* See if assigned to Mutex            */
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    y            = ptcb->OSTCBY;
    OSRdyTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;                   /* Make task not ready                 */
    if (OSRdyTbl[y] == 0u) {
        OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBStat |= OS_STAT_SUSPEND;                         /* Status of task is 'SUSPENDED'       */
    OS_EXIT_CRITICAL();
    if (self == OS_TRUE) {                                      /* Context switch only if SELF         */
        OS_Sched();                                             /* Find new highest priority task      */
    }
    return (OS_ERR_NONE);
}
#endif
