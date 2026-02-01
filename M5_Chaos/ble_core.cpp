// =============================================================================
// ble_core.cpp - FIXED SCAN RESPONSE & VISIBILITY
// =============================================================================

#include "ble_core.h"
#include <Arduino.h>

// HID Report Map
static const uint8_t _hidReportDescriptor[] PROGMEM = {
  0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x85, 0x01,
  0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00,
  0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02,
  0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x06,
  0x75, 0x08, 0x15, 0x00, 0x25, 0x65, 0x05, 0x07,
  0x19, 0x00, 0x29, 0x65, 0x81, 0x00, 0xC0,
  
  0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01, 0x85, 0x02,
  0x19, 0x00, 0x2A, 0x3C, 0x02, 0x15, 0x00, 0x26, 0x3C, 0x02,
  0x95, 0x01, 0x75, 0x10, 0x81, 0x00, 
  0xC0
};

// Globals
static BleMode currentMode = BLE_OFF;
static NimBLEServer* bleServer = nullptr;
static NimBLEHIDDevice* bleHid = nullptr;
static NimBLECharacteristic* inputMedia = nullptr;
static NimBLECharacteristic* inputKeyboard = nullptr;

volatile bool bleConnected = false;
uint16_t bleConnId = 0; 

// Callbacks
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
    bleConnected = true;
    bleConnId = desc->conn_handle; 
    Serial.println("[BLE] Client connected");
    pServer->updateConnParams(desc->conn_handle, 6, 6, 0, 100); 
  }
  
  void onDisconnect(NimBLEServer* pServer) {
    bleConnected = false;
    Serial.println("[BLE] Client disconnected");
    if (currentMode == BLE_HID) {
      delay(500);
      NimBLEDevice::startAdvertising();
      Serial.println("[BLE] Restarting advertising");
    }
  }
};

// =============================================================================
// HID MODE
// =============================================================================

bool startBleHid(const char* deviceName) {
  Serial.printf("[BLE] Starting HID mode: %s\n", deviceName);
  
  if (NimBLEDevice::isInitialized()) {
    NimBLEDevice::deinit(true);
    delay(500);
  }
  
  NimBLEDevice::init(deviceName);
  
  #ifdef ESP_PLATFORM
    NimBLEDevice::setPowerLevel(ESP_PWR_LVL_P9);
  #else
    NimBLEDevice::setPower(9);
  #endif
  
  // Security
  NimBLEDevice::setSecurityAuth(true, true, true);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
  NimBLEDevice::setSecurityInitKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
  NimBLEDevice::setSecurityRespKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);

  // Server & HID
  bleServer = NimBLEDevice::createServer();
  bleServer->setCallbacks(new MyServerCallbacks());

  bleHid = new NimBLEHIDDevice(bleServer);
  bleHid->setManufacturer("M5Stack");
  bleHid->setPnp(0x02, 0x05ac, 0x820a, 0x0210);
  bleHid->setHidInfo(0x00, 0x01);
  bleHid->setReportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  
  inputKeyboard = bleHid->getInputReport(1);
  inputMedia = bleHid->getInputReport(2);

  bleHid->setBatteryLevel(100);
  bleHid->startServices();
  
  // Advertising
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->setAppearance(HID_KEYBOARD);
  pAdvertising->addServiceUUID(bleHid->getHidService()->getUUID());
  
  // FIXED: Manually construct Scan Response Data
  // This replaces the missing setScanResponse(true) method
  NimBLEAdvertisementData advData;
  advData.setFlags(0x06); // 0x06 = General Discoverable + BLE Only (Required for Windows/iOS)
  advData.setAppearance(HID_KEYBOARD);
  advData.setPartialServices(NimBLEUUID("1812")); // Advertise HID Service UUID
  pAdvertising->setAdvertisementData(advData); 
  
  // Scan Response (Name)
  NimBLEAdvertisementData scanDef;
  scanDef.setName(deviceName); 
  pAdvertising->setScanResponseData(scanDef);
  
  pAdvertising->start();
  
  currentMode = BLE_HID;
  Serial.println("[BLE] HID mode started");
  return true;
}

void stopBleHid() {
  Serial.println("[BLE] Stopping HID mode");
  
  if (bleConnected && bleServer != nullptr) {
    bleServer->disconnect(bleConnId);
    delay(200);
  }
  
  if (NimBLEDevice::getAdvertising()) {
    NimBLEDevice::getAdvertising()->stop();
  }
  
  NimBLEDevice::deinit(true);
  delay(500);
  
  bleConnected = false;
  currentMode = BLE_OFF;
  Serial.println("[BLE] HID mode stopped");
}

