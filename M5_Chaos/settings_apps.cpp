#include "sys_core.h"
#include "apps.h"
#include <WiFi.h>

// --- SYNC TIME ---
void app_sync_time() {
  hardResetRadios();
  
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  s.fillScreen(BLACK);
  
  // Manually draw header to avoid missing "menu_draw.h"
  s.fillRect(0, 0, 240, 25, DARK_GREY);
  s.setTextColor(WHITE, DARK_GREY);
  s.setTextSize(1);
  s.drawString("SYNCING TIME...", 5, 5);
  s.pushSprite(0, 0);

  // Connect
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int t = 0;
  while (WiFi.status() != WL_CONNECTED && t < 20) {
    delay(500);
    s.fillCircle(120 + (t * 5) - 50, 70, 2, WHITE); // Simple loading animation
    s.pushSprite(0, 0);
    t++;
  }

  s.fillScreen(BLACK);
  s.fillRect(0, 0, 240, 25, (WiFi.status() == WL_CONNECTED) ? GREEN : RED);
  s.setTextColor(BLACK);
  
  if (WiFi.status() == WL_CONNECTED) {
    s.drawCenterString("CONNECTED", 120, 5);
    
    configTime(0, 3600, "pool.ntp.org");
    struct tm ti;
    if (getLocalTime(&ti)) {
      auto dt = M5.Rtc.getDateTime();
      dt.time.hours = ti.tm_hour;
      dt.time.minutes = ti.tm_min;
      dt.time.seconds = ti.tm_sec;
      dt.date.year = ti.tm_year + 1900;
      dt.date.month = ti.tm_mon + 1;
      dt.date.date = ti.tm_mday;
      M5.Rtc.setDateTime(dt);
      
      s.setTextColor(WHITE, BLACK);
      s.drawCenterString("Time Updated!", 120, 60);
    }
  } else {
    s.drawCenterString("FAILED", 120, 5);
    s.setTextColor(WHITE, BLACK);
    s.drawCenterString("Check WiFi Settings", 120, 60);
  }
  
  s.pushSprite(0, 0);
  delay(2000); 
  s.deleteSprite(); 
  
  WiFi.disconnect(true);
  hardResetRadios();
}

// --- BRIGHTNESS SETTING ---
void brightness_loop() {
  int b = screen_brightness; // Local copy to modify
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  while (true) {
    M5.update();
    
    // Cycle Brightness
    if (M5.BtnA.wasPressed()) {
      b += 40;
      if (b > 255) b = 10;  
      M5.Display.setBrightness(b);
    }
    
    // Save & Exit
    if (M5.BtnB.wasPressed()) {
      screen_brightness = (uint8_t)b; // Update global variable
      
      // FIXED: Use "system" namespace to match sys_core.cpp
      prefs.begin("system", false); 
      // FIXED: Use putUChar to match uint8_t
      prefs.putUChar("bright", screen_brightness);
      prefs.end();
      break;
    }

    s.fillScreen(BLACK);
    s.fillRect(0, 0, 240, 28, DARK_GREY);
    s.setTextColor(WHITE, DARK_GREY);
    s.drawString("BRIGHTNESS", 5, 7);

    // Draw Bar
    int width = map(b, 0, 255, 0, 200);
    s.drawRect(20, 60, 200, 20, WHITE);
    s.fillRect(22, 62, width - 4, 16, THEME_COLOR);

    s.setTextColor(WHITE, BLACK);
    s.setCursor(100, 90);
    s.printf("%d%%", map(b, 0, 255, 0, 100));

    s.setTextColor(GREY, BLACK);
    s.drawCenterString("A: Change   B: Save", 120, 120);
    s.pushSprite(0, 0);
    delay(50);
  }
  s.deleteSprite();
}

// --- TIMEOUT SETTING ---
void timeout_loop() {
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  
  // Reset timer so we don't sleep while in the menu
  last_interaction = millis();

  while (true) {
    M5.update();
    
    // Cycle Timeout
    if (M5.BtnA.wasPressed()) {
      timeout_index = (timeout_index + 1) % 6;
      last_interaction = millis(); // Keep screen awake
    }
    
    // Save & Exit
    if (M5.BtnB.wasPressed()) {
      // FIXED: Use "system" namespace
      prefs.begin("system", false);
      prefs.putInt("timeout", timeout_index);
      prefs.end();
      break;
    }

    s.fillScreen(BLACK);
    s.fillRect(0, 0, 240, 28, DARK_GREY);
    s.setTextColor(WHITE, DARK_GREY);
    s.drawString("SCREEN TIMEOUT", 5, 7);

    // Dynamic Color for "Never" (Red warning)
    if (timeout_index == 5) s.setTextColor(RED, BLACK);
    else s.setTextColor(WHITE, BLACK);

    s.setTextSize(3);
    s.drawCenterString(timeout_names[timeout_index], 120, 60);

    s.setTextSize(1);
    s.setTextColor(GREY, BLACK);
    s.drawCenterString("A: Change   B: Save", 120, 120);
    
    s.pushSprite(0, 0);
    delay(50);
  }
  s.deleteSprite();
}

// --- SOUND TOGGLE ---
void toggle_sound() {
    soundEnabled = !soundEnabled; 
    
    // Save preference
    prefs.begin("system", false);
    prefs.putBool("sound", soundEnabled);
    prefs.end();
    
    M5.Speaker.setVolume(soundEnabled ? 128 : 0);
    if(soundEnabled) toneLite(1000, 100);

    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(soundEnabled ? GREEN : RED, BLACK);
    M5.Display.drawCenterString(soundEnabled ? "SOUND ON" : "SOUND OFF", 120, 60);
    delay(1000);
}

// --- SLEEP ---
void sleep_device() {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE);
    M5.Display.drawCenterString("SLEEPING...", 120, 60);
    delay(500);
    M5.Display.setBrightness(0);
    M5.Display.sleep();
    
    // We only reach here if we wake up
    // But usually M5.Display.sleep just turns off screen.
    // To actually power down:
    M5.Power.powerOff(); 
}