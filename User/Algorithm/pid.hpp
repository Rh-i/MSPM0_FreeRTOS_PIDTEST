/**
 * @file pid.hpp
 * @brief 位置式PID控制器 — 类声明
 *
 * @note 位置式 PID 算法:
 *   output = Kp×e(k) + Ki×Σe(k) + Kd×(e(k)-e(k-1))
 *   积分项带限幅，输出带限幅
 */

#ifndef __PID_HPP__
#define __PID_HPP__

class Pid
{
public:
  Pid();

  /**
   * @brief PID 初始化
   * @param kp       比例系数
   * @param ki       积分系数
   * @param kd       微分系数
   * @param out_max  输出限幅（绝对值）
   * @param i_max    积分限幅（绝对值）
   */
  void  init(float kp, float ki, float kd, float out_max, float i_max);

  /**
   * @brief PID 计算（位置式）
   * @param target   目标值
   * @param feedback 反馈值
   * @return PID 输出值（已限幅）
   */
  float calculate(float target, float feedback);

  /**
   * @brief PID 重置
   * @note 清零积分累计和微分历史，保留系数和限幅不变
   */
  void  reset();

  /* ---- 访问器 ---- */
  float get_output() const { return _output; }
  float get_error()  const { return _error; }

private:
  /* ---- PID 系数 ---- */
  float _kp; /* 比例系数 */
  float _ki; /* 积分系数 */
  float _kd; /* 微分系数 */

  /* ---- 运行状态 ---- */
  float _target;    /* 目标值 */
  float _feedback;  /* 反馈值 */
  float _error;     /* 当前误差 */

  /* ---- 积分 ---- */
  float _integral;      /* 积分累计 */
  float _integral_max;  /* 积分限幅 */

  /* ---- 输出 ---- */
  float _output;      /* PID 输出 */
  float _output_max;  /* 输出限幅 */

  /* ---- 微分 ---- */
  float _last_error;  /* 上次误差 */
};

#endif // __PID_HPP__
