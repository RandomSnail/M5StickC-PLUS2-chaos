#pragma once
#include <Arduino.h>
#include <M5Unified.h> 

// 1. Define your Folders (The IDs)
enum AppFolder : uint8_t {
  FOLDER_ROOT = 0,
  FOLDER_TOOLS,
  FOLDER_WIRELESS,
  FOLDER_GAMES,
  FOLDER_PRANKS,
  FOLDER_SYSTEM
};

// 2. Define the Folder Names (The Text)
// --- THIS WAS MISSING ---
static const char* folder_names[] = {
  "MAIN MENU",   // FOLDER_ROOT (0)
  "TOOLS",      // FOLDER_TOOLS (1)
  "WIRELESS",    // FOLDER_TOOLS (2)
  "GAMES",      // FOLDER_GAMES (3)
  "PRANKS",     // FOLDER_PRANKS (4)
  "SYSTEM"      // FOLDER_SYSTEM (5)
};

// 3. Define the structure of an App
typedef void (*AppLoopFunction)(); 

struct AppEntry {
  const char* name;          // Name on screen
  M5Canvas* icon;       // Icon data
  AppLoopFunction loop_func; // The function to run
  AppFolder folder;          // Which menu it belongs to
};