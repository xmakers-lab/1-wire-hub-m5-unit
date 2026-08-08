#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "OneWireMaster.h"

// DS2482-800（1-Wire×8chマルチプレクサ）とのI2C通信を行うクラス。
// チップ制御（Device Reset／Status読出し／Channel Select）に加えて、
// 1-Wireバスのビット操作（Reset／Write Byte／Read Byte）をOneWireMasterとして提供する。
//
// 注意: DS2482自体のリセット（deviceReset、DRSTコマンド）と、
// 1-Wireバスのリセット（OneWireMaster::reset、1WRSコマンド）は別物。
// selectChannel()で選択したチャンネルに対してreset/writeByte/readByteが作用する。
class DS2482 : public OneWireMaster {
public:
  static const uint8_t NUM_CHANNELS = 8;

  explicit DS2482(uint8_t address = 0x18, TwoWire &wire = Wire);

  // DS2482チップ自体のリセット（DRST）。I2C通信が成立するかの確認も兼ねる。
  bool deviceReset();
  uint8_t readStatus();
  bool selectChannel(uint8_t channel);

  // OneWireMaster
  bool reset() override;
  void writeByte(uint8_t data) override;
  uint8_t readByte() override;

private:
  bool writeCommand(uint8_t cmd);
  bool writeCommand(uint8_t cmd, uint8_t data);
  uint8_t readByteRaw();
  uint8_t readRegister(uint8_t pointerCode);
  uint8_t waitBusy();

  uint8_t _address;
  TwoWire &_wire;
};
