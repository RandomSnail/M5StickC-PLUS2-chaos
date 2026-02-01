#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h> 
#include "sys_core.h"
#include "apps.h"
#include "web_assets.h"      // <--- Make sure this is included for INDEX_HTML
#include "payload_library.h"
#include "ble_core.h"        // <--- Required for typeText()

// --- GLOBALS ---
WebServer dashServer(80);
Preferences wifiPrefs;

// --- WIFI SETUP HTML (Separate from Main Dashboard) ---
const char* WIFI_HTML = R"rawliteral(
<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>
<style>body{font-family:sans-serif;background:#222;color:#fff;text-align:center;padding:20px}
button{width:100%;padding:15px;margin:10px 0;background:#ff9900;border:none;color:white;font-size:16px;border-radius:5px}
input{width:100%;padding:10px;margin:5px 0;border-radius:5px;border:none;background:#ddd;color:#000}
.back{background:#555;margin-top:20px}</style></head>
<body><h2>WiFi Setup</h2>
<div id='list'>Scanning...</div>
<br><form action='/save' method='POST'>
<input type='text' name='ssid' id='ssid' placeholder='SSID'>
<input type='text' name='pass' placeholder='Password'>
<button type='submit'>Save & Connect</button></form>
<a href='/'><button class='back'>&larr; Back to Remote</button></a>
<script>
fetch('/scan_json').then(r=>r.json()).then(d=>{
  let h='';
  d.forEach(n=>{h+=`<button onclick="document.getElementById('ssid').value='${n.ssid}'">${n.ssid} (${n.rssi})</button>`});
  document.getElementById('list').innerHTML=h;
});
</script></body></html>
)rawliteral";


// --- HANDLERS ---

void handleRoot() {
  dashServer.send(200, "text/html", INDEX_HTML); // Loads your new Dashboard
}

void handleList() {
  String json = "[";
  for (int i = 0; i < payloadCount; i++) {
    if (i > 0) json += ",";
    json += "{\"id\":" + String(i) + ",";
    json += "\"name\":\"" + String(payloads[i].name) + "\",";
    json += "\"cat\":" + String(payloads[i].category) + "}";
  }
  json += "]";
  dashServer.send(200, "application/json", json);
}

void handleRun() {
  if (dashServer.hasArg("id")) {
    int id = dashServer.arg("id").toInt();
    if (id >= 0 && id < payloadCount) {
      dashServer.send(200, "text/plain", payloads[id].name);
      // Run the payload
      payloads[id].execute();
      log_event("WEB", payloads[id].name);
    } else {
      dashServer.send(400, "text/plain", "Invalid ID");
    }
  } else {
    dashServer.send(400, "text/plain", "Missing ID");
  }
}

void handleStats() {
  String json = "{";
  json += "\"bat\":" + String(getBatteryPercent()) + ",";
  json += "\"ram\":" + String(ESP.getFreeHeap() / 1024) + ",";
  json += "\"uptime\":\"" + String(millis() / 60000) + "m\"";
  json += "}";
  dashServer.send(200, "application/json", json);
}

void handleLogs() {
  prefs.begin("logs", true);
  int count = prefs.getInt("count", 0);
  String logText = "";
  int start = (count > 10) ? count - 10 : 0;
  for (int i = 0; i < 10; i++) {
     int idx = (start + i) % 10;
     String entry = prefs.getString(("evt_" + String(idx)).c_str(), "");
     if (entry != "") logText += entry + "\n";
  }
  prefs.end();
  if (logText == "") logText = "No logs found.";
  dashServer.send(200, "text/plain", logText);
}

// --- NEW FEATURES HANDLERS ---

void handleType() {
  if (dashServer.hasArg("msg")) {
    String msg = dashServer.arg("msg");
    // Needs ble_core.h included at the top
    typeText(msg.c_str()); 
    dashServer.send(200, "text/plain", "OK");
  } else {
    dashServer.send(400, "text/plain", "No Msg");
  }
}

void handleReboot() {
  dashServer.send(200, "text/plain", "Rebooting...");
  delay(100);
  ESP.restart();
}

// --- WIFI CONFIG HANDLERS ---

void handleSetupPage() {
  dashServer.send(200, "text/html", WIFI_HTML);
}

void handleScanJson() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  dashServer.send(200, "application/json", json);
}

void handleSaveWifi() {
  String ssid = dashServer.arg("ssid");
  String pass = dashServer.arg("pass");
  if (ssid.length() > 0) {
    wifiPrefs.begin("wifi_creds", false);
    wifiPrefs.putString("ssid", ssid);
    wifiPrefs.putString("pass", pass);
    wifiPrefs.end();
    dashServer.send(200, "text/html", "Saved! Rebooting... <meta http-equiv='refresh' content='2;url=/'>");
    delay(1000);
    ESP.restart();
  } else {
    dashServer.send(400, "text/plain", "SSID Missing");
  }
}

// --- MAIN LOOP ---

void web_interface_loop() {
  hardResetRadios();
  
  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  // Ensure BLE is ready for the "Live Injector" to work
  // Note: We are NOT calling startBleHid() here because it might conflict with WiFi radio usage
  // However, for typeText() to work, the HID driver needs to be initialized. 
  // If connection is unstable, you may need to choose: WiFi Remote OR Bluetooth Remote.
  
  // 1. Initialize Mode: AP + STATION
  WiFi.mode(WIFI_AP_STA);
  
  // 2. Start AP (Always Available Backup)
  WiFi.softAP("M5-Chaos", "m5stack123");
  
  // 3. Try Connect to Saved WiFi
  wifiPrefs.begin("wifi_creds", true);
  String savedSSID = wifiPrefs.getString("ssid", "");
  String savedPass = wifiPrefs.getString("pass", "");
  wifiPrefs.end();
  
  if (savedSSID != "") {
    s.fillScreen(BLACK);
    s.setTextColor(ORANGE);
    s.drawCenterString("Connecting to WiFi...", 120, 60);
    s.pushSprite(0,0);
    
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 3500) {
      delay(100);
    }
  }

  // 4. Register Routes
  dashServer.on("/", handleRoot);
  dashServer.on("/list", handleList);
  dashServer.on("/run", handleRun);
  dashServer.on("/stats", handleStats);
  dashServer.on("/logs", handleLogs);
  
  // New Handlers
  dashServer.on("/type", handleType);
  dashServer.on("/reboot", handleReboot);
  
  // Wifi Config Handlers
  dashServer.on("/setup", handleSetupPage);
  dashServer.on("/scan_json", handleScanJson);
  dashServer.on("/save", handleSaveWifi);
  
  dashServer.begin();

  while (true) {
    M5.update();
    if (checkExit()) break;
    
    dashServer.handleClient();
    
    // UI Update
    if (millis() % 1000 == 0) {
        s.fillScreen(BLACK);
        s.fillRect(0, 0, 240, 28, THEME_COLOR);
        s.setTextColor(BLACK, THEME_COLOR);
        s.drawCenterString("WEB REMOTE", 120, 7);
        
        s.setTextColor(WHITE, BLACK);
        s.setTextSize(1);
        
        s.setCursor(5, 40);
        s.printf("AP: M5-Chaos  (192.168.4.1)");
        
        s.setCursor(5, 60);
        if (WiFi.status() == WL_CONNECTED) {
          s.setTextColor(GREEN, BLACK);
          s.printf("WiFi: %s", WiFi.localIP().toString().c_str());
        } else {
          s.setTextColor(RED, BLACK);
          s.printf("WiFi: Not Connected");
        }

        s.setTextColor(ORANGE, BLACK);
        s.drawCenterString("Remote Active", 120, 110);
        
        s.pushSprite(0, 0);
    }
  }
  
  dashServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  hardResetRadios();
  s.deleteSprite();
}