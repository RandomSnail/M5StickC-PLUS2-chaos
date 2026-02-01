#pragma once
#include "app_defs.h"

// State variables
extern AppFolder currentFolder;
extern int menuCursor;
extern int filtered_count;
extern int filtered_indexes[20]; // Max 20 apps per folder to be safe

// Functions
void menu_init();     // Call this in setup()
void menu_tick();     // Call this in loop()
void rebuild_filter(); // Call this when changing folders