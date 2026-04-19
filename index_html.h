const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body {
  font-family: Arial;
  text-align: center;
  background: #111;
  color: white;
}

.card {
  background: #1e1e1e;
  padding: 20px;
  border-radius: 20px;
  width: 300px;
  margin: auto;
  box-shadow: 0 0 20px rgba(0,0,0,0.5);
}

.temp {
  font-size: 48px;
  margin: 10px 0;
}

.temp-container {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 12px;
}

.led {
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background-color: #555;
  box-shadow: 0 0 5px #000 inset;
  transition: background-color 0.3s, box-shadow 0.3s;
}

.led.on {
  background-color: #00ff00;
  box-shadow: 0 0 12px #00ff00;
}

input[type=range] {
  width: 100%;
}

button {
  padding: 10px 20px;
  margin: 5px;
  border-radius: 10px;
  border: none;
  font-size: 16px;
  cursor: pointer;
}

.cool { background: #007bff; color: white; }
.heat { background: #ff5722; color: white; }
.off  { background: #555; color: white; }

.mode {
  margin-top: 10px;
  font-size: 18px;
}

.switch {
  position: relative;
  display: inline-block;
  width: 50px;
  height: 26px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  background-color: #555;
  border-radius: 34px;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  transition: .3s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 20px;
  width: 20px;
  left: 3px;
  bottom: 3px;
  background-color: white;
  border-radius: 50%;
  transition: .3s;
}

input:checked + .slider {
  background-color: #4CAF50;
}

input:checked + .slider:before {
  transform: translateX(24px);
}

/* Form WiFi */
#wifiForm {
  display: none;
  margin-top: 15px;
  text-align: left;
}

#wifiForm input[type=text], #wifiForm input[type=password] {
  width: 100%;
  padding: 5px;
  margin: 5px 0;
  border-radius: 5px;
  border: none;
}

#wifiForm button {
  width: 100%;
  background-color: #4CAF50;
  color: white;
  font-size: 16px;
}

.showBtn {
  margin-top: 10px;
  padding: 5px 10px;
  border-radius: 5px;
  border: none;
  background-color: #777;
  color: white;
  cursor: pointer;
}

.tabs {
  display: flex;
  justify-content: center;
  margin-bottom: 15px;
}

.tabBtn {
  flex: 1;
  padding: 10px;
  background: #333;
  border: none;
  color: white;
  cursor: pointer;
  border-radius: 10px 10px 0 0;
}

.tabBtn.active {
  background: #4CAF50;
}

.tabContent {
  display: none;
}

.tabContent.active {
  display: block;
}

.inline-setting {
  display: flex;
  gap: 10px;
  align-items: center;
  margin-top: 5px;
}

.inline-setting input {
  width: 40px;
  padding: 6px;
  border-radius: 8px;
  border: none;
}

.inline-setting button {
  padding: 6px 10px;
  border-radius: 8px;
  font-size: 14px;
}
@keyframes spin {
  from { transform: rotate(0deg);}
  to { transform: rotate(360deg);}
}

#fanIcon {
  width: 24px;
  height: 24px;
}

#fanIcon.spin {
  animation: spin 1s linear infinite;
  transform-origin: 50% 50%; /* fondamentale per far ruotare intorno al centro */
}

#fanBlades {
  transition: fill 0.3s;
}

#fanBlades.on {
  fill: #00ccff; /* tutte e 3 le pale diventano celesti */
}

@keyframes spin {
  from { transform: rotate(0deg);}
  to { transform: rotate(360deg);}
}

.setting-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  margin-top: 10px;
}

.label {
  flex: 1;
  text-align: left;
}

.setting-box {
  background: #1a1a1a;
  padding: 12px;
  border-radius: 15px;
  margin-top: 12px;
  box-shadow: 0 0 10px rgba(0,0,0,0.4);
}

.setting-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.setting-inputs {
  display: flex;
  gap: 10px;
  margin-top: 10px;
  justify-content: space-between;
}

.input-group {
  display: flex;
  flex-direction: column;
  align-items: center;
  flex: 1;
}

