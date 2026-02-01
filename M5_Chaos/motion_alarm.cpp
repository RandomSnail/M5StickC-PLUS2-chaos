#include "sys_core.h"
#include "apps.h"
#include <M5Unified.h>

void motion_alarm_loop() {
  hardResetRadios();
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  // 1. ARMING SEQUENCE
  for (int i = 5; i > 0; i--) {
    s.fillScreen(BLACK);
    s.setTextColor(ORANGE);
    s.setTextSize(2);
    s.drawCenterString("ARMING...", 120, 40);
    
    s.setTextSize(4);
    s.setTextColor(WHITE);
    s.setCursor(110, 70);
    s.print(i);
    
    s.pushSprite(0, 0);
    toneLite(1000, 100);
    delay(1000);
  }

  // 2. BASELINE READING
  float startX, startY, startZ;
  M5.Imu.getAccel(&startX, &startY, &startZ);
  
  bool triggered = false;
  
  s.fillScreen(BLACK);
  s.setTextColor(GREEN);
  s.setTextSize(2);
  s.drawCenterString("ARMED", 120, 50);
  s.setTextSize(1);
  s.setTextColor(GREY);
  s.drawCenterString("(Don't move me)", 120, 80);
  s.pushSprite(0, 0);
  
  // Clean start tone
  toneLite(2000, 200);

  // 3. MONITOR LOOP
  while (true) {
    M5.update();
    
    // Hold B to Disarm/Exit
    if (M5.BtnB.wasPressed()) {
      toneLite(1000, 50);
      break; 
    }

    if (!triggered) {
      // Check for movement
      float x, y, z;
      M5.Imu.getAccel(&x, &y, &z);
      
      // Calculate delta (sensitivity = 0.15)
      float delta = abs(x - startX) + abs(y - startY) + abs(z - startZ);
      
      if (delta > 0.15) { 
        triggered = true;
      }
      delay(100);
    } 
    else {
      // --- ALARM STATE ---
      // 1. Flash Screen
      s.fillScreen(RED);
      s.setTextColor(WHITE);
      s.setTextSize(3);
      s.drawCenterString("THEFT!", 120, 50);
      s.pushSprite(0, 0);
      
      // 2. Scream
      M5.Speaker.tone(3000, 200); 
      delay(100);
      M5.Speaker.tone(4000, 200);
      
      s.fillScreen(BLACK); // Strobe effect
      s.pushSprite(0, 0);
      delay(100);
    }
  }
  
  s.deleteSprite();
}