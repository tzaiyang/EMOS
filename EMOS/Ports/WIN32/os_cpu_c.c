/*
*********************************************************************************************************
*                                         
* File    : OS_CPU_C.C
* By      : Hshihua
* Version : V2.0
*********************************************************************************************************
*/
#define  OS_CPU_GLOBALS
#include "includes.h"
#include	<windows.h>
#include	<mmsystem.h>

#define    ALLOW_CS_RECURSION                                                  /* allow recursion of critical sections                                                     */

#define    SET_AFFINITY_MASK                                                   /* must be used for miltiprocessor systems                                                  */

#define    OS_CPU_TRACE                                                        /* allow print trace messages                                                               */

#define     WIN_MM_TICK                                                        /* Enabling WIN_MM_TICK will cause the uC/OS-II port to use the high resolution Multimedia  */
                                                                               /* timer instead of Sleep.  The higher resolution timer has a resolution of 1 ms, which     */
                                                                               /* results in a much more "real-time" feel - jdf                                            */

#define     WIN_MM_MIN_RES (1)                                                 /* Minimum timer resolution                                                                 */


/*
*********************************************************************************************************
*                                                    Prototypes
*********************************************************************************************************
*/
DWORD WINAPI OSCtxSwW32( LPVOID lpParameter );
DWORD WINAPI OSTickW32 ( LPVOID lpParameter );
DWORD WINAPI OSTaskW32 ( LPVOID lpParameter );

/*
*********************************************************************************************************
*                                                    Globals
*********************************************************************************************************
*/
INT32U              OSTerminateTickW32     = 0;
INT32U              OSTerminateCtxSwW32    = 0;
INT32U              OSTerminateTickCtrlW32 = 0;

HANDLE              OSTick32Handle;
HANDLE              OSCtxSwW32Event;
HANDLE              OSCtxSwW32Handle;

#ifdef WIN_MM_TICK
HANDLE              OSTickEventHandle;
DWORD               OSTickTimer;
TIMECAPS            OSTimeCap;
#endif


CRITICAL_SECTION    OSCriticalSection;
HANDLE              OSSemaphore;

OS_EMU_STK          *SS_SP;

#ifdef ALLOW_CS_RECURSION
    DWORD           ThreadID = 0;
    int             Recursion = 0;
#endif


/*
*********************************************************************************************************
*                                     OSInitHookBegin(void) 
* Description: This function is called by OSInit() at the beginning of OSInit().
*********************************************************************************************************
*/
void OSInitHookBegin(void)
{
    OSSemaphore = CreateSemaphore( NULL, 1, 1, NULL );
}


/*
*********************************************************************************************************
*                                                    OS_ENTER_CRITICAL()
* Defines the beginning of a critical section of code.
*********************************************************************************************************
*/

void OS_ENTER_CRITICAL()
{

    if( WaitForSingleObject( OSSemaphore, 0 ) == WAIT_TIMEOUT )
    {
        if( GetCurrentThreadId() != ThreadID )
            WaitForSingleObject( OSSemaphore, INFINITE );
    }
    ThreadID = GetCurrentThreadId();
    ++Recursion;

}

/*
*********************************************************************************************************
*                                                    OS_EXIT_CRITICAL()
* Defines the end of a critical section of code.
*********************************************************************************************************
*/

void OS_EXIT_CRITICAL()
{
    if( Recursion > 0 ) {
        if(--Recursion == 0 ) {
            ThreadID = 0;
            ReleaseSemaphore( OSSemaphore, 1, NULL );
        }
    }
    else {
#ifdef OS_CPU_TRACE
    OS_Printf("Error: OS_EXIT_CRITICAL\n");
#endif
    }

}

/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*********************************************************************************************************
*/

OS_STK *OSTaskStkInit (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt)
{
    OS_EMU_STK *stk;

    stk             = (OS_EMU_STK *)((char*)ptos-sizeof(OS_EMU_STK));   /* Load stack pointer          */
    stk->pData      = pdata;
    stk->Opt        = opt;
    stk->Task       = task;
    stk->Handle     = NULL;
    stk->Id         = 0;
    stk->Exit       = 0;

    return ((void *)stk);
}

