
#pragma once
#include <Arduino.h>

enum PayloadCategory {
  CAT_FUN,
  CAT_PRANK,
  CAT_DEMO,
  CAT_UTILITY
};

struct BadUSBPayload {
  const char* name;
  const char* description;
  PayloadCategory category;
  void (*execute)();
};

// Forward declarations
void payload_chrome_dino();
void payload_notepad_hello();
void payload_rickroll();
void payload_matrix_cmd();
void payload_fake_update();
void payload_upside_down();
void payload_open_calculator();
void payload_youtube_search();
void payload_github();
void payload_type_message();
void payload_shutdown_prank();
void payload_fullscreen_video();
void payload_open_settings();
void payload_wifi_passwords();
void payload_sysinfo();

// Payload Registry
const BadUSBPayload payloads[] = {
  // FUN CATEGORY
  {"Chrome Dino",    "Opens Chrome offline game",           CAT_FUN,     payload_chrome_dino},
  {"Rick Roll",      "Opens the legendary video",          CAT_FUN,     payload_rickroll},
  {"Matrix Rain",    "CMD matrix effect",                  CAT_FUN,     payload_matrix_cmd},
  {"YouTube",        "Searches YouTube",                   CAT_FUN,     payload_youtube_search},
  
  // PRANK CATEGORY
  {"Fake Update",    "Windows update screen",              CAT_PRANK,   payload_fake_update},
  {"Upside Down",    "Flips display orientation",          CAT_PRANK,   payload_upside_down},
  {"Shutdown Timer", "Fake shutdown countdown",            CAT_PRANK,   payload_shutdown_prank},
  {"Type Message",   "Types creepy message",               CAT_PRANK,   payload_type_message},
  {"Fullscreen Vid", "Maximizes current video",            CAT_PRANK,   payload_fullscreen_video},
  
  // DEMO CATEGORY  
  {"Notepad Hello",  "Opens Notepad and types",            CAT_DEMO,    payload_notepad_hello},
  {"Calculator",     "Opens calculator app",               CAT_DEMO,    payload_open_calculator},
  {"GitHub",         "Opens your GitHub profile",          CAT_DEMO,    payload_github},
  
  // UTILITY CATEGORY
  {"Settings",       "Opens Windows settings",             CAT_UTILITY, payload_open_settings},
  {"WiFi Passwords", "Shows saved WiFi passwords",         CAT_UTILITY, payload_wifi_passwords},
  {"System Info",    "Displays system information",        CAT_UTILITY, payload_sysinfo},
};

const int payloadCount = sizeof(payloads) / sizeof(BadUSBPayload);
// ===========================================================================
// INSTALLATION INSTRUCTIONS
// =============================================================================

/*
1. Add to apps.h:
   void payload_menu_loop();

2. Add to app_registry.cpp (replace existing Fake Update or add new):
   { "Payload Menu", &ICON_UPDATE_SPRITE, payload_menu_loop, FOLDER_PRANKS },

3. Create these files:
   - payload_library.h
   - payload_executions.cpp
   - payload_menu.cpp

4. Make sure ble_core.cpp has the typeText() improvements from earlier

FEATURES:
- 15 pre-built payloads
- 4 categories: Fun, Prank, Demo, Utility
- Scrollable menu with descriptions
- Execute with one button press
- Automatic logging to event system
- Works on Windows (most payloads)
- Easy to add more payloads

HOW TO ADD MORE PAYLOADS:
1. Add function declaration to payload_library.h
2. Implement function in payload_executions.cpp
3. Add entry to payloads[] array

EXAMPLE NEW PAYLOAD:
void payload_my_custom() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  typeText("my command here");
  pressKey(KEY_RETURN);
  releaseAllKeys();
  log_event("PAYLOAD", "My Custom");
}

Then add to array:
{"My Payload", "Does something cool", CAT_FUN, payload_my_custom},

POPULAR PAYLOADS TO ADD:
- Discord/Slack message sender
- Browser bookmark injector  
- Wallpaper changer
- Mouse jiggler
- Volume controls
- Brightness controls
- Screenshot taker
- Clipboard stealer (educational only!)
*/