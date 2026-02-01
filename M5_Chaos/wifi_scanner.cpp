#include "sys_core.h"
#include "apps.h"
#include <WiFi.h>
#include <M5Unified.h>
#include "menu_draw.h"

void wifi_scanner_loop() {
  hardResetRadios();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  
  int networks = 0;
  int scroll = 0;
  
  // SCAN STATE MACHINE
  // 0 = Idle (Showing results)
  // 1 = Scanning (Waiting for hardware)
  int scanState = 1; 
  
  // Start the first scan immediately in the BACKGROUND (true = async)
  WiFi.scanNetworks(true);

  while (true) {
    M5.update();
    if (checkExit()) break; // Uses your standard exit check

    // ----------------------------------------------------
    // 1. INPUT HANDLING
    // ----------------------------------------------------
    
    // A: RESCAN (Only if not already scanning)
    if (M5.BtnA.wasPressed() && scanState == 0) {
      toneLite(1500, 30);
      WiFi.scanNetworks(true); // Start Async Scan
      scanState = 1;
      scroll = 0;
    }
    
    // B: SCROLL DOWN
    if (M5.BtnB.wasPressed()) {
      if (networks > 0) {
        scroll++;
        if (scroll >= networks) scroll = 0; // Wrap to top
        toneLite(1000, 20);
      }
    }
    
    // C (POWER): SCROLL UP
    if (M5.BtnPWR.wasPressed()) {
      if (networks > 0) {
        scroll--;
        if (scroll < 0) scroll = networks - 1; // Wrap to bottom
        toneLite(1000, 20);
      }
    }

    // ----------------------------------------------------
    // 2. SCAN LOGIC
    // ----------------------------------------------------
    if (scanState == 1) {
        // Check if the background scan is finished
        int status = WiFi.scanComplete();
        if (status >= 0) {
            networks = status;
            scanState = 0; // Scan done, switch to Idle
        }
    }

    // ----------------------------------------------------
    // 3. DRAW UI
    // ----------------------------------------------------
    s.fillScreen(BLACK);
    drawHeaderSprite(s,"WIFI SCANNER"); // Uses your helper
    
    if (scanState == 1) {
      // === LOADING ANIMATION ===
      s.setTextColor(THEME_COLOR);
      s.setTextSize(2);
      s.drawCenterString("SCANNING...", 120, 55);
      
      // Moving progress bar
      int x = 120 + (sin(millis() / 150.0) * 80);
      s.fillRect(x - 10, 85, 20, 4, WHITE);
      
      // Secondary text
      s.setTextSize(1);
      s.setTextColor(GREY);
      s.drawCenterString("Please Wait", 120, 100);
      
    } else {
      // === RESULTS LIST ===
      if (networks == 0) {
        s.setTextColor(RED);
        s.setTextSize(2);
        s.drawCenterString("No Networks", 120, 60);
      } else {
        // Info Header
        s.setTextColor(GREY);
        s.setTextSize(1);
        s.setCursor(5, 28);
        s.printf("Found: %d  (Scroll: %d)", networks, scroll + 1);
        
        // List up to 4 items
        // Logic: Keep the 'scroll' index as the top item
        for (int i = 0; i < 4; i++) {
          int idx = scroll + i;
          
          // If we run off the end, wrap around visually (optional) or just stop
          // Let's just stop at the end for clarity
          if (idx >= networks) break; 
          
          int y = 45 + (i * 20);
          
          // Selection Box (First item is always "selected" visually in this view mode)
          if (i == 0) {
             s.fillRect(0, y - 2, 240, 18, DARK_GREY);
             s.setTextColor(THEME_COLOR);
          } else {
             s.setTextColor(WHITE);
          }

          // SSID
          s.setTextSize(1);
          s.setCursor(5, y);
          String ssid = WiFi.SSID(idx);
          if (ssid.length() > 16) ssid = ssid.substring(0, 16) + "..";
          s.print(ssid);
          
          // RSSI (Signal)
          int rssi = WiFi.RSSI(idx);
          s.setCursor(160, y);
          // Dynamic Color for Signal
          uint16_t sigColor = RED;
          if (rssi > -60) sigColor = GREEN;
          else if (rssi > -75) sigColor = ORANGE;
          
          if (i != 0) s.setTextColor(sigColor); // Keep theme color for selected item text
          s.printf("%d", rssi);
          
          // Lock Icon
          s.setCursor(200, y);
          s.setTextColor(i == 0 ? THEME_COLOR : WHITE);
          if (WiFi.encryptionType(idx) == WIFI_AUTH_OPEN) s.print("OPEN");
          else s.print("LOCK");
        }
      }
      
      // Footer controls
      s.setTextColor(GREY);
      s.setTextSize(1);
      s.drawCenterString("A: SCAN   B: DOWN   C: UP", 120, 125);
    }
    
    s.pushSprite(0, 0);
    // delay(20); // Removed: Use M5.update() timing naturally
  }
  
  WiFi.scanDelete();
  WiFi.mode(WIFI_OFF);
  hardResetRadios();
  s.deleteSprite();
}