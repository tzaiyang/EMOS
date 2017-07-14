/*
*********************************************************************************************************
*                                          The Real-Time Kernel
*
* File    : EMOS.H
* By      : tzaiyang
* Version : V2.91
*********************************************************************************************************
*/

#ifndef   EMOS_H
#define   EMOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <os_cfg_r.h>
#include <os_cpu.h>

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#ifndef  OS_FALSE
#define  OS_FALSE                       0u
#endif

#ifndef  OS_TRUE
#define  OS_TRUE                        1u
#endif

#define  OS_ASCII_NUL            (INT8U)0

#define  OS_PRIO_SELF                0xFFu              /* Indicate SELF priority                      */

#if OS_TASK_STAT_EN > 0u
#define  OS_N_SYS_TASKS                 2u              /* Number of system tasks                      */
#else
#define  OS_N_SYS_TASKS                 1u
#endif

#define  OS_TASK_STAT_PRIO  (OS_LOWEST_PRIO - 1u)       /* Statistic task priority                     */
#define  OS_TASK_IDLE_PRIO  (OS_LOWEST_PRIO)            /* IDLE      task priority                     */

#if OS_LOWEST_PRIO <= 63u
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8u + 1u) /* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8u + 1u) /* Size of ready table                         */
#else
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 16u + 1u)/* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 16u + 1u)/* Size of ready table                         */
#endif

#define  OS_TASK_IDLE_ID            65535u              /* ID numbers for Idle, Stat and Timer tasks   */
#define  OS_TASK_STAT_ID            65534u
#define  OS_TASK_TMR_ID             65533u

#define  OS_EVENT_EN           (((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u) || (OS_SEM_EN > 0u) || (OS_MUTEX_EN > 0u))

#define  OS_TCB_RESERVED        ((OS_TCB *)1)

/*$PAGE*/
/*
*********************************************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************
*/
#define  OS_STAT_RDY                 0x00u  /* Ready to run                                            */
#define  OS_STAT_SEM                 0x01u  /* Pending on semaphore                                    */
#define  OS_STAT_MBOX                0x02u  /* Pending on mailbox                                      */
#define  OS_STAT_Q                   0x04u  /* Pending on queue                                        */
#define  OS_STAT_SUSPEND             0x08u  /* Task is suspended                                       */
#define  OS_STAT_MUTEX               0x10u  /* Pending on mutual exclusion semaphore                   */
#define  OS_STAT_FLAG                0x20u  /* Pending on event flag group                             */
#define  OS_STAT_MULTI               0x80u  /* Pending on multiple events                              */

#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)

/*
*********************************************************************************************************
*                           TASK PEND STATUS (Status codes for OSTCBStatPend)
*********************************************************************************************************
*/
#define  OS_STAT_PEND_OK                0u  /* Pending status OK, not pending, or pending complete     */
#define  OS_STAT_PEND_TO                1u  /* Pending timed out                                       */
#define  OS_STAT_PEND_ABORT             2u  /* Pending aborted                                         */

/*
*********************************************************************************************************
*                                        OS_EVENT types
*********************************************************************************************************
*/
#define  OS_EVENT_TYPE_UNUSED           0u
#define  OS_EVENT_TYPE_MBOX             1u
#define  OS_EVENT_TYPE_Q                2u
#define  OS_EVENT_TYPE_SEM              3u
#define  OS_EVENT_TYPE_MUTEX            4u
#define  OS_EVENT_TYPE_FLAG             5u


/*
*********************************************************************************************************
*                                         EVENT FLAGS
*********************************************************************************************************
*/
#define  OS_FLAG_WAIT_CLR_ALL           0u  /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_AND           0u

#define  OS_FLAG_WAIT_CLR_ANY           1u  /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_OR            1u

#define  OS_FLAG_WAIT_SET_ALL           2u  /* Wait for ALL    the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_AND           2u

#define  OS_FLAG_WAIT_SET_ANY           3u  /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_OR            3u


#define  OS_FLAG_CONSUME             0x80u  /* Consume the flags if condition(s) satisfied             */


#define  OS_FLAG_CLR                    0u
#define  OS_FLAG_SET                    1u

/*
*********************************************************************************************************
*                                   Values for OSTickStepState
*
* Note(s): This feature is used by uC/OS-View.
*********************************************************************************************************
*/

