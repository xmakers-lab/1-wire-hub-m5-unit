#include "DS18B20.h"

// DS18B20 1-Wireコマンド
static const uint8_t CMD_SKIP_ROM     = 0xCC;
static const uint8_t CMD_CONVERT_T    = 0x44;
static const uint8_t CMD_READ_SCRATCH = 0xBE;

DS18B20::DS18B20(OneWireMaster &master) : _master(master) {}

static uint8_t oneWireCrc8(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0;
  for (uint8_t i = 0; i < len; i++) {
    uint8_t inbyte = data[i];
    for (uint8_t j = 0; j < 8; j++) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

bool DS18B20::requestConversion() {
  if (!_master.reset()) return false; // センサ未接続（プレゼンスパルスなし）
  _master.writeByte(CMD_SKIP_ROM);
  _master.writeByte(CMD_CONVERT_T);
  return true;
}

bool DS18B20::readTemperature(float &tempC) {
  if (!_master.reset()) return false;
  _master.writeByte(CMD_SKIP_ROM);
  _master.writeByte(CMD_READ_SCRATCH);

  uint8_t scratchpad[9];
  for (uint8_t i = 0; i < 9; i++) {
    scratchpad[i] = _master.readByte();
  }

  if (oneWireCrc8(scratchpad, 8) != scratchpad[8]) return false; // CRC不一致

  int16_t raw = (scratchpad[1] << 8) | scratchpad[0];
  tempC = raw / 16.0f;
  return true;
}
