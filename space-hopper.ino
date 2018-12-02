#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define FRAME_RATE 8.0

void setup() {
  u8g2.begin();
  u8g2.setContrast(0);
}


void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_tf);
  u8g2.drawStr(10, 20, "space-hopper v1.0");
  u8g2.sendBuffer();

}