.input-group label {
  font-size: 12px;
  margin-bottom: 3px;
  color: #aaa;
}

.input-group input {
  width: 60%;
  padding: 6px;
  border-radius: 8px;
  border: none;
  text-align: center;
}
</style>
<script type="text/javascript" language="javascript">
document.addEventListener("DOMContentLoaded", function() {

  var ws = new WebSocket("ws://" + location.host + "/ws");

  ws.onopen = function() {
    console.log("WS connected");
  };

  ws.onmessage = function(event) {
    var data = JSON.parse(event.data);
    document.getElementById("temp").innerText = data.temp;
    document.getElementById("setval").innerText = data.set;
    document.getElementById("mode").innerText = data.mode;
    document.getElementById("slider").value = data.set;
    document.getElementById("autoToggle").checked = data.auto;
    var coolOffsetInput = document.getElementById("coolOffsetInput");
    if (document.activeElement !== coolOffsetInput) {
        coolOffsetInput.value = data.coolOffset;
    }
    var heatOffsetInput = document.getElementById("heatOffsetInput");
    if (document.activeElement !== heatOffsetInput) {
        heatOffsetInput.value = data.heatOffset;
    }

    var coolPwmInput = document.getElementById("coolPwmInput");
    if (document.activeElement !== coolPwmInput) {
        coolPwmInput.value = data.coolPwm;
    }
    var heatPwmInput = document.getElementById("heatPwmInput");
    if (document.activeElement !== heatPwmInput) {
        heatPwmInput.value = data.heatPwm;
    }    

    var text = document.getElementById("peltierText");
    var led = document.getElementById("peltierLed");
    if (data.peltier) {
      led.classList.add("on");
    } else {
        led.classList.remove("on");
    }

    var fanIcon = document.getElementById("fanIcon");
    if (data.fan) {
        fanBlades.classList.add("on");
      fanIcon.classList.add("spin");
    } else {
        fanBlades.classList.remove("on");
      fanIcon.classList.remove("spin");
    }

    var fanCoolToggle = document.getElementById("fanCoolAutoToggle");
    var fanCoolInput  = document.getElementById("fanCoolTimeInput");
    var fanCoolInterTimeInput = document.getElementById("fanCoolInterTimeInput");
    var fanCoolInterDiv = document.getElementById("fanCoolInputs");

    // FAN COOL
    fanCoolToggle.checked = data.fanCoolAuto;

    fanCoolInterDiv.style.display = data.fanCoolAuto ? "flex" : "none";

    if (document.activeElement !== fanCoolInput)
      fanCoolInput.value = data.fanCoolTime;

    if (document.activeElement !== fanCoolInterTimeInput)
      fanCoolInterTimeInput.value = data.fanCoolInterTime;
    

    // FAN HEAT
    var fanHeatToggle = document.getElementById("fanHeatAutoToggle");
    var fanHeatInput  = document.getElementById("fanHeatTimeInput");
    var fanHeatInterTimeInput = document.getElementById("fanHeatInterTimeInput");
    var fanHeatInterDiv = document.getElementById("fanHeatInputs");
    fanHeatToggle.checked = data.fanHeatAuto;

    fanHeatInterDiv.style.display = data.fanHeatAuto ? "flex" : "none";

    if (document.activeElement !== fanHeatInput)
      fanHeatInput.value = data.fanHeatTime;

    if (document.activeElement !== fanHeatInterTimeInput)
      fanHeatInterTimeInput.value = data.fanHeatInterTime;
    
  };

  document.getElementById("slider").addEventListener("change", function() {
    var val = parseFloat(this.value).toFixed(1);
    document.getElementById("setval").innerText = val;
    ws.send("SET:" + val);
  });

  document.querySelector(".cool").addEventListener("click", function() {
    ws.send("MODE:FREDDO");
  });

  document.querySelector(".heat").addEventListener("click", function() {
    ws.send("MODE:CALDO");
  });

  document.getElementById("autoToggle").addEventListener("change", function() {
    if (this.checked)
      ws.send("AUTO:ON");
    else
      ws.send("AUTO:OFF");
  });

  // Toggle form WiFi
  document.getElementById("showWifiForm").addEventListener("click", function(){
    var f = document.getElementById("wifiForm");
    if(f.style.display === "none")
      f.style.display = "block";
    else
      f.style.display = "none";
  });

  // Submit WiFi form
  document.getElementById("wifiForm").addEventListener("submit", function(e){
    e.preventDefault();
    var ssid = document.getElementById("ssidInput").value;
    var pass = document.getElementById("passInput").value;
    ws.send("WIFI:" + ssid + "|" + pass);
    alert("WiFi salvato, il dispositivo si riavvierà");
  });

  // GESTIONE TAB
  document.querySelectorAll(".tabBtn").forEach(btn => {
    btn.addEventListener("click", function() {
      // reset bottoni
      document.querySelectorAll(".tabBtn").forEach(b => b.classList.remove("active"));
      this.classList.add("active");

      // nascondi contenuti
      document.querySelectorAll(".tabContent").forEach(t => t.classList.remove("active"));

      // mostra tab selezionata
      document.getElementById(this.dataset.tab).classList.add("active");
      });
  });

  document.getElementById("coolOffsetInput").addEventListener("change", function(){
    var offset = parseFloat(this.value).toFixed(2);
    ws.send("COOL_OFFSET:" + offset);
  });

  document.getElementById("heatOffsetInput").addEventListener("change", function(){
    var offset = parseFloat(this.value).toFixed(2);
    ws.send("HEAT_OFFSET:" + offset);
  });

  document.getElementById("coolPwmInput").addEventListener("change", function(){
    var pwm = parseFloat(this.value).toFixed(2);
    ws.send("COOL_PWM:" + pwm);
  });

  document.getElementById("heatPwmInput").addEventListener("change", function(){
    var pwm = parseFloat(this.value).toFixed(2);
    ws.send("HEAT_PWM:" + pwm);
  });

  // Fan Heat Auto toggle
  document.getElementById("fanHeatAutoToggle").addEventListener("change", function(){
      if(this.checked)
          ws.send("FANHEAT_AUTO:ON");
      else
          ws.send("FANHEAT_AUTO:OFF");
  });

  // Fan Cool Auto toggle
  document.getElementById("fanCoolAutoToggle").addEventListener("change", function(){
      if(this.checked)
          ws.send("FANCOOL_AUTO:ON");
      else
          ws.send("FANCOOL_AUTO:OFF");
  });

  var fanCoolInputs = document.getElementById("fanCoolInputs");
  var fanCoolInput  = document.getElementById("fanCoolTimeInput");
  var fanCoolInterTimeInput  = document.getElementById("fanCoolInterTimeInput");

  var fanHeatInput  = document.getElementById("fanHeatTimeInput");
  var fanHeatInterTimeInput  = document.getElementById("fanHeatInterTimeInput");

  fanCoolInput.addEventListener("change", function(){
    ws.send("FANCOOL_TIME:" + this.value);
  });

  fanHeatInput.addEventListener("change", function(){
    ws.send("FANHEAT_TIME:" + this.value);
  });

  fanCoolInterTimeInput.addEventListener("change", function(){
    ws.send("FANCOOL_INTERTIME:" + this.value);
  });

  fanHeatInterTimeInput.addEventListener("change", function(){
    ws.send("FANHEAT_INTERTIME:" + this.value);
  });
});
</script>
</head>

