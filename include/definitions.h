#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define HOST SPI2_HOST

#define CASCADE_SIZE                4
#define MOSI_PIN                    7
#define CS_PIN                      10
#define CLK_PIN                     6

#define MESSAGE_QUEUE_ITEM_SIZE     128
#define MESSAGE_QUEUE_LENGTH        5

#define COMMAND_QUEUE_ITEM_SIZE     1
#define COMMAND_QUEUE_LENGTH        10

#define POWER_ON_COMMAND            1
#define POWER_OFF_COMMAND           2
#define RESET_COMMAND               3
#define FORCE_POWER_OFF_COMMAND     4


#endif