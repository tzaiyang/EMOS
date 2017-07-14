/*
*********************************************************************************************************
*
* File    : OS_CPU_C.H
* By      : Hshihua
* Version : V2.0
*********************************************************************************************************
*/
#ifndef _OS_CPU_H
#define _OS_CPU_H

#include <stdio.h>
#include <windows.h>

/*
*********************************************************************************************************
*                                              Defines
*********************************************************************************************************
*/

//#define _WIN32_WINNT          0x0400 
#define  OS_CPU_EXT           extern
#define  OS_TASK_SW()         SetEvent(OSCtxSwW32Event)
#define  OS_STK_GROWTH        1                             /* Stack grows from HIGH to LOW memory on 80x86  */

/*
*********************************************************************************************************
*                                              GLOBALS
*********************************************************************************************************
*/

OS_CPU_EXT  CRITICAL_SECTION  OSCriticalSection;
OS_CPU_EXT  HANDLE            OSCtxSwW32Event;

/*
*********************************************************************************************************
*                                              Prototypes
*********************************************************************************************************
*/
int      OS_Printf( char* str, ... );            /* analog of printf, but use critical sections                                */

void     OS_ENTER_CRITICAL();
void     OS_EXIT_CRITICAL();

void     OSStartHighRdy();
void     OSIntCtxSw();

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned short OS_STK;                   /* Each stack entry is 16-bit wide                    */

#define BYTE           INT8S                     /* Define data types for backward compatibility ...   */
#define UBYTE          INT8U                    
#define WORD           INT16S                   
#define UWORD          INT16U
#define LONG           INT32S
#define ULONG          INT32U

typedef struct {
    void    *pData;
    INT16U  Opt;
    void    (*Task)(void*);
    void *  Handle;
    INT32U  Id;
    INT32   Exit;
} OS_EMU_STK;

#endif                                           /*_OS_CPU_H                                                                   */
