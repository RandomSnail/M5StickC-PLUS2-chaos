#include <M5Unified.h>
#include <Preferences.h> 
#include "sys_core.h"
#include "dino_assets.h" 

// ===============================
// CONFIG
// ===============================
#define GROUND_Y 110 
#define GRAVITY  0.6f
#define JUMP_V   -7.5f

void dino_loop() {
  Preferences prefs;
  prefs.begin("dino", true);
  int hi = prefs.getInt("hi", 0);
  prefs.end();
  
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  float y = GROUND_Y;
  float vy = 0;
  bool air = false;
  bool duck = false;
  bool retrying = false;
  bool isDead = false;
  
  float obsX = 240;
  int obsType = 0; // 0=SmallCactus, 1=LargeCactus, 2=Bird
  float speed = 4.0;
  int score = 0;
  
  float groundX = 0;
  
  // Animation frame counters
  unsigned long lastRunFrame = 0;
  unsigned long lastDuckFrame = 0;
  unsigned long lastBirdFrame = 0;
  bool runFrame = false;      // false=frame1, true=frame2
  bool duckFrame = false;
  bool birdFrame = false;
  
  // Cloud animation
  float cloudX = 200;
  float cloudX2 = 100;

  while (true) {
    M5.update();
    unsigned long now = millis();

    // --- INPUT ---
    duck = M5.BtnB.isPressed() && !air; // Can only duck on ground
    if (M5.BtnA.wasPressed() && !air && !isDead) {
      vy = JUMP_V;
      air = true;
    }

    // --- PHYSICS ---
    if (!isDead) {
      y += vy;
      vy += GRAVITY;
      if (y >= GROUND_Y) {
        y = GROUND_Y;
        vy = 0;
        air = false;
      }

      // --- OBSTACLE LOGIC ---
      obsX -= speed;
      if (obsX < -30) {
        obsX = 240 + random(20, 80);
        obsType = random(0, 3); 
        score++;
        if (score % 10 == 0 && speed < 10) speed += 0.5;
      }

      // --- GROUND SCROLLING ---
      groundX -= speed;
      if (groundX <= -GROUND_W) groundX = 0;
      
      // --- CLOUD SCROLLING ---
      cloudX -= speed * 0.3;
      cloudX2 -= speed * 0.25;
      if (cloudX < -CLOUD_W) cloudX = 240;
      if (cloudX2 < -CLOUD_W) cloudX2 = 240;
    }

    // --- ANIMATION FRAMES ---
    if (!isDead) {
      // Running animation
      if (now - lastRunFrame > DINO_RUN_FRAME_DELAY && !duck && !air) {
        runFrame = !runFrame;
        lastRunFrame = now;
      }
      
      // Ducking animation
      if (now - lastDuckFrame > DINO_DUCK_FRAME_DELAY && duck) {
        duckFrame = !duckFrame;
        lastDuckFrame = now;
      }
      
      // Bird flapping
      if (now - lastBirdFrame > BIRD_FLAP_DELAY) {
        birdFrame = !birdFrame;
        lastBirdFrame = now;
      }
    }

    // --- HITBOXES ---
    int dw = duck ? DINO_DUCK_W : DINO_W;
    int dh = duck ? DINO_DUCK_H : DINO_H;
    int dy = duck ? GROUND_Y - DINO_DUCK_H + 4 : (int)y - dh + 4;

    int ow, oh, oy;
    if (obsType == 0) {
      ow = CACTUS_SMALL_W;
      oh = CACTUS_SMALL_H;
      oy = GROUND_Y - CACTUS_SMALL_H + 4;
    } else if (obsType == 1) {
      ow = CACTUS_LARGE_W;
      oh = CACTUS_LARGE_H;
      oy = GROUND_Y - CACTUS_LARGE_H + 4;
    } else {
      ow = BIRD_W;
      oh = BIRD_H;
      oy = GROUND_Y - 30; // Bird flies higher
    }

    // Collision Detection (with padding for better accuracy)
    if (!isDead && obsX < 30 && obsX > 0) {
       int dinoLeft = 20 + 2;
       int dinoRight = 20 + dw - 2;
       int dinoTop = dy + 2;
       int dinoBottom = dy + dh - 2;
       
       int obsLeft = (int)obsX + 2;
       int obsRight = (int)obsX + ow - 2;
       int obsTop = oy + 2;
       int obsBottom = oy + oh - 2;
       
       if (dinoRight > obsLeft && dinoLeft < obsRight &&
           dinoBottom > obsTop && dinoTop < obsBottom) {
          // HIT!
          isDead = true;
          if (score > hi) {
             hi = score;
             prefs.begin("dino", false);
             prefs.putInt("hi", score);
             prefs.end();
          }
       }
    }

    // --- DRAWING ---
    s.fillScreen(BLACK);

    // 1. Draw Clouds (background)
    s.drawBitmap((int)cloudX, 15, CLOUD_BMP, CLOUD_W, CLOUD_H, DARKGREY);
    s.drawBitmap((int)cloudX2, 30, CLOUD_BMP, CLOUD_W, CLOUD_H, DARKGREY);

    // 2. Draw Ground (Tiled)
    for (int i = 0; i < 9; i++) {
        s.drawBitmap((int)groundX + (i * GROUND_W), GROUND_Y + 2, 
                     GROUND_BMP, GROUND_W, GROUND_H, WHITE);
    }
    s.drawLine(0, GROUND_Y+2, 240, GROUND_Y+2, WHITE);

    // 3. Draw Obstacle
    if (obsType == 0) {
        s.drawBitmap((int)obsX, oy, CACTUS_SMALL_BMP, CACTUS_SMALL_W, CACTUS_SMALL_H, RED);
    } else if (obsType == 1) {
        s.drawBitmap((int)obsX, oy, CACTUS_LARGE_BMP, CACTUS_LARGE_W, CACTUS_LARGE_H, RED);
    } else {
        const uint8_t* birdSprite = birdFrame ? BIRD_FLY2_BMP : BIRD_FLY1_BMP;
        s.drawBitmap((int)obsX, oy, birdSprite, BIRD_W, BIRD_H, ORANGE);
    }

    // 4. Draw Dino
    const uint8_t* dinoSprite;
    if (isDead) {
        dinoSprite = DINO_DEAD_BMP;
        s.drawBitmap(20, dy, dinoSprite, DINO_W, DINO_H, RED);
    } else if (duck) {
        dinoSprite = duckFrame ? DINO_DUCK2_BMP : DINO_DUCK1_BMP;
        s.drawBitmap(20, dy, dinoSprite, DINO_DUCK_W, DINO_DUCK_H, GREEN);
    } else if (air) {
        dinoSprite = DINO_STAND_BMP; // No leg movement while jumping
        s.drawBitmap(20, dy, dinoSprite, DINO_W, DINO_H, GREEN);
    } else {
        dinoSprite = runFrame ? DINO_RUN2_BMP : DINO_RUN1_BMP;
        s.drawBitmap(20, dy, dinoSprite, DINO_W, DINO_H, GREEN);
    }

    // 5. Draw Score
    s.setTextColor(WHITE);
    s.setTextSize(1);
    
    // Draw HI score
    s.setCursor(140, 5);
    s.print("HI ");
    int hiTmp = hi;
    for (int i = 4; i >= 0; i--) {
      int digit = (hiTmp / (int)pow(10, i)) % 10;
      s.drawBitmap(170 + (4-i)*6, 5, NUMBERS[digit], NUM_W, NUM_H, LIGHTGREY);
    }
    
    // Draw current score
    int scoreTmp = score;
    for (int i = 4; i >= 0; i--) {
      int digit = (scoreTmp / (int)pow(10, i)) % 10;
      s.drawBitmap(200 + (4-i)*6, 5, NUMBERS[digit], NUM_W, NUM_H, WHITE);
    }

    // 6. Game Over Screen
    if (isDead) {
      s.fillRect(40, 45, 160, 45, BLACK);
      s.drawRect(40, 45, 160, 45, WHITE);
      
      // Draw "GAME OVER" text using character sprites
      const uint8_t* gameOver[] = {CHAR_G, CHAR_A, CHAR_M, CHAR_E};
      const uint8_t* over[] = {CHAR_O, CHAR_V, CHAR_E, CHAR_R};
      
      int startX = 60;
      for (int i = 0; i < 4; i++) {
        s.drawBitmap(startX + i*8, 52, gameOver[i], TEXT_CHAR_W, TEXT_CHAR_H, WHITE);
      }
      for (int i = 0; i < 4; i++) {
        s.drawBitmap(startX + 35 + i*8, 52, over[i], TEXT_CHAR_W, TEXT_CHAR_H, WHITE);
      }
      
      s.setTextColor(LIGHTGREY);
      s.drawCenterString("A=Retry  B=Exit", 120, 72);
      
      // Handle retry input
      if (M5.BtnA.wasPressed()) {
        score = 0;
        speed = 4.0;
        obsX = 240;
        y = GROUND_Y;
        vy = 0;
        air = false;
        duck = false;
        isDead = false;
        groundX = 0;
      }
      
      if (M5.BtnB.wasPressed()) {
        break;
      }
    }

    s.pushSprite(0, 0);
    delay(20); 
  }
  s.deleteSprite();
}