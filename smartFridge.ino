#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
#include "wifiData.h"
#include "btsDriver.h"
#include "index_html.h"

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static AsyncWebServer server(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

WifiData wifiData;
BtsDriver btsModule = BtsDriver(DEFAULT_LEDC_RESOLUTION);
mode workingMode;
volatile bool autoMode;

float currentTemp = 0;
float setTemp;
String mode;
bool wifiConnected = false;
bool fanStateView = false;

unsigned long temperatureReadingTime = 0;
unsigned long fanInterTime = 0;
unsigned long fanTime = 0;
unsigned long lastWifiCheck = 0;

void applyMode() {
  if (mode == "OFF")
    btsModule.allOffWithStore();
  else if (mode == "FREDDO") {
    btsModule.changeMode(FREDDO);
  } else if (mode == "CALDO") {
    btsModule.changeMode(CALDO);
  }
  
  workingMode = btsModule.getState();
}

void setup() {
  uint8_t wifiTryCounter = 0;
  //Serial.begin(115200);
  btsModule.setAsOutputPin();
  btsModule.allOff();
  btsModule.loadCoolTemperatureOffset();
  btsModule.loadHeatTemperatureOffset();
  btsModule.loadCoolPwm();
  btsModule.loadHeatPwm();
  btsModule.loadFanCoolAuto();
  btsModule.loadFanHeatAuto();
  btsModule.loadFanHeatAutoTime();
  btsModule.loadFanCoolAutoTime();
  btsModule.loadFanHeatAutoInterTime();
  btsModule.loadFanCoolAutoInterTime();
  delay(1000);

  ArduinoOTA.setHostname("miniFrigo");
  ArduinoOTA.setPassword("69860");
  ArduinoOTA
  .onStart([]() {
    ////Serial.println("OTA: Start");
  })
  .onEnd([]() {
    ////Serial.println("\nOTA: End");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    ////Serial.printf("Progress: %u%%\r", (progress * 100) / total);
  })
  .onError([](ota_error_t error) {
    ////Serial.printf("Error[%u]\n", error);
  });

  
  wifiData.load();
  if (!wifiData.isValidWifi()){
    //startAccessPointMode();
    WiFi.mode(WIFI_AP);
    WiFi.softAP("MiniFrigo-Setup");

    IPAddress IP = WiFi.softAPIP();
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiData.getSsid(), wifiData.getPassword());
    
    while (WiFi.status() != WL_CONNECTED)
    {
      if (wifiTryCounter >= 9)
        break;
      wifiTryCounter++;
      delay(500);
    }
    wifiConnected = (WiFi.status() == WL_CONNECTED);
    if (!wifiConnected)
    {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("MiniFrigo-Setup");

      IPAddress IP = WiFi.softAPIP();
      //startAccessPointMode();
      //return;
    }
    else
      ArduinoOTA.begin();
  }

  workingMode = btsModule.loadStoredState();
  if (workingMode == OFF)
    mode = "OFF";
  else if (workingMode == CALDO)
    mode = "CALDO";
  else if (workingMode == FREDDO)
    mode = "FREDDO";

  autoMode = btsModule.loadAutoMode();
  setTemp = btsModule.loadSetTemp();

  sensors.begin();  

  wsHandler.onConnect([](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    ////Serial.printf("Client %" PRIu32 " connected\n", client->id());

    // manda stato iniziale
    String json = "{";
    if (currentTemp < -20) {
      json += "\"temp\":\"--\",";
    } else {
      json += "\"temp\":" + String(currentTemp, 2) + ",";
    }
    json += "\"coolOffset\":" + String(btsModule.coolOffsetTemp, 2) + ",";
    json += "\"heatOffset\":" + String(btsModule.heatOffsetTemp, 2) + ",";
    json += "\"set\":" + String(setTemp, 2) + ",";
    json += "\"auto\":" + String(autoMode ? "true" : "false") + ",";
    json += "\"fanHeatAuto\":" + String(btsModule.fanHeatAuto ? "true" : "false") + ",";
    json += "\"fanCoolAuto\":" + String(btsModule.fanCoolAuto ? "true" : "false") + ",";
    json += "\"fanCoolTime\":" + String(btsModule.fanCoolAutoTime) + ",";
    json += "\"fanHeatTime\":" + String(btsModule.fanHeatAutoTime) + ",";
    json += "\"fanCoolInterTime\":" + String(btsModule.fanCoolAutoInterTime) + ",";
    json += "\"fanHeatInterTime\":" + String(btsModule.fanHeatAutoInterTime) + ",";
    json += "\"mode\":\"" + mode + "\"";
    json += "}";

    client->text(json);
  });

  wsHandler.onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
    String msg = "";
    for (size_t i = 0; i < len; i++)
      msg += (char)data[i];

    if(msg.startsWith("WIFI:")){
      int sep = msg.indexOf('|');
      String newSsid = msg.substring(5, sep);
      String newPass = msg.substring(sep+1);
      wifiData.setSsid(newSsid);
      wifiData.setPassword(newPass);
      ESP.restart();
    }

    if (msg.startsWith("SET:")) {
      setTemp = msg.substring(4).toFloat();
      btsModule.changeSetTemp(setTemp);
    }

    if (msg == "MODE:FREDDO") {
      mode = "FREDDO";
    } else if (msg == "MODE:CALDO") {
      mode = "CALDO";
    }

    if (msg == "AUTO:ON") {
      btsModule.changeAutoMode(true);
      autoMode = btsModule.getAutoMode();
    }
    else if (msg == "AUTO:OFF") {
      btsModule.changeAutoMode(false);
      autoMode = btsModule.getAutoMode();
    }

    if (msg.startsWith("COOL_OFFSET:")) {
      float newOffset = msg.substring(12).toFloat();
      btsModule.changeCoolTemperatureOffset(newOffset);
    }

    if (msg.startsWith("HEAT_OFFSET:")) {
      float newOffset = msg.substring(12).toFloat();
      btsModule.changeHeatTemperatureOffset(newOffset);
    }

    // PWM
    if (msg.startsWith("COOL_PWM:")) {
      uint8_t newPwm = msg.substring(9).toInt();
      btsModule.changeCoolPwm(newPwm);
    }

    if (msg.startsWith("HEAT_PWM:")) {
      uint8_t newPwm = msg.substring(9).toInt();
      btsModule.changeHeatPwm(newPwm);
    }
    // *PWM

    if (msg.startsWith("FANHEAT_AUTO:ON")) {
      btsModule.changeFanHeatAuto(true);
    } else if (msg.startsWith("FANHEAT_AUTO:OFF")) {
      btsModule.changeFanHeatAuto(false);
    }

    if (msg.startsWith("FANCOOL_AUTO:ON")) {
      btsModule.changeFanCoolAuto(true);
    } else if (msg.startsWith("FANCOOL_AUTO:OFF")) {
      btsModule.changeFanCoolAuto(false);
    }

    if (msg.startsWith("FANCOOL_TIME:")) {
      unsigned long newFanCoolTime = msg.substring(13).toInt();
      btsModule.changeFanCoolAutoTime(newFanCoolTime);
    }

    if (msg.startsWith("FANHEAT_TIME:")) {
      unsigned long newFanHeatTime = msg.substring(13).toInt();
      btsModule.changeFanHeatAutoTime(newFanHeatTime);
    }

    if (msg.startsWith("FANCOOL_INTERTIME:")) {
      unsigned long newFanCoolnterTime = msg.substring(18).toInt();
      btsModule.changeFanCoolAutoInterTime(newFanCoolnterTime);
    }

    if (msg.startsWith("FANHEAT_INTERTIME:")) {
      unsigned long newFanHeatInterTime = msg.substring(18).toInt();
      btsModule.changeFanHeatAutoInterTime(newFanHeatInterTime);
    }
    
    applyMode();

    // Notifica tutto
    String json = "{";
    if (currentTemp < -20) {
      json += "\"temp\":\"--\",";
    } else {
      json += "\"temp\":" + String(currentTemp, 2) + ",";
    }
    json += "\"set\":" + String(setTemp, 2) + ",";
    json += "\"coolOffset\":" + String(btsModule.coolOffsetTemp, 2) + ",";
    json += "\"heatOffset\":" + String(btsModule.heatOffsetTemp, 2) + ",";
    json += "\"coolPwm\":" + String(btsModule.coolPwm) + ",";
    json += "\"heatPwm\":" + String(btsModule.heatPwm) + ",";
    json += "\"auto\":" + String(autoMode ? "true" : "false") + ",";
    json += "\"peltier\":" + String(btsModule.getPeltierState() ? "true" : "false") + ",";
    json += "\"fan\":" + String(fanStateView ? "true" : "false") + ",";
    json += "\"fanHeatAuto\":" + String(btsModule.fanHeatAuto ? "true" : "false") + ",";
    json += "\"fanCoolAuto\":" + String(btsModule.fanCoolAuto ? "true" : "false") + ",";
    json += "\"fanCoolTime\":" + String(btsModule.fanCoolAutoTime) + ",";
    json += "\"fanHeatTime\":" + String(btsModule.fanHeatAutoTime) + ",";
    json += "\"fanCoolInterTime\":" + String(btsModule.fanCoolAutoInterTime) + ",";
    json += "\"fanHeatInterTime\":" + String(btsModule.fanHeatAutoInterTime) + ",";
    json += "\"mode\":\"" + mode + "\"";
    json += "}";
    server->textAll(json);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html; charset=utf-8", index_html);
  });

  server.addHandler(&ws);
  server.begin();
}