#if OS_TICK_STEP_EN > 0u
#define  OS_TICK_STEP_DIS               0u  /* Stepping is disabled, tick runs as mormal               */
#define  OS_TICK_STEP_WAIT              1u  /* Waiting for uC/OS-View to set OSTickStepState to _ONCE  */
#define  OS_TICK_STEP_ONCE              2u  /* Process tick once and wait for next cmd from uC/OS-View */
#endif

/*
*********************************************************************************************************
*       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
*********************************************************************************************************
*/
#define  OS_DEL_NO_PEND                 0u
#define  OS_DEL_ALWAYS                  1u

/*
*********************************************************************************************************
*                                        OS_Pend() OPTIONS
*
* These #defines are used to establish the options for OS_PendAbort().
*********************************************************************************************************
*/
#define  OS_PEND_OPT_NONE               0u  /* NO option selected                                      */
#define  OS_PEND_OPT_BROADCAST          1u  /* Broadcast action to ALL tasks waiting                   */



/*
*********************************************************************************************************
*                                 TASK OPTIONS (see OSTaskCreateExt())
*********************************************************************************************************
*/
#define  OS_TASK_OPT_NONE          0x0000u  /* NO option selected                                      */
#define  OS_TASK_OPT_STK_CHK       0x0001u  /* Enable stack checking for the task                      */
#define  OS_TASK_OPT_STK_CLR       0x0002u  /* Clear the stack when the task is create                 */
#define  OS_TASK_OPT_SAVE_FP       0x0004u  /* Save the contents of any floating-point registers       */

/*
*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************
*/
#define OS_ERR_NONE                     0u

#define OS_ERR_PRIO_EXIST              40u
#define OS_ERR_PRIO                    41u
#define OS_ERR_PRIO_INVALID            42u

#define OS_ERR_SCHED_LOCKED            50u
#define OS_ERR_SEM_OVF                 51u

#define OS_ERR_TASK_CREATE_ISR         60u
#define OS_ERR_TASK_DEL                61u
#define OS_ERR_TASK_DEL_IDLE           62u
#define OS_ERR_TASK_DEL_REQ            63u
#define OS_ERR_TASK_DEL_ISR            64u
#define OS_ERR_TASK_NAME_TOO_LONG      65u
#define OS_ERR_TASK_NO_MORE_TCB        66u
#define OS_ERR_TASK_NOT_EXIST          67u
#define OS_ERR_TASK_NOT_SUSPENDED      68u
#define OS_ERR_TASK_OPT                69u
#define OS_ERR_TASK_RESUME_PRIO        70u
#define OS_ERR_TASK_SUSPEND_IDLE       71u
#define OS_ERR_TASK_SUSPEND_PRIO       72u
#define OS_ERR_TASK_WAITING            73u

#define OS_ERR_TIME_NOT_DLY            80u
#define OS_ERR_TIME_INVALID_MINUTES    81u
#define OS_ERR_TIME_INVALID_SECONDS    82u
#define OS_ERR_TIME_INVALID_MS         83u
#define OS_ERR_TIME_ZERO_DLY           84u
#define OS_ERR_TIME_DLY_ISR            85u

#define OS_ERR_MEM_INVALID_PART        90u
#define OS_ERR_MEM_INVALID_BLKS        91u
#define OS_ERR_MEM_INVALID_SIZE        92u
#define OS_ERR_MEM_NO_FREE_BLKS        93u
#define OS_ERR_MEM_FULL                94u
#define OS_ERR_MEM_INVALID_PBLK        95u
#define OS_ERR_MEM_INVALID_PMEM        96u
#define OS_ERR_MEM_INVALID_PDATA       97u
#define OS_ERR_MEM_INVALID_ADDR        98u
#define OS_ERR_MEM_NAME_TOO_LONG       99u

#define OS_ERR_NOT_MUTEX_OWNER        100u

#define OS_ERR_FLAG_INVALID_PGRP      110u
#define OS_ERR_FLAG_WAIT_TYPE         111u
#define OS_ERR_FLAG_NOT_RDY           112u
#define OS_ERR_FLAG_INVALID_OPT       113u
#define OS_ERR_FLAG_GRP_DEPLETED      114u
#define OS_ERR_FLAG_NAME_TOO_LONG     115u

