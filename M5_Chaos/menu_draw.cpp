#include "menu_draw.h"
#include "menu_logic.h"
#include "app_registry.h"
#include "sys_core.h"
#include <M5Unified.h>
#include "app_defs.h"

// --- NEW HELPER FUNCTION FOR APPS (PREVENTS FLICKER) ---
void drawHeaderSprite(M5Canvas &s, const char* title) {
  // Draw the background bar (Using THEME_COLOR for apps looks nice)
  s.fillRect(0, 0, 240, 25, THEME_COLOR); 
  
  // Draw the Title Text
  s.setTextColor(WHITE);
  s.setTextSize(1);
  s.setTextDatum(middle_center);          // Center text alignment
  s.drawString(title, 120, 12);           // Draw at center of the bar
  
  // Reset alignment so subsequent text draws normally (top-left)
  s.setTextDatum(top_left);               
}

void drawMenu() {
  M5.Display.fillScreen(BLACK);
  
  // 1. WINDOW FRAME
  M5.Display.drawRect(0, 0, 240, 135, DARK_GREY);
  
  // 2. HEADER
  M5.Display.fillRect(1, 1, 238, 22, DARK_GREY);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE, DARK_GREY);
  M5.Display.setCursor(6, 6);
  if (currentFolder == FOLDER_ROOT) M5.Display.print("MAIN MENU");
  else M5.Display.print(folder_names[currentFolder]);

  // Battery (Simple Text to save space/clean look)
  int bat = M5.Power.getBatteryLevel();
  uint16_t batColor = (bat > 20) ? GREEN : RED;
  M5.Display.setTextColor(batColor, DARK_GREY);
  M5.Display.setCursor(205, 6);
  M5.Display.printf("%d%%", bat);

  // 3. LIST LOGIC
  M5.Display.setTextSize(2);
  int totalItems = (currentFolder == FOLDER_ROOT) ? 5 : filtered_count; // 5 Root folders now
  const int ITEMS_PER_PAGE = 4;
  
  int topIndex = 0;
  if (menuCursor >= ITEMS_PER_PAGE) {
      topIndex = menuCursor - (ITEMS_PER_PAGE - 1);
  }

  // Draw Items
  const char* rootItems[] = { "TOOLS", "WIRELESS", "GAMES", "PRANKS", "SYSTEM" }; // Added WIRELESS

  for (int i = 0; i < ITEMS_PER_PAGE; i++) {
    int itemIndex = topIndex + i;
    if (itemIndex >= totalItems) break;

    int y = 35 + (i * 22);

    // Cursor Selection Box
    if (itemIndex == menuCursor) {
      M5.Display.fillRect(4, y - 2, 220, 20, THEME_COLOR);
      M5.Display.setTextColor(BLACK, THEME_COLOR);
      M5.Display.setCursor(10, y);
      M5.Display.print("> ");
    } else {
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.setCursor(10, y);
      M5.Display.print("  ");
    }

    // Item Name
    if (currentFolder == FOLDER_ROOT) {
      if (itemIndex < 5) M5.Display.print(rootItems[itemIndex]);
    } else {
      int realIndex = filtered_indexes[itemIndex];
      M5.Display.print(app_registry[realIndex].name);
    }
  }

  // 4. SCROLLBAR (Right Side)
  if (totalItems > ITEMS_PER_PAGE) {
    int barHeight = 90; // Available vertical space
    int sliderHeight = max(10, barHeight / totalItems);
    int sliderY = 35 + ((menuCursor * (barHeight - sliderHeight)) / (totalItems - 1));
    
    // Draw Track
    M5.Display.drawFastVLine(234, 35, barHeight, DARK_GREY);
    // Draw Slider
    M5.Display.fillRect(233, sliderY, 3, sliderHeight, THEME_COLOR);
  }

  // 5. STATUS BAR (Bottom)
  M5.Display.fillRect(1, 120, 238, 14, BLACK);
  M5.Display.drawLine(0, 120, 240, 120, DARK_GREY);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(GREY, BLACK);
  M5.Display.drawCenterString("A:OK   B:DN   C:UP", 120, 124);
}