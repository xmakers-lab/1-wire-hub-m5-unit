#include <M5Unified.h>
#include <DS2482.h>
#include <DS18B20.h>

// DS2482S-800（1-Wire×6chマルチ温度センサーUnit）の基本動作確認サンプル。
// DS2482ライブラリ（I2C通信＋1-Wireビット操作）とDS18B20ライブラリ（デバイスプロトコル）を
// 組み合わせて、ch0〜ch5のDS18B20から温度を読み取り、LCDとシリアルに表示する。
//
// パイプライン方式：
//   1. ch0〜ch5に順にConvert Tを発行（全ch分の変換開始）
//   2. DS18B20の変換時間（12bit分解能で約750ms）分、まとめて1回だけ待機
//   3. ch0〜ch5を順に読出し＋CRC検証

static const uint8_t ACTIVE_CHANNELS = 6; // 初回SKUはch0〜ch5の6ch実装

DS2482 ds2482;               // AD0/AD1/AD2固定のためI2Cアドレスは既定値0x18を使用
DS18B20 sensor(ds2482);      // 各chともSkip ROMで単独DS18B20をアクセス

bool channelPresent[ACTIVE_CHANNELS];
float channelTemperature[ACTIVE_CHANNELS];

// 全ch分の変換開始→750ms待機→全ch分の読出しをまとめて行う。
void updateAllChannels() {
  for (uint8_t ch = 0; ch < ACTIVE_CHANNELS; ch++) {
    channelPresent[ch] = ds2482.selectChannel(ch) && sensor.requestConversion();
  }

  delay(750); // 12bit分解能の変換完了待ち（全ch共通で1回のみ）

  for (uint8_t ch = 0; ch < ACTIVE_CHANNELS; ch++) {
    if (!channelPresent[ch]) continue; // 変換開始時に未検出だったchはスキップ
    float tempC;
    channelPresent[ch] = ds2482.selectChannel(ch) && sensor.readTemperature(tempC); // 読出し失敗（CRC不一致等）も未検出扱い
    if (channelPresent[ch]) channelTemperature[ch] = tempC;
  }
}

void setup() {
  M5.begin();
  Wire.begin();

  M5.Lcd.setTextSize(2);

  if (!ds2482.deviceReset()) {
    Serial.println("DS2482-800が見つかりません（I2C 0x18を確認してください）");
    M5.Lcd.println("DS2482-800 not found");
  } else {
    Serial.println("DS2482-800を初期化しました");
  }
}

void loop() {
  updateAllChannels();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  for (uint8_t ch = 0; ch < ACTIVE_CHANNELS; ch++) {
    if (channelPresent[ch]) {
      M5.Lcd.printf("CH%d: %.2f C\n", ch, channelTemperature[ch]);
      Serial.printf("CH%d: %.2f C\n", ch, channelTemperature[ch]);
    } else {
      M5.Lcd.printf("CH%d: ---\n", ch);
      Serial.printf("CH%d: ---\n", ch);
    }
  }

  delay(250);
}
