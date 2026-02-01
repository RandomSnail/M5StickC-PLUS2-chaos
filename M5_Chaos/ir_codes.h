#pragma once
#include <Arduino.h>
#include <IRremoteESP8266.h>

// Define structure for IR signals
struct IrEntry {
  const char* name;
  decode_type_t protocol;
  uint16_t bits;
  uint64_t data;
};

// =============================================================================
// TV POWER CODES DATABASE
// =============================================================================
static const IrEntry IR_POWER_CODES[] = {
    {"Universal NEC", NEC,          32, 0x20DF10EF}, // Works on many cheap TVs
    {"Samsung",       SAMSUNG,      32, 0xE0E040BF},
    {"LG",            NEC,          32, 0x20DF10EF},
    {"Sony 12-bit",   SONY,         12, 0xA90},
    {"Sony 15-bit",   SONY,         15, 0xA90},
    {"Sony 20-bit",   SONY,         20, 0xA90},
    {"Panasonic",     PANASONIC,    48, 0x40040100BCBD},
    {"Vizio",         NEC,          32, 0x20DF10EF},
    {"Sharp",         SHARP,        15, 0x41A2},
    {"Philips",       RC6,          20, 0x1000C},
    {"Toshiba",       NEC,          32, 0xE0E040BF},
    {"Hisense",       NEC,          32, 0xFB04EF00},
    {"Roku TV",       NEC,          32, 0x5743C03F},
    {"Apple TV",      NEC,          32, 0x77E1FA05}, // Menu/Wake
    {"Xbox One",      NEC,          32, 0x800F740C}  // Power
};

static const int IR_CODE_COUNT = sizeof(IR_POWER_CODES) / sizeof(IrEntry);