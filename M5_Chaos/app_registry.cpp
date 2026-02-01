#include "app_registry.h"
#include "app_defs.h"
#include "sys_core.h"
#include "apps.h"
#include "icon.h" 

const AppEntry app_registry[] = {
  // { "Name on Screen",  ICON_NAME,    function_name,     FOLDER_NAME },

  
  /* -------- TOOLS -------- */
  { "Clock",        &ICON_CLOCK_SPRITE,        clock_loop,        FOLDER_TOOLS },
  { "Stopwatch",    &ICON_TIMER_SPRITE,        stopwatch_loop,    FOLDER_TOOLS },
  { "IR Remote",    &ICON_TV_SPRITE,           ir_remote_loop,    FOLDER_TOOLS },
  { "Pomodoro",     &ICON_POMO_SPRITE,         pomodoro_loop,     FOLDER_TOOLS },
  { "Level",        &ICON_LEVEL_SPRITE,        level_loop,        FOLDER_TOOLS },
  { "Tally",        &ICON_TALLY_SPRITE,        tally_loop,        FOLDER_TOOLS },
  { "Flashlight",   &ICON_FLASHLIGHT_SPRITE,   flashlight_loop,   FOLDER_TOOLS },
  { "Metronome",    &ICON_METRO_SPRITE,        metronome_loop,    FOLDER_TOOLS },
  { "Decibel",      &ICON_DB_SPRITE,           decibel_meter_loop,FOLDER_TOOLS },
  { "Motion Alarm", &ICON_PANIC_SPRITE,        motion_alarm_loop, FOLDER_TOOLS },
 

  /* -------- WIRELESS -------*/
  { "Media Remote", &ICON_MEDIA_SPRITE,        media_remote_loop, FOLDER_WIRELESS },
  { "WiFi Scan",    &ICON_WIFI_SPRITE,         wifi_scanner_loop,  FOLDER_WIRELESS},
  { "BLE Scan",     &ICON_BLE_SPRITE,          ble_scanner_loop,   FOLDER_WIRELESS },
  { "Deauth Detect", &ICON_WIFI_SPRITE,        deauth_detector_loop, FOLDER_WIRELESS },
  { "Web Remote",   &ICON_WIFI_SPRITE,         web_interface_loop, FOLDER_WIRELESS },
  { "Payload Menu", &ICON_UPDATE_SPRITE,       payload_menu_loop, FOLDER_WIRELESS },
  
  /* -------- GAMES -------- */
  { "Dino Run",     &ICON_DINO_SPRITE,         dino_loop,         FOLDER_GAMES },
  { "Snake",        &ICON_SNAKE_SPRITE,        snake_loop,        FOLDER_GAMES },
  { "Reaction",     &ICON_REACT_SPRITE,        reaction_loop,     FOLDER_GAMES },
  { "Coin Toss",    &ICON_COIN_SPRITE,         coin_loop,         FOLDER_GAMES },
  { "Cyber Copter",   &ICON_DINO_SPRITE,       game_copter_loop,  FOLDER_GAMES },
  { "Space Defender", &ICON_ROCKET_SPRITE,     game_space_loop,   FOLDER_GAMES },

  /* -------- PRANKS -------- */
  { "WiFi Spam",    &ICON_WIFI_SPRITE,         wifi_menu_loop,    FOLDER_PRANKS },
  { "BLE Spam",     &ICON_BLE_SPRITE,          ble_menu_loop,     FOLDER_PRANKS },
  { "Evil Portal",  &ICON_PORTAL_SPRITE,       portal_loop,       FOLDER_PRANKS },
  { "Annoy-a-tron", &ICON_ANNOY_SPRITE,        annoy_loop,        FOLDER_PRANKS },
  
  /* -------- SYSTEM -------- */
  { "Brightness",   &ICON_BRIGHT_SPRITE,       brightness_loop,  FOLDER_SYSTEM },
  { "Timeout",      &ICON_TIMEOUT_SPRITE,      timeout_loop,     FOLDER_SYSTEM },
  { "Sound",        &ICON_SOUND_SPRITE,        toggle_sound,     FOLDER_SYSTEM },
  { "Theme Editor", &ICON_THEME_SPRITE,       theme_editor_loop, FOLDER_SYSTEM },
  { "Sync Time",    &ICON_CLOCK_SPRITE,        app_sync_time,    FOLDER_SYSTEM },
  { "Device Info",  &ICON_THEME_SPRITE,       device_info_loop,  FOLDER_SYSTEM },
  { "Event Logs",   &ICON_TIMEOUT_SPRITE,     view_logs_loop,    FOLDER_SYSTEM },
};

// Calculate count automatically
const uint8_t app_count = sizeof(app_registry) / sizeof(AppEntry);