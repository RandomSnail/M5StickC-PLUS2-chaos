#include "sys_core.h"
#include "ble_core.h"
#include "apps.h"

void media_remote_loop() {
  // Start BLE
  if (!startBleHid("M5-Remote")) {
    Serial.println("[ERROR] Failed to start BLE HID");
    return;
  }

  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  bool currentConnected = false;
  unsigned long lastVolUp = 0;
  unsigned long lastVolDown = 0;

  while (true) {
    M5.update();
    
    // EXIT
    if (checkExit()) break;

    // --- FAILSAFE: Force update connection state directly from Server ---
    // This fixes the "Stuck on Pairing" bug if the callback missed the event
    if (NimBLEDevice::getServer() && NimBLEDevice::getServer()->getConnectedCount() > 0) {
       bleConnected = true;
    } else {
       bleConnected = false;
    }
    // ------------------------------------------------------------------

    // Connection Change Sound
    if (bleConnected != currentConnected) {
      currentConnected = bleConnected;
      toneLite(currentConnected ? 2000 : 500, 150);
    }

    s.fillScreen(BLACK);

    if (!currentConnected) {
      // --- PAIRING SCREEN ---
      s.setTextColor(ORANGE, BLACK);
      s.setTextSize(2);
      s.drawCenterString("PAIRING...", 120, 30);

      s.setTextColor(WHITE, BLACK);
      s.setTextSize(1);
      s.drawCenterString("Connect to: M5-Remote", 120, 70);
      
      // Blinking Dot
      if (millis() % 1000 < 500) s.fillCircle(120, 100, 4, WHITE);
    }
    else {
      // --- CONNECTED SCREEN ---
      s.fillRect(0, 0, 240, 28, GREEN);
      s.setTextColor(BLACK, GREEN);
      s.setTextSize(2);
      s.drawCenterString("CONNECTED", 120, 5);

      s.setTextColor(WHITE, BLACK);
      s.setTextSize(2);

      // Buttons
      s.drawRect(10, 40, 100, 50, WHITE);
      s.drawCenterString("PLAY", 60, 55);

      s.drawRect(130, 40, 100, 50, WHITE);
      s.drawCenterString("NEXT", 180, 55);

      s.setTextSize(1);
      s.setTextColor(GREY, BLACK);
      s.drawCenterString("Hold A: Vol+   Hold B: Vol-", 120, 110);

      // --- INPUTS ---
      // Play/Pause
      if (M5.BtnA.wasPressed()) {
        sendMediaKey(MEDIA_PLAY_PAUSE);
        s.fillRect(10, 40, 100, 50, THEME_COLOR); // Visual feedback
        toneLite(1500, 20);
      }

      // Next
      if (M5.BtnB.wasPressed()) {
        sendMediaKey(MEDIA_NEXT);
        s.fillRect(130, 40, 100, 50, THEME_COLOR); // Visual feedback
        toneLite(1500, 20);
      }

      // Volume Up (Hold A)
      if (M5.BtnA.pressedFor(300) && millis() - lastVolUp > 150) {
        sendMediaKey(MEDIA_VOL_UP);
        lastVolUp = millis();
      }

      // Volume Down (Hold B)
      if (M5.BtnB.pressedFor(300) && millis() - lastVolDown > 150) {
        sendMediaKey(MEDIA_VOL_DOWN);
        lastVolDown = millis();
      }
    }

    s.pushSprite(0, 0);
    delay(20);
  }

  stopBleHid();
  s.deleteSprite();
}