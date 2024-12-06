#ifndef MAIN_H
#define MAIN_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <system.h>
#include <max7219.h>

/**
 * @brief 
 * 
 */
typedef struct
{
    QueueHandle_t       xQueue;
    sys_disp_params_c   *displayParams;
} disp_task_params_c;

/**
 * @brief 
 * 
 */
typedef struct
{
    QueueHandle_t xDataQueue;
    QueueHandle_t xCommandQueue;
} queue_struct_c;

/**
 * @brief 
 * 
 */
typedef struct
{
    queue_struct_c  *queues;
    gpio_config_c   *pins;
    
} command_handler_params_c;


#endif