#ifndef MAIN_H
#define MAIN_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <system.h>

typedef struct
{
    QueueHandle_t xQueue;
    DisplayParams *displayParams;
} TaskParameters_c;

#endif