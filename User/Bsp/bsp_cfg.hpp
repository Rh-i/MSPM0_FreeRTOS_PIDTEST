/**
 * @file bsp_cfg.hpp
 * @brief BSP 层总配置 — 全局声明
 */

#ifndef __BSP_CFG_HPP__
#define __BSP_CFG_HPP__

#include "bsp_uart.hpp"

/* ==================== 函数声明 ==================== */
void bsp_init();

/* ==================== 全局声明 ==================== */
extern BspUart<64> bsp_uart0;
extern BspUart<64> bsp_uart1;

#endif // __BSP_CFG_HPP__
