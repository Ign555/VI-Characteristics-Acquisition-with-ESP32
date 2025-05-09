#include "tc74.h"

int8_t lireTemperature(int addr){

    //Variables
    int temp;

    //Demande de lecture de la température
    Wire.beginTransmission(addr);
    Wire.write(0x00); 
    Wire.endTransmission();
    
    delay(100); 

    Wire.requestFrom(addr, 1);

    if (Wire.available()) {
        temp = Wire.read();
        return temp;
    }

    return -1;
}