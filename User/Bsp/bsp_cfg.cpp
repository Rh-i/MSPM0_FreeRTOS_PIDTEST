/**
 * @file bsp_cfg.cpp
 * @brief BSP 层总配置 — 全局实例 + 初始化
 */

#include "bsp_cfg.hpp"
#include "ti_msp_dl_config.h"

/* ==================== 全局实例 ==================== */
BspUart<64> bsp_uart0;
BspUart<64> bsp_uart1;
BspPwm       bsp_pwm;
BspQei       bsp_qei;

/* ==================== BSP 初始化 ==================== */
void bsp_init()
{
  /* UART */
  bsp_uart0.init({UART0,
                  UART0_INT_IRQn,
                  DMA_CH0_CHAN_ID,
                  UART0_INST_DMA_TRIGGER});
  bsp_uart1.init({UART1,
                  UART1_INT_IRQn,
                  DMA_CH1_CHAN_ID,
                  UART1_INST_DMA_TRIGGER});

  /* PWM — TIMG7, 80MHz, period=10000, CH0 + CH1 */
  bsp_pwm.init({PWM_MOTOR_INST,
                PWM_MOTOR_INST_CLK_FREQ,
                10000,
                2,
                {DL_TIMER_CC_0_INDEX, DL_TIMER_CC_1_INDEX}});

  /* QEI — TIMG8 */
  bsp_qei.init({QEI_0_INST});
}
