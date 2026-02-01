#include "sys_core.h"

// --- Decibel ---
void decibel_meter_loop() {
  hardResetRadios();
  M5.Mic.begin();
  M5Canvas sprite(&M5.Display);
  sprite.createSprite(240, 135);
  int max_vol = 0;

  while (true) {
    M5.update();
    if (checkExit()) break;

    M5.Mic.record(mic_data, 256, 16000);
    int current_max = 0;
    for (int i = 0; i < 256; i++)
      if (abs(mic_data[i]) > current_max) current_max = abs(mic_data[i]);
    if (current_max > max_vol) max_vol = current_max;

    sprite.fillScreen(BLACK);
    for (int i = 0; i < 240; i++) {
      int val = mic_data[i] / 150;
      sprite.drawPixel(i, 67 + val, THEME_COLOR);
    }

    sprite.setTextColor(WHITE);
    sprite.setTextSize(1);
    sprite.drawString("DB METER", 5, 5);

    int bar = map(current_max, 0, 10000, 0, 200);
    sprite.drawRect(20, 100, 200, 10, WHITE);
    sprite.fillRect(20, 100, bar, 10, (bar > 150) ? RED : GREEN);
    sprite.pushSprite(0, 0);
    delay(50);
  }
  M5.Mic.end();
  sprite.deleteSprite();
}
