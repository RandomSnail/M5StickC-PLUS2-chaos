#include "sys_core.h"

void snake_loop() {
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  int snkX[100], snkY[100];
  int len = 5;
  int dir = 1;
  int foodX = 10, foodY = 10;
  bool gameOver = false;

  // --- RANDOM BACKGROUND DOTS ---
  int dotsX[50], dotsY[50];
  for (int i = 0; i < 50; i++) {
    dotsX[i] = random(0, 240);
    dotsY[i] = random(0, 135);
  }

  // Initial Setup
  for (int i = 0; i < len; i++) {
    snkX[i] = 10 - i;
    snkY[i] = 10;
  }

  while (true) {
    M5.update();
    if (checkExit()) break;  // Hold B to exit

    if (gameOver) {
      // RESTART LOGIC (Button A)
      if (M5.BtnA.wasPressed()) {
        len = 5;
        dir = 1;
        foodX = random(1, 29);
        foodY = random(1, 16);
        for (int i = 0; i < len; i++) {
          snkX[i] = 10 - i;
          snkY[i] = 10;
        }
        gameOver = false;
        toneLite(1500, 100);
      }
    } else {
      // Game Logic
      if (M5.BtnA.wasPressed()) dir = (dir + 3) % 4;
      if (M5.BtnB.wasPressed() && !M5.BtnB.pressedFor(800)) dir = (dir + 1) % 4;

      for (int i = len - 1; i > 0; i--) {
        snkX[i] = snkX[i - 1];
        snkY[i] = snkY[i - 1];
      }

      if (dir == 0) snkY[0]--;
      if (dir == 1) snkX[0]++;
      if (dir == 2) snkY[0]++;
      if (dir == 3) snkX[0]--;

      // Collision Checks
      if (snkX[0] < 0 || snkX[0] >= 30 || snkY[0] < 0 || snkY[0] >= 17) gameOver = true;
      for (int i = 1; i < len; i++)
        if (snkX[0] == snkX[i] && snkY[0] == snkY[i]) gameOver = true;

      // Eat Food
      if (snkX[0] == foodX && snkY[0] == foodY) {
        len++;
        toneLite(2000, 20);
        foodX = random(1, 29);
        foodY = random(1, 16);
      }
    }

    s.fillScreen(BLACK);

    // DRAW BACKGROUND DOTS
    for (int i = 0; i < 50; i++) {
      s.drawPixel(dotsX[i], dotsY[i], 0x2124);  // Dark Grey
    }

    if (gameOver) {
      s.setTextColor(RED);
      s.drawCenterString("GAME OVER", 120, 40);
      s.setTextColor(WHITE);
      s.drawCenterString("A: Restart", 120, 70);
      s.drawCenterString("Hold B to Exit", 120, 95);
    } else {
      s.fillRect(foodX * 8, foodY * 8, 7, 7, RED);
      for (int i = 0; i < len; i++) {
        s.fillRect(snkX[i] * 8, snkY[i] * 8, 7, 7, (i == 0) ? GREEN : THEME_COLOR);
      }
    }
    s.pushSprite(0, 0);
    delay(100);
  }
  s.deleteSprite();
}