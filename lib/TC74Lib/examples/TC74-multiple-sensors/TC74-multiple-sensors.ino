/* TC74-Example
 * Author: Chawin 'FaultyTwo' Treesugol
 * Date: 5/5/2022
 * FROM: https://github.com/FaultyTwo/TC74-arduino-lib
 * 
 * Desc: With the power of I2C, 
 * I bring you the usage of library with multiple TC4 sensors!
 * 
 * Enjoy!
 */
#include "TC74.h"
#define first 2000
#define second 2050
#define third 2100

TC74 dvc1(0x4A); //A2 Address
TC74 dvc2(0x4B); //A3 Address
TC74 dvc3(0x4C); //A4 Address

void setup() {
  dvc1.begin();
  dvc2.begin();
  dvc3.begin();
  Serial.begin(9600);
  while(dvc3.isStandby()){ //wait until all devices are ready
    
  }
  Serial.println("Home Temperature System Engaged");
}

void loop() {
  static unsigned long timer1, timer2, timer3;
  
  if(millis() - timer1 > first){
    timer1 = millis();
    Serial.print("First sensor : ");
    Serial.println(dvc1.readTemperature('c'));
  }

  if(millis() - timer2 > second){
    timer2 = millis();
    Serial.print("Second sensor : ");
    Serial.println(dvc2.readTemperature('C'));
  }

  if(millis() - timer3 > third){
    timer3 = millis();
    Serial.print("Third sensor : ");
    Serial.println(dvc3.readTemperature('c'));
    Serial.println();
  }
}
