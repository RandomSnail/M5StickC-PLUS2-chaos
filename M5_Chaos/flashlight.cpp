#include "sys_core.h"

// --- FLASHLIGHT ---
void flashlight_loop() {
  int prevBrightness = M5.Display.getBrightness();
  M5.Display.fillScreen(WHITE);
  M5.Display.setBrightness(255);

  while (true) {
    M5.update();
    if (M5.BtnB.pressedFor(1000) || M5.BtnA.wasPressed()) break;
    delay(20);
  }

  M5.Display.setBrightness(prevBrightness);
}