/**
 * @file bsp_i2c.cpp
 * @brief MSPM0 BspI2c — 实现
 */

#include "bsp_i2c.hpp"

/* ==================== 构造 ==================== */
BspI2c::BspI2c() {}

/* ==================== 初始化 ==================== */
bool BspI2c::init(const Config &cfg)
{
  if (!cfg.i2c)
    return false;
  _cfg = cfg;
  return true;
}

/* ==================== 状态查询 ==================== */

void BspI2c::waitIdle()
{
  while (!(DL_I2C_getControllerStatus(_cfg.i2c) & DL_I2C_CONTROLLER_STATUS_IDLE))
    ;
}

void BspI2c::waitNotBusy()
{
  while (DL_I2C_getControllerStatus(_cfg.i2c) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
    ;
}

/* ==================== 阻塞式基础操作 ==================== */

void BspI2c::write(uint8_t dev_addr, const uint8_t *data, size_t len)
{
  waitIdle();

  DL_I2C_fillControllerTXFIFO(_cfg.i2c, data, len);
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                  DL_I2C_CONTROLLER_DIRECTION_TX, len);

  waitNotBusy();
  waitIdle();
}

uint8_t BspI2c::readReg8(uint8_t dev_addr, uint8_t reg)
{
  uint8_t rx = 0x00;

  waitIdle();

  /* 写寄存器地址 */
  DL_I2C_fillControllerTXFIFO(_cfg.i2c, &reg, 1);
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                  DL_I2C_CONTROLLER_DIRECTION_TX, 1);

  waitNotBusy();
  waitIdle();

  /* 读数据 */
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                  DL_I2C_CONTROLLER_DIRECTION_RX, 1);

  while (DL_I2C_isControllerRXFIFOEmpty(_cfg.i2c))
    ;
  rx = DL_I2C_receiveControllerData(_cfg.i2c);

  return rx;
}

uint16_t BspI2c::readReg16(uint8_t dev_addr, uint8_t reg_low)
{
  uint8_t  low  = readReg8(dev_addr, reg_low);
  uint8_t  high = readReg8(dev_addr, (uint8_t)(reg_low + 1));
  uint16_t val  = ((uint16_t)high << 8) | (uint16_t)low;
  return val;
}

/* ==================== 流水线式传输 ==================== */

void BspI2c::startTx(uint8_t dev_addr, size_t count)
{
  waitIdle();
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                  DL_I2C_CONTROLLER_DIRECTION_TX, count);
}

bool BspI2c::isTxFifoFull()
{
  return DL_I2C_isControllerTXFIFOFull(_cfg.i2c);
}

void BspI2c::txByte(uint8_t data)
{
  while (DL_I2C_isControllerTXFIFOFull(_cfg.i2c))
    ;
  DL_I2C_transmitControllerData(_cfg.i2c, data);
}

void BspI2c::waitTxDone()
{
  waitNotBusy();
  waitIdle();
}
