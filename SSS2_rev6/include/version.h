#include <Arduino.h>

//softwareVersion
char softwareVersion[] = "SSS2*REV6*1.2*";

void listSoftware(){
 if (Serial){
  Serial.print("FIRMWARE ");
  Serial.println(softwareVersion);
 }
}