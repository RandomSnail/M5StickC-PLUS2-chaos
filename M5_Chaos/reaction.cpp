#include "sys_core.h"
#include "apps.h"

// --- Reaction ---

void reaction_loop() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.drawCenterString("Wait for GREEN...", 120, 60);
  delay(random(2000, 5000));

  M5.Display.fillScreen(GREEN);
  unsigned long start = millis();
  while (true) {
    M5.update();
    if (M5.BtnA.wasPressed()) {
      unsigned long diff = millis() - start;
      M5.Display.fillScreen(BLACK);
      M5.Display.setCursor(50, 60);
      M5.Display.setTextSize(2);
      M5.Display.printf("%lu ms", diff);
      toneLite(1000, 100);
      delay(2000);
      return;
    }
    if (checkExit()) return;
  }
}