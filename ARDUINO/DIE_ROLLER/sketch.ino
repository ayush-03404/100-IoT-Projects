#include <U8g2lib.h>
#include <Wire.h>

/* ---------------- PIN DEFINITIONS ---------------- */
#define BTN_ROLL   22
#define BTN_RESET  23

/* ---------------- OBJECTS ---------------- */
// SH1106 constructor (works for SH1107)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

/* ---------------- SYSTEM STATE ---------------- */
enum State { IDLE, ROLLING, RESULT };
State currentState = IDLE;

int diceValue = 1;
unsigned long lastAnimTime = 0;
int animFrame = 0;

/* ---------------- SETUP ---------------- */
void setup() {
  pinMode(BTN_ROLL, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  u8g2.begin();
  u8g2.clearBuffer();

  drawDice(diceValue);
}

/* ---------------- LOOP ---------------- */
void loop() {
  if (!digitalRead(BTN_RESET)) {
    resetDice();
    delay(250);
  }

  if (!digitalRead(BTN_ROLL) && currentState != ROLLING) {
    currentState = ROLLING;
    animFrame = 0;
    delay(250);
  }

  switch (currentState) {
    case IDLE:
      idleAnimation();
      break;
    case ROLLING:
      rollingAnimation();
      break;
    case RESULT:
      break;
  }
}

/* ---------------- DICE DRAWING ---------------- */
void drawDice(int value) {
  u8g2.clearBuffer();

  // Make dice fill almost entire screen
  int diceSize = 64;
  int x0 = (128 - diceSize)/1;  // horizontal center
  int y0 = (64 - diceSize)/1;   // vertical center

  // Draw border fully within OLED bounds
  u8g2.drawFrame(x0, y0, diceSize, diceSize);

  // Dot positions inside dice
  int cx[3] = { x0 + diceSize/4, x0 + diceSize/2, x0 + 3*diceSize/4 };
  int cy[3] = { y0 + diceSize/4, y0 + diceSize/2, y0 + 3*diceSize/4 };

  auto dot = [&](int x, int y) { u8g2.drawDisc(x, y, 5, U8G2_DRAW_ALL); };

  switch (value) {
    case 1: dot(cx[1], cy[1]); break;
    case 2: dot(cx[0], cy[0]); dot(cx[2], cy[2]); break;
    case 3: dot(cx[0], cy[0]); dot(cx[1], cy[1]); dot(cx[2], cy[2]); break;
    case 4: dot(cx[0], cy[0]); dot(cx[2], cy[0]); dot(cx[0], cy[2]); dot(cx[2], cy[2]); break;
    case 5: dot(cx[0], cy[0]); dot(cx[2], cy[0]); dot(cx[1], cy[1]); dot(cx[0], cy[2]); dot(cx[2], cy[2]); break;
    case 6:
      dot(cx[0], cy[0]); dot(cx[0], cy[1]); dot(cx[0], cy[2]);
      dot(cx[2], cy[0]); dot(cx[2], cy[1]); dot(cx[2], cy[2]);
      break;
  }

  u8g2.sendBuffer();
}

/* ---------------- ANIMATION ---------------- */
void idleAnimation() {
  if (millis() - lastAnimTime > 700) {
    lastAnimTime = millis();
    diceValue++;
    if (diceValue > 6) diceValue = 1;
    drawDice(diceValue);
  }
}

void rollingAnimation() {
  if (millis() - lastAnimTime > 80) {
    lastAnimTime = millis();
    int temp = random(1, 7);
    drawDice(temp);
    animFrame++;

    if (animFrame > 18) {
      diceValue = random(1, 7);
      drawDice(diceValue);
      currentState = RESULT;
    }
  }
}

/* ---------------- RESET ---------------- */
void resetDice() {
  currentState = IDLE;
  diceValue = 1;
  drawDice(diceValue);
}