#define OS_ERR_PIP_LOWER              120u


/*$PAGE*/
/*
*********************************************************************************************************
*                                          EVENT CONTROL BLOCK
*********************************************************************************************************
*/

#if OS_LOWEST_PRIO <= 63u
typedef  INT8U    OS_PRIO;
#else
typedef  INT16U   OS_PRIO;
#endif

#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
typedef struct os_event {
    INT8U    OSEventType;                    /* Type of event control block (see OS_EVENT_TYPE_xxxx)    */
    void    *OSEventPtr;                     /* Pointer to message or queue structure                   */
    INT16U   OSEventCnt;                     /* Semaphore Count (not used if other EVENT type)          */
    OS_PRIO  OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
    OS_PRIO  OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */

#if OS_EVENT_NAME_EN > 0u
    INT8U   *OSEventName;
#endif
} OS_EVENT;
#endif


/*
*********************************************************************************************************
*                                       EVENT FLAGS CONTROL BLOCK
*********************************************************************************************************
*/

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)

#if OS_FLAGS_NBITS == 8u                    /* Determine the size of OS_FLAGS (8, 16 or 32 bits)       */
typedef  INT8U    OS_FLAGS;
#endif

#if OS_FLAGS_NBITS == 16u
typedef  INT16U   OS_FLAGS;
#endif

#if OS_FLAGS_NBITS == 32u
typedef  INT32U   OS_FLAGS;
#endif


typedef struct os_flag_grp {                /* Event Flag Group                                        */
    INT8U         OSFlagType;               /* Should be set to OS_EVENT_TYPE_FLAG                     */
    void         *OSFlagWaitList;           /* Pointer to first NODE of task waiting on event flag     */
    OS_FLAGS      OSFlagFlags;              /* 8, 16 or 32 bit flags                                   */
#if OS_FLAG_NAME_EN > 0u
    INT8U        *OSFlagName;
#endif
} OS_FLAG_GRP;



typedef struct os_flag_node {               /* Event Flag Wait List Node                               */
    void         *OSFlagNodeNext;           /* Pointer to next     NODE in wait list                   */
    void         *OSFlagNodePrev;           /* Pointer to previous NODE in wait list                   */
    void         *OSFlagNodeTCB;            /* Pointer to TCB of waiting task                          */
    void         *OSFlagNodeFlagGrp;        /* Pointer to Event Flag Group                             */
    OS_FLAGS      OSFlagNodeFlags;          /* Event flag to wait on                                   */
    INT8U         OSFlagNodeWaitType;       /* Type of wait:                                           */
                                            /*      OS_FLAG_WAIT_AND                                   */
                                            /*      OS_FLAG_WAIT_ALL                                   */
                                            /*      OS_FLAG_WAIT_OR                                    */
                                            /*      OS_FLAG_WAIT_ANY                                   */
} OS_FLAG_NODE;
#endif

/*
*********************************************************************************************************
*                                            TASK STACK DATA
*********************************************************************************************************
*/

#if OS_TASK_CREATE_EXT_EN > 0u
typedef struct os_stk_data {
    INT32U  OSFree;                    /* Number of free bytes on the stack                            */
    INT32U  OSUsed;                    /* Number of bytes used on the stack                            */
} OS_STK_DATA;
#endif

/*
*********************************************************************************************************
*                                  MEMORY PARTITION DATA STRUCTURES
*********************************************************************************************************
*/

#if (OS_MEM_EN > 0u) && (OS_MAX_MEM_PART > 0u)
typedef struct os_mem {                     /* MEMORY CONTROL BLOCK                                    */
	void   *OSMemAddr;                      /* Pointer to beginning of memory partition                */
	void   *OSMemFreeList;                  /* Pointer to list of free memory blocks                   */
	INT32U  OSMemBlkSize;                   /* Size (in bytes) of each block of memory                 */
	INT32U  OSMemNBlks;                     /* Total number of blocks in this partition                */
	INT32U  OSMemNFree;                     /* Number of memory blocks remaining in this partition     */
#if OS_MEM_NAME_EN > 0u
	INT8U  *OSMemName;                      /* Memory partition name                                   */
#endif
} OS_MEM;


