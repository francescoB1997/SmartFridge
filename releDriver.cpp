#include <Arduino.h>
#include <cstdint>
#include <cstddef>
#include "releDriver.h"


ReleDriver::ReleDriver(uint8_t res) {
  this->peltierState = false;
  this->setAsOutputPin();
  ledcAttach(PWM_R, FREQ, res);
  ledcAttach(PWM_L, FREQ, res);
  this->maxOutputValue = (1UL << res) - 1;
}

void ReleDriver::setAsOutputPin(){
  pinMode(EN_R, OUTPUT);
  pinMode(EN_L, OUTPUT);
}

void ReleDriver::allOff() {
  digitalWrite(EN_R, LOW);
  digitalWrite(EN_L, LOW);
  ledcWrite(PWM_R, 0);
  ledcWrite(PWM_L, 0);
  this->peltierState = false;
  this->fanState = false;
}

void ReleDriver::allOffWithStore() {
  this->allOff();
  this->state = OFF;
  this->storeState();
}

void ReleDriver::generaliOff(){
  digitalWrite(EN_R, LOW);
  digitalWrite(EN_L, LOW);
}
void ReleDriver::generaliOn(){
  digitalWrite(EN_R, HIGH);
  digitalWrite(EN_L, HIGH);
}

// ************************ MODIFICA 22/03/2026 ************************
// Aggiunta funzionalità per accendere solo la ventola interna
void ReleDriver::generaleGroundOn(){
  digitalWrite(EN_R, HIGH);
}

void ReleDriver::generaleGroundOff(){
  digitalWrite(EN_R, LOW);
}

// The relè is activated ONLY if the mode is COLD.
void ReleDriver::setFanOn(){
  this->fanState = true;
  digitalWrite(EN_R, HIGH);
  digitalWrite(EN_L, LOW);
}

void ReleDriver::setFanOff(){
  this->fanState = false;
  this->generaliOff();
}

bool ReleDriver::getFanState(){
  return this->fanState;
}

//PRIVATE
void ReleDriver::storeCoolTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(LAST_TEMP_COOL_OFFSET, this->coolOffsetTemp);
  this->workingData.end();
}

void ReleDriver::storeHeatTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(LAST_TEMP_HEAT_OFFSET, this->heatOffsetTemp);
  this->workingData.end();
}

void ReleDriver::changeCoolTemperatureOffset( float newCoolOffset){
  this->coolOffsetTemp = newCoolOffset;
  this->storeCoolTemperatureOffset();
}

float ReleDriver::loadCoolTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->coolOffsetTemp = this->workingData.getFloat(LAST_TEMP_COOL_OFFSET, DEFAULT_TEMPERATURE_OFFSET);
  this->workingData.end();
  return this->coolOffsetTemp;
}

void ReleDriver::changeHeatTemperatureOffset( float newHeatOffset){
  this->heatOffsetTemp = newHeatOffset;
  this->storeHeatTemperatureOffset();
}

float ReleDriver::loadHeatTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->heatOffsetTemp = this->workingData.getFloat(LAST_TEMP_HEAT_OFFSET, DEFAULT_TEMPERATURE_OFFSET);
  this->workingData.end();
  return this->heatOffsetTemp;
}

//PRIVATE
void ReleDriver::storeFanHeatAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putBool(LAST_FAN_HEAT_AUTO, this->fanHeatAuto);
  this->workingData.end();
}

void ReleDriver::changeFanHeatAuto(bool newFanHeatAuto){
  this->fanHeatAuto = newFanHeatAuto;
  this->storeFanHeatAuto();
}

bool ReleDriver::loadFanHeatAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanHeatAuto = this->workingData.getBool(LAST_FAN_HEAT_AUTO, DEFAULT_FAN_AUTO);
  this->workingData.end();
  return this->fanHeatAuto;
}

//PRIVATE
void ReleDriver::storeFanCoolAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putBool(LAST_FAN_COOL_AUTO, this->fanCoolAuto);
  this->workingData.end();
}

void ReleDriver::changeFanCoolAuto(bool newFanCoolAuto){
  this->fanCoolAuto = newFanCoolAuto;
  this->storeFanCoolAuto();
}

bool ReleDriver::loadFanCoolAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanCoolAuto = this->workingData.getBool(LAST_FAN_COOL_AUTO, DEFAULT_FAN_AUTO);
  this->workingData.end();
  return this->fanCoolAuto;
}

void ReleDriver::changeFanHeatAutoTime(unsigned long newTime){
  this->fanHeatAutoTime = newTime;
  this->storeFanHeatAutoTime();
}

//PRIVATE
void ReleDriver::storeFanHeatAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_HEAT_TIME_AUTO, this->fanHeatAutoTime);
  this->workingData.end();
}

unsigned long ReleDriver::loadFanHeatAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanHeatAutoTime = this->workingData.getULong64(LAST_FAN_HEAT_TIME_AUTO, DEFAULT_FAN_AUTO_TIME);
  this->workingData.end();
  return this->fanHeatAutoTime;
}