<body>
<div class="card">
  <h2>MiniFrigo Lievito Madre</h2>
  <div style="font-size:12px; color:#777; margin-top:-8px; margin-bottom:10px;">
    Firmware v2.0.0
  </div>

<!-- TAB HEADER -->
<div class="tabs">
  <button class="tabBtn active" data-tab="mainTab">Controllo</button>
  <button class="tabBtn" data-tab="settingsTab">Impostazioni</button>
</div>

<!-- TAB 1 -->
<div id="mainTab" class="tabContent active">

  <div class="temp-container">
    <div class="temp">
      <span id="temp">--</span> °C
    </div>
    <div id="peltierLed" class="led"></div>

      <!-- Ventola 3 pale simmetriche -->
  <!-- Ventola 3 pale più grandi e colore uniforme -->
  <svg id="fanIcon" width="60" height="60" viewBox="0 0 24 24">
      <circle cx="12" cy="12" r="2" fill="#111"/> <!-- centro ventola -->

      <g id="fanBlades" fill="#555"> <!-- tutte le pale nello stesso gruppo -->
        <path d="M12 2 L15 12 L12 12 Z"/> <!-- pala 1 -->
        <path d="M12 2 L15 12 L12 12 Z" transform="rotate(120 12 12)"/> <!-- pala 2 -->
        <path d="M12 2 L15 12 L12 12 Z" transform="rotate(240 12 12)"/> <!-- pala 3 -->
      </g>
    </svg>
  </div>

  <p>Set: <span id="setval">25</span> °C</p>
  <input type="range" min="5" max="40" step="0.1" id="slider">

  <div>
    <button class="cool">Freddo</button>
    <button class="heat">Caldo</button>
  </div>

  <div class="mode">
    Modalità: <span id="mode">--</span>
  </div>

  <br>

  <div class="switch-container">
    <label class="switch">
      <input type="checkbox" id="autoToggle">
      <span class="slider"></span>
    </label>
    <div style="margin-top:8px;">Auto</div>
  </div>

