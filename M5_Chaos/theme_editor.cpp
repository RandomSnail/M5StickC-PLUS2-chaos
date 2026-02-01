#include "sys_core.h"
#include "apps.h"


struct ThemeOption {
    const char* name;
    uint16_t color;
};

const ThemeOption themes[] = {
    {"Cyber Teal",   0x067F},
    {"Hacker Green", 0x07E0},
    {"Warning Red",  0xF800},
    {"Deep Blue",    0x001F},
    {"Amber",        0xFD20},
    {"Purple",       0x780F},
    {"High Vis",     0xFFE0},
    {"White",        0xFFFF}
};
const int themeCount = sizeof(themes) / sizeof(ThemeOption);

void theme_editor_loop() {
    M5Canvas s(&M5.Display);
    s.createSprite(240, 135);

    int cursor = 0;
    uint16_t originalColor = THEME_COLOR; // Save original
    
    // Find current index
    for(int i=0; i<themeCount; i++) {
        if(themes[i].color == THEME_COLOR) {
            cursor = i;
            break;
        }
    }

    while (true) {
        M5.update();
        
        // SAVE
        if (M5.BtnA.wasPressed()) {
            set_theme_color(themes[cursor].color);
            toneLite(2000, 100);
            
            // Show saved confirmation
            s.fillScreen(BLACK);
            s.setTextColor(GREEN);
            s.drawCenterString("SAVED!", 120, 60);
            s.pushSprite(0, 0);
            delay(1000);
            break;
        }
        
        // CANCEL / NEXT
        if (M5.BtnB.wasPressed()) {
            // Short press: next color
            if (!M5.BtnB.pressedFor(800)) {
                cursor = (cursor + 1) % themeCount;
                THEME_COLOR = themes[cursor].color; // Preview
                toneLite(1000, 30);
            }
        }
        
        // CANCEL (hold B)
        if (M5.BtnB.pressedFor(800)) {
            THEME_COLOR = originalColor; // Restore
            toneLite(500, 100);
            
            s.fillScreen(BLACK);
            s.setTextColor(ORANGE);
            s.drawCenterString("CANCELLED", 120, 60);
            s.pushSprite(0, 0);
            delay(1000);
            break;
        }

        s.fillScreen(BLACK);

        // Header with live preview
        s.fillRect(0, 0, 240, 30, THEME_COLOR);
        s.setTextColor(BLACK, THEME_COLOR);
        s.drawCenterString("THEME EDITOR", 120, 7);

        // Color swatch
        s.fillCircle(120, 70, 30, themes[cursor].color);
        s.drawCircle(120, 70, 32, WHITE);

        // Name
        s.setTextColor(WHITE, BLACK);
        s.setTextSize(2);
        s.drawCenterString(themes[cursor].name, 120, 110);

        // Controls
        s.setTextSize(1);
        s.setTextColor(GREY, BLACK);
        s.drawCenterString("A: Save  B: Next  Hold B: Cancel", 120, 125);

        s.pushSprite(0, 0);
        delay(50);
    }
    s.deleteSprite();
}
