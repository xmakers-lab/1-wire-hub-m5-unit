# basic

DS2482S-800（1-Wire×8chマルチプレクサ）とのI2C通信のみを確認するサンプルです。1-Wireバスに接続するセンサー（DS18B20等）は扱わず、DS2482-800チップ自体との通信（Device Reset／Status読出し／Channel Select）が正しく行えるかだけを検証します。

## 動作

1. 起動時にDevice Resetを発行し、DS2482-800を検出できるか確認する（検出できた場合はStatusレジスタも読み出す）
2. ループ内でch0〜ch7を順にChannel Selectし、選択が成功したか（読出し確認コードの一致）をLCD・シリアルに表示する

## ライブラリ構成

- `lib/DS2482` — DS2482-800のチップ制御（Device Reset／Status読出し／Channel Select）と、1-Wireビット操作（`OneWireMaster`実装）を行うライブラリです。本サンプルではチップとのI2C通信確認のみに用途を絞って使用しています。
- `lib/OneWireMaster` — 1-Wireバスマスタの抽象インターフェース。`DS2482`はこれを実装しています。

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
