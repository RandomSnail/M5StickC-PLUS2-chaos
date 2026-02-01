#include "sys_core.h"
#include "apps.h"
#include <WiFi.h>
#include <M5Unified.h>

// --- CONFIGURATION ---
enum ClockStyle { STYLE_BOLD, STYLE_CYBER, STYLE_BINARY };
const char* wd[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

// --- HELPERS ---

void drawBatteryWidget(M5Canvas& canvas, int x, int y, uint16_t color) {
  int bat = getBatteryPercent();
  
  // Draw battery body
  canvas.drawRect(x, y, 24, 10, color);
  canvas.fillRect(x + 24, y + 2, 2, 6, color);
  
  // Fill level
  int w = map(bat, 0, 100, 0, 20);
  if (w > 0) {
      uint16_t fill = (bat > 20) ? color : RED;
      canvas.fillRect(x + 2, y + 2, w, 6, fill);
  }
}

// Helper to draw a "LED" for the binary clock
void drawLed(M5Canvas& s, int x, int y, bool on) {
    if (on) {
        s.fillRoundRect(x, y, 14, 10, 2, THEME_COLOR);
        // Add a "shine" pixel for polish
        s.drawPixel(x+2, y+2, WHITE); 
    } else {
        s.drawRoundRect(x, y, 14, 10, 2, DARK_GREY);
        s.fillRect(x+2, y+2, 10, 6, BLACK);
    }
}

// --- MAIN LOOP ---

void clock_loop() {
  hardResetRadios();
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  
  int style = STYLE_BOLD;
  
  m5::rtc_time_t t;
  m5::rtc_date_t d;

  // --- INITIAL CHECK ---
  M5.Rtc.getTime(&t);
  if (t.hours == 0 && t.minutes == 0 && t.seconds < 10) {
      // Optional: Auto-sync could go here
  }

  while (true) {
    M5.update();

    // --- 1. POWER CHECK ---
    if (checkPowerManagement()) {
        // If true, we just woke up. Clear buttons and restart loop so we don't accidentally exit.
        M5.BtnA.wasPressed(); 
        M5.BtnB.wasPressed();
        continue; 
    }
    
    // --- 2. SLEEP CHECK ---
    if (is_screen_off) {
        delay(100); // Low power wait
        continue;   // Don't draw anything
    }
    
    // INPUTS
    // EXIT LOGIC
    if (M5.BtnA.wasPressed()) {
      toneLite(1000, 50);
      break; // Break the loop
    }
    if (M5.BtnB.wasPressed()) { style = (style + 1) % 3; toneLite(1200, 30); }

    // GET TIME
    M5.Rtc.getTime(&t);
    M5.Rtc.getDate(&d);

    s.fillScreen(BLACK);

    // ============================================================
    // FACE 1: BOLD DIGITAL (Modern, Clean, Smartwatch Style)
    // ============================================================
    if (style == STYLE_BOLD) {
        // 1. Time (Huge)
        s.setTextColor(WHITE, BLACK);
        s.setTextSize(4); // Max size
        int timeWidth = 115; // Approximate width of "00:00"
        s.setCursor(20, 45); 
        s.printf("%02d:%02d", t.hours, t.minutes);

        // 2. Seconds (Small, next to time)
        s.setTextSize(2);
        s.setTextColor(GREY, BLACK);
        s.setCursor(150, 60);
        s.printf("%02d", t.seconds);

        // 3. Date Bar (Top)
        s.setTextSize(1);
        s.setTextColor(THEME_COLOR, BLACK);
        s.setCursor(10, 10);
        // Weekday + Date
        s.printf("%s %02d/%02d", wd[d.weekDay % 7], d.month, d.date);

        // 4. Battery (Top Right)
        drawBatteryWidget(s, 205, 8, WHITE);

        // 5. Bottom Decorative Line & Info
        s.fillRect(20, 95, 200, 2, THEME_COLOR);
        s.setTextColor(GREY, BLACK);
        s.drawCenterString("M5 CHAOS DECK", 120, 110);
    }

    // ============================================================
    // FACE 2: CYBER TERMINAL (Hacker, Glitch, Scanlines)
    // ============================================================
    else if (style == STYLE_CYBER) {
        // 1. Outer Frame
        s.drawRect(0, 0, 240, 135, THEME_COLOR);
        s.drawRect(2, 2, 236, 131, BLACK); // Inner cut
        
        // 2. Header Status
        s.fillRect(0, 0, 240, 18, THEME_COLOR);
        s.setTextColor(BLACK, THEME_COLOR);
        s.setTextSize(1);
        s.setCursor(5, 5);
        s.print("SYSTEM: ONLINE");
        
        // Battery in header
        int bat = getBatteryPercent();
        s.setCursor(190, 5);
        s.printf("BAT:%d%%", bat);

        // 3. Main Time (Monospace look)
        s.setTextColor(WHITE, BLACK);
        s.setTextSize(3);
        s.setCursor(45, 50);
        s.printf("%02d:%02d", t.hours, t.minutes);
        
        // Blinking cursor block
        if (millis() % 1000 < 500) {
            s.fillRect(140, 50, 15, 22, THEME_COLOR);
        }

        // 4. "Network" Stats (Decorations)
        s.setTextSize(1);
        s.setTextColor(THEME_COLOR);
        s.setCursor(10, 115);
        s.printf("UP: %02d:%02d", millis()/3600000, (millis()/60000)%60);
        
        s.setCursor(140, 115);
        s.printf("MEM: %dKB", ESP.getFreeHeap()/1024);

        // 5. Fake Scanlines (Draws horizontal lines every 4 pixels)
        for (int i=20; i<135; i+=4) {
            s.drawFastHLine(2, i, 236, 0); // 0 = BLACK (erases pixels to create gap)
        }
    }

    // ============================================================
    // FACE 3: BINARY PRO (Tech, Grid, Labeled)
    // ============================================================
    else if (style == STYLE_BINARY) {
        // Grid setup
        int startX = 60;
        int startY = 25;
        int gapX = 30; // Spacing between columns (HH MM SS)
        int gapY = 18; // Spacing between rows (8 4 2 1)
        
        // 1. Draw Labels (8, 4, 2, 1) on the left
        s.setTextColor(GREY);
        s.setTextSize(1);
        s.setCursor(5, startY);      s.print("8");
        s.setCursor(5, startY+gapY);   s.print("4");
        s.setCursor(5, startY+gapY*2); s.print("2");
        s.setCursor(5, startY+gapY*3); s.print("1");

        // 2. Draw Column Labels (H M S) at bottom
        s.setTextColor(THEME_COLOR);
        s.drawCenterString("H", startX + 7, 105);
        s.drawCenterString("M", startX + gapX*2 + 7, 105);
        s.drawCenterString("S", startX + gapX*4 + 7, 105);

        // Helper Lambda for columns
        auto drawColumn = [&](int val, int colIndex) {
            int x = startX + (colIndex * 18) + (colIndex/2 * 12); // Spacing logic
            // Check bits 3 down to 0 (8, 4, 2, 1)
            drawLed(s, x, startY,          (val & 8));
            drawLed(s, x, startY + gapY,   (val & 4));
            drawLed(s, x, startY + gapY*2, (val & 2));
            drawLed(s, x, startY + gapY*3, (val & 1));
        };

        // 3. Draw the 6 columns
        drawColumn(t.hours / 10, 0);
        drawColumn(t.hours % 10, 1);
        drawColumn(t.minutes / 10, 2);
        drawColumn(t.minutes % 10, 3);
        drawColumn(t.seconds / 10, 4);
        drawColumn(t.seconds % 10, 5);
        
        // 4. Battery Widget (Bottom Right)
        drawBatteryWidget(s, 210, 120, DARK_GREY);
    }

    s.pushSprite(0, 0);
    delay(100); // 10fps is enough for a clock
  }
  s.deleteSprite();

while (M5.BtnA.isPressed()) {
    M5.update();
    delay(10);
  }
}