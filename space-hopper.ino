#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// graphics
#include "ship_0.h"
#include "ship_1.h"
#include "ship_2.h"
#include "ship_3.h"
#include "asteroid_0.h"

#define FRAME_RATE 16
#define BOOST_AMOUNT 4
#define SHIP_SIZE 8
#define ASTEROID_SIZE 8
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
#define NUM_ASTEROIDS 4
#define ASTEROID_MAX_SPEED 6

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

enum GAMESTATE {
  START, RUNNING, STOPPED
};

GAMESTATE gameState = START;

uint8_t loopCount = 0;

uint8_t shipY = SCREEN_HEIGHT;
uint8_t shipBoost = 0;

struct asteroid {
  asteroid() : x(SCREEN_WIDTH), y(SCREEN_HEIGHT), s(1) {}
  int16_t x;
  int16_t y;
  uint8_t s;
};

asteroid asteroids[NUM_ASTEROIDS];

uint16_t asteroidCount = 0;

void drawAsteroid(asteroid& a) {
  u8g2.drawXBM(a.x, a.y - ASTEROID_SIZE, ASTEROID_SIZE, ASTEROID_SIZE, asteroid_0_bits);
}

void printAsteroid(asteroid& a) {
  Serial.print(a.x);
  Serial.print(", ");
  Serial.print(a.y);
  Serial.print(", ");
  Serial.println(a.s);
}

void moveAsteroid(asteroid& a) {
  if (a.x < 0) {
    // remove asteroid and stop it
    asteroidCount++;
    a.x = SCREEN_WIDTH;
    a.y = SCREEN_HEIGHT;
    a.s = 0;
  } else {
    a.x -= a.s;
  }
}

void drawShip() {
  if (shipBoost == 0 && shipY > 24) {
    u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_0_bits);
  } else {
    switch (loopCount % 3) {
      case 0:
        u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_1_bits);
        break;
      case 1:
        u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_2_bits);
        break;
      case 2:
      default:
        u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_3_bits);
    }
  }

}

void moveShip() {
  if (shipY < SCREEN_HEIGHT) {
    shipY++;
  }

  // only boost if we dont go out side the screen
  if (shipY > SHIP_SIZE) {
    // only remove as much height as screen is available
    shipY -= shipY - SHIP_SIZE > shipBoost ? shipBoost : shipY;
  }

  // make sure ship keeps inside screen (remove?)
  shipY = shipY < SHIP_SIZE ? SHIP_SIZE : shipY;

  if (shipBoost > 0) {
    shipBoost--;
  }
}

void boostShip() {
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
  if (gameState == START) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr(12, 8, "SpaceHopper v1.0");
    u8g2.drawStr(4, 20, "by @pauls_3d_things");
    u8g2.drawStr(6, 30, "~~ Press Button ~~");
    u8g2.sendBuffer();

    delay(250);

    buttonState = digitalRead(D5);
    if (buttonState == HIGH) {
      gameState = RUNNING;
    }
    return;
  } else if (gameState == STOPPED) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr(4, 8, "SpaceHopper v1.0");
    u8g2.drawStr(4, 20, "~~ Press Button ~~");
    u8g2.drawStr(4, 30, String("Score: " + String(asteroidCount)).c_str());
    u8g2.sendBuffer();

    delay(250);

    buttonState = digitalRead(D5);
    if (buttonState == HIGH) {
      gameState = START;
    }
    return;
  }

  loopCount++;
  buttonState = digitalRead(D5);
  if (buttonState == HIGH) {
    boostShip();
  }
  moveShip();

  for (uint8_t i = 0; i < NUM_ASTEROIDS; i++) {
    uint8_t r = random(0, 10);

    if (asteroids[i].x >= SCREEN_WIDTH && r == 2) {
      asteroids[i].s = random(1, ASTEROID_MAX_SPEED);
      asteroids[i].y = random(ASTEROID_SIZE, SCREEN_HEIGHT);
    }
    moveAsteroid(asteroids[i]);
  }

  // TODO: calculate collisions

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_tf);
  u8g2.drawStr(110, 10, String(asteroidCount).c_str());

  drawShip();
  for (uint8_t i = 0; i < NUM_ASTEROIDS; i++) {
    drawAsteroid(asteroids[i]);
  }

  u8g2.sendBuffer();

  delay(1000 / FRAME_RATE);
}

