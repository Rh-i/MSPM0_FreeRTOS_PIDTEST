/**
 * @file bsp_cfg.hpp
 * @brief BSP 层总配置 — 全局声明
 */

#ifndef __BSP_CFG_HPP__
#define __BSP_CFG_HPP__

#include "bsp_uart.hpp"
#include "bsp_pwm.hpp"
#include "bsp_qei.hpp"
#include "bsp_i2c.hpp"

/* ==================== 函数声明 ==================== */
void bsp_init();

/* ==================== 全局声明 ==================== */
extern BspUart<64> bsp_uart0;
extern BspUart<64> bsp_uart1;
extern BspPwm       bsp_pwm;
extern BspQei       bsp_qei;
extern BspI2c       bsp_i2c0;
extern BspI2c       bsp_i2c1;

#endif // __BSP_CFG_HPP__