typedef struct os_mem_data {
	void   *OSAddr;                         /* Ptr to the beginning address of the memory partition    */
	void   *OSFreeList;                     /* Ptr to the beginning of the free list of memory blocks  */
	INT32U  OSBlkSize;                      /* Size (in bytes) of each memory block                    */
	INT32U  OSNBlks;                        /* Total number of blocks in the partition                 */
	INT32U  OSNFree;                        /* Number of memory blocks free                            */
	INT32U  OSNUsed;                        /* Number of memory blocks used                            */
} OS_MEM_DATA;
#endif

/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    OS_STK          *OSTCBStkPtr;           /* Pointer to current top of stack                         */

#if OS_TASK_CREATE_EXT_EN > 0u
    void            *OSTCBExtPtr;           /* Pointer to user definable data for TCB extension        */
    OS_STK          *OSTCBStkBottom;        /* Pointer to bottom of stack                              */
    INT32U           OSTCBStkSize;          /* Size of task stack (in number of stack elements)        */
    INT16U           OSTCBOpt;              /* Task options as passed by OSTaskCreateExt()             */
    INT16U           OSTCBId;               /* Task ID (0..65535)                                      */
#endif

    struct os_tcb   *OSTCBNext;             /* Pointer to next     TCB in the TCB list                 */
    struct os_tcb   *OSTCBPrev;             /* Pointer to previous TCB in the TCB list                 */

#if (OS_EVENT_EN)
    OS_EVENT        *OSTCBEventPtr;         /* Pointer to          event control block                 */
#endif

#if (OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u)
    OS_EVENT       **OSTCBEventMultiPtr;    /* Pointer to multiple event control blocks                */
#endif

#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u)
    void            *OSTCBMsg;              /* Message received from OSMboxPost() or OSQPost()         */
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
#if OS_TASK_DEL_EN > 0u
    OS_FLAG_NODE    *OSTCBFlagNode;         /* Pointer to event flag node                              */
#endif
    OS_FLAGS         OSTCBFlagsRdy;         /* Event flags that made task ready to run                 */
#endif

    INT32U           OSTCBDly;              /* Nbr ticks to delay task or, timeout waiting for event   */
    INT8U            OSTCBStat;             /* Task      status                                        */
    INT8U            OSTCBStatPend;         /* Task PEND status                                        */
    INT8U            OSTCBPrio;             /* Task priority (0 == highest)                            */

    INT8U            OSTCBX;                /* Bit position in group  corresponding to task priority   */
    INT8U            OSTCBY;                /* Index into ready table corresponding to task priority   */
    OS_PRIO          OSTCBBitX;             /* Bit mask to access bit position in ready table          */
    OS_PRIO          OSTCBBitY;             /* Bit mask to access bit position in ready group          */

#if OS_TASK_DEL_EN > 0u
    INT8U            OSTCBDelReq;           /* Indicates whether a task needs to delete itself         */
#endif

#if OS_TASK_PROFILE_EN > 0u
    INT32U           OSTCBCtxSwCtr;         /* Number of time the task was switched in                 */
    INT32U           OSTCBCyclesTot;        /* Total number of clock cycles the task has been running  */
    INT32U           OSTCBCyclesStart;      /* Snapshot of cycle counter at start of task resumption   */
    OS_STK          *OSTCBStkBase;          /* Pointer to the beginning of the task stack              */
    INT32U           OSTCBStkUsed;          /* Number of bytes used from the stack                     */
#endif

#if OS_TASK_NAME_EN > 0u
    INT8U           *OSTCBTaskName;
#endif

#if OS_TASK_REG_TBL_SIZE > 0u
    INT32U           OSTCBRegTbl[OS_TASK_REG_TBL_SIZE];
#endif
} OS_TCB;

/*$PAGE*/
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_EXT  INT32U            OSCtxSwCtr;               /* Counter of number of context switches           */

#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
OS_EXT  OS_EVENT         *OSEventFreeList;          /* Pointer to list of free EVENT control blocks    */
OS_EXT  OS_EVENT          OSEventTbl[OS_MAX_EVENTS];/* Table of EVENT control blocks                   */
#endif