void ReleDriver::changeFanCoolAutoTime(unsigned long newTime){
  this->fanCoolAutoTime = newTime;
  this->storeFanCoolAutoTime();
}

//PRIVATE
void ReleDriver::storeFanCoolAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_COOL_TIME_AUTO, this->fanCoolAutoTime);
  this->workingData.end();
}

unsigned long ReleDriver::loadFanCoolAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanCoolAutoTime = this->workingData.getULong64(LAST_FAN_COOL_TIME_AUTO, DEFAULT_FAN_AUTO_TIME);
  this->workingData.end();
  return this->fanCoolAutoTime;
}

// NEW

void ReleDriver::changeFanHeatAutoInterTime(unsigned long newInterTime){
  this->fanHeatAutoInterTime = newInterTime;
  this->storeFanHeatAutoInterTime();
}

//PRIVATE
void ReleDriver::storeFanHeatAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_HEAT_INTERTIME_AUTO, this->fanHeatAutoInterTime);
  this->workingData.end();
}

unsigned long ReleDriver::loadFanHeatAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanHeatAutoInterTime = this->workingData.getULong64(LAST_FAN_HEAT_INTERTIME_AUTO, DEFAULT_FAN_AUTO_INTERTIME);
  this->workingData.end();
  return this->fanHeatAutoInterTime;
}


void ReleDriver::changeFanCoolAutoInterTime(unsigned long newInterTime){
  this->fanCoolAutoInterTime = newInterTime;
  this->storeFanCoolAutoInterTime();
}

//PRIVATE
void ReleDriver::storeFanCoolAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_COOL_INTERTIME_AUTO, this->fanCoolAutoInterTime);
  this->workingData.end();
}

unsigned long ReleDriver::loadFanCoolAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanCoolAutoInterTime = this->workingData.getULong64(LAST_FAN_COOL_INTERTIME_AUTO, DEFAULT_FAN_AUTO_INTERTIME);
  this->workingData.end();
  return this->fanCoolAutoInterTime;
}

// **********************************************************************

void ReleDriver::changeMode(mode newMode){
  this->state = newMode;
  this->storeState();
}

void ReleDriver::HeatOn(){
  if (this->state != HEAT)
    this->allOff();
  this->generaliOn();

  ledcWrite(PWM_R, 0);
  ledcWrite(PWM_L, this->maxOutputValue);
  this->peltierState = true;
}

void ReleDriver::ColdOn(){
  if (this->state != FREDDO)
    this->allOff();
  this->generaliOn();
  
  ledcWrite(PWM_L, 0);
  ledcWrite(PWM_R, this->maxOutputValue);
  Serial.print("INTERNAL: MaxVol: ");
  Serial.println(this->maxOutputValue);
  this->peltierState = true;
}

void ReleDriver::HeatOff(){
  ledcWrite(PWM_L, 0);
  ledcWrite(PWM_R, 0);
  this->peltierState = false;
}

void ReleDriver::ColdOff(){
  ledcWrite(PWM_L, 0);
  ledcWrite(PWM_R, 0);
  this->peltierState = false;
}

bool ReleDriver::getGeneraliRelayState(){
  return (digitalRead(EN_R) && digitalRead(EN_L));
}

mode ReleDriver::getState() {
    return this->state;
}

mode ReleDriver::loadStoredState() {
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->state = static_cast<mode>(this->workingData.getUInt(LAST_WORKING_MODE, static_cast<uint8_t>(OFF)));
  this->workingData.end();
  return this->state;
} 

void ReleDriver::storeState(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putUInt(LAST_WORKING_MODE, static_cast<uint8_t>(this->state));
  this->workingData.end();
}

void ReleDriver::storeAutoMode(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putBool(LAST_AUTO_MODE, this->autoMode);
  this->workingData.end();
}

void ReleDriver::storeSetTemp(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(SET_TEMP_KEY, this->setTemp);
  this->workingData.end();
}

bool ReleDriver::loadAutoMode(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->autoMode = this->workingData.getBool(LAST_AUTO_MODE, false);
  this->workingData.end();
  return this->autoMode;
}

void ReleDriver::changeAutoMode(bool newAutoMode){
  this->autoMode = newAutoMode;
  if (!this->autoMode)
    this->allOffWithStore();
  this->storeAutoMode();
}

bool ReleDriver::getAutoMode(){
  return this->autoMode;
}

float ReleDriver::getSetTemp(){
  return this->setTemp;
}

float ReleDriver::loadSetTemp(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->setTemp = this->workingData.getFloat(SET_TEMP_KEY, DEFAULT_SET_TEMPERATURE);
  this->workingData.end();
  return this->setTemp;
}

void ReleDriver::changeSetTemp(float newSetTemp){
  this->setTemp = newSetTemp;
  this->storeSetTemp();
}

bool ReleDriver::getPeltierState(){
  return this->peltierState;
}