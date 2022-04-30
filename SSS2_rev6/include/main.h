#include <Arduino.h>
#include <setPinModes.h>
#include <start_can.h>
#include <button_ops.h>
//#include <version.h>


//#include <SPI.h>
//#include <OneButton.h>
//#include <EEPROM.h>



elapsedMillis serial_display_timer;
uint32_t display_counter;


uint16_t pwmValue[numPWMs] = {500,1000,1500,2000,2500,3000};
uint16_t pwmFrequency[numPWMs] = {245,245,200,200,100,100};

