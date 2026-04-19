#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
#include "releDriver.h"
#include "wifiData.h"
#include "releDriver.h"
#include "index_html.h"

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static AsyncWebServer server(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

WifiData wifiData;
ReleDriver relayModule = ReleDriver(DEFAULT_LEDC_RESOLUTION);
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

void applyMode() {
  if (mode == "OFF")
    relayModule.allOffWithStore();
  else if (mode == "FREDDO") {
    relayModule.changeMode(FREDDO);
  } else if (mode == "CALDO") {
    relayModule.changeMode(CALDO);
  }
  
  workingMode = relayModule.getState();
}

void setup() {
  uint8_t wifiTryCounter = 0;
  Serial.begin(115200);
  relayModule.setAsOutputPin();
  relayModule.allOff();
  relayModule.loadCoolTemperatureOffset();
  relayModule.loadHeatTemperatureOffset();
  relayModule.loadFanCoolAuto();
  relayModule.loadFanHeatAuto();
  relayModule.loadFanHeatAutoTime();
  relayModule.loadFanCoolAutoTime();
  relayModule.loadFanHeatAutoInterTime();
  relayModule.loadFanCoolAutoInterTime();
  delay(1000);

  ArduinoOTA.setHostname("miniFrigo");
  ArduinoOTA.setPassword("69860");
  ArduinoOTA
  .onStart([]() {
    //Serial.println("OTA: Start");
  })
  .onEnd([]() {
    //Serial.println("\nOTA: End");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress * 100) / total);
  })
  .onError([](ota_error_t error) {
    //Serial.printf("Error[%u]\n", error);
  });

  
  wifiData.load();
  if (!wifiData.isValidWifi()){
    //startAccessPointMode();
    WiFi.mode(WIFI_AP);
    WiFi.softAP("MiniFrigo-Setup");

    IPAddress IP = WiFi.softAPIP();
  }
  else {
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

  workingMode = relayModule.loadStoredState();
  if (workingMode == OFF)
    mode = "OFF";
  else if (workingMode == CALDO)
    mode = "CALDO";
  else if (workingMode == FREDDO)
    mode = "FREDDO";

  autoMode = relayModule.loadAutoMode();
  setTemp = relayModule.loadSetTemp();

  sensors.begin();  

  wsHandler.onConnect([](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    //Serial.printf("Client %" PRIu32 " connected\n", client->id());

    // manda stato iniziale
    String json = "{";
    if (currentTemp < -20) {
      json += "\"temp\":\"--\",";
    } else {
      json += "\"temp\":" + String(currentTemp, 2) + ",";
    }
    json += "\"coolOffset\":" + String(relayModule.coolOffsetTemp, 2) + ",";
    json += "\"heatOffset\":" + String(relayModule.heatOffsetTemp, 2) + ",";
    json += "\"set\":" + String(setTemp, 2) + ",";
    json += "\"auto\":" + String(autoMode ? "true" : "false") + ",";
    json += "\"fanHeatAuto\":" + String(relayModule.fanHeatAuto ? "true" : "false") + ",";
    json += "\"fanCoolAuto\":" + String(relayModule.fanCoolAuto ? "true" : "false") + ",";
    json += "\"fanCoolTime\":" + String(relayModule.fanCoolAutoTime) + ",";
    json += "\"fanHeatTime\":" + String(relayModule.fanHeatAutoTime) + ",";
    json += "\"fanCoolInterTime\":" + String(relayModule.fanCoolAutoInterTime) + ",";
    json += "\"fanHeatInterTime\":" + String(relayModule.fanHeatAutoInterTime) + ",";
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
      relayModule.changeSetTemp(setTemp);
    }

    if (msg == "MODE:FREDDO") {
      mode = "FREDDO";
    } else if (msg == "MODE:CALDO") {
      mode = "CALDO";
    }

    if (msg == "AUTO:ON") {
      relayModule.changeAutoMode(true);
      autoMode = relayModule.getAutoMode();
    }
    else if (msg == "AUTO:OFF") {
      relayModule.changeAutoMode(false);
      autoMode = relayModule.getAutoMode();
    }

    if (msg.startsWith("COOL_OFFSET:")) {
      float newOffset = msg.substring(12).toFloat();
      relayModule.changeCoolTemperatureOffset(newOffset);
    }

    if (msg.startsWith("HEAT_OFFSET:")) {
      float newOffset = msg.substring(12).toFloat();
      relayModule.changeHeatTemperatureOffset(newOffset);
    }

    if (msg.startsWith("FANHEAT_AUTO:ON")) {
      relayModule.changeFanHeatAuto(true);
    } else if (msg.startsWith("FANHEAT_AUTO:OFF")) {
      relayModule.changeFanHeatAuto(false);
    }

    if (msg.startsWith("FANCOOL_AUTO:ON")) {
      relayModule.changeFanCoolAuto(true);
    } else if (msg.startsWith("FANCOOL_AUTO:OFF")) {
      relayModule.changeFanCoolAuto(false);
    }

    if (msg.startsWith("FANCOOL_TIME:")) {
      unsigned long newFanCoolTime = msg.substring(13).toInt();
      relayModule.changeFanCoolAutoTime(newFanCoolTime);
    }

    if (msg.startsWith("FANHEAT_TIME:")) {
      unsigned long newFanHeatTime = msg.substring(13).toInt();
      relayModule.changeFanHeatAutoTime(newFanHeatTime);
    }

    if (msg.startsWith("FANCOOL_INTERTIME:")) {
      unsigned long newFanCoolnterTime = msg.substring(18).toInt();
      relayModule.changeFanCoolAutoInterTime(newFanCoolnterTime);
    }

    if (msg.startsWith("FANHEAT_INTERTIME:")) {
      unsigned long newFanHeatInterTime = msg.substring(18).toInt();
      relayModule.changeFanHeatAutoInterTime(newFanHeatInterTime);
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
    json += "\"coolOffset\":" + String(relayModule.coolOffsetTemp, 2) + ",";
    json += "\"heatOffset\":" + String(relayModule.heatOffsetTemp, 2) + ",";
    json += "\"auto\":" + String(autoMode ? "true" : "false") + ",";
    json += "\"peltier\":" + String(relayModule.getPeltierState() ? "true" : "false") + ",";
    json += "\"fan\":" + String(fanStateView ? "true" : "false") + ",";
    json += "\"fanHeatAuto\":" + String(relayModule.fanHeatAuto ? "true" : "false") + ",";
    json += "\"fanCoolAuto\":" + String(relayModule.fanCoolAuto ? "true" : "false") + ",";
    json += "\"fanCoolTime\":" + String(relayModule.fanCoolAutoTime) + ",";
    json += "\"fanHeatTime\":" + String(relayModule.fanHeatAutoTime) + ",";
    json += "\"fanCoolInterTime\":" + String(relayModule.fanCoolAutoInterTime) + ",";
    json += "\"fanHeatInterTime\":" + String(relayModule.fanHeatAutoInterTime) + ",";
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
      relayModule.allOff();
    }
    else {
      if (autoMode) {
        Serial.println("# AUTO MODE ON");
        switch(workingMode){
          case CALDO:
              Serial.print("* CASE CALDO. Heat Offset: ");
              Serial.println(relayModule.heatOffsetTemp);
              if (currentTemp < (setTemp - relayModule.heatOffsetTemp) ) {
                if (relayModule.getFanState()) {
                  relayModule.setFanOff();
                  relayModule.allOff();
                }
                fanStateView = true; // Fai vedere che gira (il relè della massa è spento, visto che si è in HEAT)
                relayModule.HeatOn();
                relayModule.generaliOn();
              } else if (currentTemp >= setTemp) {
                  if (relayModule.getPeltierState())
                    relayModule.allOff();
                }                
            break;
          case FREDDO:
              Serial.print("* CASE FREDDO. Cool Offset: ");
              Serial.println(relayModule.coolOffsetTemp);
              if (currentTemp > (setTemp + relayModule.coolOffsetTemp) ) {
                if (relayModule.getFanState()) {
                  relayModule.setFanOff();
                  relayModule.allOff();
                }
                Serial.println("** (currentTemp > (setTemp + relayModule.offsetTemp)");
                fanStateView = true; // Fai vedere la ventola che gira (il relè della massa è attivo)
                relayModule.ColdOn();
                relayModule.generaliOn();
                Serial.println("** ColdOn() e generaliOn()");
              } else if (currentTemp <= setTemp) {
                Serial.println("** (currentTemp <= setTemp)");
                if (relayModule.getPeltierState())
                {
                  Serial.println("*** getPeltierState è true --> allOff()");
                  relayModule.allOff();
                }
              } else {Serial.println("** limbo");}
            break;
          case OFF:
              //Serial.println("CASE OFF");
              relayModule.allOff();
              break;
          default:
            break;
        }
      }
      else{
        Serial.println("# AUTO MODE OFF");
        if (relayModule.getFanState() || fanStateView)
        {
          if (relayModule.getFanState())
            Serial.println("## relayModule.getFanState() è true");
          if (fanStateView)
            Serial.println("## fanStateView è true");
          if (relayModule.getPeltierState()) {
            Serial.println("### relayModule.getPeltierState() è true --> allOff()");
            relayModule.allOff();
          }
          Serial.println("## setFanOn() e || generaleGroundOn()");
          relayModule.setFanOn();
          relayModule.generaleGroundOn();
        }
        else
          relayModule.allOff();
      }
      //else {
      //  relayModule.allOff();
      //}
    }

    if (workingMode == FREDDO) 
    {
      if ((!relayModule.getPeltierState()) && (relayModule.fanCoolAuto) && ( (!autoMode) || (currentTemp <= (setTemp + relayModule.coolOffsetTemp))) )
      {
        Serial.println("Ventola attivabile. Check time");
        if (relayModule.getFanState() || ((millis() - fanInterTime) > (relayModule.fanCoolAutoInterTime * 1000)))
        {
          if ((millis() - fanTime) > (relayModule.fanCoolAutoTime * 1000)) 
          {
            if (!relayModule.getFanState())
            {
              relayModule.setFanOn();
              relayModule.generaleGroundOn();
            } else {
              relayModule.setFanOff();
              relayModule.generaleGroundOff();
            }
            fanStateView = relayModule.getFanState(); 
            fanTime = millis();
          }
          fanInterTime = millis();
        }
      } else {
        if (!relayModule.getPeltierState())
        {
          relayModule.generaleGroundOff();
          relayModule.setFanOff();
          fanStateView = false;
        }
        //fanInterTime = millis() - (relayModule.fanCoolAutoInterTime * 1000);
        //fanTime = millis() - (relayModule.fanCoolAutoTime * 1000);
        fanInterTime = fanTime = 0;
        //fanInterTime = 0; // Permette alla ri-attivazione della AUTO-FAN, di attivare subito la ventola
      }
    }
    else if (workingMode == CALDO) {
      if ((!relayModule.getPeltierState()) && (relayModule.fanHeatAuto) && ((!autoMode) || (currentTemp >= (setTemp - relayModule.heatOffsetTemp))) )
      {
          if (relayModule.getFanState() || ((millis() - fanInterTime) > (relayModule.fanHeatAutoInterTime * 1000)))
          {
              if ((millis() - fanTime) > (relayModule.fanHeatAutoTime * 1000)) 
              {
                  if (!relayModule.getFanState())
                  {
                      relayModule.setFanOn();
                      relayModule.generaleGroundOn();
                  } else {
                      relayModule.setFanOff();
                      relayModule.generaleGroundOff();
                  }
                  fanStateView = relayModule.getFanState(); 
                  fanTime = millis();
              }
              fanInterTime = millis();
          }
      } 
      else 
      {
          if (!relayModule.getPeltierState())
          {
              relayModule.generaleGroundOff();
              relayModule.setFanOff();
              fanStateView = false;
          }
          fanInterTime = fanTime = 0; // Permette riattivazione immediata
      }
    }
    //Da testare e caricare nell'ESP. Adesso non lo faccio perchè non avrei tempo di smontarlo e caricare
    //il codide ' da usb se è sbagliato.

    String json = "{";
    if (currentTemp < -20) {
      json += "\"temp\":\"--\",";
    } else {
      json += "\"temp\":" + String(currentTemp, 2) + ",";
    }
    json += "\"coolOffset\":" + String(relayModule.coolOffsetTemp, 2) + ",";
    json += "\"heatOffset\":" + String(relayModule.heatOffsetTemp, 2) + ",";
    json += "\"set\":" + String(setTemp, 2) + ",";
    json += "\"auto\":" + String(autoMode ? "true" : "false") + ",";
    json += "\"peltier\":" + String(relayModule.getPeltierState() ? "true" : "false") + ",";
    json += "\"fan\":" + String(fanStateView ? "true" : "false") + ",";
    json += "\"fanHeatAuto\":" + String(relayModule.fanHeatAuto ? "true" : "false") + ",";
    json += "\"fanCoolAuto\":" + String(relayModule.fanCoolAuto ? "true" : "false") + ",";
    json += "\"fanCoolTime\":" + String(relayModule.fanCoolAutoTime) + ",";
    json += "\"fanHeatTime\":" + String(relayModule.fanHeatAutoTime) + ",";
    json += "\"fanCoolInterTime\":" + String(relayModule.fanCoolAutoInterTime) + ",";
    json += "\"fanHeatInterTime\":" + String(relayModule.fanHeatAutoInterTime) + ",";
    json += "\"mode\":\"" + mode + "\"";
    json += "}";

    ws.textAll(json);
  }
}