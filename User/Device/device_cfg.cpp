/**
 * @file device_cfg.cpp
 * @brief 设备层总配置 — 设备实例与初始化
 */

#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

/* ==================== 全局实例 ==================== */
TiGyro ti_gyro;
Motor  motor;

/* ==================== 设备初始化 ==================== */
void device_init()
{
  ti_gyro.init(&bsp_uart1);

  /* 电机初始化
   * 编码器: 13线 x4倍频, 减速比28:1, 轮周长19.48cm
   * 控制: 20ms周期, PID=用户自行调整, PWM上限=10000
   */
  motor.init({
    &bsp_pwm,
    &bsp_qei,
    1,      /* pwm_ch_fwd: CH0 正转 */
    0,      /* pwm_ch_rev: CH1 反转 */
    13,     /* encoder_lines */
    20.41f, /* wheel_perimeter_cm */
    28,     /* gear_ratio */
    10,     /* control_period_ms */
    20.0f,   /* kp */
    7.0f,   /* ki */
    0.2f,   /* kd */
    10000   /* pwm_max (period) */
  });
}
