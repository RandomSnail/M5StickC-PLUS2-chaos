#include "sys_core.h"

// --- LEVEL (IMU) ---
void level_loop() {
  if (!M5.Imu.isEnabled()) M5.Imu.begin();
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  bool sideMode = false;  // False = Flat (default), True = Side/Edge

  while (true) {
    M5.update();
    if (checkExit()) break;

    // Toggle mode
    if (M5.BtnA.wasPressed()) {
      sideMode = !sideMode;
      toneLite(1000, 50);
    }

    M5.Imu.update();
    auto imuData = M5.Imu.getImuData();
    float ax = imuData.accel.x;
    float ay = imuData.accel.y;
    float az = imuData.accel.z;

    int x_pos, y_pos;

    if (!sideMode) {
      // Flat Mode: Uses X and Y accel
      x_pos = (120 - (ay * 90));
      y_pos = (67 + (ax * 90));
    } else {
      // Side Mode: Uses Z (screen facing out) and Y (long axis)
      // Assumes device is resting on its long edge
      x_pos = (120 - (ay * 90));
      y_pos = (67 + (az * 90));
    }

    s.fillScreen(BLACK);

    // Center Target
    s.drawCircle(120, 67, 20, WHITE);
     s.drawCircle(120, 67, 10, GREY);
      s.drawCircle(120, 67, 5, GREY);

    // Constrain bubble position
      int bubble_x = constrain(x_pos, 10, 230);
      int bubble_y = constrain(y_pos, 10, 125);
      
      // Calculate distance from center
      float dist = sqrt(pow(bubble_x - 120, 2) + pow(bubble_y - 67, 2));
      
      // Color changes based on how level it is
      uint16_t bubbleColor;
      if (dist < 5) {
          bubbleColor = GREEN;  // Perfect level
      } else if (dist < 15) {
          bubbleColor = 0xFFE0; // Yellow (close)
      } else {
          bubbleColor = RED;    // Not level
      }
      
      // Draw bubble with gradient effect
      s.fillCircle(bubble_x, bubble_y, 12, bubbleColor);
      s.fillCircle(bubble_x - 2, bubble_y - 2, 4, WHITE); // Shine effect
      s.drawCircle(bubble_x, bubble_y, 12, WHITE); // Outline
      
      // UI
      s.setTextColor(WHITE);
      s.setTextSize(1);
      s.drawString(sideMode ? "SIDE MODE" : "FLAT MODE", 5, 5);
      s.drawCenterString("A: Mode   Hold B: Exit", 120, 120);
      
      s.pushSprite(0,0); 
      delay(20);
  }
  s.deleteSprite();
}