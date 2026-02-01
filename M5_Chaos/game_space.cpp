#include "sys_core.h"
#include "apps.h"
#include <M5Unified.h>
#include <vector>

struct Bullet { float x, y; bool active; };
struct Enemy { float x, y; bool active; };

void game_space_loop() {
    hardResetRadios();
    M5Canvas s(&M5.Display);
    s.createSprite(240, 135);

    float playerX = 120;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    
    int score = 0;
    bool gameOver = false;
    int spawnTimer = 0;
    int shootTimer = 0;

    while (!gameOver) {
        M5.update();
        if (M5.BtnB.pressedFor(1000)) break; // Hold B to exit

        // --- CONTROLS ---
        if (M5.BtnB.isPressed()) playerX -= 4; // Left
        if (M5.BtnA.isPressed()) playerX += 4; // Right
        
        // Clamp bounds
        if (playerX < 10) playerX = 10;
        if (playerX > 230) playerX = 230;

        // --- SHOOTING (Auto) ---
        shootTimer++;
        if (shootTimer > 8) { // Fire rate
            bullets.push_back({playerX, 120, true});
            shootTimer = 0;
            toneLite(3000, 10);
        }

        // --- SPAWNING ---
        spawnTimer++;
        if (spawnTimer > 30) {
            enemies.push_back({(float)random(10, 230), -10, true});
            spawnTimer = 0;
        }

        s.fillScreen(BLACK);

        // --- UPDATE & DRAW BULLETS ---
        for (auto &b : bullets) {
            if (!b.active) continue;
            b.y -= 5; // Bullet speed
            s.drawLine(b.x, b.y, b.x, b.y + 5, THEME_COLOR);
            if (b.y < 0) b.active = false;
        }

        // --- UPDATE & DRAW ENEMIES ---
        for (auto &e : enemies) {
            if (!e.active) continue;
            e.y += 2; // Enemy speed
            
            // Draw Asteroid
            s.fillCircle(e.x, e.y, 6, DARK_GREY);
            s.drawCircle(e.x, e.y, 7, WHITE);

            // Collision: Bullet hit Enemy
            for (auto &b : bullets) {
                if (b.active && abs(b.x - e.x) < 10 && abs(b.y - e.y) < 10) {
                    b.active = false;
                    e.active = false;
                    score += 10;
                    toneLite(1000, 30);
                    // Draw explosion
                    s.fillCircle(e.x, e.y, 10, ORANGE);
                }
            }

            // Collision: Enemy hit Player
            if (abs(e.x - playerX) < 15 && e.y > 115) {
                gameOver = true;
            }

            if (e.y > 140) e.active = false;
        }

        // Draw Player
        s.fillTriangle(playerX, 120, playerX-8, 134, playerX+8, 134, THEME_COLOR);

        // UI
        s.setTextColor(WHITE);
        s.setCursor(5, 5);
        s.printf("SCORE: %d", score);
        
        s.pushSprite(0, 0);
        delay(20);
    }

    s.fillScreen(RED);
    s.setTextColor(WHITE);
    s.drawCenterString("GAME OVER", 120, 50);
    s.setCursor(90, 70);
    s.printf("Final: %d", score);
    s.pushSprite(0,0);
    delay(2000);
    s.deleteSprite();
}