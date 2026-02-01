#include "sys_core.h"
#include "apps.h"
#include "ble_core.h"
#include <NimBLEDevice.h>

#define MAX_BLE_DEVICES 20

// Simple data structure for the UI list
struct BLEDeviceInfo {
    String name;
    String address;
    int rssi;
    bool active;
};

BLEDeviceInfo bleDevices[MAX_BLE_DEVICES];
int bleDeviceCount = 0;

// Callback class to handle found devices
class BLEScanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        if (!advertisedDevice) return;

        String addr = advertisedDevice->getAddress().toString().c_str();

        // Check if we already have this device
        int existingIndex = -1;
        for (int i = 0; i < bleDeviceCount; i++) {
            if (bleDevices[i].address == addr) {
                existingIndex = i;
                break;
            }
        }

        // Update existing or Add new
        if (existingIndex >= 0) {
            bleDevices[existingIndex].rssi = advertisedDevice->getRSSI();
            bleDevices[existingIndex].active = true;
            if (advertisedDevice->haveName()) {
                bleDevices[existingIndex].name = advertisedDevice->getName().c_str();
            }
        } 
        else if (bleDeviceCount < MAX_BLE_DEVICES) {
            bleDevices[bleDeviceCount].address = addr;
            bleDevices[bleDeviceCount].rssi = advertisedDevice->getRSSI();
            bleDevices[bleDeviceCount].active = true;
            bleDevices[bleDeviceCount].name = advertisedDevice->haveName() ? advertisedDevice->getName().c_str() : "Unknown";
            bleDeviceCount++;
        }
    }
};

void ble_scanner_loop() {
    // 1. Initialize BLE Stack
    if (!startBleScan()) {
        Serial.println("[ERROR] Failed to start BLE SCAN");
        return;
    }

    M5Canvas s(&M5.Display);
    s.createSprite(240, 135);

    // 2. Configure Scanner
    NimBLEScan* pScan = NimBLEDevice::getScan();
    if (!pScan) {
        Serial.println("[ERROR] Failed to get scan object");
        return;
    }
    
    // FIXED: Correct method name for your library version
    pScan->setScanCallbacks(new BLEScanCallbacks());
    pScan->setActiveScan(true); 
    pScan->setInterval(100);
    pScan->setWindow(99);

    bleDeviceCount = 0;
    int scroll = 0;
    bool continuousScan = false;

    while (true) {
        M5.update();

        // EXIT
        if (M5.BtnB.pressedFor(1000)) {
            if (pScan->isScanning()) pScan->stop();
            break;
        }

        // TOGGLE SCAN (Button A)
        if (M5.BtnA.wasPressed()) {
            continuousScan = !continuousScan;
            toneLite(continuousScan ? 2000 : 1000, 50);
            
            if (continuousScan) {
                bleDeviceCount = 0; 
                scroll = 0;
                // FIXED: Your library expects start(duration, isContinue, restart)
                // 0 = Infinite, false = don't continue (clear cache)
                pScan->start(0, false); 
            } else {
                pScan->stop();
            }
        }

        // SCROLL (Button B click)
        if (M5.BtnB.wasPressed() && !M5.BtnB.pressedFor(500) && bleDeviceCount > 0) {
            scroll = (scroll + 1) % bleDeviceCount;
            toneLite(1000, 20);
        }

        // DRAW UI
        s.fillScreen(BLACK);
        
        // Header
        s.fillRect(0, 0, 240, 24, DARK_GREY);
        s.setTextColor(WHITE, DARK_GREY);
        s.drawString("BLE SCANNER", 5, 4);

        // Status Bar
        s.setTextColor(GREY, BLACK);
        s.setTextSize(1);
        s.setCursor(5, 28);
        s.printf("Found: %d", bleDeviceCount);

        s.setCursor(160, 28);
        s.setTextColor(continuousScan ? GREEN : ORANGE, BLACK);
        s.print(continuousScan ? "SCANNING" : "PAUSED");

        // List View
        if (bleDeviceCount == 0 && !continuousScan) {
            s.setTextColor(WHITE, BLACK);
            s.drawCenterString("Press A to Start", 120, 70);
        } 
        else if (bleDeviceCount > 0) {
            for (int i = 0; i < 4; i++) {
                int idx = scroll + i;
                if (idx >= bleDeviceCount) break;

                int y = 45 + (i * 20);
                
                if (i == 0) {
                    s.fillRect(0, y - 2, 240, 18, 0x333333); 
                    s.setTextColor(THEME_COLOR, 0x333333);
                } else {
                    s.setTextColor(WHITE, BLACK);
                }

                s.setTextSize(1);
                s.setCursor(5, y);
                
                String name = bleDevices[idx].name;
                if (name.length() > 14) name = name.substring(0, 13) + ".";
                s.print(name);

                int rssi = bleDevices[idx].rssi;
                s.setCursor(130, y);
                uint16_t rssiColor = (rssi > -60) ? GREEN : (rssi > -80) ? ORANGE : RED;
                s.setTextColor(rssiColor, (i==0 ? 0x333333 : BLACK));
                s.printf("%d", rssi);

                s.setTextColor(GREY, (i==0 ? 0x333333 : BLACK));
                s.setCursor(190, y);
                String addr = bleDevices[idx].address;
                s.print(addr.substring(addr.length() - 5));
            }
        }

        s.setTextColor(GREY, BLACK);
        s.drawCenterString("A: Toggle   B: Scroll / Hold Exit", 120, 120);

        s.pushSprite(0, 0);
        delay(50);
    }

    stopBleScan();
    s.deleteSprite(); 
}