#include <WiFi.h>
#include <esp_wifi.h>
#include <cstring>
#include "sys_core.h"
#include "menu_draw.h"

// --- SSIDs ---
const char* funny_ssids[] = { 
    "FBI Van", "Free WiFi", "Virus.exe", "Skynet", "Loading...", "Error 404", 
    "Mom Click Here", "Martin Router King", "Wu-Tang LAN", "404 Network Unavailable",
    "Pretty Fly for a WiFi", "Silence of the LANs", "The LAN Before Time", "Get Off My LAN" 
};

const char* rick_ssids[] = { 
    "Never gonna", "give you up", "Never gonna", "let you down", 
    "Never gonna", "run around", "and desert you" 
};

// --- PACKET BUILDER (Returns Packet Size) ---
static int buildBeacon(uint8_t* pkt, const char* ssid, uint16_t seq) {
  memset(pkt, 0, 128);
  pkt[0] = 0x80; pkt[1] = 0x00; // Frame Control
  for (int i = 4; i < 10; i++) pkt[i] = 0xFF; // Dest: Broadcast
  for (int i = 10; i < 16; i++) pkt[i] = random(256); // Random Src
  memcpy(pkt + 16, pkt + 10, 6); // BSSID = Src
  pkt[22] = (seq << 4) & 0xFF; pkt[23] = (seq >> 4) & 0xFF; // Seq
  
  // Timestamp (8 bytes) left as 0x00 is fine for spamming
  
  pkt[32] = 0x64; pkt[33] = 0x00; // Interval
  pkt[34] = 0x01; pkt[35] = 0x04; // Capabilities

  int len = strlen(ssid);
  pkt[36] = 0x00; pkt[37] = len; // SSID Tag ID & Len
  memcpy(pkt + 38, ssid, len);   // Copy SSID

  int pos = 38 + len;
  
  // Supported Rates
  const uint8_t rates[] = {0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x24, 0x30, 0x48, 0x6C};
  pkt[pos] = 0x01;      // Tag ID
  pkt[pos+1] = sizeof(rates); // Tag Len
  memcpy(pkt + pos + 2, rates, sizeof(rates));
  
  return pos + 2 + sizeof(rates); // Return total packet size
}

// --- SPAMMER LOGIC ---
void start_wifi_spam(int type) {
  // 1. Setup WiFi
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true); 

  // 2. Setup Sprite
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  uint8_t pkt[128];
  uint16_t seq = 0;
  int frame = 0;

  int funnyCount = sizeof(funny_ssids) / sizeof(funny_ssids[0]);
  int rickCount  = sizeof(rick_ssids) / sizeof(rick_ssids[0]);

  while (true) {
    M5.update();
    if (checkExit()) break; 

    // Channel hop 
    if (frame % 10 == 0) {
      esp_wifi_set_channel(random(1, 13), WIFI_SECOND_CHAN_NONE);
    }

    const char* ssid = (type == 0) ? funny_ssids[frame % funnyCount] : rick_ssids[frame % rickCount];
    
    // Build and Send (Using correct size)
    int packetSize = buildBeacon(pkt, ssid, seq++);
    esp_wifi_80211_tx(WIFI_IF_STA, pkt, packetSize, false); 

    // --- VISUAL UPDATE ---
    // Only redraw every 5 frames to save CPU for spamming
    if (frame % 5 == 0) {
        s.fillScreen(BLACK);
        
        // 1. Draw Header to Sprite
        drawHeaderSprite(s, "WIFI SPAM"); 
        
        // 2. Draw Status to Sprite (NOT M5.Display)
        s.setTextColor(WHITE);
        s.drawCenterString("Broadcasting...", 120, 40);

        s.fillRect(0, 70, 240, 30, BLACK); // Clear old text area
        s.setTextColor(ORANGE);
        s.drawCenterString(ssid, 120, 85);
        
        // 3. Push the whole frame
        s.pushSprite(0, 0);
    }

    frame++;
    delay(25); // Throttle
  }

  // Cleanup
  s.deleteSprite();
  esp_wifi_set_promiscuous(false);
  hardResetRadios(); // Ensure WiFi is clean for next app
}

void wifi_menu_loop() {
    M5Canvas s(&M5.Display);
    s.createSprite(240, 135);
    
    int cursor = 0;
    const char* options[] = { "Funny SSIDs", "Rick Roll" };
    bool dirty = true;

    while(true) {
        M5.update();
        
        if (M5.BtnB.pressedFor(1000)) {
            toneLite(500, 200);
            break;
        }
        
        if (M5.BtnB.wasPressed()) {
            cursor = !cursor;
            toneLite(1000, 20);
            dirty = true;
        }
        
        if (M5.BtnA.wasPressed()) {
            toneLite(2000, 50);
            
            // Delete sprite before leaving to save RAM
            s.deleteSprite();
            
            start_wifi_spam(cursor);
            
            // Recreate sprite when coming back
            s.createSprite(240, 135); 
            dirty = true;
            // Wait for button release so we don't auto-click things
            while(M5.BtnA.isPressed()) M5.update();
        }

        if (dirty) {
            s.fillScreen(BLACK);
            drawHeaderSprite(s, "WIFI SPAMMER"); // Replaced manual drawing
            
            for (int i = 0; i < 2; i++) {
                s.setTextColor(i == cursor ? GREEN : WHITE, BLACK);
                if (i == cursor) s.drawString(">", 20, 50 + (i * 30));
                s.drawString(options[i], 40, 50 + (i * 30));
            }
            
            s.setTextColor(GREY, BLACK);
            s.drawCenterString("A: Start   B: Next   Hold B: Exit", 120, 120);
            s.pushSprite(0, 0);
            dirty = false;
        }
        delay(20);
    }
    s.deleteSprite();
}