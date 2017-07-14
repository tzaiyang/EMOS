/*
*********************************************************************************************************
*                                          The Real-Time Kernel
*                                             CORE FUNCTIONS
* File    : OS_CORE.C
* By      : tzaiyang
* Version : V2.0
*********************************************************************************************************
*/
#ifndef  OS_MASTER_FILE
#define  OS_GLOBALS
#include <emos.h>
#endif

/*
*********************************************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*********************************************************************************************************
*/
INT8U  const  OSUnMapTbl[256] = {
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F                   */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF                   */
};

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  OS_InitEventList(void);
static  void  OS_InitMisc(void);
static  void  OS_InitRdyList(void);
static  void  OS_InitTaskIdle(void);
#if OS_TASK_STAT_EN > 0u
static  void  OS_InitTaskStat(void);
#endif
static  void  OS_InitTCBList(void);
static  void  OS_SchedNew(void);

/*
*********************************************************************************************************
*                                             INITIALIZATION
*********************************************************************************************************
*/
void  OSInit (void)
{
    OSInitHookBegin();                                           /* Call port specific initialization code   */

    OS_InitMisc();                                               /* Initialize miscellaneous variables       */

    OS_InitRdyList();                                            /* Initialize the Ready List                */

    OS_InitTCBList();                                            /* Initialize the free list of OS_TCBs      */

    OS_InitTaskIdle();                                           /* Create the Idle Task                     */

#if OS_TASK_STAT_EN > 0u
    OS_InitTaskStat();                                           /* Create the Statistic Task                */
#endif

}

/*
*********************************************************************************************************
*                                              ENTER ISR
*********************************************************************************************************
*/
void  OSIntEnter (void)
{
    if (OSRunning == OS_TRUE) {
        if (OSIntNesting < 255u) {
            OSIntNesting++;                      /* Increment ISR nesting level                        */
        }
    }
}

/*
*********************************************************************************************************
*                                               EXIT ISR
*********************************************************************************************************
*/
void  OSIntExit (void)
{

    if (OSRunning == OS_TRUE) {
        OS_ENTER_CRITICAL();
        if (OSIntNesting > 0u) {                           /* Prevent OSIntNesting from wrapping       */
            OSIntNesting--;
        }
        if (OSIntNesting == 0u) {                          /* Reschedule only if all ISRs complete ... */
            if (OSLockNesting == 0u) {                     /* ... and not locked.                      */
                OS_SchedNew();
                OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
                if (OSPrioHighRdy != OSPrioCur) {          /* No Ctx Sw if current task is highest rdy */
#if OS_TASK_PROFILE_EN > 0u
                    OSTCBHighRdy->OSTCBCtxSwCtr++;         /* Inc. # of context switches to this task  */
#endif
                    OSCtxSwCtr++;                          /* Keep track of the number of ctx switches */
                    OSIntCtxSw();                          /* Perform interrupt level ctx switch       */
                }
            }
        }
        OS_EXIT_CRITICAL();
    }
}


/*
*********************************************************************************************************
*                                          ENABLE SCHEDULING
*********************************************************************************************************
*/
#if OS_SCHED_LOCK_EN > 0u
void  OSSchedUnlock (void)
{
    if (OSRunning == OS_TRUE) {                            /* Make sure multitasking is running        */
        OS_ENTER_CRITICAL();
        if (OSLockNesting > 0u) {                          /* Do not decrement if already 0            */
            OSLockNesting--;                               /* Decrement lock nesting level             */
            if (OSLockNesting == 0u) {                     /* See if scheduler is enabled and ...      */
                if (OSIntNesting == 0u) {                  /* ... not in an ISR                        */
                    OS_EXIT_CRITICAL();
                    OS_Sched();                            /* See if a HPT is ready                    */
                } else {
                    OS_EXIT_CRITICAL();
                }
            } else {
                OS_EXIT_CRITICAL();
            }
        } else {
            OS_EXIT_CRITICAL();
        }
    }
}
#endif

