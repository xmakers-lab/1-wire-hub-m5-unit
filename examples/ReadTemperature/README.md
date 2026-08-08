# ReadTemperature

DS2482S-800（1-Wire×6chマルチ温度センサーUnit）を使い、ch0〜ch5に接続したDS18B20から温度を読み取ってLCD・シリアルに表示するサンプルです。

## 動作

1. ch0〜ch5に順にConvert Tを発行し、全ch分の変換を開始する
2. DS18B20の変換時間（12bit分解能で約750ms）分、まとめて1回だけ待機する
3. ch0〜ch5を順に読出し、CRC検証したうえで温度を確定する

未接続、またはCRC不一致のチャンネルはLCD上で `---` と表示されます。

## ライブラリ構成

`lib/` 配下に、責務ごとに3つのライブラリを分けています。

```
[DS18B20] --依存--> [OneWireMaster] <--実装-- [DS2482]
```

- `lib/OneWireMaster` — 1-Wireバスマスタの抽象インターフェース（`reset` / `writeByte` / `readByte`）。
- `lib/DS2482` — DS2482-800のチップ制御（Device Reset／Status読出し／Channel Select）と、1-Wireビット操作（`OneWireMaster`の実装）を行うライブラリ。
- `lib/DS18B20` — DS18B20のデバイスプロトコル（Skip ROM／Convert T／Read Scratchpad／CRC検証）を行うドライバ。`OneWireMaster`にのみ依存し、DS2482を直接は知らない。

DS18B20ドライバがDS2482に直接依存せず`OneWireMaster`インターフェース越しに1-Wire通信を行う構成のため、DS2482以外の1-Wireマスタ実装（ビットバンギング等）と組み合わせても動作します。

`selectChannel()`によるチャンネル選択はDS2482S-800固有のマルチプレクサ機能のため、`OneWireMaster`インターフェースには含めず、`DS2482`クラス独自のメソッドとして提供しています（`main.cpp`側でチャンネルごとに呼び分けます）。

## 動作環境

- [PlatformIO](https://platformio.org/)
- Framework: Arduino
- Board: `m5stack-core-esp32`
- 依存ライブラリ: [M5Unified](https://github.com/m5stack/M5Unified)

## 使い方

```
pio run -t upload
pio device monitor
```

DS2482-800が見つからない場合はI2Cアドレス（既定値 `0x18`、AD0/AD1/AD2固定）を確認してください。
