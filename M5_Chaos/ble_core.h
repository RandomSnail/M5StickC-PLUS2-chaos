// =============================================================================
// ble_core.h - FIXED VERSION
// =============================================================================

#pragma once
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>

// ----------------- STATE -----------------
extern volatile bool bleConnected;

// ----------------- MEDIA KEYS -----------------
// Standard HID Consumer Usage IDs
#define MEDIA_PLAY_PAUSE  0xCD
#define MEDIA_NEXT        0xB5
#define MEDIA_PREV        0xB6
#define MEDIA_VOL_UP      0xE9
#define MEDIA_VOL_DOWN    0xEA
#define MEDIA_MUTE        0xE2

// ----------------- MODIFIER KEYS -----------------
#define KEY_LEFT_CTRL   0x01
#define KEY_LEFT_SHIFT  0x02
#define KEY_LEFT_ALT    0x04
#define KEY_LEFT_GUI    0x08
#define KEY_RIGHT_CTRL  0x10
#define KEY_RIGHT_SHIFT 0x20
#define KEY_RIGHT_ALT   0x40
#define KEY_RIGHT_GUI   0x80

// ----------------- KEYBOARD KEYS -----------------
#define KEY_A        0x04
#define KEY_R        0x15
#define KEY_RETURN   0x28
#define KEY_ESC      0x29
#define KEY_BACKSPACE 0x2A
#define KEY_TAB      0x2B
#define KEY_SPACE    0x2C
#define KEY_F11      0x44

// ----------------- BLE MODES -----------------
enum BleMode {
  BLE_OFF,
  BLE_HID,
  BLE_SPAM,
  BLE_SCAN
};

// ----------------- API -----------------

// HID Mode (Media Remote, Payloads)
bool startBleHid(const char* deviceName = "M5-Remote");
void stopBleHid();

// Spam Mode (Advertising attacks)
bool startBleSpam();
void stopBleSpam();

// Scan Mode (BLE Scanner)
bool startBleScan();
void stopBleScan();

// Helper for System Reset
void resetBleState(); // <--- ADD THIS LINE

// Media & Keyboard Actions
void sendMediaKey(uint8_t key);
void pressKey(uint8_t key, uint8_t modifier = 0);
void releaseAllKeys();
void typeText(const char* text);