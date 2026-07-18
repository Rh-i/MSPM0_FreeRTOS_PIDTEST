/**
 * @file main.cpp
 * @brief DMA 串口回环 — 使用 BspUart 类
 */

#include "main.hpp"

/* ========================================================================== */
extern "C" void testTask(void *arg)
{
  ti_gyro.set_mode(TiGyro::REPORT_MODE_ATTITUDE);
  ti_gyro.start_report(true);

  while (1)
  {
    ti_gyro.update(portMAX_DELAY);

    const TiGyro::ReportData &report = ti_gyro.get_last_report();
    if (report.valid)
    {
      bsp_uart0.uart_printf("yaw=%.6f pitch=%.6f roll=%.6f\r\n",
                            report.yaw_deg,
                            report.pitch_deg,
                            report.roll_deg);
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

extern "C" void blinkTask(void *arg)
{
  while (1)
  {
    DL_GPIO_togglePins(GPIOB, DL_GPIO_PIN_22);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

int main()
{
  SYSCFG_DL_init();
  bsp_init();
  device_init();

  TaskHandle_t h1, h2;
  xTaskCreate(blinkTask, "blink", 0x80, NULL, configMAX_PRIORITIES - 1, &h1);
  xTaskCreate(testTask, "test", 0x200, NULL, configMAX_PRIORITIES - 1, &h2);
  vTaskStartScheduler();
}