/*
*********************************************************************************************************
*                                          START MULTITASKING
*********************************************************************************************************
*/
void  OSStart (void)
{
    if (OSRunning == OS_FALSE) {
        OS_SchedNew();                               /* Find highest priority's task priority number   */
        OSPrioCur     = OSPrioHighRdy;
        OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy]; /* Point to highest priority task ready to run    */
        OSTCBCur      = OSTCBHighRdy;
        OSStartHighRdy();                            /* Execute target specific code to start task     */
    }
}

/*
*********************************************************************************************************
*                                        STATISTICS INITIALIZATION
*
*                                             OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------)
*                                            OSIdleCtrMax
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0u
void  OSStatInit (void)
{
    OSTimeDly(2u);                               /* Synchronize with clock tick                        */
    OS_ENTER_CRITICAL();
    OSIdleCtr    = 0uL;                          /* Clear idle counter                                 */
    OS_EXIT_CRITICAL();
    OSTimeDly(OS_TICKS_PER_SEC / 10u);           /* Determine MAX. idle counter value for 1/10 second  */
    OS_ENTER_CRITICAL();
    OSIdleCtrMax = OSIdleCtr;                    /* Store maximum idle counter count in 1/10 second    */
    OSStatRdy    = OS_TRUE;
    OS_EXIT_CRITICAL();
}
#endif

/*
*********************************************************************************************************
*                                         PROCESS SYSTEM TICK
*********************************************************************************************************
*/
void  OSTimeTick (void)
{
    OS_TCB    *ptcb;
#if OS_TICK_STEP_EN > 0u
    BOOLEAN    step;
#endif

#if OS_TIME_GET_SET_EN > 0u
    OS_ENTER_CRITICAL();                                   /* Update the 32-bit tick counter               */
    OSTime++;
    OS_EXIT_CRITICAL();
#endif
    if (OSRunning == OS_TRUE) {
#if OS_TICK_STEP_EN > 0u
        switch (OSTickStepState) {                         /* Determine whether we need to process a tick  */
            case OS_TICK_STEP_DIS:                         /* Yes, stepping is disabled                    */
                 step = OS_TRUE;
                 break;

            case OS_TICK_STEP_WAIT:                        /* No,  waiting for uC/OS-View to set ...       */
                 step = OS_FALSE;                          /*      .. OSTickStepState to OS_TICK_STEP_ONCE */
                 break;

            case OS_TICK_STEP_ONCE:                        /* Yes, process tick once and wait for next ... */
                 step            = OS_TRUE;                /*      ... step command from uC/OS-View        */
                 OSTickStepState = OS_TICK_STEP_WAIT;
                 break;

            default:                                       /* Invalid case, correct situation              */
                 step            = OS_TRUE;
                 OSTickStepState = OS_TICK_STEP_DIS;
                 break;
        }
        if (step == OS_FALSE) {                            /* Return if waiting for step command           */
            return;
        }
#endif
        ptcb = OSTCBList;                                  /* Point at first TCB in TCB list               */
        while (ptcb->OSTCBPrio != OS_TASK_IDLE_PRIO) {     /* Go through all TCBs in TCB list              */
            OS_ENTER_CRITICAL();
            if (ptcb->OSTCBDly != 0u) {                    /* No, Delayed or waiting for event with TO     */
                ptcb->OSTCBDly--;                          /* Decrement nbr of ticks to end of delay       */
                if (ptcb->OSTCBDly == 0u) {                /* Check for timeout                            */

                    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY) {
                        ptcb->OSTCBStat  &= (INT8U)~(INT8U)OS_STAT_PEND_ANY;          /* Yes, Clear status flag   */
                        ptcb->OSTCBStatPend = OS_STAT_PEND_TO;                 /* Indicate PEND timeout    */
                    } else {
                        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;
                    }

                    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY) {  /* Is task suspended?       */
                        OSRdyGrp               |= ptcb->OSTCBBitY;             /* No,  Make ready          */
                        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    }
                }
            }
            ptcb = ptcb->OSTCBNext;                        /* Point at next TCB in TCB list                */
            OS_EXIT_CRITICAL();
        }
    }
}

