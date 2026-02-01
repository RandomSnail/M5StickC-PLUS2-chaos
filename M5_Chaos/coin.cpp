#include "sys_core.h"

// --- Coin-Toss ---

void coin_loop() {
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  if (!M5.Imu.isEnabled()) M5.Imu.begin();
  uint16_t GOLD = 0xFFE0;
  uint16_t SILVER = 0xBDF7;

  while (true) {
    M5.update();
    if (checkExit()) break;
    M5.Imu.update();

    if (abs(M5.Imu.getImuData().accel.x) > 1.5) {
      bool heads = random(2);

      for (int i = 0; i < 5; i++) {
        s.fillScreen(i % 2 == 0 ? GOLD : SILVER);
        s.pushSprite(0, 0);
        toneLite(1000 + (i * 100), 50);
        delay(100);
      }

      s.fillScreen(heads ? GOLD : SILVER);
      s.setTextColor(BLACK);
      s.setTextSize(3);
      s.drawCenterString(heads ? "HEADS" : "TAILS", 120, 60);
      s.setTextSize(1);
      s.drawCenterString("Hold B to Exit", 120, 100);
      s.pushSprite(0, 0);
      toneLite(heads ? 2000 : 1000, 200);

      delay(1000);
    } else {
      s.fillScreen(BLACK);
      s.setTextColor(WHITE);
      s.setTextSize(2);
      s.drawCenterString("SHAKE ME", 120, 60);
      s.pushSprite(0, 0);
    }
    delay(50);
  }
  s.deleteSprite();
}