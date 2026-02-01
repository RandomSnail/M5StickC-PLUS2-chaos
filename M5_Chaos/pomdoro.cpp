#include "sys_core.h"

// --- pomdoro ---

void pomodoro_loop() {
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  long workTime = 25 * 60;
  long restTime = 5 * 60;
  long currentTimer = workTime;
  bool isWork = true;
  bool running = false;
  unsigned long lastMs = millis();

  while (true) {
    M5.update();
    if (checkExit()) break;  // Long Press B to exit

    // Toggle Start/Stop
    if (M5.BtnA.wasPressed()) {
      running = !running;
      toneLite(1500, 50);
    }

    // Reset (Short Press B)
    if (M5.BtnB.wasPressed() && !M5.BtnB.pressedFor(800)) {
      running = false;
      currentTimer = isWork ? workTime : restTime;
      toneLite(1000, 50);
    }

    if (running && millis() - lastMs >= 1000) {
      currentTimer--;
      lastMs = millis();
      if (currentTimer <= 0) {
        toneLite(2000, 1000);
        isWork = !isWork;
        currentTimer = isWork ? workTime : restTime;
        running = false;
      }
    } else if (!running) {
      lastMs = millis();
    }

    s.fillScreen(BLACK);
    s.fillCircle(120, 67, 60, isWork ? RED : GREEN);
    s.fillCircle(120, 67, 55, BLACK);
    float angle = (float)currentTimer / (float)(isWork ? workTime : restTime) * 360.0;
    s.fillArc(120, 67, 56, 60, 270, 270 - angle, isWork ? RED : GREEN);

    s.setTextColor(WHITE);
    s.setTextSize(1);
    s.drawCenterString(isWork ? "FOCUS" : "REST", 120, 40);
    s.setTextSize(4);
    int m = currentTimer / 60;
    int sec = currentTimer % 60;
    s.setCursor(65, 60);
    s.printf("%02d:%02d", m, sec);

    // UI Controls Hint
    s.setTextSize(1);
    s.drawCenterString("A: Start/Stop   B: Reset", 120, 120);

    s.pushSprite(0, 0);
    delay(50);
  }
  s.deleteSprite();
}