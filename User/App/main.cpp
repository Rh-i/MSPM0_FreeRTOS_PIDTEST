/**
 * @file main.cpp
 */

#include "main.hpp"
#include <math.h>
#include <stdio.h>

/* ========================================================================== */

/** @brief 巡线数据全局变量（track_task 写入，oled_task 读取） */
static volatile uint16_t g_track_data = 0;

/** @brief 测试波形类型 */
enum WaveType
{
  WAVE_SINE     = 0, ///< 正弦波
  WAVE_SQUARE   = 1, ///< 方波（阶跃响应测试）
  WAVE_TRIANGLE = 2, ///< 三角波（斜坡跟踪测试）
  WAVE_SAWTOOTH = 3, ///< 锯齿波（单向扫描测试）
};

/** @brief 当前测试波形（改这里切换） */
static const WaveType kWave = WAVE_SQUARE;

/** 波形公共参数 */
static const float kWaveAmplitude = 50.0f; /* 幅值 cm/s */
static const float kWavePeriod_s  = 5.0f;  /* 周期 s */
static const float kDt_s          = 0.01f; /* 控制周期 s (10ms) */

/**
 * @brief 电机速度环控制任务 — 多种波形测试
 */
extern "C" void motor_task(void *arg)
{
  uint32_t tick = 0;

  while (1)
  {
    /* ---- 根据所选波形计算目标速度 ---- */
    float t     = (float)tick * kDt_s; /* 当前时间 s */
    float phase = t / kWavePeriod_s;   /* 归一化相位 [0,1) */
    float target;

    switch (kWave)
    {
      case WAVE_SQUARE:
        /* 方波: 前半周期 +A, 后半周期 -A */
        target = (phase - floorf(phase) < 0.5f)
                   ? kWaveAmplitude
                   : -kWaveAmplitude;
        break;

      case WAVE_TRIANGLE:
        /* 三角波: 线性上升→下降 */
        {
          float p = phase - floorf(phase); /* [0, 1) */
          target  = (p < 0.5f)
                      ? kWaveAmplitude * (4.0f * p - 1.0f)  /* 上升段: -A→+A */
                      : kWaveAmplitude * (3.0f - 4.0f * p); /* 下降段: +A→-A */
        }
        break;

      case WAVE_SAWTOOTH:
        /* 锯齿波: 从 -A 线性上升到 +A，然后跳回 */
        {
          float p = phase - floorf(phase); /* [0, 1) */
          target  = kWaveAmplitude * (2.0f * p - 1.0f);
        }
        break;

      case WAVE_SINE:
      default:
        /* 正弦波 */
        target = kWaveAmplitude * sinf(2.0f * (float)M_PI * phase);
        break;
    }

    motor.set_target_speed(target);

    /* ---- 控制更新 ---- */
    motor.update();

    /* ---- CSV 输出: target, current, pwm ---- */
    const Motor::State &s = motor.get_state();
    bsp_uart0.uart_printf(
      "%f,%f,%u\r\n",
      s.target_speed_cm_s,
      s.current_speed_cm_s,
      s.current_pwm);

    tick++;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

extern "C" void gyro_task(void *arg)
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

extern "C" void qei_task(void *arg)
{
  bsp_qei.set_count(32767);

  while (1)
  {
    uint16_t               count = bsp_qei.get_count();
    DL_TIMER_QEI_DIRECTION dir   = bsp_qei.get_direction();

    bsp_uart0.uart_printf("QEI count=%u dir=%s\r\n",
                          (unsigned)count,
                          (dir == DL_TIMER_QEI_DIR_UP) ? "UP" : "DOWN");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

extern "C" void pwm_task(void *arg)
{
  /* ---- 应用层初始化：设置初始占空比 ---- */
  bsp_pwm.set_duty(0, 1000); /* CH0: 0% */
  bsp_pwm.set_duty(1, 5000); /* CH1: 50% */

  while (1)
  {
    uint32_t pwm_freq = bsp_pwm.get_freq();
    uint32_t period   = bsp_pwm.get_period();
    uint32_t duty_ch0 = bsp_pwm.get_duty(0);
    uint32_t duty_ch1 = bsp_pwm.get_duty(1);

    bsp_uart0.uart_printf(
      "PWM: %uHz period=%u CH0=%u CH1=%u\r\n",
      (unsigned)pwm_freq,
      (unsigned)period,
      (unsigned)duty_ch0,
      (unsigned)duty_ch1);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

extern "C" void blink_task(void *arg)
{
  while (1)
  {
    DL_GPIO_togglePins(GPIOB, DL_GPIO_PIN_22);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

extern "C" void track_task(void *arg)
{
  while (1)
  {
    g_track_data = iic_track.read();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

extern "C" void oled_task(void *arg)
{
  char buf[17];

  iic_oled.clear();

  while (1)
  {
    uint16_t data = g_track_data;

    /* 第 0 行: 原始值 (HEX) */
    snprintf(buf, sizeof(buf), "T:%04X", data);
    iic_oled.showString(0, 0, buf);

    /* 第 2 行: 低 8 位 bit 图 */
    for (uint8_t i = 0; i < 12; i++)
    {
      buf[i] = (data & (1 << i)) ? '1' : '0';
    }
    buf[12] = '\0';
    iic_oled.showString(0, 2, buf);

    iic_oled.refresh();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

int main()
{
  SYSCFG_DL_init();
  bsp_init();
  device_init();

  xTaskCreate(blink_task, "blink", 0x80,  NULL, configMAX_PRIORITIES - 3, NULL);
  xTaskCreate(track_task, "track", 0x100, NULL, configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(oled_task,  "oled",  0x200, NULL, configMAX_PRIORITIES - 2, NULL);
  // xTaskCreate(pwm_task,   "pwm",   0x200, NULL, configMAX_PRIORITIES - 2, NULL);
  // xTaskCreate(qei_task,   "qei",   0x200, NULL, configMAX_PRIORITIES - 2, NULL);
  // xTaskCreate(motor_task, "motor",  0x200, NULL, configMAX_PRIORITIES - 1, NULL);
  // xTaskCreate(gyro_task,  "gyro",  0x200, NULL, configMAX_PRIORITIES - 1, NULL);

  vTaskStartScheduler();
}
