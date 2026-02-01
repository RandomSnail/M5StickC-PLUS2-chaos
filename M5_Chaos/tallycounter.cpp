#include "sys_core.h"

// --- Tally ---

void tally_loop() {
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  int count = 0;

  while (true) {
    M5.update();
    if (checkExit()) break;

    if (M5.BtnA.wasPressed()) {
      count++;
      toneLite(2000, 20);
    }
    if (M5.BtnB.wasPressed() && count > 0) {
      count--;
      toneLite(1000, 20);
    }

    s.fillScreen(BLACK);
    s.setTextSize(1);
    s.drawString("TALLY COUNTER", 5, 5);
    s.setTextSize(7);
    s.drawCenterString(String(count).c_str(), 120, 40);
    s.setTextSize(1);
    s.drawCenterString("A: +1    Hold B: Exit", 120, 120);
    s.pushSprite(0, 0);
    delay(20);
  }
  s.deleteSprite();
}