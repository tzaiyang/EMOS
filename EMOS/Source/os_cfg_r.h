/*
*********************************************************************************************************
*                                          The Real-Time Kernel
*										    Configuration File	
* File    : OS_CFG.H
* By      : tzaiyang
* Version : V2.91
*********************************************************************************************************
*/
#ifndef OS_CFG_H
#define OS_CFG_H


                                       /* ---------------------- MISCELLANEOUS ----------------------- */
#define OS_APP_HOOKS_EN           1u   /* Application-defined hooks are called from the emos hooks */
#define OS_ARG_CHK_EN             0u   /* Enable (1) or Disable (0) argument checking                  */
#define OS_CPU_HOOKS_EN           1u   /* emos hooks are found in the processor port files         */

#define OS_DEBUG_EN               1u   /* Enable(1) debug variables                                    */

#define OS_EVENT_MULTI_EN         1u   /* Include code for OSEventPendMulti()                          */
#define OS_EVENT_NAME_EN          1u   /* Enable names for Sem, Mutex, Mbox and Q                      */

#define OS_LOWEST_PRIO           63u   /* Defines the lowest priority that can be assigned ...         */
                                       /* ... MUST NEVER be higher than 254!                           */

#define OS_MAX_EVENTS            10u   /* Max. number of event control blocks in your application      */
#define OS_MAX_FLAGS              5u   /* Max. number of Event Flag Groups    in your application      */
#define OS_MAX_MEM_PART           5u   /* Max. number of memory partitions                             */
#define OS_MAX_QS                 4u   /* Max. number of queue control blocks in your application      */
#define OS_MAX_TASKS             20u   /* Max. number of tasks in your application, MUST be >= 2       */

#define OS_SCHED_LOCK_EN          1u   /* Include code for OSSchedLock() and OSSchedUnlock()           */

#define OS_TICK_STEP_EN           1u   /* Enable tick stepping feature for uC/OS-View                  */
#define OS_TICKS_PER_SEC        100u   /* Set the number of ticks in one second                        */

                                       /* --------------------- TASK STACK SIZE ---------------------- */
#define OS_TASK_TMR_STK_SIZE    128u   /* Timer      task stack size (# of OS_STK wide entries)        */
#define OS_TASK_STAT_STK_SIZE   128u   /* Statistics task stack size (# of OS_STK wide entries)        */
#define OS_TASK_IDLE_STK_SIZE   128u   /* Idle       task stack size (# of OS_STK wide entries)        */

                                       /* --------------------- TASK MANAGEMENT ---------------------- */
#define OS_TASK_CHANGE_PRIO_EN    1u   /*     Include code for OSTaskChangePrio()                      */
#define OS_TASK_CREATE_EN         1u   /*     Include code for OSTaskCreate()                          */
#define OS_TASK_CREATE_EXT_EN     1u   /*     Include code for OSTaskCreateExt()                       */
#define OS_TASK_DEL_EN            1u   /*     Include code for OSTaskDel()                             */
#define OS_TASK_NAME_EN           1u   /*     Enable task names                                        */
#define OS_TASK_PROFILE_EN        1u   /*     Include variables in OS_TCB for profiling                */
#define OS_TASK_QUERY_EN          1u   /*     Include code for OSTaskQuery()                           */
#define OS_TASK_REG_TBL_SIZE      1u   /*     Size of task variables array (#of INT32U entries)        */
#define OS_TASK_STAT_EN           1u   /*     Enable (1) or Disable(0) the statistics task             */
#define OS_TASK_STAT_STK_CHK_EN   1u   /*     Check task stacks from statistic task                    */
#define OS_TASK_SUSPEND_EN        1u   /*     Include code for OSTaskSuspend() and OSTaskResume()      */
#define OS_TASK_SW_HOOK_EN        1u   /*     Include code for OSTaskSwHook()                          */

                               /* --------------------- TIME MANAGEMENT ---------------------- */
#define OS_TIME_DLY_HMSM_EN       1u   /*     Include code for OSTimeDlyHMSM()                         */
#define OS_TIME_DLY_RESUME_EN     1u   /*     Include code for OSTimeDlyResume()                       */
#define OS_TIME_GET_SET_EN        1u   /*     Include code for OSTimeGet() and OSTimeSet()             */
#define OS_TIME_TICK_HOOK_EN      1u   /*     Include code for OSTimeTickHook()                        */

#define OS_MEM_EN                 1u
#endif
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
