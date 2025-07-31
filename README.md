# ATOMJOYSTICK-TX

M5Stack ATOM S3を使用したESP-NOW対応ジョイスティック送信機プロジェクト。ATOM JoyStickモジュールから2本のジョイスティックと4つのボタンの状態を読み取り、ESP-NOWでブロードキャスト送信します。

## 特徴

- **デュアルジョイスティック対応**: 左右2本のアナログスティック（X/Y軸）
- **4ボタン入力**: 左スティックボタン、右スティックボタン、左ボタン、右ボタン
- **ESP-NOWブロードキャスト**: 高速・低遅延でワイヤレス送信
- **リアルタイム表示**: 128x128ディスプレイでジョイスティック位置とボタン状態を視覚化
- **最適化された描画**: スプライトベースの効率的な画面更新（チラつき防止）
- **デバッグ出力**: シリアルモニターで全センサー値を確認可能

## ハードウェア要件

- [M5Stack ATOM S3](https://docs.m5stack.com/en/core/AtomS3)
- [ATOM JoyStick](https://docs.m5stack.com/en/app/Atom%20JoyStick)
- I2C接続（SDA: GPIO38, SCL: GPIO39）

## ソフトウェア要件

- [PlatformIO](https://platformio.org/)
- M5Unified ライブラリ (v0.2.7+)

## データ構造

送信されるESP-NOWデータパケット:

```cpp
struct struct_message {
  int16_t joy1_x;        // 右ジョイスティック X軸 (0-4095)
  int16_t joy1_y;        // 右ジョイスティック Y軸 (0-4095)
  int16_t joy2_x;        // 左ジョイスティック X軸 (0-4095)
  int16_t joy2_y;        // 左ジョイスティック Y軸 (0-4095)
  uint8_t btn_l_stick;   // 左ジョイスティックボタン (0/1)
  uint8_t btn_r_stick;   // 右ジョイスティックボタン (0/1)
  uint8_t btn_left;      // 左ボタン (0/1)
  uint8_t btn_right;     // 右ボタン (0/1)
  uint32_t counter;      // パケットカウンター
};
```

## ディスプレイ表示

128x128ピクセルの画面に以下を表示:

- **ジョイスティック**: 白い円内で位置を色付きドットで表示
  - 左スティック: シアン色
  - 右スティック: マゼンタ色
- **数値表示**: 各軸の生データ値
- **ボタン状態**: 押下時は緑、離したときは赤で表示
- **ラベル**: L/R（ジョイスティック）、LS/RS/L/R（ボタン）

## 使用方法

### ビルドとアップロード

```bash
# プロジェクトをクローン
git clone https://github.com/necobit/ATOMJOYSTICK-TX.git
cd ATOMJOYSTICK-TX

# ビルド
pio run

# アップロード
pio run -t upload

# シリアルモニター
pio device monitor
```

### I2Cレジスタマップ

ATOM JoyStick (I2Cアドレス: 0x59) のレジスタ:

| レジスタ | アドレス | 説明 |
|----------|----------|------|
| 左スティック X軸 | 0x00 | 16bit値 |
| 左スティック Y軸 | 0x02 | 16bit値 |
| 右スティック X軸 | 0x20 | 16bit値 |
| 右スティック Y軸 | 0x22 | 16bit値 |
| 左スティックボタン | 0x70 | 8bit値 |
| 右スティックボタン | 0x71 | 8bit値 |
| 左ボタン | 0x72 | 8bit値 |
| 右ボタン | 0x73 | 8bit値 |

## 技術仕様

- **通信方式**: ESP-NOW（ブロードキャスト）
- **更新頻度**: 200Hz（5ms間隔）
- **I2C周波数**: 標準モード（100kHz）
- **データレート**: 約20KB/s
- **遅延**: <10ms（センサー読み取りから送信まで）

## トラブルシューティング

### I2C通信エラー
```
[E][Wire.cpp:513] requestFrom(): i2cRead returned Error -1
```
- ATOM JoyStickの接続を確認
- I2Cピン配線を確認（SDA:38, SCL:39）
- 電源供給を確認

### ESP-NOW送信エラー
```
ESP-NOW初期化エラー
ピア追加失敗
```
- WiFi設定を確認
- ESP-NOWの初期化順序を確認

## ライセンス

このプロジェクトは[MIT License](LICENSE)の下で公開されています。

## 貢献

プルリクエストやイシューの報告を歓迎します。

## 参考資料

- [M5Stack ATOM S3 公式ドキュメント](https://docs.m5stack.com/en/core/AtomS3)
- [ATOM JoyStick 公式ドキュメント](https://docs.m5stack.com/en/app/Atom%20JoyStick)
- [ESP-NOW プロトコル仕様](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