void loop() 
{
  ws.cleanupClients();
  if (wifiConnected)
    ArduinoOTA.handle();

  if (millis() - temperatureReadingTime > 500) {
    sensors.requestTemperatures();
    currentTemp = sensors.getTempCByIndex(0);
    temperatureReadingTime = millis();

    if (currentTemp < -20) {
      sensors.begin();
      btsModule.allOff();
    }
    else {
      if (autoMode) {
        //Serial.println("# AUTO MODE ON");
        switch(workingMode){
          case CALDO:
              //Serial.print("* CASE CALDO. Heat Offset: ");
              //Serial.println(btsModule.heatOffsetTemp);
              if (currentTemp < (setTemp - btsModule.heatOffsetTemp) ) {
                if (btsModule.getFanState()) {
                  btsModule.setFanOff();
                  btsModule.allOff();
                }
                fanStateView = true; // Fai vedere che gira (il relè della massa è spento, visto che si è in HEAT)
                btsModule.HeatOn();
                btsModule.generaliOn();
              } else if (currentTemp >= setTemp) {
                  if (btsModule.getPeltierState())
                    btsModule.allOff();
                }                
            break;
          case FREDDO:
              //Serial.print("* CASE FREDDO. Cool Offset: ");
              //Serial.println(btsModule.coolOffsetTemp);
              if (currentTemp > (setTemp + btsModule.coolOffsetTemp) ) {
                if (btsModule.getFanState()) {
                  btsModule.setFanOff();
                  btsModule.allOff();
                }
                //Serial.println("** (currentTemp > (setTemp + btsModule.offsetTemp)");
                fanStateView = true; // Fai vedere la ventola che gira (il relè della massa è attivo)
                btsModule.ColdOn();
                btsModule.generaliOn();
                //Serial.println("** ColdOn() e generaliOn()");
              } else if (currentTemp <= setTemp) {
                //Serial.println("** (currentTemp <= setTemp)");
                if (btsModule.getPeltierState())
                {
                  //Serial.println("*** getPeltierState è true --> allOff()");
                  btsModule.allOff();
                }
              }
            break;
          case OFF:
              ////Serial.println("CASE OFF");
              btsModule.allOff();
              break;
          default:
            break;
        }
      }
      else{
        //Serial.println("# AUTO MODE OFF");
        if (btsModule.getFanState() || fanStateView)
        {
          if (btsModule.getFanState())
            //Serial.println("## btsModule.getFanState() è true");
          if (fanStateView)
            //Serial.println("## fanStateView è true");
          if (btsModule.getPeltierState()) {
            //Serial.println("### btsModule.getPeltierState() è true --> allOff()");
            btsModule.allOff();
          }
          //Serial.println("## setFanOn() e || generaleGroundOn()");
          btsModule.setFanOn();
          btsModule.generaleGroundOn();
        }
        else
          btsModule.allOff();
      }
      //else {
      //  btsModule.allOff();
      //}
    }

    if (workingMode == FREDDO) 
    {
      if ((!btsModule.getPeltierState()) && (btsModule.fanCoolAuto) && ( (!autoMode) || (currentTemp <= (setTemp + btsModule.coolOffsetTemp))) )
      {
        //Serial.println("Ventola attivabile. Check time");
        if (btsModule.getFanState() || ((millis() - fanInterTime) > (btsModule.fanCoolAutoInterTime * 1000)))
        {
          if ((millis() - fanTime) > (btsModule.fanCoolAutoTime * 1000)) 
          {
            if (!btsModule.getFanState())
            {
              btsModule.setFanOn();
              btsModule.generaleGroundOn();
            } else {
              btsModule.setFanOff();
              btsModule.generaleGroundOff();
            }
            fanStateView = btsModule.getFanState(); 
            fanTime = millis();
          }
          fanInterTime = millis();
        }
      } else {
        if (!btsModule.getPeltierState())
        {
          btsModule.generaleGroundOff();
          btsModule.setFanOff();
          fanStateView = false;
        }
        //fanInterTime = millis() - (btsModule.fanCoolAutoInterTime * 1000);
        //fanTime = millis() - (btsModule.fanCoolAutoTime * 1000);
        fanInterTime = fanTime = 0;
        //fanInterTime = 0; // Permette alla ri-attivazione della AUTO-FAN, di attivare subito la ventola
      }
    }
    else if (workingMode == CALDO) {
      if ((!btsModule.getPeltierState()) && (btsModule.fanHeatAuto) && ((!autoMode) || (currentTemp >= (setTemp - btsModule.heatOffsetTemp))) )
      {
          if (btsModule.getFanState() || ((millis() - fanInterTime) > (btsModule.fanHeatAutoInterTime * 1000)))
          {
              if ((millis() - fanTime) > (btsModule.fanHeatAutoTime * 1000)) 
              {
                  if (!btsModule.getFanState())
                  {
                      btsModule.setFanOn();
                      btsModule.generaleGroundOn();
                  } else {
                      btsModule.setFanOff();
                      btsModule.generaleGroundOff();
                  }
                  fanStateView = btsModule.getFanState(); 
                  fanTime = millis();
              }
              fanInterTime = millis();
          }
      } 
      else 
      {
          if (!btsModule.getPeltierState())
          {
              btsModule.generaleGroundOff();
              btsModule.setFanOff();
              fanStateView = false;
          }
          fanInterTime = fanTime = 0; // Permette riattivazione immediata
      }
    }

    String json = "{";
    if (currentTemp < -20) {
      json += "\"temp\":\"--\",";
    } else {
      json += "\"temp\":" + String(currentTemp, 2) + ",";
    }
    json += "\"coolOffset\":" + String(btsModule.coolOffsetTemp, 2) + ",";
    json += "\"heatOffset\":" + String(btsModule.heatOffsetTemp, 2) + ",";
    json += "\"coolPwm\":" + String(btsModule.coolPwm) + ",";
    json += "\"heatPwm\":" + String(btsModule.heatPwm) + ",";
    json += "\"set\":" + String(setTemp, 2) + ",";
    json += "\"auto\":" + String(autoMode ? "true" : "false") + ",";
    json += "\"peltier\":" + String(btsModule.getPeltierState() ? "true" : "false") + ",";
    json += "\"fan\":" + String(fanStateView ? "true" : "false") + ",";
    json += "\"fanHeatAuto\":" + String(btsModule.fanHeatAuto ? "true" : "false") + ",";
    json += "\"fanCoolAuto\":" + String(btsModule.fanCoolAuto ? "true" : "false") + ",";
    json += "\"fanCoolTime\":" + String(btsModule.fanCoolAutoTime) + ",";
    json += "\"fanHeatTime\":" + String(btsModule.fanHeatAutoTime) + ",";
    json += "\"fanCoolInterTime\":" + String(btsModule.fanCoolAutoInterTime) + ",";
    json += "\"fanHeatInterTime\":" + String(btsModule.fanHeatAutoInterTime) + ",";
    json += "\"mode\":\"" + mode + "\"";
    json += "}";

    ws.textAll(json);
  }
  wifiWatchdog();
}


void wifiWatchdog() 
{
  if (millis() - lastWifiCheck < 5000)
    return;
  lastWifiCheck = millis();

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin(wifiData.getSsid(), wifiData.getPassword());
  }
}