#if OS_TASK_STAT_EN > 0u
OS_EXT  INT8U             OSCPUUsage;               /* Percentage of CPU used                          */
OS_EXT  INT32U            OSIdleCtrMax;             /* Max. value that idle ctr can take in 1 sec.     */
OS_EXT  INT32U            OSIdleCtrRun;             /* Val. reached by idle ctr at run time in 1 sec.  */
OS_EXT  BOOLEAN           OSStatRdy;                /* Flag indicating that the statistic task is rdy  */
OS_EXT  OS_STK            OSTaskStatStk[OS_TASK_STAT_STK_SIZE];      /* Statistics task stack          */
#endif

OS_EXT  INT8U             OSIntNesting;             /* Interrupt nesting level                         */

OS_EXT  INT8U             OSLockNesting;            /* Multitasking lock nesting level                 */

OS_EXT  INT8U             OSPrioCur;                /* Priority of current task                        */
OS_EXT  INT8U             OSPrioHighRdy;            /* Priority of highest priority task               */

OS_EXT  OS_PRIO           OSRdyGrp;                        /* Ready list group                         */
OS_EXT  OS_PRIO           OSRdyTbl[OS_RDY_TBL_SIZE];       /* Table of tasks which are ready to run    */

OS_EXT  BOOLEAN           OSRunning;                       /* Flag indicating that kernel is running   */

OS_EXT  INT8U             OSTaskCtr;                       /* Number of tasks created                  */

OS_EXT  volatile  INT32U  OSIdleCtr;                                 /* Idle counter                   */


OS_EXT  OS_STK            OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];      /* Idle task stack                */


OS_EXT  OS_TCB           *OSTCBCur;                        /* Pointer to currently running TCB         */
OS_EXT  OS_TCB           *OSTCBFreeList;                   /* Pointer to list of free TCBs             */
OS_EXT  OS_TCB           *OSTCBHighRdy;                    /* Pointer to highest priority TCB R-to-R   */
OS_EXT  OS_TCB           *OSTCBList;                       /* Pointer to doubly linked list of TCBs    */
OS_EXT  OS_TCB           *OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];    /* Table of pointers to created TCBs   */
OS_EXT  OS_TCB            OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];   /* Table of TCBs                  */

#if (OS_MEM_EN > 0u) && (OS_MAX_MEM_PART > 0u)
OS_EXT  OS_MEM           *OSMemFreeList;            /* Pointer to free list of memory partitions       */
OS_EXT  OS_MEM            OSMemTbl[OS_MAX_MEM_PART];/* Storage for memory partition manager            */
#endif

#if OS_TICK_STEP_EN > 0u
OS_EXT  INT8U             OSTickStepState;          /* Indicates the state of the tick step feature    */
#endif

#if OS_TIME_GET_SET_EN > 0u
OS_EXT  volatile  INT32U  OSTime;                   /* Current value of system time (in ticks)         */
#endif

extern  INT8U   const     OSUnMapTbl[256];          /* Priority->Index    lookup table                 */


/*
*********************************************************************************************************
*                                            TASK MANAGEMENT
*********************************************************************************************************
*/
#if OS_TASK_CHANGE_PRIO_EN > 0u
INT8U         OSTaskChangePrio        (INT8U            oldprio,
                                       INT8U            newprio);
#endif

#if OS_TASK_CREATE_EN > 0u
INT8U         OSTaskCreate            (void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       OS_STK          *ptos,
                                       INT8U            prio);
#endif

#if OS_TASK_CREATE_EXT_EN > 0u
INT8U         OSTaskCreateExt         (void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       OS_STK          *ptos,
                                       INT8U            prio,
                                       INT16U           id,
                                       OS_STK          *pbos,
                                       INT32U           stk_size,
                                       void            *pext,
                                       INT16U           opt);
#endif

#if OS_TASK_DEL_EN > 0u
INT8U         OSTaskDel               (INT8U            prio);
INT8U         OSTaskDelReq            (INT8U            prio);
#endif

#if OS_TASK_NAME_EN > 0u
INT8U         OSTaskNameGet           (INT8U            prio,
                                       INT8U          **pname,
                                       INT8U           *perr);

void          OSTaskNameSet           (INT8U            prio,
                                       INT8U           *pname,
                                       INT8U           *perr);
#endif

