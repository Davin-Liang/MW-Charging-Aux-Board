#ifndef __COMMAND_H
#define	__COMMAND_H

#include "FreeRTOS.h"
#include "task.h"
#include "bsp_attenuator.h"
#include "stm32f4xx.h"

#define COMMAND_QUEUE_LENGTH 10

enum MWCommandDef{
	/*
	 * ����һ����Ƭ�����Ϸ��͵�ѹ����ѹ����ֵ��0.0V��10V��ÿ������0.1V��ͬʱÿ�η�����󣬹�һ��ʱ���ȡ���ʼƵ�ֵ��
	 *		  ÿ�ζ����Ĺ��ʼ�ֵ��Ҫ���ϴζ����ĶԱȣ����õ������ֵ���õ������ֵʱ����Ҫ��¼��ʱ���ڵ����ŵ�ѹ
	 */
	demandOne, 
	/*
	 * ���������Ƭ�����Ϸ��͵�ѹ����ѹ����ֵ��0.0V��10V��ÿ������0.1V��ͬʱÿ�η�����󣬹�һ��ʱ���ȡ���ʼƵ�ֵ��
	 *		  ÿ�ζ����Ĺ��ʼ�ֵ��Ҫ���ϴζ����ĶԱȣ����õ������ֵ���õ������ֵʱ����Ҫ��¼��ʱ���ڵ����ŵ�ѹ��
	 *        �õ����ŵ�ѹ�����õ�ԴΪ�õ�ѹ��
	 */
	demandTwo,
	demandThree,
	demandFault,
	demandMotorControl,
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
