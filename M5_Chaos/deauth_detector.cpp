#include "sys_core.h"
#include "apps.h"
#include <WiFi.h>
#include <esp_wifi.h>

// -------------------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------------------
volatile int packet_rate = 0;
volatile int deauths = 0;
unsigned long last_update = 0;

// -------------------------------------------------------------------------
// SNIFFER CALLBACK (Runs for every packet found)
// -------------------------------------------------------------------------
void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  // We only care about Management Frames (Type 0)
  // Subtype 12 (0xC) = Deauthentication
  // Subtype 10 (0xA) = Disassociation
  
  // Get the frame control field (first 2 bytes)
  uint8_t* data = pkt->payload;
  uint8_t frame_ctrl = data[0];
  uint8_t frame_type = (frame_ctrl & 0x0C) >> 2;
  uint8_t frame_subtype = (frame_ctrl & 0xF0) >> 4;

  if (frame_type == 0) { // Management
    if (frame_subtype == 12 || frame_subtype == 10) {
      deauths++;
    }
  }
  packet_rate++;
}

// -------------------------------------------------------------------------
// MAIN LOOP
// -------------------------------------------------------------------------
void deauth_detector_loop() {
  hardResetRadios(); // Clean slate

  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  // 1. Setup Sniffer
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);

  int curChannel = 1;
  int max_history[240] = {0}; // Graph history
  int graph_idx = 0;

  while (true) {
    M5.update();
    if (checkExit()) break;

    // 2. Channel Hopping (Scan all frequencies)
    if (millis() % 200 == 0) { // Hop every 200ms
      curChannel++;
      if (curChannel > 13) curChannel = 1;
      esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
    }

    // 3. Update UI every 100ms
    if (millis() - last_update > 100) {
      last_update = millis();

      s.fillScreen(BLACK);
      
      // --- HEADER ---
      // Red header if under attack, Grey if safe
      uint16_t headColor = (deauths > 5) ? RED : DARK_GREY;
      s.fillRect(0, 0, 240, 28, headColor);
      s.setTextColor(WHITE, headColor);
      s.setTextSize(2);
      if (deauths > 5) s.drawCenterString("!! ATTACK !!", 120, 5);
      else s.drawCenterString("SCANNING...", 120, 5);

      // --- STATS ---
      s.setTextSize(1);
      s.setTextColor(WHITE, BLACK);
      s.setCursor(5, 35); 
      s.printf("CH: %d", curChannel);
      s.setCursor(80, 35); 
      s.printf("PKT: %d/s", packet_rate);
      
      s.setTextColor((deauths > 0) ? RED : GREEN, BLACK);
      s.setCursor(170, 35); 
      s.printf("DEAUTH: %d", deauths);

      // --- GRAPH ---
      // Shift graph
      max_history[graph_idx] = deauths;
      graph_idx = (graph_idx + 1) % 240;

      // Draw Lines
      for (int i = 0; i < 240; i++) {
        int val = max_history[(graph_idx + i) % 240];
        if (val > 0) {
           int h = map(val, 0, 50, 0, 80); // Scale height
           if(h>80) h=80;
           s.drawLine(i, 135, i, 135 - h, RED);
        } else {
           // Draw faint green baseline for activity
           if (i % 4 == 0) s.drawPixel(i, 134, DARK_GREY); 
        }
      }

      s.pushSprite(0, 0);

      // Reset counters for next second
      packet_rate = 0;
      deauths = 0; 
    }
  }

  // Cleanup
  esp_wifi_set_promiscuous(false);
  hardResetRadios();
  s.deleteSprite();
}