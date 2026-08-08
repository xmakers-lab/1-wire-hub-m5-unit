#include "DS2482.h"

// DS2482-800 ファンクションコマンド
static const uint8_t CMD_DRST = 0xF0; // Device Reset
static const uint8_t CMD_CHSL = 0xC3; // Channel Select
static const uint8_t CMD_SRP  = 0xE1; // Set Read Pointer
static const uint8_t CMD_1WRS = 0xB4; // 1-Wire Reset
static const uint8_t CMD_1WWB = 0xA5; // 1-Wire Write Byte
static const uint8_t CMD_1WRB = 0x96; // 1-Wire Read Byte

// Set Read Pointerで指定するレジスタコード
static const uint8_t PTR_STATUS   = 0xF0;
static const uint8_t PTR_READDATA = 0xE1;

// Statusレジスタのビット
static const uint8_t STATUS_1WB = 0x01; // 1-Wire Busy
static const uint8_t STATUS_PPD = 0x02; // Presence Pulse Detect

// チャンネル選択コード（IO0〜IO7）：書込コードと読出し確認コードのペア
static const uint8_t CH_WRITE[DS2482::NUM_CHANNELS] = {0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96, 0x87};
static const uint8_t CH_READ[DS2482::NUM_CHANNELS]  = {0xB8, 0xB1, 0xAA, 0xA3, 0x9C, 0x95, 0x8E, 0x87};

DS2482::DS2482(uint8_t address, TwoWire &wire) : _address(address), _wire(wire) {}

bool DS2482::writeCommand(uint8_t cmd) {
  _wire.beginTransmission(_address);
  _wire.write(cmd);
  return _wire.endTransmission() == 0;
}

bool DS2482::writeCommand(uint8_t cmd, uint8_t data) {
  _wire.beginTransmission(_address);
  _wire.write(cmd);
  _wire.write(data);
  return _wire.endTransmission() == 0;
}

uint8_t DS2482::readByteRaw() {
  _wire.requestFrom(_address, (uint8_t)1);
  return _wire.available() ? _wire.read() : 0xFF;
}

// Device Resetを発行する。I2C通信自体が成立するかどうかの確認も兼ねる。
bool DS2482::deviceReset() {
  return writeCommand(CMD_DRST);
}

// Set Read Pointerでレジスタを指定してから1バイト読み出す。
uint8_t DS2482::readRegister(uint8_t pointerCode) {
  writeCommand(CMD_SRP, pointerCode);
  return readByteRaw();
}

uint8_t DS2482::readStatus() {
  return readRegister(PTR_STATUS);
}

// 指定チャンネルを選択し、読出し確認コードが一致するかで成否を判定する。
bool DS2482::selectChannel(uint8_t channel) {
  writeCommand(CMD_CHSL, CH_WRITE[channel]);
  return readByteRaw() == CH_READ[channel];
}

// 1-Wireコマンド発行後は読出しポインタが自動でStatusレジスタを指すため、
// busyビットが下がるまでそのまま読み続ける。
uint8_t DS2482::waitBusy() {
  uint8_t status;
  uint32_t start = millis();
  do {
    status = readByteRaw();
    if (millis() - start > 100) break; // タイムアウト保護
  } while (status & STATUS_1WB);
  return status;
}

bool DS2482::reset() {
  writeCommand(CMD_1WRS);
  return (waitBusy() & STATUS_PPD) != 0; // プレゼンスパルス検出
}

void DS2482::writeByte(uint8_t data) {
  writeCommand(CMD_1WWB, data);
  waitBusy();
}

uint8_t DS2482::readByte() {
  writeCommand(CMD_1WRB);
  waitBusy();
  writeCommand(CMD_SRP, PTR_READDATA);
  return readByteRaw();
}