/*
*********************************************************************************************************
*                                             INITIALIZATION
*                           INITIALIZE THE FREE LIST OF EVENT CONTROL BLOCKS
*********************************************************************************************************
*/
static  void  OS_InitEventList (void)
{
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
#if (OS_MAX_EVENTS > 1u)
    INT16U     ix;
    INT16U     ix_next;
    OS_EVENT  *pevent1;
    OS_EVENT  *pevent2;


    OS_MemClr((INT8U *)&OSEventTbl[0], sizeof(OSEventTbl)); /* Clear the event table                   */
    for (ix = 0u; ix < (OS_MAX_EVENTS - 1u); ix++) {        /* Init. list of free EVENT control blocks */
        ix_next = ix + 1u;
        pevent1 = &OSEventTbl[ix];
        pevent2 = &OSEventTbl[ix_next];
        pevent1->OSEventType    = OS_EVENT_TYPE_UNUSED;
        pevent1->OSEventPtr     = pevent2;
#if OS_EVENT_NAME_EN > 0u
        pevent1->OSEventName    = (INT8U *)(void *)"?";     /* Unknown name                            */
#endif
    }
    pevent1                         = &OSEventTbl[ix];
    pevent1->OSEventType            = OS_EVENT_TYPE_UNUSED;
    pevent1->OSEventPtr             = (OS_EVENT *)0;
#if OS_EVENT_NAME_EN > 0u
    pevent1->OSEventName            = (INT8U *)(void *)"?"; /* Unknown name                            */
#endif
    OSEventFreeList                 = &OSEventTbl[0];
#else
    OSEventFreeList                 = &OSEventTbl[0];       /* Only have ONE event control block       */
    OSEventFreeList->OSEventType    = OS_EVENT_TYPE_UNUSED;
    OSEventFreeList->OSEventPtr     = (OS_EVENT *)0;
#if OS_EVENT_NAME_EN > 0u
    OSEventFreeList->OSEventName    = (INT8U *)"?";         /* Unknown name                            */
#endif
#endif
#endif
}

/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                    INITIALIZE MISCELLANEOUS VARIABLES
*********************************************************************************************************
*/
static  void  OS_InitMisc (void)
{
#if OS_TIME_GET_SET_EN > 0u
    OSTime                    = 0uL;                       /* Clear the 32-bit system clock            */
#endif

    OSIntNesting              = 0u;                        /* Clear the interrupt nesting counter      */
    OSLockNesting             = 0u;                        /* Clear the scheduling lock counter        */

    OSTaskCtr                 = 0u;                        /* Clear the number of tasks                */

    OSRunning                 = OS_FALSE;                  /* Indicate that multitasking not started   */

    OSCtxSwCtr                = 0u;                        /* Clear the context switch counter         */
    OSIdleCtr                 = 0uL;                       /* Clear the 32-bit idle counter            */

#if OS_TASK_STAT_EN > 0u
    OSIdleCtrRun              = 0uL;
    OSIdleCtrMax              = 0uL;
    OSStatRdy                 = OS_FALSE;                  /* Statistic task is not ready              */
#endif
}

/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                       INITIALIZE THE READY LIST
*********************************************************************************************************
*/
static  void  OS_InitRdyList (void)
{
    INT8U  i;


    OSRdyGrp      = 0u;                                    /* Clear the ready list                     */
    for (i = 0u; i < OS_RDY_TBL_SIZE; i++) {
        OSRdyTbl[i] = 0u;
    }

    OSPrioCur     = 0u;
    OSPrioHighRdy = 0u;

    OSTCBHighRdy  = (OS_TCB *)0;
    OSTCBCur      = (OS_TCB *)0;
}

/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                         CREATING THE IDLE TASK
*********************************************************************************************************
*/
static  void  OS_InitTaskIdle (void)
{
    #if OS_STK_GROWTH == 1u
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1u],
                       OS_TASK_IDLE_PRIO);
    #else
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[0],
                       OS_TASK_IDLE_PRIO);
    #endif
}
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                      CREATING THE STATISTIC TASK
*********************************************************************************************************
*/
#if OS_TASK_STAT_EN > 0u
static  void  OS_InitTaskStat (void)
{
    #if OS_STK_GROWTH == 1u
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1u],     /* Set Top-Of-Stack               */
                       OS_TASK_STAT_PRIO);                             /* One higher than the idle task  */
    #else
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[0],                              /* Set Top-Of-Stack               */
                       OS_TASK_STAT_PRIO);                             /* One higher than the idle task  */
    #endif
