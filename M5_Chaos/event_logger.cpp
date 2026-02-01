#include "sys_core.h"
#include "apps.h"
#include "menu_draw.h"
// Log event to Preferences
void log_event(const char* category, const char* event) {
  prefs.begin("logs", false);
  
  int count = prefs.getInt("count", 0);
  char key[16];
  
  // Store event with timestamp
  snprintf(key, 16, "evt_%d", count % 10);  // Keep last 10 events
  
  // Format: [category] event
  char logEntry[64];
  snprintf(logEntry, 64, "[%s] %s", category, event);
  
  prefs.putString(key, logEntry);
  prefs.putInt("count", count + 1);
  prefs.end();
}

// Clear all logs
void clear_logs() {
  prefs.begin("logs", false);
  prefs.clear();
  prefs.end();
  toneLite(1500, 100);
}

// View logs
void view_logs_loop() {
  M5Canvas s(&M5.Display);
  if (!s.createSprite(240, 135)) return; // Safety check
  
  int scroll = 0;
  
  while (true) {
    M5.update();
    if (checkExit()) break;
    
    prefs.begin("logs", true); // true = Read Only mode for viewing
    int totalCount = prefs.getInt("count", 0);
    int availableLogs = (totalCount > 10) ? 10 : totalCount;

    // Scroll
    int maxScroll = (availableLogs > 5) ? (availableLogs - 5) : 0;

    if (M5.BtnB.wasPressed()) {
      scroll++;
      if (scroll > maxScroll) scroll = 0; // Wrap back to top
      toneLite(1000, 20);
    }
    
    // Clear logs (hold A for 2 seconds)
    if (M5.BtnA.pressedFor(2000)) {
      s.fillScreen(BLACK);
      drawHeaderSprite(s,"EVENT LOGS");
      s.setTextColor(RED);
      s.drawCenterString("Clearing...", 120, 60);
      s.pushSprite(0, 0);
      
      prefs.end();
      clear_logs();
      
      scroll = 0;
      totalCount = 0;

      s.fillScreen(BLACK);
      drawHeaderSprite(s,"EVENT LOGS");
      s.setTextColor(GREEN);
      s.drawCenterString("Logs Cleared!", 120, 60);
      s.pushSprite(0, 0);
      delay(1000);
      
      while(M5.BtnA.isPressed()) M5.update();
      continue;
    }
    
    s.fillScreen(BLACK);
    drawHeaderSprite(s,"EVENT LOGS");
    
    prefs.begin("logs", true);
    
   if (totalCount == 0) {
      s.setTextColor(GREY);
      s.drawCenterString("No events logged", 120, 60);
    } else {
      s.setTextColor(GREY);
      s.setTextSize(1);
      s.setCursor(5, 28);
      s.printf("Total: %d (Showing Last %d)", totalCount, availableLogs);
      
    // We loop 5 times to fill the screen rows
      for (int i = 0; i < 5; i++) {
        
        // Logical Index: 0 is the newest, 1 is second newest...
        int viewIndex = scroll + i;
        
        if (viewIndex >= availableLogs) break; // Stop if we run out of logs

        // Math to find the specific "evt_X" key for this log
        // If total is 12, newest is at index 11 (evt_1). 
        // We want (12 - 1 - viewIndex)
        int realIndex = (totalCount - 1 - viewIndex) % 10;
        
        // Handle negative modulo result (just in case)
        if (realIndex < 0) realIndex += 10;

        char key[16];
        snprintf(key, 16, "evt_%d", realIndex);
        
        String entry = prefs.getString(key, "Err");
        
        int y = 45 + (i * 16);
        
        // Highlight first row only if we are at top of list? 
        // Or just use color to distinguish rows. 
        // Let's keep your highlight logic (top row is highlighted)
        if (i == 0) {
           s.fillRect(0, y - 2, 240, 15, DARK_GREY);
           s.setTextColor(THEME_COLOR);
        } else {
           s.setTextColor(WHITE);
        }
        
        s.setCursor(5, y);
        if (entry.length() > 32) entry = entry.substring(0, 29) + "...";
        s.print(entry);
      }
    }
    
    prefs.end();
    
    s.setTextColor(GREY);
    s.drawCenterString("Hold A: Clear   B: Scroll", 120, 125);
    
    s.pushSprite(0, 0);
    delay(50);
  }
  
  s.deleteSprite();
}