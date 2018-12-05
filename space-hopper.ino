#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// graphics
#include "ship_0.h"
#include "ship_1.h"
#include "ship_2.h"
#include "ship_3.h"

#define FRAME_RATE 16
#define BOOST_AMOUNT 4

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

uint8_t loopCount = 0;

uint8_t shipY = 32;
uint8_t shipBoost = 0;

void drawShip() {
  if (shipBoost == 0 && shipY > 24) {
    u8g2.drawXBM(0, shipY - 8, 8, 8, ship_0_bits);
  } else {
    switch (loopCount % 3) {
      case 0:
        u8g2.drawXBM(0, shipY - 8, 8, 8, ship_1_bits);
        break;
      case 1:
        u8g2.drawXBM(0, shipY - 8, 8, 8, ship_2_bits);
        break;
      case 2:
        u8g2.drawXBM(0, shipY - 8, 8, 8, ship_3_bits);
        break;
      default:
        u8g2.drawXBM(0, shipY - 8, 8, 8, ship_0_bits);
    }
  }

}

void moveShip() {
  if (shipY < 32) {
    shipY++;
  }

  // only boost if we dont go out side the screen
  if (shipY > 8) {
    // only remove as much height as screen is available
    shipY -= shipY - 8 > shipBoost ? shipBoost : shipY;
  }

  shipY = shipY < 8 ? 8 : shipY;

  if (shipBoost > 0) {
    shipBoost--;
  }
}

void boostShip() {
  Serial.println("boosted");
  shipBoost = BOOST_AMOUNT;
}


int buttonState = 0;
void setup() {
  Serial.begin(9600);
  pinMode(D5, INPUT);
  u8g2.begin();
  u8g2.setContrast(0);
}


void loop() {
  loopCount++;
  buttonState = digitalRead(D5);
  if (buttonState == HIGH) {
    boostShip();
  }

  moveShip();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_tf);
  u8g2.drawStr(10, 10, String("Y:" + String(shipY)).c_str());

  drawShip();

  u8g2.sendBuffer();

  delay(1000 / FRAME_RATE);
  Serial.println("loop");

}

