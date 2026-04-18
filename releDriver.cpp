#include <Arduino.h>
#include <cstdint>
#include <cstddef>
#include "releDriver.h"


ReleDriver::ReleDriver() {
  uint8_t relPin[] = { RELAY_GEN1, RELAY_GEN2, RELAY_FREDDO_1, RELAY_FREDDO_2, RELAY_CALDO_1, RELAY_CALDO_2};  

  this->numRele = sizeof(relPin) / sizeof(uint8_t);
  this->relayPin = new uint8_t[this->numRele];
  this->peltierState = false;
  memcpy(this->relayPin, relPin, this->numRele);
}

void ReleDriver::setAsOutputPin(){
  for (uint8_t i = 0; i < this->numRele; i++){
    pinMode(this->relayPin[i], OUTPUT);
  }
}

void ReleDriver::allOff() {
  for(uint8_t i = 0; i < this->numRele; i++)
    digitalWrite(this->relayPin[i], LOW);
  this->peltierState = false;
  delay(50);
}

void ReleDriver::allOffWithStore() {
  this->allOff();
  this->state = OFF;
  this->storeState();
}

void ReleDriver::generaliOff(){
  for(uint8_t i = 0; i < 2; i++)
    digitalWrite(this->relayPin[i], LOW);
}

void ReleDriver::generaliOn(){
  for(uint8_t i = 0; i < 2; i++)
    digitalWrite(this->relayPin[i], HIGH);
}

// ************************ MODIFICA 22/03/2026 ************************
// Aggiunta funzionalità per accendere solo la ventola interna
void ReleDriver::generaleGroundOn(){
  digitalWrite(RELAY_GEN2, HIGH);
}

void ReleDriver::generaleGroundOff(){
  digitalWrite(RELAY_GEN2, LOW);
}

// The relè is activated ONLY if the mode is COLD.
void ReleDriver::setFanOn(){
  this->fanState = true;
  digitalWrite(RELAY_FREDDO_1, HIGH);
}

void ReleDriver::setFanOff(){
  this->fanState = false;
  digitalWrite(RELAY_FREDDO_1, LOW);
}

bool ReleDriver::getFanState(){
  return this->fanState;
}

//PRIVATE
void ReleDriver::storeTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(LAST_TEMP_OFFSET, this->offsetTemp);
  this->workingData.end();
}

void ReleDriver::changeTemperatureOffset( float newOffset){
  this->offsetTemp = newOffset;
  this->storeTemperatureOffset();
}

float ReleDriver::loadTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->offsetTemp = this->workingData.getFloat(LAST_TEMP_OFFSET, DEFAULT_TEMPERATURE_OFFSET);
  this->workingData.end();
  return this->offsetTemp;
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

void ReleDriver::relayHeatOn(){
  if (this->state != HEAT)
    this->allOff();
  digitalWrite(RELAY_CALDO_1, HIGH);
  digitalWrite(RELAY_CALDO_2, HIGH);
  this->peltierState = true;
}

void ReleDriver::relayColdOn(){
  if (this->state != COLD)
    this->allOff();
  digitalWrite(RELAY_FREDDO_1, HIGH);
  digitalWrite(RELAY_FREDDO_2, HIGH);
  this->peltierState = true;
}

void ReleDriver::relayHeatOff(){
  digitalWrite(RELAY_CALDO_1, LOW);
  digitalWrite(RELAY_CALDO_2, LOW);
  this->peltierState = false;
}

void ReleDriver::relayColdOff(){
  digitalWrite(RELAY_FREDDO_1, LOW);
  digitalWrite(RELAY_FREDDO_2, LOW);
  this->peltierState = false;
}

bool ReleDriver::getGeneraliRelayState(){
  return digitalRead(RELAY_GEN1) && digitalRead(RELAY_GEN2);
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