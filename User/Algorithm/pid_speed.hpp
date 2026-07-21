/**
 * @file pid_speed.hpp
 * @brief 速度环PID — 内嵌 Pid 实例，自包含
 */

#ifndef __PID_SPEED_HPP__
#define __PID_SPEED_HPP__

#include "pid.hpp"
#include <stdint.h>

class PidSpeed
{
public:
  PidSpeed();

  void  init(float kp, float ki, float kd, float out_max);
  float calculate_speed(float target_speed, float current_speed_cm_s);
  void  reset();

  float get_output()    const { return _pid.get_output(); }
  float get_speed_cm_s() const { return _speed_cm_s; }

private:
  Pid     _pid;           /* 内嵌的位置式PID */
  float   _speed_cm_s;    /* 当前速度 cm/s */
  float   _target_speed;  /* 目标速度 cm/s */
};

#endif // __PID_SPEED_HPP__