#endif

}
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
*********************************************************************************************************
*/
static  void  OS_InitTCBList (void)
{
    INT8U    ix;
    INT8U    ix_next;
    OS_TCB  *ptcb1;
    OS_TCB  *ptcb2;

    OS_MemClr((INT8U *)&OSTCBTbl[0],     sizeof(OSTCBTbl));      /* Clear all the TCBs                 */
    OS_MemClr((INT8U *)&OSTCBPrioTbl[0], sizeof(OSTCBPrioTbl));  /* Clear the priority table           */
    for (ix = 0u; ix < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1u); ix++) {    /* Init. list of free TCBs     */
        ix_next =  ix + 1u;
        ptcb1   = &OSTCBTbl[ix];
        ptcb2   = &OSTCBTbl[ix_next];
        ptcb1->OSTCBNext = ptcb2;
#if OS_TASK_NAME_EN > 0u
        ptcb1->OSTCBTaskName = (INT8U *)(void *)"?";             /* Unknown name                       */
#endif
    }
    ptcb1                   = &OSTCBTbl[ix];
    ptcb1->OSTCBNext        = (OS_TCB *)0;                       /* Last OS_TCB                        */
#if OS_TASK_NAME_EN > 0u
    ptcb1->OSTCBTaskName    = (INT8U *)(void *)"?";              /* Unknown name                       */
#endif
    OSTCBList               = (OS_TCB *)0;                       /* TCB lists initializations          */
    OSTCBFreeList           = &OSTCBTbl[0];
}

/*
*********************************************************************************************************
*                                        CLEAR A SECTION OF MEMORY
* Description: This function is called by other emos services to clear a contiguous block of RAM.
*********************************************************************************************************
*/
void  OS_MemClr (INT8U  *pdest,
                 INT16U  size)
{
    while (size > 0u) {
        *pdest++ = (INT8U)0;
        size--;
    }
}

/*
*********************************************************************************************************
*                                              SCHEDULER
*********************************************************************************************************
*/
void  OS_Sched (void)
{
    OS_ENTER_CRITICAL();
    if (OSIntNesting == 0u) {                          /* Schedule only if all ISRs done and ...       */
        if (OSLockNesting == 0u) {                     /* ... scheduler is not locked                  */
            OS_SchedNew();
            OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
            if (OSPrioHighRdy != OSPrioCur) {          /* No Ctx Sw if current task is highest rdy     */
#if OS_TASK_PROFILE_EN > 0u
                OSTCBHighRdy->OSTCBCtxSwCtr++;         /* Inc. # of context switches to this task      */
#endif
                OSCtxSwCtr++;                          /* Increment context switch counter             */
                OS_TASK_SW();                          /* Perform a context switch                     */
            }
        }
    }
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                              FIND HIGHEST PRIORITY TASK READY TO RUN
*********************************************************************************************************
*/
static  void  OS_SchedNew (void)
{
	INT8U   y;
	y = OSUnMapTbl[OSRdyGrp];
	OSPrioHighRdy = (INT8U)((y << 3u) + OSUnMapTbl[OSRdyTbl[y]]);
}

/*
*********************************************************************************************************
*                                DETERMINE THE LENGTH OF AN ASCII STRING
*********************************************************************************************************
*/
INT8U  OS_StrLen (INT8U *psrc)
{
    INT8U  len;

    len = 0u;
    while (*psrc != OS_ASCII_NUL) {
        psrc++;
        len++;
    }
    return (len);
}

/*
*********************************************************************************************************
*                                              IDLE TASK
*********************************************************************************************************
*/
void  OS_TaskIdle (void *p_arg)
{
    p_arg = p_arg;                               /* Prevent compiler warning for not using 'p_arg'     */
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtr++;
        OS_EXIT_CRITICAL();
        OSTaskIdleHook();                        /* Call user definable HOOK                           */
    }
}

