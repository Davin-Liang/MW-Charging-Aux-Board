#include "data_sum_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "file_common.h"
#include "lwip_recv_task.h"
#include "command_struct.h"
#include "stdio.h"
#include <string.h> 
#include "send_command.h"
#include "command_task.h"
#include "main.h"

static void data_sum_Task(void * param);
static int datetime_to_filename(const DateTime_t *dt, const char *filename, 
                        char *output, size_t output_size);
static uint8_t validate_date_time(const DateTime_t * dt);
static struct MotorData_t currentMotorData;
static struct Optimal_v_p_t currentOptimalVP;
static struct CurrentV_P_Ch_t currentVPCh;
static DateTime_t g_currentDateTime;
static char g_nameOutput[40];
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
    insert_task_handle(g_dataSumTaskHandle, "data_sum");
		g_motorDataQueue = xQueueCreate(MOTOR_DATA_QUEUE_LEN, sizeof(struct MotorData_t));
    g_optimalVPDataQueue = xQueueCreate(OPTIMAL_V_P_DATA_QUEUE_LEN, sizeof(struct Optimal_v_p_t));
    g_currentVPChQueue = xQueueCreate(CURRENT_V_P_CH_QUEUE_LEN, sizeof(struct CurrentV_P_Ch_t));
		vTaskSuspend(NULL);
  
    while (1)
    {
      if (pdPASS == xQueueReceive(g_motorDataQueue, &currentMotorData, 10))
      {
        /* 打印电机数据 */
        mutual_printf("Position:(%.2f,%.2f)\r\n", currentMotorData.x, currentMotorData.y);
        /* 通过以太网上传电机数据 */
        send_motor_data_command(1, currentMotorData.x, currentMotorData.y, 0); // TODO:
      }

      if (pdPASS == xQueueReceive(g_timeDataQueue, &g_currentDateTime, 10))
      {
        /* 打印日期时间数据 */
        printf("获取时间: %04u-%02u-%02u %02u:%02u\n",
              g_currentDateTime.year, g_currentDateTime.month, g_currentDateTime.day,
              g_currentDateTime.hour, g_currentDateTime.minute);
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
        #if SD_NOTE
        datetime_to_filename(&g_currentDateTime, "optimal_result.csv", g_nameOutput, sizeof(g_nameOutput));
        write_x_y_v_p_to_csv(g_nameOutput, // g_nameOutput
                              currentMotorData.x,
                              currentMotorData.y,
                              currentOptimalVP.optimalVs,
                              currentOptimalVP.optimalP);
        #endif

        /* 通过以太网上传最优结果 */
        send_opt_res_data_command(1, currentMotorData.x, 
                                      currentMotorData.y, 
                                      currentOptimalVP.optimalP, 
                                      currentOptimalVP.optimalVs); // TODO:
      }

      if (pdPASS == xQueueReceive(g_currentVPChQueue, &currentVPCh, 10))
      {
        /* 打印当前通道的电压、功率 */
        mutual_printf("channel: %d current V: %.2f current P: %.3f\r\n",
        currentVPCh.channel,
        currentVPCh.currentV,
        currentVPCh.currentP);

        /* 往 SD 卡中写入当前通道的电压、功率、通道 */
        #if SD_NOTE
        datetime_to_filename(&g_currentDateTime, "current_result.csv", g_nameOutput, sizeof(g_nameOutput));
        write_v_p_to_csv(g_nameOutput, // g_nameOutput
                          currentVPCh.currentV, 
                          currentVPCh.currentP, 
                          currentVPCh.channel);
        #endif

        /* 通过以太网上传当前通道的电压、功率、通道 */
        send_current_vpch_command(1, currentVPCh.channel, currentVPCh.currentV, currentVPCh.currentP);
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

/**
  * @brief  日期时间验证
  * @param  dt 时间结构体
  * @return 0 全部有效 | 1 年份无效 | 2 月份无效 | 3 日期无效 | 4 时间无效 | 5 星期无效
  **/
uint8_t validate_date_time(const DateTime_t * dt) 
{
    // 验证年份
    if (dt->year < 2023 || dt->year > 2100) {
        return 1; // 年份无效
    }
    
    // 验证月份
    if (dt->month < 1 || dt->month > 12) {
        return 2; // 月份无效
    }
    
    // 验证日期
    uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // 闰年判断
    if (dt->month == 2) {
        uint8_t is_leap_year = ((dt->year % 4 == 0) && (dt->year % 100 != 0)) || (dt->year % 400 == 0);
        if (is_leap_year) days_in_month[1] = 29;
    }
    
    if (dt->day < 1 || dt->day > days_in_month[dt->month - 1]) {
        return 3; // 日期无效
    }
    
    // 验证时间
    if (dt->hour > 23 || dt->minute > 59) {
        return 4; // 时间无效
    }
    
    // 验证星期
    if (dt->week_day > 6) {
        return 5; // 星期无效
    }
    
    return 0; // 有效
}

/**
  * @brief  通过日期生成文件名
  * @param  dt 时间结构体
  * @param filename 基础文件名
  * @param output 输出文件名
  * @param output_size 用于存储输出文件名的缓冲区大小
  * @return 0 成功 | -1 参数错误 | -3 缓冲区不足或格式化错误
  **/
static int datetime_to_filename(const DateTime_t *dt, const char *filename, 
                        char *output, size_t output_size) 
{
    if (dt == NULL || filename == NULL || output == NULL)
        return -1; // 参数错误
    
    /* 验证日期时间有效性 */
    return validate_date_time(dt);
    
    /* 查找文件名中的扩展名 */
    const char *dot = strrchr(filename, '.');
    const char *name_part = filename;
    const char *ext_part = "";
    
    if (dot != NULL) 
    {
        /* 分离文件名和扩展名 */
        size_t name_len = dot - filename;
        ext_part = dot; // 包含点号的扩展名
    }
    
    /* 构建新文件名: 文件名_YYYYMMDD_HHMMSS.扩展名 */
    int result = snprintf(output, output_size, "%.*s_%04u%02u%02u_%02u%02u%s",
                         (int)(dot ? (dot - filename) : strlen(filename)),
                         name_part,
                         dt->year, dt->month, dt->day,
                         dt->hour, dt->minute,
                         ext_part);
    
    if (result < 0 || (size_t)result >= output_size)
        return -3; // 缓冲区不足或格式化错误
    
    return 0; // 成功
}

void data_sum_test1(void)
{
  struct MotorData_t currentMotorData;
  struct Optimal_v_p_t currentOptimalVP;
  struct CurrentV_P_Ch_t currentVPCh;

  currentMotorData.x = 10;
  currentMotorData.y = 100;
  send_motor_data_command(g_sock, currentMotorData.x, currentMotorData.y, 0);
  vTaskDelay(2000);

  currentOptimalVP.optimalP = 10.11;
  for (int i = 0; i < 4; i ++)
    currentOptimalVP.optimalVs[i] = 13.5;
  send_opt_res_data_command(g_sock, currentMotorData.x, 
                                      currentMotorData.y, 
                                      currentOptimalVP.optimalP, 
                                      currentOptimalVP.optimalVs);
  vTaskDelay(2000);
  
  currentVPCh.channel = 2;
  currentVPCh.currentP = 12.22;
  currentVPCh.currentV = 5.55;
  send_current_vpch_command(g_sock, currentVPCh.channel, currentVPCh.currentV, currentVPCh.currentP); 
  vTaskDelay(2000);                                   
}

