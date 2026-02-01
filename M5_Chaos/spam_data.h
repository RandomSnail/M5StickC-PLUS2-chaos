#pragma once
#include <Arduino.h>

// --- BLE SPAM ASSETS ---

// iOS Device Types (Sour Apple)
// Structure: { Type Byte, Length Byte }
const uint8_t APPLE_DEVICES[][2] = {
  { 0x07, 0x19 },  // AirPods Pro
  { 0x07, 0x19 },  // AirPods 3
  { 0x02, 0x15 },  // iBeacon (Generic)
  { 0x05, 0x15 },  // Airdrop Transfer
  { 0x09, 0x08 },  // AirPlay Target
  { 0x0B, 0x09 },  // Apple Watch Setup
  { 0x0C, 0x0E },  // Handoff
  { 0x0D, 0x0E },  // WiFi Password Share
  { 0x10, 0x0E },  // Nearby Action
  { 0x20, 0x15 },  // Apple TV Setup
  { 0x27, 0x0E }   // Siri Setup
};

// Android Model IDs (Fast Pair)
// These trigger the "Connect to [Device Name]" popup
const uint32_t ANDROID_MODELS[] = {
  0x089608,  // JBL Flip 6
  0x074218,  // Jabra Elite 3
  0x9EA225,  // Pixel Buds Pro
  0x2CC528,  // Sony WH-1000XM5
  0x618536,  // Galaxy Buds2 Pro
  0x546654,  // OnePLus Buds
  0xA3C33D   // Generic Fast Pair
};