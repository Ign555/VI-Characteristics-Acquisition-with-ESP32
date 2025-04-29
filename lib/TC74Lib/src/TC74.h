#ifndef TC74_ft
#define TC74_ft

#include "Arduino.h"
#include "Wire.h"

#if (ARDUINO <  100) && !defined(MPIDE)
#error TC74 library requires Arduino 1.0 or later
#endif

#define STANDBY true
#define NORMAL false

class TC74{
  public:
    TC74(uint8_t adr);
	// Read the temperature from the sensor.
	float readTemperature(char c);
	// Configure the power mode of TC74.
    void TC74Mode(bool mode);
	// Check if TC74 is in STANDBY mode or not.
    bool isStandby();
	// Initiate a TC74.
    void begin(TwoWire &wirePort = Wire);
  private:
    uint8_t _adr;
    TwoWire *_wire; //for different i2c port config
};

#endif
