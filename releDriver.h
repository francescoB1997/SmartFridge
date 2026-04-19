#ifndef releDriver_h
#define releDriver_h

#include <Preferences.h>

#define EN_R 20
#define EN_L 21
#define PWM_R 0
#define PWM_L 1

#define DEFAULT_SET_TEMPERATURE 25.0
#define DEFAULT_TEMPERATURE_OFFSET 0.5
#define DEFAULT_FAN_AUTO false
#define DEFAULT_FAN_AUTO_TIME 30
#define DEFAULT_FAN_AUTO_INTERTIME 30
#define SET_TEMP_KEY "prevSet"

#define FREQ 5000
#define DEFAULT_LEDC_RESOLUTION 10

#define WORKING_DATA_NAMESPACE "workingData"
#define LAST_WORKING_MODE "lastMode"
#define LAST_AUTO_MODE "lastAutoMode"
#define LAST_TEMP_COOL_OFFSET "lastTCO"
#define LAST_TEMP_HEAT_OFFSET "lastTHO"
#define LAST_FAN_HEAT_AUTO "lastFHA"
#define LAST_FAN_COOL_AUTO "lastFCA"
#define LAST_FAN_HEAT_TIME_AUTO "lastFHTA"
#define LAST_FAN_COOL_TIME_AUTO "lastFCTA"
#define LAST_FAN_HEAT_INTERTIME_AUTO "lastFHITA"
#define LAST_FAN_COOL_INTERTIME_AUTO "lastFCITA"


enum mode{OFF, HEAT, FREDDO};

class ReleDriver{
  private:
    uint32_t maxOutputValue;
    mode state;
    bool autoMode;
    bool peltierState;
    bool fanState;
    Preferences workingData;
    float setTemp;
    void storeState();
    void storeAutoMode();
    void storeSetTemp();
    void storeHeatTemperatureOffset();
    void storeCoolTemperatureOffset();
    void storeFanHeatAuto();
    void storeFanCoolAuto();
    void storeFanHeatAutoTime();
    void storeFanCoolAutoTime();
    void storeFanHeatAutoInterTime();
    void storeFanCoolAutoInterTime();
    

  public:
    float heatOffsetTemp;
    float coolOffsetTemp;
    bool fanHeatAuto;
    bool fanCoolAuto;
    unsigned long fanHeatAutoTime;
    unsigned long fanCoolAutoTime;
    unsigned long fanHeatAutoInterTime;
    unsigned long fanCoolAutoInterTime;
    ReleDriver(uint8_t);

    void setAsOutputPin();
    void allOff();
    void allOffWithStore();
    void changeMode(mode);
    void generaliOff();
    void generaliOn();
    void HeatOn();
    void HeatOff();
    void ColdOff();
    void ColdOn();
    bool getGeneraliRelayState();

    mode getState();
    mode loadStoredState();
    bool getAutoMode();
    bool loadAutoMode();
    void changeAutoMode(bool);
    float getSetTemp();
    float loadSetTemp();
    void changeSetTemp(float);
    bool getPeltierState();

    void generaleGroundOn();
    void generaleGroundOff();
    void setFanOn();
    void setFanOff();
    bool getFanState();

    void changeHeatTemperatureOffset(float);
    float loadHeatTemperatureOffset();
    void changeCoolTemperatureOffset(float);
    float loadCoolTemperatureOffset();

    void changeFanHeatAuto(bool);
    bool loadFanHeatAuto();

    void changeFanCoolAuto(bool);
    bool loadFanCoolAuto();

    void changeFanHeatAutoTime(unsigned long);
    unsigned long loadFanHeatAutoTime();

    void changeFanCoolAutoTime(unsigned long);
    unsigned long loadFanCoolAutoTime();

    void changeFanCoolAutoInterTime(unsigned long);
    unsigned long loadFanCoolAutoInterTime();

    void changeFanHeatAutoInterTime(unsigned long);
    unsigned long loadFanHeatAutoInterTime();
};

#endif