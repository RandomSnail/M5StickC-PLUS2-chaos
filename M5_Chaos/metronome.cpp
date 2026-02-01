#include "sys_core.h"
// --- metronome ---

void metronome_loop() {
  int bpm = 60;
  bool running = false;
  unsigned long lastBeat = 0;
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  while (true) {
    M5.update();
    if (checkExit()) break;

    if (M5.BtnA.wasPressed()) {
      if (running) bpm += 5;
      else running = true;
    }
    if (M5.BtnB.wasPressed() && !M5.BtnB.pressedFor(800)) {
      if (running) bpm -= 5;
      if (bpm < 30) {
        running = false;
        bpm = 60;
      }
    }

    if (running && millis() - lastBeat > (60000 / bpm)) {
      M5.Speaker.tone(2000, 50);
      s.fillScreen(WHITE);
      s.pushSprite(0, 0);
      delay(50);
      lastBeat = millis();
    }

    s.fillScreen(BLACK);
    s.setTextSize(2);
    s.drawCenterString("METRONOME", 120, 20);
    s.setTextSize(5);
    s.setCursor(80, 55);
    s.printf("%d", bpm);
    s.setTextSize(1);
    s.drawCenterString(running ? "A: +5   B: -5" : "A: START", 120, 115);
    s.pushSprite(0, 0);
    delay(20);
  }
  s.deleteSprite();
}