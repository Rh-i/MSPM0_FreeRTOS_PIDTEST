/**
 * @file iic_oled.cpp
 * @brief I2C OLED 设备驱动 — 实现
 *
 * @note 所有逻辑保持与原 OLED.c 一致，仅重构为 C++ 类形式。
 */

#include "iic_oled.hpp"
#include "fort.h"

/* ==================== 构造 ==================== */
IicOled::IicOled() {}

/* ==================== 初始化 ==================== */
bool IicOled::init(BspI2c *i2c, uint8_t dev_addr)
{
  if (!i2c)
    return false;

  _i2c      = i2c;
  _dev_addr = dev_addr;

  /* SSD1306 初始化序列（与原 OLED_Init 一致） */
  writeCmd(0xAE);   // 关闭显示
  writeCmd(0x00);   // 列低地址
  writeCmd(0x10);   // 列高地址
  writeCmd(0x40);   // 起始行
  writeCmd(0x81);   // 对比度设置
  writeCmd(0xCF);   // 对比度值
  writeCmd(0xA1);   // 段重映射（左右翻转）
  writeCmd(0xC8);   // COM 扫描方向（上下翻转）
  writeCmd(0xA6);   // 正常显示（非反色）
  writeCmd(0xA8);   // 多路复用比
  writeCmd(0x3F);   // 1/64 占空比
  writeCmd(0xD3);   // 显示偏移
  writeCmd(0x00);   // 偏移 0
  writeCmd(0xD5);   // 显示时钟分频/振荡频率
  writeCmd(0x80);   // 默认值
  writeCmd(0xD9);   // 预充电周期
  writeCmd(0xF1);   // 值
  writeCmd(0xDA);   // COM 引脚配置
  writeCmd(0x12);   // 交替模式
  writeCmd(0xDB);   // VCOMH 电平
  writeCmd(0x30);   // 值
  writeCmd(0x20);   // 寻址模式
  writeCmd(0x02);   // 页寻址模式
  writeCmd(0x8D);   // 电荷泵
  writeCmd(0x14);   // 使能
  clear();
  writeCmd(0xAF);   // 开启显示

  return true;
}

/* ==================== 命令发送 ==================== */
void IicOled::writeCmd(uint8_t cmd)
{
  uint8_t data[2] = {0x00, cmd};
  _i2c->write(_dev_addr, data, 2);
}

/* ==================== 显示控制 ==================== */

void IicOled::displayOn()
{
  writeCmd(0x8D);   // 电荷泵使能
  writeCmd(0x14);   // 开启电荷泵
  writeCmd(0xAF);   // 点亮屏幕
}

void IicOled::displayOff()
{
  writeCmd(0x8D);   // 电荷泵使能
  writeCmd(0x10);   // 关闭电荷泵
  writeCmd(0xAE);   // 关闭屏幕
}

void IicOled::colorTurn(bool enable)
{
  if (enable)
    writeCmd(0xA7);   // 反色显示
  else
    writeCmd(0xA6);   // 正常显示
}

void IicOled::displayTurn(bool enable)
{
  if (enable)
  {
    writeCmd(0xC0);   // COM 反转
    writeCmd(0xA0);   // 段反转
  }
  else
  {
    writeCmd(0xC8);   // COM 正常
    writeCmd(0xA1);   // 段正常
  }
}

/* ==================== 显存操作 ==================== */

void IicOled::clear()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t n = 0; n < 128; n++)
    {
      _gram[n][i] = 0x00;
    }
  }
  refresh();
}

void IicOled::refresh()
{
  uint8_t page, col;

  for (page = 0; page < 8; page++)
  {
    /* 设置页地址和列起始地址 */
    writeCmd(0xB0 + page);
    writeCmd(0x00);
    writeCmd(0x10);

    /* 启动批量传输: 1 控制字节 + 128 像素数据 */
    _i2c->startTx(_dev_addr, 129);

    _i2c->txByte(0x40);   // 控制字节: 数据模式

    /* 连续发送 128 个像素数据 */
    for (col = 0; col < 128; col++)
    {
      _i2c->txByte(_gram[col][page]);
    }

    /* 等待该页传输完成 */
    _i2c->waitTxDone();
  }
}

void IicOled::showChar(uint8_t x, uint8_t y, uint8_t chr)
{
  for (uint8_t i = 0; i <= 8; i++)
  {
    _gram[x * 8 + i][y * 2]     = F8X16[chr - ' '][i];
    _gram[x * 8 + i][y * 2 + 1] = F8X16[chr - ' '][i + 8];
  }
}

void IicOled::showString(uint8_t x, uint8_t y, const char *str)
{
  uint8_t j = 0;
  while (str[j] != '\0')
  {
    showChar(x, y, str[j]);
    x++;
    if (x > 15)
      return;
    j++;
  }
}
