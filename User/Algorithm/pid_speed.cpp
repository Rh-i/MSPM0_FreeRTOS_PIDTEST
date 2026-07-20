/**
 * @file pid_speed.cpp
 * @brief 速度环PID — 实现
 *
 * @note 速度计算公式:
 *   speed_cm_s = delta_count × WHEEL_PERIMETER_CM / ENCODER_RESOLUTION / (CONTROL_PERIOD_MS/1000)
 *              = delta_count × 19.48 / 2000 / 0.02
 *              = delta_count × 0.487 cm/s
 *
 * @note 输出为 PWM 占空比 (0 ~ out_max)
 */

#include "pid_speed.hpp"

/* ==================== 构造 / 析构 ==================== */

PidSpeed::PidSpeed()
{
  init(0, 0, 0, 0);
}

/* ==================== 初始化 ==================== */

/**
 * @brief 速度环PID初始化
 *
 * @param kp       比例系数
 * @param ki       积分系数
 * @param kd       微分系数
 * @param out_max  输出限幅（PWM 占空比最大值）
 *
 * @note 积分限幅自动设为 out_max 的 80%（防止深度饱和同时保留足够调节余量）
 */
void PidSpeed::init(float kp, float ki, float kd, float out_max)
{
  _pid.init(kp, ki, kd, out_max, out_max * 0.80f);

  _last_count   = 0;
  _delta_count  = 0;
  _speed_cm_s   = 0;
  _target_speed = 0;
}

/* ==================== 计算 ==================== */

/**
 * @brief 速度环PID计算（直接传入速度值，不做编码器换算）
 *
 * @param target_speed      目标速度 cm/s
 * @param current_speed_cm_s 当前速度 cm/s（由外部 Motor 换算）
 * @return PWM 输出值
 */
float PidSpeed::calculate_speed(float target_speed, float current_speed_cm_s)
{
  _speed_cm_s   = current_speed_cm_s;
  _target_speed = target_speed;

  return _pid.calculate(target_speed, current_speed_cm_s);
}

/* ==================== 重置 ==================== */

/**
 * @brief 速度环PID重置
 *
 * @note 清零积分累计和速度状态，保留 PID 系数不变
 */
void PidSpeed::reset()
{
  _pid.reset();

  _last_count   = 0;
  _delta_count  = 0;
  _speed_cm_s   = 0;
  _target_speed = 0;
}