/*
*********************************************************************************************************
*                                            STATISTICS TASK
*
*                                          OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                         OSIdleCtrMax
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0u
void  OS_TaskStat (void *p_arg)
{
    p_arg = p_arg;                               /* Prevent compiler warning for not using 'p_arg'     */
    while (OSStatRdy == OS_FALSE) {
        OSTimeDly(2u * OS_TICKS_PER_SEC / 10u);  /* Wait until statistic task is ready                 */
    }
    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) {
        OSCPUUsage = 0u;
#if OS_TASK_SUSPEND_EN > 0u
        (void)OSTaskSuspend(OS_PRIO_SELF);
#else
        for (;;) {
            OSTimeDly(OS_TICKS_PER_SEC);
        }
#endif
    }
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtrRun = OSIdleCtr;                /* Obtain the of the idle counter for the past second */
        OSIdleCtr    = 0uL;                      /* Reset the idle counter for the next second         */
        OS_EXIT_CRITICAL();
        OSCPUUsage   = (INT8U)(100uL - OSIdleCtrRun / OSIdleCtrMax);
        OSTaskStatHook();                        /* Invoke user definable hook                         */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
        OS_TaskStatStkChk();                     /* Check the stacks for each task                     */
#endif
        OSTimeDly(OS_TICKS_PER_SEC / 10u);       /* Accumulate OSIdleCtr for the next 1/10 second      */
    }
}
#endif

/*
*********************************************************************************************************
*                                      CHECK ALL TASK STACKS
* Description: This function is called by OS_TaskStat() to check the stacks of each active task.
*********************************************************************************************************
*/
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void  OS_TaskStatStkChk (void)
{
    OS_TCB      *ptcb;
    OS_STK_DATA  stk_data;
    INT8U        err;
    INT8U        prio;
	
    for (prio = 0u; prio <= OS_TASK_IDLE_PRIO; prio++) {
        err = OSTaskStkChk(prio, &stk_data);
        if (err == OS_ERR_NONE) {
            ptcb = OSTCBPrioTbl[prio];
            if (ptcb != (OS_TCB *)0) {                               /* Make sure task 'ptcb' is ...   */
                if (ptcb != OS_TCB_RESERVED) {                       /* ... still valid.               */
#if OS_TASK_PROFILE_EN > 0u
                    #if OS_STK_GROWTH == 1u
                    ptcb->OSTCBStkBase = ptcb->OSTCBStkBottom + ptcb->OSTCBStkSize;
                    #else
                    ptcb->OSTCBStkBase = ptcb->OSTCBStkBottom - ptcb->OSTCBStkSize;
                    #endif
                    ptcb->OSTCBStkUsed = stk_data.OSUsed;            /* Store the number of bytes used */
#endif
                }
            }
        }
    }
}
#endif