/*$PAGE*/
#if OS_CPU_HOOKS_EN
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*********************************************************************************************************
*/

void OSTCBInitHook(OS_TCB *ptcb)
{
    OS_EMU_STK  *stack;
    
    stack = (OS_EMU_STK*) ptcb->OSTCBStkPtr;

    stack->Handle = CreateThread( NULL, 0, OSTaskW32, ptcb, CREATE_SUSPENDED, &stack->Id );
    
#ifdef SET_AFFINITY_MASK
    if( SetThreadAffinityMask( stack->Handle, 1 ) == 0 ) 
	{	
#ifdef OS_CPU_TRACE
        OS_Printf("Error: SetThreadAffinityMask\n");
#endif
    }
#endif
}


/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*********************************************************************************************************
*/

void OSTaskStatHook (void)
{
}


/*
*********************************************************************************************************
*                                               TASK IDLE HOOK
*********************************************************************************************************
*/

void OSTaskIdleHook (void)
{
}
#endif

/*
;*********************************************************************************************************
;                                          START MULTITASKING
;*********************************************************************************************************
*/
void OSStartHighRdy()
{
    DWORD  dwID;

    //OSInitTrace(100000);

    OS_ENTER_CRITICAL();

    ++OSRunning;

    OSCtxSwW32Event  = CreateEvent(NULL,FALSE,FALSE,NULL);
    OSCtxSwW32Handle = CreateThread( NULL, 0, OSCtxSwW32, 0, 0, &dwID );

    SetPriorityClass(OSCtxSwW32Handle,THREAD_PRIORITY_HIGHEST);

#ifdef SET_AFFINITY_MASK
    if( SetThreadAffinityMask( OSCtxSwW32Handle, 1 ) == 0 ) {
#ifdef OS_CPU_TRACE
        OS_Printf("Error: SetThreadAffinityMask\n");
#endif
       }
#endif
    
	SetThreadPriority(OSCtxSwW32Handle,THREAD_PRIORITY_TIME_CRITICAL);

    OSTick32Handle = CreateThread( NULL, 0, OSTickW32, 0, 0, &dwID );
    SetPriorityClass(OSTick32Handle,THREAD_PRIORITY_HIGHEST);

#ifdef SET_AFFINITY_MASK
    if( SetThreadAffinityMask( OSTick32Handle, 1 ) == 0 ) 
	{
#ifdef OS_CPU_TRACE
        OS_Printf("Error: SetThreadAffinityMask\n");
#endif
    }
#endif

	SetThreadPriority(OSTick32Handle,THREAD_PRIORITY_HIGHEST);

#ifdef WIN_MM_TICK
    timeGetDevCaps(&OSTimeCap, sizeof(OSTimeCap));

    if( OSTimeCap.wPeriodMin < WIN_MM_MIN_RES )
        OSTimeCap.wPeriodMin = WIN_MM_MIN_RES;

    timeBeginPeriod(OSTimeCap.wPeriodMin);

    OSTickEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    OSTickTimer       = timeSetEvent((1000/OS_TICKS_PER_SEC),OSTimeCap.wPeriodMin,(LPTIMECALLBACK)OSTickEventHandle, dwID,TIME_PERIODIC|TIME_CALLBACK_EVENT_SET);
#endif
    
    
    SS_SP = (OS_EMU_STK*) OSTCBHighRdy->OSTCBStkPtr;                      /* OSTCBCur = OSTCBHighRdy;     */
                                                                          /* OSPrioCur = OSPrioHighRdy;   */
    ResumeThread(SS_SP->Handle);

    OS_EXIT_CRITICAL();
    
    WaitForSingleObject(OSCtxSwW32Handle,INFINITE);

#ifdef WIN_MM_TICK
    timeKillEvent(OSTickTimer);
    timeEndPeriod(OSTimeCap.wPeriodMin);
    CloseHandle(OSTickEventHandle);
#endif

	CloseHandle(OSTick32Handle);
    CloseHandle(OSCtxSwW32Event);
}

