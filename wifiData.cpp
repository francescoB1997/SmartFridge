#include "wifiData.h"

WifiData::WifiData() {
}

WifiData::WifiData(String s, String p) {
  prefs.begin(WIFI_NAMESPACE, false);
  prefs.putString("ssid", s);
  prefs.putString("password", p);
  prefs.end();
}

void WifiData::load(){
  prefs.begin(WIFI_NAMESPACE, false);
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("password", "");
  prefs.end();
}

String WifiData::getSsid() {
  return ssid;
}

String WifiData::getPassword() {
  prefs.begin(WIFI_NAMESPACE, false);
  String p = prefs.getString("password", "");
  prefs.end();
  return p;
}

void WifiData::setSsid(String s) {
  prefs.begin(WIFI_NAMESPACE, false);
  ssid = s;
  prefs.putString("ssid", ssid);
  prefs.end();
}

void WifiData::setPassword(String p) {
  prefs.begin(WIFI_NAMESPACE, false);
  password = p;
  prefs.putString("password", password);
  prefs.end();
}

bool WifiData::isValidWifi(){
  return (this->getSsid() != "") && (this->getPassword() != "");
}
