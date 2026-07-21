/**
 * @file bsp_i2c.hpp
 * @brief MSPM0 BspI2c — I2C 控制器模式封装（阻塞式）
 *
 * @note 硬件初始化由 SysConfig 生成的 SYSCFG_DL_I2Cx_init() 完成，
 *       BspI2c 仅封装运行时 I2C 读写操作。
 *
 * @note 本封装为阻塞式，适用于简单外设（OLED、PCA9555 等），
 *       不依赖中断或 DMA。
 *
 * 使用示例:
 * @code
 *   BspI2c i2c0;
 *   i2c0.init({I2C0_INST});
 *   uint8_t cmd[] = {0x00, 0xAF};
 *   i2c0.write(0x3C, cmd, 2);
 * @endcode
 */

#ifndef __BSP_I2C_MSPM0_HPP__
#define __BSP_I2C_MSPM0_HPP__

#include "ti_msp_dl_config.h"

class BspI2c
{
public:
  /**
   * @brief I2C 初始化配置结构体
   */
  struct Config
  {
    I2C_Regs *i2c; ///< I2C 外设实例指针（如 I2C0_INST、I2C1_INST）
  };

  BspI2c();

  /**
   * @brief 绑定 I2C 硬件实例
   * @param cfg I2C 外设配置
   * @return true  绑定成功
   * @return false i2c 为空指针
   */
  bool init(const Config &cfg);

  /** @brief 获取绑定的 I2C 外设指针 */
  I2C_Regs *get_i2c() const { return _cfg.i2c; }

  /* ==================== 状态查询 ==================== */

  /** @brief 等待 I2C 控制器空闲（IDLE 标志置位） */
  void waitIdle();

  /** @brief 等待 I2C 控制器不忙（BUSY 标志清零） */
  void waitNotBusy();

  /* ==================== 阻塞式基础操作 ==================== */

  /**
   * @brief 向设备写入数据（阻塞至传输完成）
   * @param dev_addr 7 位设备地址
   * @param data     数据缓冲区
   * @param len      数据长度
   * @note 适用于 OLED 命令写入等场景，data[0] 通常为控制字节
   */
  void write(uint8_t dev_addr, const uint8_t *data, size_t len);

  /**
   * @brief 读取设备寄存器（写寄存器地址 → 重新起始 → 读 1 字节）
   * @param dev_addr 7 位设备地址
   * @param reg      寄存器地址
   * @return 寄存器值
   */
  uint8_t readReg8(uint8_t dev_addr, uint8_t reg);

  /**
   * @brief 读取设备连续两个寄存器（写寄存器地址 → 重新起始 → 读 2 字节）
   * @param dev_addr 7 位设备地址
   * @param reg_low  低地址寄存器（先读）
   * @return 16 位值，reg_low 在低字节，reg_low+1 在高字节
   */
  uint16_t readReg16(uint8_t dev_addr, uint8_t reg_low);

  /* ==================== 流水线式传输（用于 OLED 批量刷新） ==================== */

  /**
   * @brief 启动 TX 传输（不等待完成，立即返回）
   * @param dev_addr 7 位设备地址
   * @param count    本次传输总字节数
   * @note 启动后调用 txByte() 逐字节填入 FIFO，最后调用 waitTxDone() 等待完成
   */
  void startTx(uint8_t dev_addr, size_t count);

  /** @brief 检查 TX FIFO 是否已满 */
  bool isTxFifoFull();

  /**
   * @brief 发送 1 字节（自动等待 FIFO 有空位）
   * @param data 要发送的字节
   * @note 必须在 startTx() 之后、waitTxDone() 之前调用
   */
  void txByte(uint8_t data);

  /** @brief 等待 TX 传输完成并恢复空闲 */
  void waitTxDone();

private:
  Config _cfg {};
};

#endif // __BSP_I2C_MSPM0_HPP__
