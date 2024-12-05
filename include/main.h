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
    QueueHandle_t xQueue;
    display_parameters_c *displayParams;
} task_parameters_c;

#endif