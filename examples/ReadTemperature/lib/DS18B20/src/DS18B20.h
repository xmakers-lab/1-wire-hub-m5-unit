#pragma once

#include <Arduino.h>

#include "OneWireMaster.h"

// DS18B20（1-Wire温度センサー）のドライバ。
// OneWireMasterインターフェースのみに依存するため、DS2482経由に限らず、
// 任意の1-Wireマスタ実装（ビットバンギング等）と組み合わせて使用できる。
//
// Skip ROMコマンドを使用するため、1本の1-WireバスにDS18B20が
// 単独接続されている前提（複数デバイス共存には非対応）。
class DS18B20 {
public:
  explicit DS18B20(OneWireMaster &master);

  // 温度変換を開始する。センサ未接続（プレゼンスパルスなし）の場合はfalseを返す。
  // 変換完了まで約750ms（12bit分解能）かかるため、呼出し側で待機すること。
  bool requestConversion();

  // 変換済みの温度をScratchpadから読み出す。
  // センサ未接続、またはCRC不一致の場合はfalseを返す。
  bool readTemperature(float &tempC);

private:
  OneWireMaster &_master;
};
