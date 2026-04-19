#include <Arduino.h>
#include <cstdint>
#include <cstddef>
#include "btsDriver.h"

BtsDriver::BtsDriver(uint8_t res) {
  this->peltierState = false;
  this->setAsOutputPin();
  ledcAttach(PWM_R, FREQ, res);
  ledcAttach(PWM_L, FREQ, res);
  this->maxOutputValue = (1UL << res) - 1;
}

void BtsDriver::setAsOutputPin(){
  pinMode(EN_R, OUTPUT);
  pinMode(EN_L, OUTPUT);
}

void BtsDriver::allOff() {
  digitalWrite(EN_R, LOW);
  digitalWrite(EN_L, LOW);
  ledcWrite(PWM_R, 0);
  ledcWrite(PWM_L, 0);
  this->peltierState = false;
  this->fanState = false;
}

void BtsDriver::allOffWithStore() {
  this->allOff();
  this->state = OFF;
  this->storeState();
}

void BtsDriver::generaliOff(){
  digitalWrite(EN_R, LOW);
  digitalWrite(EN_L, LOW);
}
void BtsDriver::generaliOn(){
  digitalWrite(EN_R, HIGH);
  digitalWrite(EN_L, HIGH);
}

// ************************ MODIFICA 22/03/2026 ************************
// Aggiunta funzionalità per accendere solo la ventola interna
void BtsDriver::generaleGroundOn(){
  digitalWrite(EN_R, HIGH);
}

void BtsDriver::generaleGroundOff(){
  digitalWrite(EN_R, LOW);
}

void BtsDriver::setFanOn(){
  this->fanState = true;
  digitalWrite(EN_R, HIGH);
  digitalWrite(EN_L, LOW);
}

void BtsDriver::setFanOff(){
  this->fanState = false;
  this->generaliOff();
}

bool BtsDriver::getFanState(){
  return this->fanState;
}

//PRIVATE
void BtsDriver::storeCoolTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(LAST_TEMP_COOL_OFFSET, this->coolOffsetTemp);
  this->workingData.end();
}

void BtsDriver::storeHeatTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(LAST_TEMP_HEAT_OFFSET, this->heatOffsetTemp);
  this->workingData.end();
}

void BtsDriver::storeCoolPwm(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putUInt(LAST_COOL_PWM, this->coolPwm);
  this->workingData.end();
}

void BtsDriver::storeHeatPwm(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putUInt(LAST_HEAT_PWM, this->heatPwm);
  this->workingData.end();
}

void BtsDriver::changeCoolTemperatureOffset(float newCoolOffset){
  this->coolOffsetTemp = newCoolOffset;
  this->storeCoolTemperatureOffset();
}

float BtsDriver::loadCoolTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->coolOffsetTemp = this->workingData.getFloat(LAST_TEMP_COOL_OFFSET, DEFAULT_TEMPERATURE_OFFSET);
  this->workingData.end();
  return this->coolOffsetTemp;
}

void BtsDriver::changeHeatTemperatureOffset(float newHeatOffset){
  this->heatOffsetTemp = newHeatOffset;
  this->storeHeatTemperatureOffset();
}

float BtsDriver::loadHeatTemperatureOffset(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->heatOffsetTemp = this->workingData.getFloat(LAST_TEMP_HEAT_OFFSET, DEFAULT_TEMPERATURE_OFFSET);
  this->workingData.end();
  return this->heatOffsetTemp;
}

// PWM

void BtsDriver::changeCoolPwm(uint8_t newCoolPwm){
  this->coolPwm = newCoolPwm;
  this->storeCoolPwm();
}

uint8_t BtsDriver::loadCoolPwm(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->coolPwm = this->workingData.getFloat(LAST_COOL_PWM, DEFAULT_PWM);
  this->workingData.end();
  return this->coolPwm;
}

void BtsDriver::changeHeatPwm(uint8_t newHeatPwm){
  this->heatPwm = newHeatPwm;
  this->storeHeatPwm();
}

uint8_t BtsDriver::loadHeatPwm(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->heatPwm = this->workingData.getFloat(LAST_HEAT_PWM, DEFAULT_PWM);
  this->workingData.end();
  return this->heatPwm;
}

//* PWM

//PRIVATE
void BtsDriver::storeFanHeatAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putBool(LAST_FAN_HEAT_AUTO, this->fanHeatAuto);
  this->workingData.end();
}

void BtsDriver::changeFanHeatAuto(bool newFanHeatAuto){
  this->fanHeatAuto = newFanHeatAuto;
  this->storeFanHeatAuto();
}

bool BtsDriver::loadFanHeatAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanHeatAuto = this->workingData.getBool(LAST_FAN_HEAT_AUTO, DEFAULT_FAN_AUTO);
  this->workingData.end();
  return this->fanHeatAuto;
}

//PRIVATE
void BtsDriver::storeFanCoolAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putBool(LAST_FAN_COOL_AUTO, this->fanCoolAuto);
  this->workingData.end();
}

void BtsDriver::changeFanCoolAuto(bool newFanCoolAuto){
  this->fanCoolAuto = newFanCoolAuto;
  this->storeFanCoolAuto();
}

bool BtsDriver::loadFanCoolAuto(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanCoolAuto = this->workingData.getBool(LAST_FAN_COOL_AUTO, DEFAULT_FAN_AUTO);
  this->workingData.end();
  return this->fanCoolAuto;
}

void BtsDriver::changeFanHeatAutoTime(unsigned long newTime){
  this->fanHeatAutoTime = newTime;
  this->storeFanHeatAutoTime();
}

