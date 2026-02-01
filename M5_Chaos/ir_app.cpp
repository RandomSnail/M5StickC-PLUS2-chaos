#include "sys_core.h"
#include "apps.h"
#include <M5Unified.h>
#include <IRremoteESP8266.h>
#include "ir_codes.h"
#include <IRsend.h>

// M5StickC Plus IR Transmitter is on GPIO 19
IRsend irsend(19);

// FIX: Use the generic sender to avoid "has no member sendSamsung" errors
void fire_ir_code(const IrEntry* code) {
  // arguments: protocol_type, data, number_of_bits
  irsend.send(code->protocol, code->data, code->bits);
}

void ir_remote_loop() {
  hardResetRadios(); // Turn off WiFi/BT to save power for the IR LED
  irsend.begin();
  
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);
  
  int cursor = 0;
  bool blasting = false;
  
  while (true) {
    M5.update();
    if (checkExit()) break;
    
    // --- INPUT ---
    if (M5.BtnB.wasPressed() && !blasting) {
      cursor = (cursor + 1) % (IR_CODE_COUNT + 1); // +1 for "BLAST ALL" option
      toneLite(1000, 20);
    }
    
    // --- FIRE SIGNAL ---
    if (M5.BtnA.wasPressed()) {
      if (cursor == IR_CODE_COUNT) {
        // === BLASTER MODE (Fire Everything) ===
        blasting = true;
        s.fillScreen(BLACK);
        s.fillRect(0, 0, 240, 28, RED);
        s.setTextColor(WHITE, RED);
        s.setTextSize(2);
        s.drawCenterString("TV-B-GONE", 120, 50);
        s.setTextSize(1);
        s.drawCenterString("FIRING ALL CODES...", 120, 80);
        s.pushSprite(0, 0);
        delay(500);
        
        for (int i = 0; i < IR_CODE_COUNT; i++) {
            // Check exit in case user panics
            M5.update(); 
            if (M5.BtnB.isPressed()) break;

          // UI Update
            s.fillScreen(BLACK);
            
            // Header
            s.fillRect(0, 0, 240, 30, RED);
            s.setTextColor(WHITE, RED);
            s.setTextSize(2);
            s.drawCenterString("BLASTING...", 120, 5);

            // Info
            s.setTextColor(WHITE, BLACK);
            s.setTextSize(2);
            s.drawCenterString("Target:", 120, 50);
            s.setTextColor(ORANGE);
            s.drawCenterString(IR_POWER_CODES[i].name, 120, 75);
            
            // Progress Bar
            int progress = ((i + 1) * 240) / IR_CODE_COUNT;
            s.fillRect(0, 120, progress, 15, RED);
            
            s.pushSprite(0, 0);
            
            // Fire!
            fire_ir_code(&IR_POWER_CODES[i]);
            delay(150); // Speed delay
        }
        blasting = false;
        
      } else {
        // === SINGLE SHOT ===
        s.fillScreen(BLACK);
       // Draw Firing Graphic
        s.fillCircle(120, 67, 50, RED); 
        s.fillCircle(120, 67, 40, ORANGE);
        s.setTextColor(BLACK);
        s.setTextSize(2);
        s.drawCenterString("SENDING", 120, 60);
        s.pushSprite(0, 0);
        
        fire_ir_code(&IR_POWER_CODES[cursor]);
        delay(200);
      }
    }
    
    // --- MAIN UI DRAW ---
    if (!blasting) {
      s.fillScreen(BLACK);
      
      // Header
      s.fillRect(0, 0, 240, 30, THEME_COLOR);
      s.setTextColor(BLACK, THEME_COLOR);
      s.setTextSize(2);
      s.drawCenterString("IR REMOTE", 120, 8);
      
      // List View Logic
      int topIndex = 0;
      if (cursor > 2) topIndex = cursor - 2;
      if (topIndex > IR_CODE_COUNT - 3) topIndex = IR_CODE_COUNT - 3;
      if (topIndex < 0) topIndex = 0;

      for (int i = 0; i < 4; i++) {
        int idx = topIndex + i;
        if (idx > IR_CODE_COUNT) break;
        
        int y = 35 + (i * 24);
        
        // Highlight
        if (idx == cursor) {
          s.fillRect(0, y - 2, 240, 22, DARK_GREY);
          s.setTextColor(THEME_COLOR);
          s.drawString(">", 5, y);
        } else {
          s.setTextColor(WHITE);
       }
        
        s.setTextSize(2);
        s.setCursor(25, y);
        
        if (idx == IR_CODE_COUNT) {
          s.setTextColor(RED); // "Blast All" is special
          s.print("[ BLAST ALL ]");
        } else {
          s.print(IR_POWER_CODES[idx].name);
        }
      }
      
      // Footer
      s.fillRect(0, 120, 240, 15, BLACK);
      s.drawLine(0, 120, 240, 120, GREY);
      s.setTextColor(GREY);
      s.setTextSize(1);
      s.drawCenterString("A: FIRE   B: SCROLL   Hold B: EXIT", 120, 123);
      
      s.pushSprite(0, 0);
      delay(20);
    }
  }
  s.deleteSprite();
}