/*
*********************************************************************************************************
*                                            INITIALIZE TCB
*********************************************************************************************************
*/
INT8U  OS_TCBInit (INT8U    prio,
                   OS_STK  *ptos,
                   OS_STK  *pbos,
                   INT16U   id,
                   INT32U   stk_size,
                   void    *pext,
                   INT16U   opt)
{
    OS_TCB    *ptcb;

#if OS_TASK_REG_TBL_SIZE > 0u
    INT8U      i;
#endif

    OS_ENTER_CRITICAL();
    ptcb = OSTCBFreeList;                                  /* Get a free TCB from the free TCB list    */
    if (ptcb != (OS_TCB *)0) {
        OSTCBFreeList            = ptcb->OSTCBNext;        /* Update pointer to free TCB list          */
        OS_EXIT_CRITICAL();
        ptcb->OSTCBStkPtr        = ptos;                   /* Load Stack pointer in TCB                */
        ptcb->OSTCBPrio          = prio;                   /* Load task priority into TCB              */
        ptcb->OSTCBStat          = OS_STAT_RDY;            /* Task is ready to run                     */
        ptcb->OSTCBStatPend      = OS_STAT_PEND_OK;        /* Clear pend status                        */
        ptcb->OSTCBDly           = 0u;                     /* Task is not delayed                      */

#if OS_TASK_CREATE_EXT_EN > 0u
        ptcb->OSTCBExtPtr        = pext;                   /* Store pointer to TCB extension           */
        ptcb->OSTCBStkSize       = stk_size;               /* Store stack size                         */
        ptcb->OSTCBStkBottom     = pbos;                   /* Store pointer to bottom of stack         */
        ptcb->OSTCBOpt           = opt;                    /* Store task options                       */
        ptcb->OSTCBId            = id;                     /* Store task ID                            */
#else
        pext                     = pext;                   /* Prevent compiler warning if not used     */
        stk_size                 = stk_size;
        pbos                     = pbos;
        opt                      = opt;
        id                       = id;
#endif

#if OS_TASK_DEL_EN > 0u
        ptcb->OSTCBDelReq        = OS_ERR_NONE;
#endif

#if OS_LOWEST_PRIO <= 63u                                         /* Pre-compute X, Y                  */
        ptcb->OSTCBY             = (INT8U)(prio >> 3u);
        ptcb->OSTCBX             = (INT8U)(prio & 0x07u);
#else                                                             /* Pre-compute X, Y                  */
        ptcb->OSTCBY             = (INT8U)((INT8U)(prio >> 4u) & 0xFFu);
        ptcb->OSTCBX             = (INT8U) (prio & 0x0Fu);
#endif
                                                                  /* Pre-compute BitX and BitY         */
        ptcb->OSTCBBitY          = (OS_PRIO)(1uL << ptcb->OSTCBY);
        ptcb->OSTCBBitX          = (OS_PRIO)(1uL << ptcb->OSTCBX);

#if (OS_EVENT_EN)
        ptcb->OSTCBEventPtr      = (OS_EVENT  *)0;         /* Task is not pending on an  event         */
#if (OS_EVENT_MULTI_EN > 0u)
        ptcb->OSTCBEventMultiPtr = (OS_EVENT **)0;         /* Task is not pending on any events        */
#endif
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u) && (OS_TASK_DEL_EN > 0u)
        ptcb->OSTCBFlagNode  = (OS_FLAG_NODE *)0;          /* Task is not pending on an event flag     */
#endif

#if (OS_MBOX_EN > 0u) || ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u))
        ptcb->OSTCBMsg       = (void *)0;                  /* No message received                      */
#endif

#if OS_TASK_PROFILE_EN > 0u
        ptcb->OSTCBCtxSwCtr    = 0uL;                      /* Initialize profiling variables           */
        ptcb->OSTCBCyclesStart = 0uL;
        ptcb->OSTCBCyclesTot   = 0uL;
        ptcb->OSTCBStkBase     = (OS_STK *)0;
        ptcb->OSTCBStkUsed     = 0uL;
#endif

#if OS_TASK_NAME_EN > 0u
        ptcb->OSTCBTaskName    = (INT8U *)(void *)"?";
#endif

#if OS_TASK_REG_TBL_SIZE > 0u                              /* Initialize the task variables            */
        for (i = 0u; i < OS_TASK_REG_TBL_SIZE; i++) {
            ptcb->OSTCBRegTbl[i] = 0u;
        }
#endif

        OSTCBInitHook(ptcb);

        OS_ENTER_CRITICAL();
        OSTCBPrioTbl[prio] = ptcb;
        ptcb->OSTCBNext    = OSTCBList;                    /* Link into TCB chain                      */
        ptcb->OSTCBPrev    = (OS_TCB *)0;
        if (OSTCBList != (OS_TCB *)0) {
            OSTCBList->OSTCBPrev = ptcb;
        }
        OSTCBList               = ptcb;
        OSRdyGrp               |= ptcb->OSTCBBitY;         /* Make task ready to run                   */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OSTaskCtr++;                                       /* Increment the #tasks counter             */
        OS_EXIT_CRITICAL();
        return (OS_ERR_NONE);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_TASK_NO_MORE_TCB);
}
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
