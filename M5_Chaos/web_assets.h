#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>M5 CHAOS DECK</title>
  <style>
    body { background-color: #121212; color: #00FF00; font-family: 'Courier New', monospace; text-align: center; margin: 0; padding: 20px; }
    h1 { border-bottom: 2px solid #00FF00; padding-bottom: 10px; text-shadow: 0 0 10px #00FF00; }
    
    /* Cards */
    .card { background: #1E1E1E; border: 1px solid #333; border-radius: 8px; padding: 15px; margin-bottom: 20px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }
    
    /* Buttons */
    .btn { background: #004400; color: #00FF00; border: 1px solid #00FF00; padding: 12px 20px; margin: 5px; border-radius: 4px; cursor: pointer; font-size: 16px; width: 100%; max-width: 300px; transition: 0.2s; }
    .btn:active { background: #00FF00; color: #000; box-shadow: 0 0 15px #00FF00; }
    
    /* Danger Buttons (Red) */
    .btn-dang { border-color: #FF0000; color: #FF0000; background: #440000; }
    .btn-dang:active { background: #FF0000; color: #000; }

    /* System Buttons (Cyan) */
    .btn-sys { border-color: #00FFFF; color: #00FFFF; background: #004444; }
    .btn-sys:active { background: #00FFFF; color: #000; }

    /* Inputs */
    .input-box { width: 70%; padding: 10px; background: #000; color: #00FF00; border: 1px solid #333; border-radius: 4px; margin-bottom: 10px; font-family: inherit; }

    .stat-box { display: flex; justify-content: space-around; font-size: 14px; color: #AAA; }
    #log-output { width: 100%; height: 150px; background: #000; border: 1px solid #333; color: #CCC; font-size: 12px; padding: 5px; overflow-y: scroll; text-align: left; }
    a { text-decoration: none; }
  </style>
</head>
<body>
  <h1>M5 CHAOS REMOTE</h1>
  
  <div class="card">
    <div class="stat-box">
      <span id="bat">BAT: --%</span>
      <span id="uptime">UP: --:--</span>
      <span id="ram">RAM: --KB</span>
    </div>
  </div>

  <div class="card">
    <h3>LIVE INJECTOR</h3>
    <input type="text" id="liveText" class="input-box" placeholder="Type to inject...">
    <button class="btn" onclick="sendLiveText()">SEND KEYSTROKES</button>
  </div>

  <div class="card">
    <h3>PAYLOAD LAUNCHER</h3>
    <div id="payload-list">Loading payloads...</div>
  </div>

  <div class="card">
    <h3>SYSTEM LOGS</h3>
    <div id="log-output">Connecting to logs...</div>
    <br>
    <button class="btn" onclick="fetchLogs()">Refresh Logs</button>
  </div>

  <div class="card">
    <h3>DEVICE CONTROL</h3>
    <a href="/setup">
      <button class="btn btn-sys">⚙️ WiFi Setup</button>
    </a>
    <button class="btn btn-sys" onclick="rebootDev()">⚠️ Reboot Device</button>
  </div>

<script>
  // --- Payload Functions ---
  function runPayload(id) {
    fetch('/run?id=' + id)
      .then(r => r.text())
      .then(t => console.log("Executed: " + t));
  }

  // --- Live Injector ---
  function sendLiveText() {
    var t = document.getElementById('liveText').value;
    if(!t) return;
    fetch('/type?msg=' + encodeURIComponent(t))
      .then(() => {
         document.getElementById('liveText').value = ""; // Clear input
         alert("Sent!");
      });
  }

  // --- Stats & Logs ---
  function fetchStats() {
    fetch('/stats').then(r => r.json()).then(d => {
      document.getElementById('bat').innerText = "BAT: " + d.bat + "%";
      document.getElementById('uptime').innerText = "UP: " + d.uptime;
      document.getElementById('ram').innerText = "RAM: " + d.ram + "KB";
    });
  }

  function fetchLogs() {
    fetch('/logs').then(r => r.text()).then(t => {
      let logBox = document.getElementById('log-output');
      logBox.innerHTML = t.replace(/\n/g, "<br>");
      logBox.scrollTop = logBox.scrollHeight; // Auto-scroll to bottom
    });
  }
  
  function loadButtons() {
    fetch('/list').then(r => r.json()).then(d => {
      let html = "";
      d.forEach(p => {
        let cls = (p.cat === 1) ? "btn btn-dang" : "btn"; // 1 = PRANK
        html += `<button class="${cls}" onclick="runPayload(${p.id})">${p.name}</button><br>`;
      });
      document.getElementById('payload-list').innerHTML = html;
    });
  }

  // --- System Functions ---
  function rebootDev() {
    if(confirm("Are you sure you want to reboot the M5?")) {
        fetch('/reboot').then(() => alert("Rebooting... Connection lost."));
    }
  }

  setInterval(fetchStats, 5000);
  loadButtons();
  fetchStats();
  fetchLogs();
</script>
</body>
</html>
)rawliteral";