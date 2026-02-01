#include "sys_core.h"
#include "apps.h"
#include <M5Unified.h>
#include <vector>

// =========================================================================
// GAME 1: CYBER COPTER
// A = Thrust Up (Release to fall)
// B = Exit
// =========================================================================

struct CopterObstacle {
    int x;
    int gapY;
    int gapHeight;
    bool passed;
};

void game_copter_loop() {
    hardResetRadios();
    M5Canvas s(&M5.Display);
    s.createSprite(240, 135);
    
    // Physics
    float playerY = 67;
    float velocity = 0;
    const float gravity = 0.4;
    const float lift = -5.0; // Jump strength
    
    int score = 0;
    int speed = 4;
    bool gameOver = false;
    
    // Obstacles
    std::vector<CopterObstacle> walls;
    int wallTimer = 0;

    // Wait for start
    s.fillScreen(BLACK);
    s.setTextColor(THEME_COLOR);
    s.setTextSize(2);
    s.drawCenterString("CYBER COPTER", 120, 50);
    s.setTextSize(1);
    s.setTextColor(WHITE);
    s.drawCenterString("A: THRUST   B: EXIT", 120, 80);
    s.pushSprite(0,0);
    while(!M5.BtnA.wasPressed()) { M5.update(); if(M5.BtnB.wasPressed()) return; }

    while (!gameOver) {
        M5.update();
        if (M5.BtnB.wasPressed()) break;

        // --- PHYSICS ---
        if (M5.BtnA.isPressed()) {
            velocity -= 0.6; // Thrust accumulation
            if(velocity < lift) velocity = lift; // Cap speed
        } else {
            velocity += gravity;
        }
        playerY += velocity;

        // Floor/Ceiling collision
        if (playerY < 0 || playerY > 125) gameOver = true;

        // --- WALL GEN ---
        wallTimer++;
        if (wallTimer > 45) { // Spawn rate
            CopterObstacle w;
            w.x = 240;
            w.gapHeight = 50; // Difficulty
            w.gapY = random(10, 135 - w.gapHeight - 10);
            w.passed = false;
            walls.push_back(w);
            wallTimer = 0;
        }

        // --- DRAW ---
        s.fillScreen(BLACK);
        
        // Draw Player (Triangle)
        int px = 40;
        int py = (int)playerY;
        s.fillTriangle(px, py, px-10, py-5, px-10, py+5, THEME_COLOR);
        s.fillTriangle(px-10, py-2, px-15, py-2, px-15, py+2, ORANGE); // Engine exhaust

        // Draw Walls
        for (int i = 0; i < walls.size(); i++) {
            walls[i].x -= speed;
            
            // Draw Top Pillar
            s.fillRect(walls[i].x, 0, 15, walls[i].gapY, DARK_GREY);
            s.drawRect(walls[i].x, 0, 15, walls[i].gapY, WHITE);
            
            // Draw Bottom Pillar
            int bottomY = walls[i].gapY + walls[i].gapHeight;
            s.fillRect(walls[i].x, bottomY, 15, 135 - bottomY, DARK_GREY);
            s.drawRect(walls[i].x, bottomY, 15, 135 - bottomY, WHITE);

            // Collision Check
            if (px + 5 > walls[i].x && px - 10 < walls[i].x + 15) {
                if (py - 5 < walls[i].gapY || py + 5 > bottomY) {
                    gameOver = true;
                    toneLite(500, 200);
                }
            }

            // Scoring
            if (!walls[i].passed && walls[i].x < px) {
                score++;
                walls[i].passed = true;
                toneLite(2000, 20);
                if (score % 5 == 0) speed++; // Speed up
            }
        }
        
        // Cleanup offscreen walls
        if (walls.size() > 0 && walls[0].x < -20) walls.erase(walls.begin());

        // UI
        s.setTextSize(1);
        s.setTextColor(WHITE);
        s.setCursor(5, 5);
        s.printf("SCORE: %d", score);

        s.pushSprite(0, 0);
        delay(20);
    }

    // Game Over Screen
    s.fillScreen(RED);
    s.setTextColor(BLACK);
    s.drawCenterString("CRASHED", 120, 50);
    s.setCursor(90, 70);
    s.printf("Score: %d", score);
    s.pushSprite(0,0);
    delay(2000);
    s.deleteSprite();
}