#pragma once

#include <Arduino.h>

// 1-Wireバスマスタの抽象インターフェース。
// DS18B20等のデバイスドライバはこのインターフェースのみに依存することで、
// DS2482（I2Cブリッジ）経由か、ビットバンギング実装かを意識せず動作できる。
class OneWireMaster {
public:
  virtual ~OneWireMaster() = default;

  // 1-Wire Resetパルスを送出する。プレゼンスパルスを検出できればtrueを返す。
  virtual bool reset() = 0;

  virtual void writeByte(uint8_t data) = 0;
  virtual uint8_t readByte() = 0;
};
