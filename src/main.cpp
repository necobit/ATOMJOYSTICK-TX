#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <M5Unified.h>
#include <Wire.h>

#define JOYSTICK_I2C_ADDR 0x59
#define STICK_L_X_REG 0x00
#define STICK_L_Y_REG 0x02
#define STICK_R_X_REG 0x20
#define STICK_R_Y_REG 0x22
#define BTN_L_STICK_REG 0x70
#define BTN_R_STICK_REG 0x71
#define BTN_LEFT_REG 0x72
#define BTN_RIGHT_REG 0x73

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message
{
  int16_t joy1_x;
  int16_t joy1_y;
  int16_t joy2_x;
  int16_t joy2_y;
  uint8_t btn_l_stick;
  uint8_t btn_r_stick;
  uint8_t btn_left;
  uint8_t btn_right;
  uint32_t counter;
} struct_message;

struct_message myData;

LGFX_Sprite leftStickSprite(&M5.Display);
LGFX_Sprite rightStickSprite(&M5.Display);
LGFX_Sprite leftTextSprite(&M5.Display);
LGFX_Sprite rightTextSprite(&M5.Display);

int prevLeftX = -1, prevLeftY = -1;
int prevRightX = -1, prevRightY = -1;
int16_t prevLeftValX = -1, prevLeftValY = -1;
int16_t prevRightValX = -1, prevRightValY = -1;
uint8_t prevButtons = 0xFF;

void initDisplay()
{
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(1);
  M5.Display.drawString("L", 10, 10);
  M5.Display.drawString("R", 70, 10);

  M5.Display.drawCircle(32, 50, 25, TFT_WHITE);
  M5.Display.drawCircle(32, 50, 24, TFT_WHITE);
  M5.Display.drawCircle(96, 50, 25, TFT_WHITE);
  M5.Display.drawCircle(96, 50, 24, TFT_WHITE);

  M5.Display.drawString("LS", 5, 105);
  M5.Display.drawString("RS", 35, 105); 
  M5.Display.drawString("L", 65, 105);
  M5.Display.drawString("R", 95, 105);

  leftStickSprite.createSprite(60, 60);
  rightStickSprite.createSprite(60, 60);
  leftTextSprite.createSprite(50, 25);
  rightTextSprite.createSprite(50, 25);
}

void updateJoystickSprite(LGFX_Sprite *sprite, int centerX, int centerY, int radius,
                          int16_t stickX, int16_t stickY, uint16_t color,
                          int *prevX, int *prevY)
{
  int mapX = map(stickX, 0, 4095, -radius + 10, radius - 10);
  int mapY = map(stickY, 0, 4095, -radius + 10, radius - 10);

  int dotX = centerX + mapX;
  int dotY = centerY + mapY;

  if (dotX != *prevX || dotY != *prevY)
  {
    if (*prevX != -1)
    {
      M5.Display.fillCircle(*prevX, *prevY, 3, TFT_BLACK);
    }

    M5.Display.fillCircle(dotX, dotY, 3, color);

    *prevX = dotX;
    *prevY = dotY;
  }
}

void updateTextSprite(LGFX_Sprite *sprite, int x, int y, int16_t valX, int16_t valY,
                      int16_t *prevValX, int16_t *prevValY)
{
  if (valX != *prevValX)
  {
    M5.Display.fillRect(x, y, 50, 10, TFT_BLACK);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(1);
    M5.Display.drawString(String(valX), x, y);
    *prevValX = valX;
  }

  if (valY != *prevValY)
  {
    M5.Display.fillRect(x, y + 10, 50, 10, TFT_BLACK);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(1);
    M5.Display.drawString(String(valY), x, y + 10);
    *prevValY = valY;
  }
}

void updateButtonDisplay(uint8_t btnLStick, uint8_t btnRStick, uint8_t btnLeft, uint8_t btnRight) {
  uint8_t currentButtons = (btnLStick << 3) | (btnRStick << 2) | (btnLeft << 1) | btnRight;
  
  if (currentButtons != prevButtons) {
    M5.Display.fillRect(5, 115, 20, 10, btnLStick ? TFT_GREEN : TFT_RED);
    M5.Display.fillRect(35, 115, 20, 10, btnRStick ? TFT_GREEN : TFT_RED);
    M5.Display.fillRect(65, 115, 20, 10, btnLeft ? TFT_GREEN : TFT_RED);
    M5.Display.fillRect(95, 115, 20, 10, btnRight ? TFT_GREEN : TFT_RED);
    
    prevButtons = currentButtons;
  }
}

