#include "ble_core.h"
#include "sys_core.h"
#include "apps.h"

// Helper function to wait for app to open
void waitForApp(int ms = 1000) {
  delay(ms);
}

// -----------------------------------------------------------------------------
// FUN PAYLOADS
// -----------------------------------------------------------------------------

void payload_chrome_dino() {
  // Win + R
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  // Type chrome://dino
  typeText("chrome://dino");
  delay(200);
  
  // Enter
  pressKey(KEY_RETURN);
  waitForApp(1000);
  
  // Press space to start game
  pressKey(KEY_SPACE);
  releaseAllKeys();
  
  log_event("PAYLOAD", "Chrome Dino");
}

void payload_rickroll() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
  delay(200);
  
  pressKey(KEY_RETURN);
  waitForApp(3000);
  
  // Fullscreen
  pressKey(KEY_F11);
  releaseAllKeys();
  
  log_event("PAYLOAD", "Rick Roll");
}

void payload_matrix_cmd() {
  // Open CMD
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  typeText("cmd");
  pressKey(KEY_RETURN);
  waitForApp(1000);
  
  // Change colors to green on black
  typeText("color 0A");
  pressKey(KEY_RETURN);
  delay(500);
  
  // Matrix effect command
  typeText("@echo off");
  pressKey(KEY_RETURN);
  delay(200);
  
  typeText(":loop");
  pressKey(KEY_RETURN);
  delay(200);
  
  typeText("echo %random%%random%%random%%random%%random%");
  pressKey(KEY_RETURN);
  delay(200);
  
  typeText("goto loop");
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  log_event("PAYLOAD", "Matrix Rain");
}

void payload_youtube_search() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("https://www.youtube.com/results?search_query=m5stack");
  delay(200);
  
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  log_event("PAYLOAD", "YouTube Search");
}

// -----------------------------------------------------------------------------
// PRANK PAYLOADS
// -----------------------------------------------------------------------------

void payload_fake_update() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("https://fakeupdate.net/win10ue");
  delay(200);
  
  pressKey(KEY_RETURN);
  waitForApp(3000);
  
  // Fullscreen
  pressKey(KEY_F11);
  releaseAllKeys();
  
  log_event("PAYLOAD", "Fake Update");
}

void payload_upside_down() {
  // Ctrl + Alt + Down (works on some Intel graphics)
  pressKey(0x51, KEY_LEFT_CTRL | KEY_LEFT_ALT);  // Down arrow
  delay(100);
  releaseAllKeys();
  
  delay(3000);
  
  // Flip back after 3 seconds
  pressKey(0x52, KEY_LEFT_CTRL | KEY_LEFT_ALT);  // Up arrow
  releaseAllKeys();
  
  log_event("PAYLOAD", "Display Flip");
}

void payload_shutdown_prank() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("shutdown /s /t 60 /c \"System will shutdown in 60 seconds\"");
  delay(200);
  
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  // To cancel: shutdown /a
  
  log_event("PAYLOAD", "Shutdown Prank");
}

void payload_type_message() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("notepad");
  pressKey(KEY_RETURN);
  waitForApp(1000);
  
  delay(500);
  typeText("I am watching you...");
  delay(1000);
  pressKey(KEY_RETURN);
  pressKey(KEY_RETURN);
  delay(500);
  typeText("Do not look behind you...");
  delay(1000);
  pressKey(KEY_RETURN);
  pressKey(KEY_RETURN);
  delay(1000);
  typeText("Just kidding :)");
  releaseAllKeys();
  
  log_event("PAYLOAD", "Creepy Message");
}

void payload_fullscreen_video() {
  // Press F to go fullscreen (works on YouTube, most video players)
  pressKey(0x09);  // F key
  delay(100);
  releaseAllKeys();
  
  log_event("PAYLOAD", "Fullscreen");
}

// -----------------------------------------------------------------------------
// DEMO PAYLOADS
// -----------------------------------------------------------------------------

void payload_notepad_hello() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("notepad");
  pressKey(KEY_RETURN);
  waitForApp(1000);
  
  typeText("Hello from M5Stack!");
  pressKey(KEY_RETURN);
  typeText("This is a BadUSB demo.");
  releaseAllKeys();
  
  log_event("PAYLOAD", "Notepad Hello");
}

void payload_open_calculator() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("calc");
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  log_event("PAYLOAD", "Calculator");
}

void payload_github() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("https://github.com");
  delay(200);
  
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  log_event("PAYLOAD", "GitHub");
}

// -----------------------------------------------------------------------------
// UTILITY PAYLOADS
// -----------------------------------------------------------------------------

void payload_open_settings() {
  // Win + I opens Settings
  pressKey(0x0C, KEY_LEFT_GUI);  // I key
  releaseAllKeys();
  
  log_event("PAYLOAD", "Settings");
}

void payload_wifi_passwords() {
  // Open CMD
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  typeText("cmd");
  pressKey(KEY_RETURN);
  waitForApp(1000);
  
  // Show WiFi profiles
  typeText("netsh wlan show profiles");
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  log_event("PAYLOAD", "WiFi Info");
}

void payload_sysinfo() {
  pressKey(KEY_R, KEY_LEFT_GUI);
  waitForApp(500);
  
  typeText("msinfo32");
  pressKey(KEY_RETURN);
  releaseAllKeys();
  
  log_event("PAYLOAD", "System Info");
}
