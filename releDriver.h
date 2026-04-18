#ifndef releDriver_h
#define releDriver_h

#include <Preferences.h>

#define RELAY_FREDDO_1 8
#define RELAY_FREDDO_2 9
#define RELAY_CALDO_1 21
#define RELAY_CALDO_2 20
#define RELAY_GEN1 7
#define RELAY_GEN2 10

#define DEFAULT_SET_TEMPERATURE 25.0
#define DEFAULT_TEMPERATURE_OFFSET 0.5
#define DEFAULT_FAN_AUTO false
#define DEFAULT_FAN_AUTO_TIME 30
#define DEFAULT_FAN_AUTO_INTERTIME 30
#define SET_TEMP_KEY "prevSet"

#define WORKING_DATA_NAMESPACE "workingData"
#define LAST_WORKING_MODE "lastMode"
#define LAST_AUTO_MODE "lastAutoMode"
#define LAST_TEMP_OFFSET "lastOffset"
#define LAST_FAN_HEAT_AUTO "lastFHA"
#define LAST_FAN_COOL_AUTO "lastFCA"
#define LAST_FAN_HEAT_TIME_AUTO "lastFHTA"
#define LAST_FAN_COOL_TIME_AUTO "lastFCTA"
#define LAST_FAN_HEAT_INTERTIME_AUTO "lastFHITA"
#define LAST_FAN_COOL_INTERTIME_AUTO "lastFCITA"



enum mode{OFF, HEAT, COLD};

class ReleDriver{
  private:
    uint8_t *relayPin;
    size_t numRele;
    mode state;
    bool autoMode;
    bool peltierState;
    bool fanState;
    Preferences workingData;
    float setTemp;
    void storeState();
    void storeAutoMode();
    void storeSetTemp();
    void storeTemperatureOffset();
    void storeFanHeatAuto();
    void storeFanCoolAuto();
    void storeFanHeatAutoTime();
    void storeFanCoolAutoTime();
    void storeFanHeatAutoInterTime();
    void storeFanCoolAutoInterTime();
    

  public:
    float offsetTemp;
    bool fanHeatAuto;
    bool fanCoolAuto;
    unsigned long fanHeatAutoTime;
    unsigned long fanCoolAutoTime;
    unsigned long fanHeatAutoInterTime;
    unsigned long fanCoolAutoInterTime;
    ReleDriver();

    void setAsOutputPin();
    void allOff();
    void allOffWithStore();
    void changeMode(mode);
    void generaliOn();
    void generaliOff();
    void relayColdOn();
    void relayHeatOn();
    void relayHeatOff();
    void relayColdOff();
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

    void changeTemperatureOffset(float);
    float loadTemperatureOffset();

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