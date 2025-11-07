#ifndef __COMMAND_H
#define	__COMMAND_H

#include "FreeRTOS.h"
#include "task.h"
#include "bsp_attenuator.h"
#include "stm32f4xx.h"

#define COMMAND_QUEUE_LENGTH 10

enum MWCommandDef{
	/*
	 * 需求一：单片机不断发送电压给电压，数值从0.0V到10V，每次增加0.1V，同时每次发送完后，过一段时间读取功率计的值，
	 *		  每次读到的功率计值需要和上次读到的对比，最后得到最大功率值，得到最大功率值时，需要记录此时对于的最优电压
	 */
	demandOne, 
	/*
	 * 需求二：单片机不断发送电压给电压，数值从0.0V到10V，每次增加0.1V，同时每次发送完后，过一段时间读取功率计的值，
	 *		  每次读到的功率计值需要和上次读到的对比，最后得到最大功率值，得到最大功率值时，需要记录此时对于的最优电压。
	 *        得到最优电压后，设置电源为该电压，
	 */
	demandTwo,
	demandThree,
	demandFault,
	demandMotorControl,
	demandMutiFindOpt,
	noDemand
};

struct CommandInfo{
	enum MWCommandDef commandType;
	/* demandOne */
	int psChannel;
	/* demandThree */
	enum AttenuatorIndex_t attenuatorIndex;
	FunctionalState attNewState;
};

struct TaskHandleNode{
	TaskHandle_t taskHandle;
	char * taskName;
	
	struct TaskHandleNode * next;
};

#endif