//PRIVATE
void BtsDriver::storeFanHeatAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_HEAT_TIME_AUTO, this->fanHeatAutoTime);
  this->workingData.end();
}

unsigned long BtsDriver::loadFanHeatAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanHeatAutoTime = this->workingData.getULong64(LAST_FAN_HEAT_TIME_AUTO, DEFAULT_FAN_AUTO_TIME);
  this->workingData.end();
  return this->fanHeatAutoTime;
}


void BtsDriver::changeFanCoolAutoTime(unsigned long newTime){
  this->fanCoolAutoTime = newTime;
  this->storeFanCoolAutoTime();
}

//PRIVATE
void BtsDriver::storeFanCoolAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_COOL_TIME_AUTO, this->fanCoolAutoTime);
  this->workingData.end();
}

unsigned long BtsDriver::loadFanCoolAutoTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanCoolAutoTime = this->workingData.getULong64(LAST_FAN_COOL_TIME_AUTO, DEFAULT_FAN_AUTO_TIME);
  this->workingData.end();
  return this->fanCoolAutoTime;
}

// NEW

void BtsDriver::changeFanHeatAutoInterTime(unsigned long newInterTime){
  this->fanHeatAutoInterTime = newInterTime;
  this->storeFanHeatAutoInterTime();
}

//PRIVATE
void BtsDriver::storeFanHeatAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_HEAT_INTERTIME_AUTO, this->fanHeatAutoInterTime);
  this->workingData.end();
}

unsigned long BtsDriver::loadFanHeatAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanHeatAutoInterTime = this->workingData.getULong64(LAST_FAN_HEAT_INTERTIME_AUTO, DEFAULT_FAN_AUTO_INTERTIME);
  this->workingData.end();
  return this->fanHeatAutoInterTime;
}


void BtsDriver::changeFanCoolAutoInterTime(unsigned long newInterTime){
  this->fanCoolAutoInterTime = newInterTime;
  this->storeFanCoolAutoInterTime();
}

//PRIVATE
void BtsDriver::storeFanCoolAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putULong64(LAST_FAN_COOL_INTERTIME_AUTO, this->fanCoolAutoInterTime);
  this->workingData.end();
}

unsigned long BtsDriver::loadFanCoolAutoInterTime(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->fanCoolAutoInterTime = this->workingData.getULong64(LAST_FAN_COOL_INTERTIME_AUTO, DEFAULT_FAN_AUTO_INTERTIME);
  this->workingData.end();
  return this->fanCoolAutoInterTime;
}

// **********************************************************************

void BtsDriver::changeMode(mode newMode){
  this->state = newMode;
  this->storeState();
}

void BtsDriver::HeatOn(){
  if (this->state != CALDO)
    this->allOff();
  this->generaliOn();

  ledcWrite(PWM_R, 0);
  ledcWrite(PWM_L, this->maxOutputValue * this->heatPwm / 100);
  Serial.print("INTERNAL: CooHeatCWrite: ");
  Serial.println(this->maxOutputValue * this->heatPwm / 100);
  this->peltierState = true;
}

void BtsDriver::ColdOn(){
  if (this->state != FREDDO)
    this->allOff();
  this->generaliOn();
  
  ledcWrite(PWM_L, 0);
  ledcWrite(PWM_R, this->maxOutputValue * this->coolPwm / 100);
  Serial.print("INTERNAL: CoolCWrite: ");
  Serial.println(this->maxOutputValue * this->coolPwm / 100);
  this->peltierState = true;
}

void BtsDriver::HeatOff(){
  ledcWrite(PWM_L, 0);
  ledcWrite(PWM_R, 0);
  this->peltierState = false;
}

void BtsDriver::ColdOff(){
  ledcWrite(PWM_L, 0);
  ledcWrite(PWM_R, 0);
  this->peltierState = false;
}

bool BtsDriver::getENPinState(){
  return (digitalRead(EN_R) && digitalRead(EN_L));
}

mode BtsDriver::getState() {
    return this->state;
}

mode BtsDriver::loadStoredState() {
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->state = static_cast<mode>(this->workingData.getUInt(LAST_WORKING_MODE, static_cast<uint8_t>(OFF)));
  this->workingData.end();
  return this->state;
} 

void BtsDriver::storeState(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putUInt(LAST_WORKING_MODE, static_cast<uint8_t>(this->state));
  this->workingData.end();
}

void BtsDriver::storeAutoMode(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putBool(LAST_AUTO_MODE, this->autoMode);
  this->workingData.end();
}

void BtsDriver::storeSetTemp(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, false);
  this->workingData.putFloat(SET_TEMP_KEY, this->setTemp);
  this->workingData.end();
}

bool BtsDriver::loadAutoMode(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->autoMode = this->workingData.getBool(LAST_AUTO_MODE, false);
  this->workingData.end();
  return this->autoMode;
}

void BtsDriver::changeAutoMode(bool newAutoMode){
  this->autoMode = newAutoMode;
  if (!this->autoMode)
    this->allOffWithStore();
  this->storeAutoMode();
}

bool BtsDriver::getAutoMode(){
  return this->autoMode;
}

float BtsDriver::getSetTemp(){
  return this->setTemp;
}

float BtsDriver::loadSetTemp(){
  this->workingData.begin(WORKING_DATA_NAMESPACE, true);
  this->setTemp = this->workingData.getFloat(SET_TEMP_KEY, DEFAULT_SET_TEMPERATURE);
  this->workingData.end();
  return this->setTemp;
}

void BtsDriver::changeSetTemp(float newSetTemp){
  this->setTemp = newSetTemp;
  this->storeSetTemp();
}

bool BtsDriver::getPeltierState(){
  return this->peltierState;
}