#if OS_TASK_SUSPEND_EN > 0u
INT8U         OSTaskResume            (INT8U            prio);
INT8U         OSTaskSuspend           (INT8U            prio);
#endif

#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
INT8U         OSTaskStkChk            (INT8U            prio,
                                       OS_STK_DATA     *p_stk_data);
#endif

#if OS_TASK_QUERY_EN > 0u
INT8U         OSTaskQuery             (INT8U            prio,
                                       OS_TCB          *p_task_data);
#endif



#if OS_TASK_REG_TBL_SIZE > 0u
INT32U        OSTaskRegGet            (INT8U            prio,
                                       INT8U            id,
                                       INT8U           *perr);

void          OSTaskRegSet            (INT8U            prio,
                                       INT8U            id,
                                       INT32U           value,
                                       INT8U           *perr);
#endif

/*
*********************************************************************************************************
*                                            TIME MANAGEMENT
*********************************************************************************************************
*/

void          OSTimeDly               (INT32U           ticks);

#if OS_TIME_DLY_HMSM_EN > 0u
INT8U         OSTimeDlyHMSM           (INT8U            hours,
                                       INT8U            minutes,
                                       INT8U            seconds,
                                       INT16U           ms);
#endif

#if OS_TIME_DLY_RESUME_EN > 0u
INT8U         OSTimeDlyResume         (INT8U            prio);
#endif

#if OS_TIME_GET_SET_EN > 0u
INT32U        OSTimeGet               (void);
void          OSTimeSet               (INT32U           ticks);
#endif

void          OSTimeTick              (void);

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

void          OSInit                  (void);

void          OSIntEnter              (void);
void          OSIntExit               (void);

#if OS_SCHED_LOCK_EN > 0u
void          OSSchedLock             (void);
void          OSSchedUnlock           (void);
#endif

void          OSStart                 (void);

void          OSStatInit              (void);

INT16U        OSVersion               (void);

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
void          OS_FlagInit             (void);
void          OS_FlagUnlink           (OS_FLAG_NODE    *pnode);
#endif

void          OS_MemClr               (INT8U           *pdest,
                                       INT16U           size);

void          OS_MemCopy              (INT8U           *pdest,
                                       INT8U           *psrc,
                                       INT16U           size);

#if (OS_MEM_EN > 0u) && (OS_MAX_MEM_PART > 0u)
void          OS_MemInit              (void);
#endif

#if OS_Q_EN > 0u
void          OS_QInit                (void);
#endif

void          OS_Sched                (void);

#if (OS_EVENT_NAME_EN > 0u) || (OS_FLAG_NAME_EN > 0u) || (OS_MEM_NAME_EN > 0u) || (OS_TASK_NAME_EN > 0u)
INT8U         OS_StrLen               (INT8U           *psrc);
#endif

void          OS_TaskIdle             (void            *p_arg);

void          OS_TaskReturn           (void);

#if OS_TASK_STAT_EN > 0u
void          OS_TaskStat             (void            *p_arg);
#endif

#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void          OS_TaskStkClr           (OS_STK          *pbos,
                                       INT32U           size,
                                       INT16U           opt);
#endif

#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void          OS_TaskStatStkChk       (void);
#endif

INT8U         OS_TCBInit              (INT8U            prio,
                                       OS_STK          *ptos,
                                       OS_STK          *pbos,
                                       INT16U           id,
                                       INT32U           stk_size,
                                       void            *pext,
                                       INT16U           opt);


/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                      (Target Specific Functions)
*********************************************************************************************************
*/
void          OSInitHookBegin         (void);


void          OSTaskCreateHook        (OS_TCB          *ptcb);

void          OSTaskIdleHook          (void);

void          OSTaskStatHook          (void);
OS_STK       *OSTaskStkInit           (void           (*task)(void *p_arg),
                                       void            *p_arg,
                                       OS_STK          *ptos,
                                       INT16U           opt);

#if OS_TASK_SW_HOOK_EN > 0u
void          OSTaskSwHook            (void);
#endif

void          OSTCBInitHook           (OS_TCB          *ptcb);

#if OS_TIME_TICK_HOOK_EN > 0u
void          OSTimeTickHook          (void);
#endif


#ifdef __cplusplus
}
#endif

#endif
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
