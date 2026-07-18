/**
 * @file bsp_cfg.cpp
 * @brief BSP 层总配置 — 全局实例 + 初始化 + printf 重定向
 */

#include "bsp_cfg.hpp"
#include "ti_msp_dl_config.h"

/* ==================== 全局实例 ==================== */
BspUart<64> bsp_uart0;
BspUart<64> bsp_uart1;

/* ==================== BSP 初始化 ==================== */
void bsp_init()
{
  bsp_uart0.init({UART0,
                  UART0_INT_IRQn,
                  DMA_CH0_CHAN_ID,
                  UART0_INST_DMA_TRIGGER});
  bsp_uart1.init({UART1,
                  UART1_INT_IRQn,
                  DMA_CH1_CHAN_ID,
                  UART1_INST_DMA_TRIGGER});
}
