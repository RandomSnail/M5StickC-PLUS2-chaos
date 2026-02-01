#pragma once
#include <Arduino.h>

// --- System Apps ---
void app_sync_time();
void brightness_loop();
void timeout_loop();
void toggle_sound();
void theme_editor_loop();
void device_info_loop();
void view_logs_loop();
void log_event(const char* category, const char* event);
// Add new tool declarations here...

// --- Tools ---
void clock_loop();
void stopwatch_loop();
void pomodoro_loop();
void level_loop();
void tally_loop();
void flashlight_loop();
void metronome_loop();
void decibel_meter_loop();
void motion_alarm_loop();
// Add new tool declarations here...

// Wireless Tools
void deauth_detector_loop();
void media_remote_loop();
void wifi_scanner_loop();
void ble_scanner_loop();
void ir_remote_loop();
void web_interface_loop();
// Add new tool declarations here...

// --- Games ---
void dino_loop();
void snake_loop();
void reaction_loop();
void coin_loop();
void game_copter_loop();
void game_space_loop();
// Add new tool declarations here...

// --- Pranks ---
void wifi_menu_loop();
void ble_menu_loop();
void portal_loop();
void annoy_loop();
void payload_menu_loop();
// Add new tool declarations here...