/*
;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;*********************************************************************************************************
*/
void OSCtxSw()
{
    DWORD n = 0;

    if(!(SS_SP->Exit)) {
        n = SuspendThread(SS_SP->Handle);
    }

    //OSTrace( OBJ_SW, PT_SW_CTX, OSTCBHighRdy, 0, OSPrioCur, OSPrioHighRdy,0 );

    OSTCBCur = OSTCBHighRdy;
    OSPrioCur = OSPrioHighRdy;
    SS_SP = (OS_EMU_STK*) OSTCBHighRdy->OSTCBStkPtr;

    ResumeThread(SS_SP->Handle);
}

/*
;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From an ISR)
;                                        void OSIntCtxSw(void)
;*********************************************************************************************************
*/

void OSIntCtxSw()
{
    DWORD n = 0;

    if(!(SS_SP->Exit)) {
        n = SuspendThread(SS_SP->Handle);
    }

    //OSTrace( OBJ_SW, PT_SW_INT, OSTCBHighRdy, 0, OSPrioCur,OSPrioHighRdy,0 );

    OSTCBCur = OSTCBHighRdy;
    OSPrioCur = OSPrioHighRdy;
    SS_SP = (OS_EMU_STK*) OSTCBHighRdy->OSTCBStkPtr;

    ResumeThread(SS_SP->Handle);
}

/*
;*********************************************************************************************************
;                                            HANDLE TICK ISR
;*********************************************************************************************************
*/

void OSTickISR()
{
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OSCtxSwW32()
* Description: These functions are body of OS multitasking in WIN32.
*********************************************************************************************************
*/
DWORD WINAPI OSCtxSwW32( LPVOID lpParameter )
{
    while(!OSTerminateCtxSwW32)
    {
        if( WAIT_OBJECT_0 == WaitForSingleObject(OSCtxSwW32Event,INFINITE) )
        {
            OS_ENTER_CRITICAL();
            OSCtxSw();
            OS_EXIT_CRITICAL();
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OSTickW32()
*********************************************************************************************************
*/
DWORD WINAPI OSTickW32( LPVOID lpParameter )
{

    while(!OSTerminateTickW32)
    {
        OSTickISR();
#ifdef WIN_MM_TICK
        if( WaitForSingleObject(OSTickEventHandle, 5000) == WAIT_TIMEOUT)
        {
            #ifdef OS_CPU_TRACE
                OS_Printf("Error: MM OSTick Timeout!\n");
            #endif
        }

        ResetEvent(OSTickEventHandle);
#else
        Sleep(1000/OS_TICKS_PER_SEC);
#endif
    }

    return 0;
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OSTaskW32()
*********************************************************************************************************
*/
DWORD WINAPI OSTaskW32( LPVOID lpParameter )
{
    OS_TCB *ptcb;
    OS_EMU_STK  *stack;

    ptcb = (OS_TCB*) lpParameter;
    stack = (OS_EMU_STK*) ptcb->OSTCBStkPtr;
    
#ifdef DISABLE_PRIORITY_BOOST
        if( SetThreadPriorityBoost( stack->Handle, TRUE ) == 0 ) {
#ifdef OS_CPU_TRACE
            OS_Printf("Error: SetThreadPriorityBoost\n");
#endif
        }
#endif

    stack->Task(stack->pData);

    stack->Exit = 1;
    OSTaskDel(ptcb->OSTCBPrio);

    return 0;
}

int OS_Printf(char *str, ...)
{
    int  ret;

    va_list marker;

    va_start( marker, str );

    OS_ENTER_CRITICAL();
    ret = vprintf( str, marker );
    OS_EXIT_CRITICAL();

    va_end( marker );

    return ret;
}
