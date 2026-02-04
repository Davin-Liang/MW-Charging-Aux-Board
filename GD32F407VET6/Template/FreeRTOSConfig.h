//#ifndef FREERTOS_CONFIG_H
//#define FREERTOS_CONFIG_H

//#include "gd32f4xx.h" 

//#define configUSE_PREEMPTION		1
//#define configUSE_IDLE_HOOK			0
//#define configUSE_TICK_HOOK			0
//#define configCPU_CLOCK_HZ			( ( unsigned long ) 168000000 ) // GD32F407 主频
//#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
//#define configMAX_PRIORITIES		( 5 )
//#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )
//#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 30 * 1024 ) )
//#define configMAX_TASK_NAME_LEN		( 16 )
//#define configUSE_TRACE_FACILITY	0
//#define configUSE_16_BIT_TICKS		0
//#define configIDLE_SHOULD_YIELD		1
//#define configUSE_MUTEXES           1
//#define INCLUDE_vTaskDelay    1

///* Cortex-M4F 特定配置 */
//#ifdef __NVIC_PRIO_BITS
//	#define configPRIO_BITS       __NVIC_PRIO_BITS
//#else
//	#define configPRIO_BITS       4
//#endif

//#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xF
//#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5
//#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
//#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

///* 映射中断函数 */
//#define vPortSVCHandler    SVC_Handler
//#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler

//#endif /* FREERTOS_CONFIG_H */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* 1. 引入 GD32 固件库头文件，用于获取 SystemCoreClock */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
#endif
#include "gd32f4xx.h"

/******************************************************************************/
/* Hardware description related definitions. **********************************/
/******************************************************************************/

/* 使用系统核心时钟变量 (通常为 168000000) */
#define configCPU_CLOCK_HZ                    ( SystemCoreClock )

/******************************************************************************/
/* Scheduling behaviour related definitions. **********************************/
/******************************************************************************/

#define configTICK_RATE_HZ                    ( ( TickType_t ) 1000 ) // 1ms 一个 Tick，最常用
#define configUSE_PREEMPTION                  1
#define configUSE_TIME_SLICING                1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1 // GD32F4 支持硬件指令优化，设为 1 效率更高
#define configUSE_TICKLESS_IDLE               0   // 暂时关闭低功耗模式，降低移植难度
#define configMAX_PRIORITIES                  ( 32 ) // 增加优先级数量
#define configMINIMAL_STACK_SIZE              ( ( unsigned short ) 128 )
#define configMAX_TASK_NAME_LEN               ( 16 )
#define configTICK_TYPE_WIDTH_IN_BITS         TICK_TYPE_WIDTH_32_BITS // Cortex-M4 是 32 位机
#define configIDLE_SHOULD_YIELD               1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES 1
#define configQUEUE_REGISTRY_SIZE             8
#define configENABLE_BACKWARD_COMPATIBILITY   1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0
#define configSTACK_DEPTH_TYPE                uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE      size_t

/******************************************************************************/
/* Software timer related definitions. ****************************************/
/******************************************************************************/

#define configUSE_TIMERS                      1
#define configTIMER_TASK_PRIORITY             ( configMAX_PRIORITIES - 1 )
#define configTIMER_TASK_STACK_DEPTH          ( configMINIMAL_STACK_SIZE * 2 )
#define configTIMER_QUEUE_LENGTH              10

/******************************************************************************/
/* Memory allocation related definitions. *************************************/
/******************************************************************************/

/* 修改为 0，除非你需要自己定义 vApplicationGetIdleTaskMemory */
#define configSUPPORT_STATIC_ALLOCATION       0 
#define configSUPPORT_DYNAMIC_ALLOCATION      1

/* 堆大小设为 30KB (GD32F407 SRAM 足够大)，原 4KB 太小 */
#define configTOTAL_HEAP_SIZE                 ( ( size_t ) ( 30 * 1024 ) )

/* 修改为 0，让 FreeRTOS 自动在 heap_4.c 中定义 ucHeap 数组 */
#define configAPPLICATION_ALLOCATED_HEAP      0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP 0

/******************************************************************************/
/* Interrupt nesting behaviour configuration. *********************************/
/******************************************************************************/

/* Cortex-M4F 重要配置：中断优先级位数 */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS       __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS       4
#endif

/* 最低中断优先级 (15) */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0xF

/* FreeRTOS 可管理的最大中断优先级 (5) 
 * 高于此优先级(0-4)的中断不能调用 FreeRTOS API，但响应更快
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_API_CALL_INTERRUPT_PRIORITY   configMAX_SYSCALL_INTERRUPT_PRIORITY

/******************************************************************************/
/* Hook and callback function related definitions. ****************************/
/******************************************************************************/

#define configUSE_IDLE_HOOK                   0
#define configUSE_TICK_HOOK                   0
#define configUSE_MALLOC_FAILED_HOOK          0
#define configUSE_DAEMON_TASK_STARTUP_HOOK    0
#define configCHECK_FOR_STACK_OVERFLOW        0 // 调试时可设为 2

/******************************************************************************/
/* Run time and task stats gathering related definitions. *********************/
/******************************************************************************/

#define configGENERATE_RUN_TIME_STATS         0
#define configUSE_TRACE_FACILITY              1 // 开启可视化跟踪
#define configUSE_STATS_FORMATTING_FUNCTIONS  1 // 允许打印任务状态表

/******************************************************************************/
/* Definitions that include or exclude functionality. *************************/
/******************************************************************************/

#define configUSE_TASK_NOTIFICATIONS          1
#define configUSE_MUTEXES                     1
#define configUSE_RECURSIVE_MUTEXES           1
#define configUSE_COUNTING_SEMAPHORES         1
#define configUSE_QUEUE_SETS                  1
#define configUSE_APPLICATION_TASK_TAG        0

/* API Enable/Disable */
#define INCLUDE_vTaskPrioritySet              1
#define INCLUDE_uxTaskPriorityGet             1
#define INCLUDE_vTaskDelete                   1
#define INCLUDE_vTaskSuspend                  1
#define INCLUDE_xResumeFromISR                1
#define INCLUDE_vTaskDelayUntil               1
#define INCLUDE_vTaskDelay                    1
#define INCLUDE_xTaskGetSchedulerState        1
#define INCLUDE_xTaskGetCurrentTaskHandle     1
#define INCLUDE_uxTaskGetStackHighWaterMark   1
#define INCLUDE_xTaskGetIdleTaskHandle        1
#define INCLUDE_eTaskGetState                 1
#define INCLUDE_xEventGroupSetBitFromISR      1
#define INCLUDE_xTimerPendFunctionCall        1
#define INCLUDE_xTaskAbortDelay               1
#define INCLUDE_xTaskGetHandle                1
#define INCLUDE_xTaskResumeFromISR            1

/******************************************************************************/
/* Interrupt Handler Mapping (CRITICAL FOR GD32) ******************************/
/******************************************************************************/
/* 将 FreeRTOS 的内核中断函数映射到 GD32 启动文件的中断向量表名称 */
#define vPortSVCHandler        SVC_Handler
#define xPortPendSVHandler     PendSV_Handler
#define xPortSysTickHandler    SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
