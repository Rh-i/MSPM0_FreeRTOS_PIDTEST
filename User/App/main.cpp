/**
 * @file main.cpp
 * @author Rh
 * @brief main文件
 * @version 0.1
 * @date 2026-02-04
 * @note 在C++中使用FreeRTOS的Task函数时
 *       需要将任务函数声明为extern "C"格式
 *       同时函数参数必须是void *arg
 *       所以，我直接把任务放到一个转接文件，然后extern "C" 再在主函数调用就没问题了
 * 
 * @copyright Copyright (c) 2026
 * 
 */


#include "main.hpp"

extern "C" void printLogTask(void *arg)
{
  int count = 0;
  while (true)
  {
    count += 2;
    uart_printf("=== printLogTask thread %d ===\r\n", count);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

extern "C" void blinkTask(void *arg)
{
  while (true)
  {
    // LED 5Hz频率闪烁
    // DL_GPIO_togglePins(GPIOB, DL_GPIO_PIN_22);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

int main()
{
  SYSCFG_DL_init();

  TaskHandle_t printLogTask_handle;
  TaskHandle_t blinkTask_handle;

  xTaskCreate(printLogTask, "printLogTask", 0x80, NULL, configMAX_PRIORITIES - 1, &printLogTask_handle);
  xTaskCreate(blinkTask, "blinkTask", 0x80, NULL, configMAX_PRIORITIES - 1, &blinkTask_handle);

  vTaskStartScheduler();
}
