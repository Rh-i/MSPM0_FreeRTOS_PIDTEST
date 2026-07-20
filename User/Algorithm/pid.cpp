/**
 * @file pid.cpp
 * @brief 位置式PID控制器 — 实现
 *
 * @note 位置式 PID 公式:
 *   e(k)     = target - feedback
 *   p_out    = Kp × e(k)
 *   integral = Σ Ki × e(k)  （带限幅）
 *   d_out    = Kd × (e(k) - e(k-1))
 *   output   = p_out + integral + d_out  （带限幅）
 */

#include "pid.hpp"

/* ==================== 构造 / 析构 ==================== */

/**
 * @brief 默认构造函数 — 初始化为零参数
 */
Pid::Pid()
{
  init(0, 0, 0, 0, 0);
}

/* ==================== 初始化 ==================== */

/**
 * @brief PID 初始化
 *
 * @param kp       比例系数
 * @param ki       积分系数
 * @param kd       微分系数
 * @param out_max  输出限幅（绝对值）
 * @param i_max    积分限幅（绝对值）
 *
 * @note 调用后所有运行状态归零，保留系数和限幅
 */
void Pid::init(float kp, float ki, float kd, float out_max, float i_max)
{
  /* 系数 */
  _kp = kp;
  _ki = ki;
  _kd = kd;

  /* 限幅 */
  _output_max   = out_max;
  _integral_max = i_max;

  /* 状态清零 */
  _target     = 0;
  _feedback   = 0;
  _error      = 0;
  _integral   = 0;
  _output     = 0;
  _last_error = 0;
}

/* ==================== 计算 ==================== */

/**
 * @brief PID 计算（位置式）
 *
 * @param target   目标值
 * @param feedback 反馈值
 * @return PID 输出值（已限幅到 [-output_max, +output_max]）
 *
 * @note 每次调用会更新内部积分和微分历史
 */
float Pid::calculate(float target, float feedback)
{
  _target   = target;
  _feedback = feedback;

  /* 误差 */
  _error = target - feedback;

  /* 比例项 */
  float p_out = _kp * _error;

  /* 积分项（带限幅） */
  _integral += _ki * _error;
  if (_integral > _integral_max)
  {
    _integral = _integral_max;
  }
  if (_integral < -_integral_max)
  {
    _integral = -_integral_max;
  }

  /* 微分项 */
  float d_out = _kd * (_error - _last_error);
  _last_error = _error;

  /* 输出合成 */
  _output = p_out + _integral + d_out;

  /* 输出限幅 */
  if (_output > _output_max)
  {
    _output = _output_max;
  }
  if (_output < -_output_max)
  {
    _output = -_output_max;
  }

  return _output;
}

/* ==================== 重置 ==================== */

/**
 * @brief PID 重置
 *
 * @note 清零积分累计和微分历史，保留系数和限幅不变
 */
void Pid::reset()
{
  _error      = 0;
  _integral   = 0;
  _output     = 0;
  _last_error = 0;
}