void updateDisplay(int16_t joy1_x, int16_t joy1_y, int16_t joy2_x, int16_t joy2_y,
                  uint8_t btnLStick, uint8_t btnRStick, uint8_t btnLeft, uint8_t btnRight)
{
  updateJoystickSprite(&leftStickSprite, 32, 50, 25, joy2_x, joy2_y, TFT_CYAN,
                       &prevLeftX, &prevLeftY);
  updateJoystickSprite(&rightStickSprite, 96, 50, 25, joy1_x, joy1_y, TFT_MAGENTA,
                       &prevRightX, &prevRightY);

  updateTextSprite(&leftTextSprite, 5, 85, joy2_x, joy2_y, &prevLeftValX, &prevLeftValY);
  updateTextSprite(&rightTextSprite, 70, 85, joy1_x, joy1_y, &prevRightValX, &prevRightValY);
  
  updateButtonDisplay(btnLStick, btnRStick, btnLeft, btnRight);
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("送信ステータス: ");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("成功");
  }
  else
  {
    Serial.println("失敗");
  }
}

uint8_t readByteData(uint8_t reg)
{
  Wire.beginTransmission(JOYSTICK_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(JOYSTICK_I2C_ADDR, 1);
  if (Wire.available())
  {
    return Wire.read();
  }
  return 128;
}

int16_t readJoystickData(uint8_t reg)
{
  Wire.beginTransmission(JOYSTICK_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(JOYSTICK_I2C_ADDR, 2);
  if (Wire.available() >= 2)
  {
    uint8_t lowByte = Wire.read();
    uint8_t highByte = Wire.read();
    int16_t value = (highByte << 8) | lowByte;
    return value;
  }
  return 2048;
}

void scanI2CDevices()
{
  Serial.println("I2Cデバイススキャン開始...");
  int deviceCount = 0;

  for (byte address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.printf("I2Cデバイス発見: 0x%02X\n", address);
      deviceCount++;
    }
  }

  if (deviceCount == 0)
  {
    Serial.println("I2Cデバイスが見つかりません");
  }
  else
  {
    Serial.printf("合計 %d 個のI2Cデバイスが見つかりました\n", deviceCount);
  }
}

void setup()
{
  M5.begin();
  Serial.begin(115200);

  Wire.begin(38, 39);
  delay(100);

  scanI2CDevices();

  Serial.println("ジョイスティック初期化成功");

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW初期化エラー");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("ピア追加失敗");
    return;
  }

  Serial.println("ESP-NOW送信機 準備完了");
  initDisplay();
}

uint32_t counter = 0;

void loop()
{
  myData.joy1_x = readJoystickData(STICK_R_X_REG);
  myData.joy1_y = readJoystickData(STICK_R_Y_REG);
  myData.joy2_x = readJoystickData(STICK_L_X_REG);
  myData.joy2_y = readJoystickData(STICK_L_Y_REG);
  myData.btn_l_stick = readByteData(BTN_L_STICK_REG);
  myData.btn_r_stick = readByteData(BTN_R_STICK_REG);
  myData.btn_left = readByteData(BTN_LEFT_REG);
  myData.btn_right = readByteData(BTN_RIGHT_REG);
  myData.counter = counter;

  updateDisplay(myData.joy1_x, myData.joy1_y, myData.joy2_x, myData.joy2_y,
               myData.btn_l_stick, myData.btn_r_stick, myData.btn_left, myData.btn_right);

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.printf("ジョイスティックデータ送信 [%d] J1(%d,%d) J2(%d,%d) BTN(%d,%d,%d,%d)\n",
                  counter, myData.joy1_x, myData.joy1_y, myData.joy2_x, myData.joy2_y,
                  myData.btn_l_stick, myData.btn_r_stick, myData.btn_left, myData.btn_right);
  }
  else
  {
    Serial.println("送信エラー");
  }

  counter++;
  delay(5);
}