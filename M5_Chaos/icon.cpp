#include "icon.h"

// --------------------
// Sprites
// --------------------
M5Canvas ICON_CLOCK_SPRITE(&M5.Display);
M5Canvas ICON_TIMER_SPRITE(&M5.Display);
M5Canvas ICON_POMO_SPRITE(&M5.Display);
M5Canvas ICON_LEVEL_SPRITE(&M5.Display);
M5Canvas ICON_TALLY_SPRITE(&M5.Display);
M5Canvas ICON_FLASHLIGHT_SPRITE(&M5.Display);
M5Canvas ICON_METRO_SPRITE(&M5.Display);
M5Canvas ICON_DB_SPRITE(&M5.Display);
M5Canvas ICON_MEDIA_SPRITE(&M5.Display);

M5Canvas ICON_DINO_SPRITE(&M5.Display);
M5Canvas ICON_SNAKE_SPRITE(&M5.Display);
M5Canvas ICON_REACT_SPRITE(&M5.Display);
M5Canvas ICON_COIN_SPRITE(&M5.Display);
M5Canvas ICON_ROCKET_SPRITE(&M5.Display);
M5Canvas ICON_COPTER_SPRITE(&M5.Display);

M5Canvas ICON_WIFI_SPRITE(&M5.Display);
M5Canvas ICON_BLE_SPRITE(&M5.Display);
M5Canvas ICON_TV_SPRITE(&M5.Display);
M5Canvas ICON_PORTAL_SPRITE(&M5.Display);
M5Canvas ICON_ANNOY_SPRITE(&M5.Display);
M5Canvas ICON_UPDATE_SPRITE(&M5.Display);
M5Canvas ICON_PANIC_SPRITE(&M5.Display);

M5Canvas ICON_BRIGHT_SPRITE(&M5.Display);
M5Canvas ICON_TIMEOUT_SPRITE(&M5.Display);
M5Canvas ICON_SOUND_SPRITE(&M5.Display);
M5Canvas ICON_THEME_SPRITE(&M5.Display);

// --------------------
// 16x16 Bitmaps (made-up logos)
// --------------------
// We'll use simple pixel patterns for each icon

static const uint16_t CLOCK_BMP[16*16] = {
    0,0,0,0,0,0,WHITE,WHITE,WHITE,WHITE,0,0,0,0,0,0,
    0,0,0,0,WHITE,WHITE,0,0,0,0,WHITE,WHITE,0,0,0,0,
    0,0,0,WHITE,0,0,0,0,0,0,0,0,WHITE,0,0,0,
    0,0,WHITE,0,0,0,0,0,0,0,0,0,0,WHITE,0,0,
    0,WHITE,0,0,0,0,0,0,0,0,0,0,0,0,WHITE,0,
    0,WHITE,0,0,0,0,0,WHITE,WHITE,0,0,0,0,0,WHITE,0,
    WHITE,0,0,0,0,0,WHITE,0,0,WHITE,0,0,0,0,0,WHITE,
    WHITE,0,0,0,0,0,0,0,0,0,0,0,0,0,0,WHITE,
    WHITE,0,0,0,0,0,0,0,0,0,0,0,0,0,0,WHITE,
    0,WHITE,0,0,0,0,0,0,0,0,0,0,0,0,WHITE,0,
    0,WHITE,0,0,0,0,0,0,0,0,0,0,0,0,WHITE,0,
    0,0,WHITE,0,0,0,0,0,0,0,0,0,0,WHITE,0,0,
    0,0,0,WHITE,0,0,0,0,0,0,0,0,WHITE,0,0,0,
    0,0,0,0,WHITE,0,0,0,0,0,0,WHITE,0,0,0,0,
    0,0,0,0,0,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,0,0,0,0,0,
    0,0,0,0,0,0,WHITE,WHITE,WHITE,WHITE,0,0,0,0,0,0
};

static const uint16_t DINO_BMP[16*16] = {
    GREEN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,GREEN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,GREEN,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,GREEN,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,GREEN,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,GREEN,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,GREEN,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,GREEN,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,GREEN,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,GREEN,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,GREEN,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,GREEN,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,GREEN,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,GREEN,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,GREEN,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,GREEN
};

