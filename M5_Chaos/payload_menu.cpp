#include "sys_core.h"
#include "apps.h"
#include "ble_core.h"
#include "payload_library.h"

void payload_menu_loop() {
  if (!startBleHid("M5-Payload")) {
    Serial.println("[ERROR] Failed to start BLE HID");
    return;
  }
  
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  
  int cursor = 0;
  bool connected_once = false;
  
  while (true) {
    M5.update();
    if (checkExit()) break;
    
    // --- FAILSAFE CHECK ---
    if (NimBLEDevice::getServer() && NimBLEDevice::getServer()->getConnectedCount() > 0) {
       bleConnected = true;
    } else {
       bleConnected = false;
    }
    // ---------------------

    s.fillScreen(BLACK);
    
    if (!bleConnected) {
      // --- PAIRING SCREEN ---
      s.fillRect(0, 0, 240, 28, ORANGE);
      s.setTextColor(BLACK, ORANGE);
      s.setTextSize(1);
      s.drawString("PAYLOAD LIBRARY", 5, 7);
      
      s.setTextColor(ORANGE, BLACK);
      s.setTextSize(2);
      s.drawCenterString(connected_once ? "RECONNECTING..." : "PAIRING MODE", 120, 50);
      
      s.setTextColor(WHITE, BLACK);
      s.setTextSize(1);
      s.drawCenterString("Connect to: M5-Payload", 120, 85);
      
    } else {
      connected_once = true;
      
      // --- LIBRARY SCREEN ---
      s.fillRect(0, 0, 240, 28, THEME_COLOR);
      s.setTextColor(BLACK, THEME_COLOR);
      s.setTextSize(1);
      s.drawString("PAYLOAD LIBRARY", 5, 7);
      
      // Navigation
      if (M5.BtnB.wasPressed() && !M5.BtnB.pressedFor(800)) {
        cursor = (cursor + 1) % payloadCount;
        toneLite(1000, 20);
      }
      
      // Execute Payload
      if (M5.BtnA.wasPressed()) {
        s.fillScreen(BLACK);
        s.fillRect(0, 0, 240, 28, GREEN);
        s.setTextColor(BLACK, GREEN);
        s.drawString("EXECUTING...", 5, 7);
        
        s.setTextColor(WHITE, BLACK);
        s.setTextSize(2);
        s.drawCenterString(payloads[cursor].name, 120, 60);
        s.pushSprite(0, 0);
        
        toneLite(2000, 100);
        delay(500);
        
        // Run the payload
        payloads[cursor].execute();
        
        delay(500);
      }
      
      // Draw List
      for (int i = 0; i < 4 && (cursor + i) < payloadCount; i++) {
        int idx = cursor + i;
        int y = 35 + (i * 23);
        
        if (i == 0) {
          s.fillRect(0, y - 2, 240, 20, DARK_GREY);
          s.setTextColor(THEME_COLOR, DARK_GREY);
          s.drawString(">", 5, y);
        } else {
          s.setTextColor(WHITE, BLACK);
        }
        
        s.setTextSize(1);
        s.setCursor(15, y);
        s.print(payloads[idx].name);
        
        // Small description
        s.setTextColor(GREY, (i==0 ? DARK_GREY : BLACK));
        s.setCursor(15, y + 10);
        s.setTextSize(1); // Ensure small font
        // s.print(payloads[idx].description); // Uncomment if struct has desc
      }
      
      s.setTextColor(GREY, BLACK);
      s.drawCenterString("A: Execute  B: Next", 120, 125);
    }
    
    s.pushSprite(0, 0);
    delay(50);
  }
  
  stopBleHid();
  s.deleteSprite();
}