#ifndef TASKS_H
#define TASKS_H

/**
 * @brief
 *
 * @param pvParameters
 */
void PowerController(void *pvParameters);

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void CommandHandler(void *pvParameters);

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayMessages(void *pvParameters);

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayNews(void *pvParameters);

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayTime(void *pvParameters);

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void TelegramPollUpdates(void *pvParameters);

#endif // TASKS_H