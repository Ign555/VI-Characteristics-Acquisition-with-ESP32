#include "tc74.h"

float lireTemperature(int addr){

    //Variables
    int temp;
    float temperature;

    //Demande de lecture de la température
    Wire.beginTransmission(addr);
    Wire.write(0x00); 
    Wire.endTransmission();
    
    delay(100); 

    Wire.requestFrom(addr, 1);

    if (Wire.available()) {
        temp = Wire.read();
        temperature = (float)temp;
        return temperature;
    }

    return -1;
}