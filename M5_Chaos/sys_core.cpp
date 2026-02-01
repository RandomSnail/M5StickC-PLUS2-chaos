// =============================================================================
// sys_core.cpp - SYSTEM CORE & POWER MANAGEMENT
// =============================================================================

#include "sys_core.h"
#include <M5Unified.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <NimBLEDevice.h>
#include <Arduino.h>
#include "esp_bt.h"
#include "ble_core.h"
#include <Preferences.h>

// ----------------- GLOBAL DEFINITIONS -----------------
// These lines CREATE the variables that the linker was looking for.

Preferences prefs;
unsigned long last_interaction = 0;
bool soundEnabled = true;
uint8_t screen_brightness = 100; 
uint16_t THEME_COLOR = 0x067F; 

// !!! WIFI CREDENTIALS - THIS FIXES YOUR ERROR !!!
const char* WIFI_SSID = "YOUR_WIFI_NAME";      
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// Timeout settings
int timeout_index = 2; // Default to 1 minute
const long timeouts[6] = { 15000, 30000, 60000, 120000, 300000, 0 };
const char* timeout_names[6] = { "15s", "30s", "1m", "2m", "5m", "Never" };

bool is_screen_off = false;

// ----------------- INITIALIZATION -----------------
void sys_init() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Load saved preferences
    prefs.begin("system", true); // Read-only mode first
    soundEnabled = prefs.getBool("sound", true);
    screen_brightness = prefs.getUChar("bright", 100);
    THEME_COLOR = prefs.getUShort("theme", 0x067F);
    timeout_index = prefs.getInt("timeout", 2);
    prefs.end();

    // Apply settings
    M5.Display.setBrightness(screen_brightness);
    
    // Initialize interaction timer
    last_interaction = millis();
}

// ----------------- UTILITIES -----------------

void toneLite(unsigned int freq, unsigned long ms) {
  if (soundEnabled) {
    M5.Speaker.tone(freq, ms);
  }
}

int getBatteryPercent() {
  return M5.Power.getBatteryLevel();
}

// ----------------- POWER MANAGEMENT -----------------

// Returns TRUE if we just woke up (so apps can ignore the wake-up click)
bool checkPowerManagement() {
    // A. Check for Interaction (Wake up or Reset Timer)
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnA.isPressed() || M5.BtnB.isPressed()) {
        last_interaction = millis(); // Reset timer
        
        if (is_screen_off) {
            // WAKE UP!
            is_screen_off = false;
            M5.Display.wakeup();
            M5.Display.setBrightness(screen_brightness); // Restore brightness
            return true; // Tell app "We just woke up"
        }
    }

    // B. Check Timer (Go to Sleep)
    // index 5 is "Never" (value 0), so we skip check
    if (timeout_index < 5 && !is_screen_off) {
        if (millis() - last_interaction > timeouts[timeout_index]) {
            // SLEEP!
            is_screen_off = true;
            M5.Display.setBrightness(0);
            M5.Display.sleep();
        }
    }
    
    return false; // Normal operation
}

bool checkExit() {
  if (M5.BtnB.pressedFor(1000)) {
    toneLite(500, 300);
    // Wait for release so we don't trigger other things
    while (M5.BtnB.isPressed()) M5.update();
    return true;
  }
  return false;
}

// ----------------- GRAPHICS -----------------

void drawHeader(const char* title) {
  M5.Display.fillScreen(BLACK);
  M5.Display.fillRect(0, 0, 240, 24, DARK_GREY);
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE, DARK_GREY);
  M5.Display.drawString(title, 5, 4);
  
  int bat = getBatteryPercent();
  M5.Display.drawRect(200, 5, 30, 14, WHITE);
  M5.Display.fillRect(202, 7, (bat*26)/100, 10, (bat>20)?GREEN:RED);
}

// ----------------- RADIO MANAGEMENT -----------------

void hardResetRadios() {
    resetBleState(); // Helper from ble_core.cpp
    
    if (NimBLEDevice::isInitialized()) {
        NimBLEDevice::deinit(true);
    }
    delay(100);
}

void killRadios() {
  // Stop promiscuous / sniffing
  esp_wifi_set_promiscuous(false);

  // Stop BLE advertising if active
  if (NimBLEDevice::isInitialized()) {
    NimBLEDevice::getAdvertising()->stop();
  }

  // WiFi cleanup
  if (WiFi.getMode() != WIFI_OFF) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
  }

  // BLE cleanup
  if (NimBLEDevice::isInitialized()) {
    NimBLEDevice::deinit(true);
    delay(100);
  }

  // Bluetooth Classic
  btStop();

  delay(200);
  Serial.println("[RADIO] Reset complete");
}

void set_theme_color(uint16_t color) {
    THEME_COLOR = color;
    prefs.begin("system", false); // Read-write mode
    prefs.putUShort("theme", color);
    prefs.end();
    
    Serial.printf("[THEME] Color changed to 0x%04X\n", color);
}