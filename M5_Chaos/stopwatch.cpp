#include "sys_core.h"

void stopwatch_loop() {
  unsigned long start = 0, elapsed = 0;
  bool run = false;
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  while (true) {
    M5.update();
    if (checkExit()) break;

    if (M5.BtnA.wasPressed()) {
      if (run) {
        elapsed += (millis() - start);
        run = false;
      } else {
        start = millis();
        run = true;
      }
      toneLite(1500, 20);
    }

    if (M5.BtnB.wasPressed() && !run && !M5.BtnB.pressedFor(800)) elapsed = 0;

    unsigned long t = elapsed + (run ? (millis() - start) : 0);
    s.fillScreen(BLACK);
    s.fillRect(0, 0, 240, 24, 0x2124);
    s.setTextColor(WHITE, 0x2124);
    s.drawString("STOPWATCH", 5, 4);

    s.setTextColor(WHITE, BLACK);
    s.setTextSize(4);
    s.setCursor(20, 50);
    s.printf("%02lu:%02lu:%02lu", (t / 60000), (t % 60000) / 1000, (t % 1000) / 10);

    s.setTextSize(1);
    s.drawString(run ? "A: STOP" : "A: START  B: RESET", 60, 100);
    s.pushSprite(0, 0);
    delay(20);
  }
  s.deleteSprite();
}