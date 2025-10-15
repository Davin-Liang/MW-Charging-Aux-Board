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
  * @brief  data_sum_Task 任务主体
  * @param  param 任务参数  
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
        /* 打印电机数据 */
        mutual_printf("Position:(%.2f,%.2f)\r\n", currentMotorData.x, currentMotorData.y);
        /* 通过以太网上传电机数据 */

      }

        
      if (pdPASS == xQueueReceive(g_optimalVPDataQueue, &currentOptimalVP, 10))
      {
        /* 打印电机数据、最优功率和四个通道的最优电压 */
        mutual_printf("(%.2f,%.2f): V1 = %.2fV, V2 = %.2fV, V3 = %.2fV, V4 = %.2fV, P = %.3fdBm\r\n",
              currentMotorData.x,
              currentMotorData.y,
							currentOptimalVP.optimalVs[0],
              currentOptimalVP.optimalVs[1], 
              currentOptimalVP.optimalVs[2],
              currentOptimalVP.optimalVs[3], 
              currentOptimalVP.optimalP);


        /* 往 SD 卡中写入电机数据、最优功率和四个通道的最优电压 */
        // TODO: 缺少路径定义
        write_x_y_v_p_to_csv("0:test_optimal_result.csv",
                              currentMotorData.x,
                              currentMotorData.y,
                              currentOptimalVP.optimalVs,
                              currentOptimalVP.optimalP);

        /* 通过以太网上传最优结果 */

      }

      if (pdPASS == xQueueReceive(g_currentVPChQueue, &currentVPCh, 10))
      {
        /* 打印当前通道的电压、功率 */
        mutual_printf("channel: %d current V: %.2f current P: %.3f\r\n",
        currentVPCh.channel,
        currentVPCh.currentV,
        currentVPCh.currentP);

        /* 往 SD 卡中写入当前通道的电压、功率、通道 */
        // TODO: 缺少路径定义
        write_v_p_to_csv("0:test_current_result.csv", 
                          currentVPCh.currentV, 
                          currentVPCh.currentP, 
                          currentVPCh.channel);

        /* 通过以太网上传当前通道的电压、功率、通道 */

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


