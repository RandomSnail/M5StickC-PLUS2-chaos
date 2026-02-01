#include "menu_logic.h"
#include "menu_draw.h"
#include "sys_core.h"
#include "app_registry.h"
#include <M5Unified.h>
#include "app_defs.h"
#include "apps.h"

// Variables
AppFolder currentFolder = FOLDER_ROOT;
int menuCursor = 0;
int filtered_count = 0;
int filtered_indexes[20]; 

bool needsRedraw = true;

// Helper to update the list of apps when opening a folder
void rebuild_filter() {
  filtered_count = 0;
  menuCursor = 0;

  if (currentFolder != FOLDER_ROOT) {
    for (int i = 0; i < app_count; i++) {
      if (app_registry[i].folder == currentFolder) {
        filtered_indexes[filtered_count] = i;
        filtered_count++;
        if (filtered_count >= 20) break;
      }
    }
  }
}

void menu_init() {
  currentFolder = FOLDER_ROOT;
  menuCursor = 0;
  // Clear screen completely to remove artifacts from previous apps
  M5.Display.fillScreen(BLACK); 
  rebuild_filter();
  needsRedraw = true;
}

void menu_tick() {
  // 1. GLOBAL SHORTCUT: Hold A to return to Clock
  if (M5.BtnA.pressedFor(1000) && currentFolder == FOLDER_ROOT) {
    toneLite(1500, 50);
    clock_loop();
    
    // RESET MENU STATE ON RETURN
    menu_init(); 
    while(M5.BtnA.isPressed()) M5.update(); // Wait for release
    return;
  }

  // 2. BACK BUTTON (Hold B)
  if (M5.BtnB.pressedFor(600)) { 
    toneLite(800, 50); 
    
    if (currentFolder != FOLDER_ROOT) {
      currentFolder = FOLDER_ROOT;
      menuCursor = 0;
      needsRedraw = true;
    } else {
      clock_loop();
      menu_init();
    }
    
    while(M5.BtnB.isPressed()) M5.update(); 
    return; 
  }
  
  // SCROLL UP (Button C / Power Button)
  if (M5.BtnPWR.wasPressed()) {
    toneLite(1000, 20); // Higher pitch for "Up"
    
    int max_items = (currentFolder == FOLDER_ROOT) ? 5 : filtered_count;
    
    // Decrement cursor
    menuCursor--;
    
    // Wrap around to bottom if we go past 0
    if (menuCursor < 0) {
        if (max_items > 0) menuCursor = max_items - 1;
        else menuCursor = 0;
    }
    
    needsRedraw = true;
  }

  // SCROLL (Click B)
  if (M5.BtnB.wasPressed()) {
    toneLite(600, 20);
    menuCursor++;
    
    int max_items = (currentFolder == FOLDER_ROOT) ? 5 : filtered_count;
    if (max_items == 0) menuCursor = 0;
    else if (menuCursor >= max_items) menuCursor = 0;
    
    needsRedraw = true;
  }

  // SELECT (Click A)
  if (M5.BtnA.wasPressed()) {
    toneLite(1200, 50);
    
    if (currentFolder == FOLDER_ROOT) {
      // Enter Folder (0=TOOLS, 1=WIRELESS, 2=GAMES, 3=PRANKS, 4=SYSTEM)
      if (menuCursor == 0) currentFolder = FOLDER_TOOLS;
      else if (menuCursor == 1) currentFolder = FOLDER_WIRELESS;
      else if (menuCursor == 2) currentFolder = FOLDER_GAMES;
      else if (menuCursor == 3) currentFolder = FOLDER_PRANKS;
      else if (menuCursor == 4) currentFolder = FOLDER_SYSTEM;
      
      rebuild_filter();
      needsRedraw = true;
    } else {
      // Launch App
      if (filtered_count > 0) {
        int appIndex = filtered_indexes[menuCursor];
        if (app_registry[appIndex].loop_func) {
            // Clear screen before launching app
            M5.Display.fillScreen(BLACK); 
            
            app_registry[appIndex].loop_func(); 
            
            M5.Display.fillScreen(BLACK);
            rebuild_filter(); 
            needsRedraw = true;
            while(M5.BtnB.isPressed()) M5.update();
            return;
        }
      }
    }
  }

  // Only draw if something changed
  if (needsRedraw) {
    drawMenu();
    needsRedraw = false;
  }
}