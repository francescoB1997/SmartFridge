#ifndef WIFI_DATA_H
#define WIFI_DATA_H

#include <Arduino.h>
#include <Preferences.h>

#define WIFI_NAMESPACE "wifi"

class WifiData {
  private:
    Preferences prefs;
    String ssid;
    String password;


  public:
    WifiData();
    WifiData(String s, String p);
    void load();
    String getSsid();
    String getPassword();
    void setSsid(String s);
    void setPassword(String p);
    bool isValidWifi();
};

#endif