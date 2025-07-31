# ATOMJOYSTICK-TX

M5Stack ATOM S3 を使用した ESP-NOW 対応ジョイスティック送信機プロジェクト。ATOM JoyStick モジュールから 2 本のジョイスティックと 4 つのボタンの状態を読み取り、ESP-NOW でブロードキャスト送信します。

## 特徴

- **デュアルジョイスティック対応**: 左右 2 本のアナログスティック（X/Y 軸）
- **4 ボタン入力**: 左スティックボタン、右スティックボタン、左ボタン、右ボタン
- **ESP-NOW ブロードキャスト**: 高速・低遅延でワイヤレス送信
- **リアルタイム表示**: 128x128 ディスプレイでジョイスティック位置とボタン状態を視覚化
- **最適化された描画**: スプライトベースの効率的な画面更新（チラつき防止）
- **自動キャリブレーション**: 起動時に現在のスティック位置を中央値（2048）に設定
- **手動キャリブレーション**: RIGHTボタンでいつでも再キャリブレーション可能
- **値のクランプ処理**: 0-4095の範囲外の値を自動制限
- **デバッグ出力**: シリアルモニターで全センサー値を確認可能

## ハードウェア要件

- [M5Stack ATOM S3](https://docs.m5stack.com/en/core/AtomS3)
- [ATOM JoyStick](https://docs.m5stack.com/en/app/Atom%20JoyStick)
- I2C 接続（SDA: GPIO38, SCL: GPIO39）

## ソフトウェア要件

- [PlatformIO](https://platformio.org/)
- M5Unified ライブラリ (v0.2.7+)

## データ構造

送信される ESP-NOW データパケット:

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

128x128 ピクセルの画面に以下を表示:

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

### キャリブレーション機能

#### 自動キャリブレーション
- デバイス起動時に自動実行
- 現在のジョイスティック位置を中央値（2048）として設定
- 画面に"Calibrating..."と表示

#### 手動キャリブレーション
- **RIGHT ボタン**を押すことで実行
- いつでも現在位置を中央値にリセット可能
- ドリフト補正や初期位置調整に使用

#### 値の処理
- 補正後の値が 0 未満の場合は 0 に制限
- 補正後の値が 4095 超過の場合は 4095 に制限
- 安全な値の範囲を保証

### I2C レジスタマップ

ATOM JoyStick (I2C アドレス: 0x59) のレジスタ:

| レジスタ           | アドレス | 説明     |
| ------------------ | -------- | -------- |
| 左スティック X 軸  | 0x00     | 16bit 値 |
| 左スティック Y 軸  | 0x02     | 16bit 値 |
| 右スティック X 軸  | 0x20     | 16bit 値 |
| 右スティック Y 軸  | 0x22     | 16bit 値 |
| 左ボタン           | 0x70     | 8bit 値  |
| 右ボタン           | 0x71     | 8bit 値  |
| 左スティックボタン | 0x72     | 8bit 値  |
| 右スティックボタン | 0x73     | 8bit 値  |

## 技術仕様

- **通信方式**: ESP-NOW（ブロードキャスト）
- **更新頻度**: 200Hz（5ms 間隔）
- **I2C 周波数**: 標準モード（100kHz）
- **データレート**: 約 20KB/s
- **遅延**: <10ms（センサー読み取りから送信まで）

## トラブルシューティング

### I2C 通信エラー

```
[E][Wire.cpp:513] requestFrom(): i2cRead returned Error -1
```

- ATOM JoyStick の接続を確認
- I2C ピン配線を確認（SDA:38, SCL:39）
- 電源供給を確認

### ESP-NOW 送信エラー

```
ESP-NOW初期化エラー
ピア追加失敗
```

- WiFi 設定を確認
- ESP-NOW の初期化順序を確認

## ライセンス

このプロジェクトは[MIT License](LICENSE)の下で公開されています。

## 貢献

プルリクエストやイシューの報告を歓迎します。

## 参考資料

- [M5Stack ATOM S3 公式ドキュメント](https://docs.m5stack.com/en/core/AtomS3)
- [ATOM JoyStick 公式ドキュメント](https://docs.m5stack.com/en/app/Atom%20JoyStick)
- [ESP-NOW プロトコル仕様](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
