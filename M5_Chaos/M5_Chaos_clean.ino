#include <M5Unified.h>
#include "sys_core.h"
#include "menu_logic.h"
#include "icon.h"
#include "apps.h" 

// =====================================================
// GLOBAL VARIABLE DEFINITIONS
// =====================================================
// 1. WE REMOVED: prefs, soundEnabled, screen_brightness, THEME_COLOR
//    (Because they are already defined in sys_core.cpp)

// 2. WE KEEP: These because sys_core.h declares them 'extern', 
//    but sys_core.cpp didn't create them. We create them here.
NimBLEAdvertising* pAdvertising = nullptr;
int dino_high_score = 0;
PortalEntry portal_logs[5]; 
int portal_log_count = 0;
int16_t mic_data[256];

void setup() {
  // -------------------------------------------------
  // SYSTEM INITIALIZATION
  // -------------------------------------------------
  // This function (from sys_core.cpp) handles M5.begin, 
  // loads Settings, Theme, and WiFi credentials.
  sys_init(); 
  
  Serial.begin(115200);
  Serial.println("\n=============================");
  Serial.println("  M5 CHAOS OS v1.0");
  Serial.println("=============================");
  
  // Debug print (using the global variables from sys_core)
  Serial.printf("[CONFIG] Sound: %s\n", soundEnabled ? "ON" : "OFF");
  Serial.printf("[CONFIG] Brightness: %d\n", screen_brightness);
  Serial.printf("[CONFIG] Theme: 0x%04X\n", THEME_COLOR);

  // Initialize UI Sprites (from icon.cpp / menu_draw.cpp)
  createAllSprites();
  M5.Display.setRotation(3);
  
  // Initialize Menu Logic
  menu_init();

  Serial.printf("[MEM] Free heap: %d KB\n", ESP.getFreeHeap() / 1024);

  // Boot Sound
  toneLite(2000, 100);
  delay(100);
  toneLite(3000, 100);
  
  // Jump straight to Clock
  clock_loop(); 
}

void loop() {
  M5.update();
  menu_tick(); // Handle menu navigation
  
  // -------------------------------------------------
  // BACKGROUND TASKS
  // -------------------------------------------------
  
  // Memory Leak Check (Every 30s)
  static unsigned long lastMemCheck = 0;
  if (millis() - lastMemCheck > 30000) {
    int freeKB = ESP.getFreeHeap() / 1024;
    if (freeKB < 30) {
      Serial.printf("[WARN] Low memory: %d KB\n", freeKB);
    }
    lastMemCheck = millis();
  }

  // Low Battery Warning (Every 60s)
  static unsigned long lastBatCheck = 0;
  if (millis() - lastBatCheck > 60000) {
    int bat = getBatteryPercent();
    if (bat < 15 && bat > 0) {
      toneLite(500, 100);
      delay(100);
      toneLite(500, 100); 
    }
    lastBatCheck = millis();
  }
  
  delay(10);
}