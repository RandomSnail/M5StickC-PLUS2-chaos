#include "sys_core.h"

// --- ANNOY-A-TRON ---
void annoy_loop() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(RED);
  M5.Display.setTextSize(1);
  M5.Display.drawCenterString("ANNOY-A-TRON ACTIVE", 120, 40);
  M5.Display.drawCenterString("Hold B to Deactivate", 120, 60);
  M5.Display.drawCenterString("Screen dimming...", 120, 80);

  // Force Max Volume
  uint8_t oldVol = M5.Speaker.getVolume();
  M5.Speaker.setVolume(255);

  // Non-blocking 2 second delay
  unsigned long startWait = millis();
  while (millis() - startWait < 2000) {
    M5.update();
    if (checkExit()) {
      M5.Speaker.setVolume(oldVol);  // Restore volume on early exit
      return;
    }
    delay(10);
  }

  M5.Display.setBrightness(0);

  unsigned long lastChirp = millis();
  unsigned long interval = random(5000, 20000);  // More frequent for testing

  while (true) {
    M5.update();

    // Exit Condition
    if (checkExit()) {
      M5.Display.setBrightness(128);
      M5.Speaker.setVolume(oldVol);  // Restore volume
      return;
    }

    // Play Chirp
    if (millis() - lastChirp > interval) {
      // Lower frequency (2-5kHz) is much louder/more audible than 15kHz
      int freq = random(2000, 5000);

      // Double chirp pattern
      M5.Speaker.tone(freq, 50);
      delay(100);
      M5.Speaker.tone(freq + 500, 50);

      lastChirp = millis();
      interval = random(5000, 30000);  // Random delay 5-30s
    }

    delay(20);
  }
}