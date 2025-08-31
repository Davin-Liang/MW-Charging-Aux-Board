#ifndef __TEST_TASK_H
#define	__TEST_TASK_H

#include "FreeRTOS.h"

BaseType_t create_task_for_test(uint16_t size, UBaseType_t priority);

#endif
