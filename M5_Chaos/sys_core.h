#ifndef SYS_CORE_H
#define SYS_CORE_H

#include <M5Unified.h>
#include <Preferences.h>
#include <NimBLEDevice.h> 

// =====================================================
// DEFINITIONS & STRUCTS
// =====================================================
#define IR_PIN 9
#define IR_CHANNEL 4
#define IR_RESOLUTION 12

// Forward declaration for IR (Prevents "unknown struct" errors)
struct IrCode;
bool ir_send_code(const struct IrCode* code);

// COLORS
#ifndef DARK_GREY
#define DARK_GREY 0x2124
#endif
#define GREY 0x8410
#define ORANGE 0xFD20

// PORTAL STRUCTURE
struct PortalEntry {
  String user;
  String pass;
};

// =====================================================
// GLOBAL VARIABLES (External Declarations)
// =====================================================
// These tell the compiler "These variables exist in sys_core.cpp"

extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

extern Preferences prefs;
extern NimBLEAdvertising* pAdvertising;
extern bool soundEnabled;
extern int dino_high_score;

// Logs
extern PortalEntry portal_logs[5]; 
extern int portal_log_count;
extern int16_t mic_data[256];

// System Settings
extern uint8_t screen_brightness; // Matches sys_core.cpp (uint8_t)
extern uint16_t THEME_COLOR;
extern int timeout_index;
extern const long timeouts[6];
extern const char* timeout_names[6];
extern unsigned long last_interaction;
extern bool is_screen_off;

// =====================================================
// FUNCTIONS
// =====================================================
// CRITICAL: Added sys_init so main file can start the system
void sys_init(); 

// Graphics & Sound
void set_theme_color(uint16_t color);
void toneLite(unsigned int freq, unsigned long ms);
void drawHeader(const char* title);
int getBatteryPercent();

// System Logic
bool checkExit();
bool checkPowerManagement();

// Radio Management
void killRadios();
void hardResetRadios();

#endif