// Other icons: placeholders (colors) for now
#define MAKE_COLOR_ICON(c) { \
    M5Canvas tmp(&M5.Display); tmp.createSprite(16,16); tmp.fillScreen(c); tmp.pushSprite(0,0); tmp.deleteSprite(); \
}

void createAllSprites() {
    auto makeSprite = [](M5Canvas &sprite, const uint16_t *bmp) {
        sprite.createSprite(16,16);
        sprite.pushImage(0,0,16,16,bmp);
    };

    makeSprite(ICON_CLOCK_SPRITE, CLOCK_BMP);
    makeSprite(ICON_DINO_SPRITE, DINO_BMP);

    // All others: simple color placeholders
    ICON_TIMER_SPRITE.createSprite(16,16); ICON_TIMER_SPRITE.fillScreen(TFT_RED);
    ICON_POMO_SPRITE.createSprite(16,16); ICON_POMO_SPRITE.fillScreen(TFT_MAGENTA);
    ICON_LEVEL_SPRITE.createSprite(16,16); ICON_LEVEL_SPRITE.fillScreen(TFT_GREEN);
    ICON_TALLY_SPRITE.createSprite(16,16); ICON_TALLY_SPRITE.fillScreen(TFT_CYAN);
    ICON_FLASHLIGHT_SPRITE.createSprite(16,16); ICON_FLASHLIGHT_SPRITE.fillScreen(TFT_YELLOW);
    ICON_METRO_SPRITE.createSprite(16,16); ICON_METRO_SPRITE.fillScreen(TFT_LIGHTGREY);
    ICON_DB_SPRITE.createSprite(16,16); ICON_DB_SPRITE.fillScreen(TFT_WHITE);
    ICON_MEDIA_SPRITE.createSprite(16,16); ICON_MEDIA_SPRITE.fillScreen(TFT_BLUE);

    ICON_SNAKE_SPRITE.createSprite(16,16); ICON_SNAKE_SPRITE.fillScreen(TFT_YELLOW);
    ICON_REACT_SPRITE.createSprite(16,16); ICON_REACT_SPRITE.fillScreen(TFT_PURPLE);
    ICON_COIN_SPRITE.createSprite(16,16); ICON_COIN_SPRITE.fillScreen(TFT_ORANGE);
    ICON_ROCKET_SPRITE.createSprite(16,16); ICON_ROCKET_SPRITE.fillScreen(TFT_RED); // Placeholder color
    ICON_COPTER_SPRITE.createSprite(16,16); ICON_COPTER_SPRITE.fillScreen(TFT_RED);

    ICON_WIFI_SPRITE.createSprite(16,16); ICON_WIFI_SPRITE.fillScreen(TFT_BLUE);
    ICON_BLE_SPRITE.createSprite(16,16); ICON_BLE_SPRITE.fillScreen(TFT_CYAN);
    ICON_TV_SPRITE.createSprite(16,16); ICON_TV_SPRITE.fillScreen(TFT_RED);
    ICON_PORTAL_SPRITE.createSprite(16,16); ICON_PORTAL_SPRITE.fillScreen(TFT_MAGENTA);
    ICON_ANNOY_SPRITE.createSprite(16,16); ICON_ANNOY_SPRITE.fillScreen(TFT_YELLOW);
    ICON_UPDATE_SPRITE.createSprite(16,16); ICON_UPDATE_SPRITE.fillScreen(TFT_GREEN);
    ICON_PANIC_SPRITE.createSprite(16,16); ICON_PANIC_SPRITE.fillScreen(TFT_WHITE);

    ICON_BRIGHT_SPRITE.createSprite(16,16); ICON_BRIGHT_SPRITE.fillScreen(TFT_YELLOW);
    ICON_TIMEOUT_SPRITE.createSprite(16,16); ICON_TIMEOUT_SPRITE.fillScreen(TFT_ORANGE);
    ICON_SOUND_SPRITE.createSprite(16,16); ICON_SOUND_SPRITE.fillScreen(TFT_RED);
    ICON_THEME_SPRITE.createSprite(16,16); ICON_THEME_SPRITE.fillScreen(TFT_PURPLE);
}
