#include "data_sum_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "file_common.h"

static void data_sum_Task(void * param);
QueueHandle_t g_motorDataQueue = NULL;
QueueHandle_t g_optimalVPDataQueue = NULL;
QueueHandle_t g_currentVPChQueue = NULL;

static TaskHandle_t g_dataSumTaskHandle = NULL;

/**
  * @brief  data_sum_Task ��������
  * @param  param �������  
  * @return void
  **/
static void data_sum_Task(void * param)
{	
    struct MotorData_t currentMotorData;
    struct Optimal_v_p_t currentOptimalVP;
    struct CurrentV_P_Ch_t currentVPCh;
  
    g_motorDataQueue = xQueueCreate(MOTOR_DATA_QUEUE_LEN, sizeof(struct MotorData_t));
    g_optimalVPDataQueue = xQueueCreate(OPTIMAL_V_P_DATA_QUEUE_LEN, sizeof(struct Optimal_v_p_t));
    g_currentVPChQueue = xQueueCreate(CURRENT_V_P_CH_QUEUE_LEN, sizeof(struct CurrentV_P_Ch_t));
  
    while (1)
    {
      if (pdPASS == xQueueReceive(g_motorDataQueue, &currentMotorData, 10))
      {
        /* ��ӡ������� */
        mutual_printf("Position:(%.2f,%.2f)\r\n", currentMotorData.x, currentMotorData.y);
        /* ͨ����̫���ϴ�������� */

      }

        
      if (pdPASS == xQueueReceive(g_optimalVPDataQueue, &currentOptimalVP, 10))
      {
        /* ��ӡ������ݡ����Ź��ʺ��ĸ�ͨ�������ŵ�ѹ */
        mutual_printf("(%.2f,%.2f): V1 = %.2fV, V2 = %.2fV, V3 = %.2fV, V4 = %.2fV, P = %.3fdBm\r\n",
              currentMotorData.x,
              currentMotorData.y,
							currentOptimalVP.optimalVs[0],
              currentOptimalVP.optimalVs[1], 
              currentOptimalVP.optimalVs[2],
              currentOptimalVP.optimalVs[3], 
              currentOptimalVP.optimalP);


        /* �� SD ����д�������ݡ����Ź��ʺ��ĸ�ͨ�������ŵ�ѹ */
        // TODO: ȱ��·������
        write_x_y_v_p_to_csv("0:test_optimal_result.csv",
                              currentMotorData.x,
                              currentMotorData.y,
                              currentOptimalVP.optimalVs,
                              currentOptimalVP.optimalP);

        /* ͨ����̫���ϴ����Ž�� */

      }

      if (pdPASS == xQueueReceive(g_currentVPChQueue, &currentVPCh, 10))
      {
        /* ��ӡ��ǰͨ���ĵ�ѹ������ */
        mutual_printf("channel: %d current V: %.2f current P: %.3f\r\n",
        currentVPCh.channel,
        currentVPCh.currentV,
        currentVPCh.currentP);

        /* �� SD ����д�뵱ǰͨ���ĵ�ѹ�����ʡ�ͨ�� */
        // TODO: ȱ��·������
        write_v_p_to_csv("0:test_current_result.csv", 
                          currentVPCh.currentV, 
                          currentVPCh.currentP, 
                          currentVPCh.channel);

        /* ͨ����̫���ϴ���ǰͨ���ĵ�ѹ�����ʡ�ͨ�� */

      }

      vTaskDelay(10);
    }
}

BaseType_t create_task_for_data_sum(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )data_sum_Task,
						          (const char*    )"data_sum_Task",
						          (uint16_t       )size, 
                      (void*          )NULL,
                      (UBaseType_t    )priority, 
                      (TaskHandle_t*  )&g_dataSumTaskHandle);
}