</div>

  <!-- TAB 2 -->
  <div id="settingsTab" class="tabContent">

    <h3>WiFi</h3>

    <button class="showBtn" id="showWifiForm">Modifica WiFi</button>

    <form id="wifiForm">
      <label>SSID:</label>
      <input type="text" id="ssidInput">

      <label>Password:</label>
      <input type="password" id="passInput">

      <button type="submit">Salva & Riavvia</button>
    </form>

    <hr style="margin:15px 0; border-color:#333;">

    <h3>Parametri</h3>

    
    <div class="setting-box">
      <div class="setting-header">
        <span>Offset Temperature</span>
      </div>

      <div class="setting-inputs">
        <div class="input-group">
          <label>FREDDO (°C)</label>
          <input type="number" id="coolOffsetInput" step="0.1">
        </div>

        <div class="input-group">
          <label>CALDO (°C)</label>
          <input type="number" id="heatOffsetInput" step="0.1">
        </div>
      </div>
    </div>

    <!-- FAN COOL AUTO -->
    <div class="setting-box">
      <div class="setting-header">
        <span>Fan Cool Auto</span>

        <label class="switch">
          <input type="checkbox" id="fanCoolAutoToggle">
          <span class="slider"></span>
        </label>
      </div>

      <div class="setting-inputs" id="fanCoolInputs">
        <div class="input-group">
          <label>ACTIVE (s)</label>
          <input type="number" id="fanCoolTimeInput">
        </div>

        <div class="input-group">
          <label>INTER TIME (s)</label>
          <input type="number" id="fanCoolInterTimeInput">
        </div>
      </div>
    </div>

    <!-- FAN HEAT AUTO -->
    <div class="setting-box">
      <div class="setting-header">
        <span>Fan Heat Auto</span>

        <label class="switch">
          <input type="checkbox" id="fanHeatAutoToggle">
          <span class="slider"></span>
        </label>
      </div>

      <div class="setting-inputs" id="fanHeatInputs">
        <div class="input-group">
          <label>ACTIVE (s)</label>
          <input type="number" id="fanHeatTimeInput">
        </div>

        <div class="input-group">
          <label>INTER TIME (s)</label>
          <input type="number" id="fanHeatInterTimeInput">
        </div>
      </div>
    </div>


    <div class="setting-box">
      <div class="setting-header">
        <span>Potenza PWM</span>
      </div>

      <div class="setting-inputs">
        <div class="input-group">
          <label>FREDDO (%)</label>
          <input type="number" id="coolPwmInput" min="50" max="100" step="1">
        </div>

        <div class="input-group">
          <label>CALDO (%)</label>
          <input type="number" id="heatPwmInput" min="50" max="100" step="1">
        </div>
      </div>
    </div>
  </div>
</div>
</body>
</html>
)rawliteral";