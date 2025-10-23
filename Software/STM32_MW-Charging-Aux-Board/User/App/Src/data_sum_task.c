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
  * @brief  data_sum_Task ��������
  * @param  param �������  
  * @return void
  **/
static void data_sum_Task(void * param)
{	
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
        send_motor_data_command(1, currentMotorData.x, currentMotorData.y, 0); // TODO:
      }

      if (pdPASS == xQueueReceive(g_timeDataQueue, &g_currentDateTime, 10))
      {
        /* ��ӡ����ʱ������ */
        printf("��ȡʱ��: %04u-%02u-%02u %02u:%02u\n",
              g_currentDateTime.year, g_currentDateTime.month, g_currentDateTime.day,
              g_currentDateTime.hour, g_currentDateTime.minute);
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
        datetime_to_filename(&g_currentDateTime, "optimal_result.csv", g_nameOutput, sizeof(g_nameOutput));
        write_x_y_v_p_to_csv(g_nameOutput,
                              currentMotorData.x,
                              currentMotorData.y,
                              currentOptimalVP.optimalVs,
                              currentOptimalVP.optimalP);

        /* ͨ����̫���ϴ����Ž�� */
        send_opt_res_data_command(1, currentMotorData.x, 
                                      currentMotorData.y, 
                                      currentOptimalVP.optimalP, 
                                      currentOptimalVP.optimalVs); // TODO:
      }

      if (pdPASS == xQueueReceive(g_currentVPChQueue, &currentVPCh, 10))
      {
        /* ��ӡ��ǰͨ���ĵ�ѹ������ */
        mutual_printf("channel: %d current V: %.2f current P: %.3f\r\n",
        currentVPCh.channel,
        currentVPCh.currentV,
        currentVPCh.currentP);

        /* �� SD ����д�뵱ǰͨ���ĵ�ѹ�����ʡ�ͨ�� */
        datetime_to_filename(&g_currentDateTime, "current_result.csv", g_nameOutput, sizeof(g_nameOutput));
        write_v_p_to_csv(g_nameOutput, 
                          currentVPCh.currentV, 
                          currentVPCh.currentP, 
                          currentVPCh.channel);

        /* ͨ����̫���ϴ���ǰͨ���ĵ�ѹ�����ʡ�ͨ�� */
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
  * @brief  ����ʱ����֤
  * @param  dt ʱ��ṹ��
  * @return 0 ȫ����Ч | 1 �����Ч | 2 �·���Ч | 3 ������Ч | 4 ʱ����Ч | 5 ������Ч
  **/
uint8_t validate_date_time(const DateTime_t * dt) 
{
    // ��֤���
    if (dt->year < 2023 || dt->year > 2100) {
        return 1; // �����Ч
    }
    
    // ��֤�·�
    if (dt->month < 1 || dt->month > 12) {
        return 2; // �·���Ч
    }
    
    // ��֤����
    uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // �����ж�
    if (dt->month == 2) {
        uint8_t is_leap_year = ((dt->year % 4 == 0) && (dt->year % 100 != 0)) || (dt->year % 400 == 0);
        if (is_leap_year) days_in_month[1] = 29;
    }
    
    if (dt->day < 1 || dt->day > days_in_month[dt->month - 1]) {
        return 3; // ������Ч
    }
    
    // ��֤ʱ��
    if (dt->hour > 23 || dt->minute > 59) {
        return 4; // ʱ����Ч
    }
    
    // ��֤����
    if (dt->week_day > 6) {
        return 5; // ������Ч
    }
    
    return 0; // ��Ч
}

/**
  * @brief  ͨ�����������ļ���
  * @param  dt ʱ��ṹ��
  * @param filename �����ļ���
  * @param output ����ļ���
  * @param output_size ���ڴ洢����ļ����Ļ�������С
  * @return 0 �ɹ� | -1 �������� | -3 ������������ʽ������
  **/
static int datetime_to_filename(const DateTime_t *dt, const char *filename, 
                        char *output, size_t output_size) 
{
    if (dt == NULL || filename == NULL || output == NULL)
        return -1; // ��������
    
    /* ��֤����ʱ����Ч�� */
    return validate_date_time(dt);
    
    /* �����ļ����е���չ�� */
    const char *dot = strrchr(filename, '.');
    const char *name_part = filename;
    const char *ext_part = "";
    
    if (dot != NULL) 
    {
        /* �����ļ�������չ�� */
        size_t name_len = dot - filename;
        ext_part = dot; // ������ŵ���չ��
    }
    
    /* �������ļ���: �ļ���_YYYYMMDD_HHMMSS.��չ�� */
    int result = snprintf(output, output_size, "%.*s_%04u%02u%02u_%02u%02u%s",
                         (int)(dot ? (dot - filename) : strlen(filename)),
                         name_part,
                         dt->year, dt->month, dt->day,
                         dt->hour, dt->minute,
                         ext_part);
    
    if (result < 0 || (size_t)result >= output_size)
        return -3; // ������������ʽ������
    
    return 0; // �ɹ�
}

