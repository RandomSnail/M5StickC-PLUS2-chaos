#include "sys_core.h"
#include "apps.h"
#include <esp_system.h>

void device_info_loop() {
  M5Canvas s(&M5.Display);
  if (!s.createSprite(240, 135)) {
    Serial.println("Sprite allocation failed!");
    return; 
  }
  int page = 0;
  const int maxPages = 2;
  
  while (true) {
    M5.update();
    if (checkExit()) break;
    
    // Switch pages
    if (M5.BtnA.wasPressed()) {
      page = (page + 1) % maxPages;
      toneLite(1200, 30);
    }
    
    s.fillScreen(BLACK);
    // --- FIX: Draw Header MANUALLY on the Sprite ---
    // Instead of calling drawHeader(), we draw the rectangle and text on 's'
    s.fillRect(0, 0, 240, 25, THEME_COLOR); // Blue header bar
    s.setTextColor(WHITE);
    s.setTextDatum(middle_center);          // Center alignment
    s.drawString("DEVICE INFO", 120, 12);   // Draw title in center of bar
    s.setTextDatum(top_left);               // Reset alignment
    // -----------------------------------------------
    
    s.setTextColor(WHITE);
    s.setTextSize(1);
    
    if (page == 0) {
      // Page 1: Hardware & Memory
      s.setTextColor(THEME_COLOR);
      s.setCursor(10, 35);
      s.print("HARDWARE");
      
      s.setTextColor(WHITE);
      s.setCursor(10, 50);
      s.printf("Chip: %s", ESP.getChipModel());
      
      s.setCursor(10, 65);
      s.printf("Cores: %d", ESP.getChipCores());
      
      s.setCursor(10, 80);
      s.printf("CPU: %d MHz", ESP.getCpuFreqMHz());
      
      s.setTextColor(THEME_COLOR);
      s.setCursor(120, 35);
      s.print("MEMORY");
      
      s.setTextColor(WHITE);
      s.setCursor(10, 115);
      uint32_t freeHeap = ESP.getFreeHeap();
      uint32_t totalHeap = ESP.getHeapSize();
      s.printf("RAM: %d/%d KB", freeHeap / 1024, totalHeap / 1024);
      
    } else if (page == 1) {
      // Page 2: Storage & Power
      s.setTextColor(THEME_COLOR);
      s.setCursor(10, 35);
      s.print("STORAGE");
      
      s.setTextColor(WHITE);
      s.setCursor(10, 50);
      s.printf("Flash: %d MB", ESP.getFlashChipSize() / (1024*1024));
      
      s.setCursor(10, 65);
      // Speed is usually in Hz, divide by 1M for MHz
      s.printf("Speed: %d MHz", ESP.getFlashChipSpeed() / 1000000);
      
      s.setTextColor(THEME_COLOR);
      s.setCursor(10, 90);
      s.print("POWER");
      
      s.setTextColor(WHITE);
      s.setCursor(10, 105);
      int bat = M5.Power.getBatteryLevel(); // Ensure you use the right function
      s.printf("Bat: %d%%", bat);
      
      // Battery bar
      s.drawRect(80, 105, 50, 8, WHITE);
      uint16_t batColor = (bat > 20) ? GREEN : RED;
      // Map 0-100 to 0-46 pixels width
      s.fillRect(82, 107, map(bat, 0, 100, 0, 46), 4, batColor);
    }
    
    // Page indicator
    s.setTextColor(LIGHTGREY);
    s.setTextDatum(bottom_center);
    s.drawString("BtnA: Next Page", 120, 132);
    s.setTextDatum(top_left); // Reset
    
    s.pushSprite(0, 0);
    delay(50);
  }
  
  // Clean up memory when exiting
  s.deleteSprite();
}