// =============================================================================
// SPAM MODE
// =============================================================================

bool startBleSpam() {
  Serial.println("[BLE] Starting SPAM mode");

  if (NimBLEDevice::isInitialized()) {
    NimBLEDevice::deinit(true);
    delay(500);
  }
  
  NimBLEDevice::init("");
  
  #ifdef ESP_PLATFORM
    NimBLEDevice::setPowerLevel(ESP_PWR_LVL_P9);
  #endif
  
  currentMode = BLE_SPAM;
  Serial.println("[BLE] SPAM mode started");
  return true;
}

void stopBleSpam() {
  Serial.println("[BLE] Stopping SPAM mode");
  
  if (NimBLEDevice::getAdvertising()) {
    NimBLEDevice::getAdvertising()->stop();
  }
  
  NimBLEDevice::deinit(true);
  delay(200);
  
  currentMode = BLE_OFF;
  Serial.println("[BLE] SPAM mode stopped");
}

// =============================================================================
// SCAN MODE
// =============================================================================

bool startBleScan() {
  Serial.println("[BLE] Starting SCAN mode");

  if (NimBLEDevice::isInitialized()) {
    NimBLEDevice::deinit(true);
    delay(500);
  }
  
  NimBLEDevice::init("");
  
  currentMode = BLE_SCAN;
  Serial.println("[BLE] SCAN mode started");
  return true;
}

void stopBleScan() {
  Serial.println("[BLE] Stopping SCAN mode");
  
  if (NimBLEDevice::getScan()) {
    NimBLEDevice::getScan()->stop();
  }
  
  NimBLEDevice::deinit(true);
  delay(200);
  
  currentMode = BLE_OFF;
  Serial.println("[BLE] SCAN mode stopped");
}

// =============================================================================
// HELPERS
// =============================================================================

void resetBleState() {
  Serial.println("[BLE] Forcing Reset");
  if (NimBLEDevice::isInitialized()) {
    NimBLEDevice::deinit(true);
    delay(200);
  }
  bleConnected = false;
  currentMode = BLE_OFF;
}

void sendMediaKey(uint8_t key) {
  if (!bleConnected || !inputMedia) return;

  uint8_t report[2] = { key, 0 };
  inputMedia->setValue(report, 2);
  inputMedia->notify();
  delay(10);

  report[0] = 0;
  inputMedia->setValue(report, 2);
  inputMedia->notify();
  delay(10);
}

void pressKey(uint8_t key, uint8_t modifier) {
  if (!bleConnected || !inputKeyboard) return;

  uint8_t report[8] = { modifier, 0, key, 0, 0, 0, 0, 0 };
  inputKeyboard->setValue(report, 8);
  inputKeyboard->notify();
  delay(10);
}

void releaseAllKeys() {
  if (!bleConnected || !inputKeyboard) return;

  uint8_t report[8] = { 0 };
  inputKeyboard->setValue(report, 8);
  inputKeyboard->notify();
  delay(10);
}

void typeText(const char* text) {
  if (!bleConnected) {
    Serial.println("[BLE] Can't type - not connected");
    return;
  }
  
  while (*text) {
    char c = *text++;
    uint8_t key = 0;
    uint8_t modifier = 0;

    if (c >= 'a' && c <= 'z') key = 0x04 + (c - 'a'); 
    else if (c >= 'A' && c <= 'Z') { key = 0x04 + (c - 'A'); modifier = KEY_LEFT_SHIFT; }
    else if (c >= '1' && c <= '9') key = 0x1E + (c - '1'); 
    else if (c == '0') key = 0x27; 
    else if (c == ' ') key = 0x2C;
    else if (c == '.') key = 0x37;
    else if (c == '/') key = 0x38;
    else if (c == ':') { key = 0x33; modifier = KEY_LEFT_SHIFT; }
    else if (c == '-') key = 0x2D;
    else if (c == '_') { key = 0x2D; modifier = KEY_LEFT_SHIFT; }
    else if (c == '=') key = 0x2E;
    else if (c == '?') { key = 0x38; modifier = KEY_LEFT_SHIFT; }
    else if (c == '!') { key = 0x1E; modifier = KEY_LEFT_SHIFT; }
    else continue;

    pressKey(key, modifier);
    delay(20);
    releaseAllKeys();
    delay(20);
  }
}