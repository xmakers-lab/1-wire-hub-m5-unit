#include <M5Unified.h>
#include <DS2482.h>

// DS2482S-800（1-Wire×8chマルチプレクサ）とのI2C通信のみを確認するサンプル。
// 1-Wireバスに接続するセンサー（DS18B20等）は扱わず、DS2482-800チップ自体との
// 通信（Device Reset／Status読出し／Channel Select）が正しく行えるかだけを検証する。

DS2482 ds2482; // AD0/AD1/AD2固定のためI2Cアドレスは既定値0x18を使用

void setup() {
  M5.begin();
  Wire.begin();

  M5.Lcd.setTextSize(2);

  if (!ds2482.deviceReset()) {
    Serial.println("DS2482-800が見つかりません（I2C 0x18を確認してください）");
    M5.Lcd.println("DS2482-800 not found");
  } else {
    uint8_t status = ds2482.readStatus();
    Serial.printf("DS2482-800を検出しました（Status=0x%02X）\n", status);
    M5.Lcd.printf("DS2482-800 OK\nStatus=0x%02X\n", status);
  }
}

void loop() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Channel Select test");

  for (uint8_t ch = 0; ch < DS2482::NUM_CHANNELS; ch++) {
    bool ok = ds2482.selectChannel(ch);
    M5.Lcd.printf("CH%d: %s\n", ch, ok ? "OK" : "NG");
    Serial.printf("CH%d: %s\n", ch, ok ? "OK" : "NG");
  }

  delay(1000);
}
