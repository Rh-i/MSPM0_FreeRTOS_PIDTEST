/**
 * @file device_cfg.cpp
 * @brief 设备层总配置 — 设备实例与初始化
 */

#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

/* ==================== 全局实例 ==================== */
TiGyro ti_gyro;

/* ==================== 设备初始化 ==================== */
void device_init()
{
  ti_gyro.init(&bsp_uart1);
}
