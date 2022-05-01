#define USB_RAWHID //Include this to get the stability
#include <Arduino.h>
#include <main.h>



bool green;

void setup() {
  // put your setup code here, to run once:
  Serial.println("Starting Up...");
  setPinModes();
  start_can();
  start_button();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  button.tick();
  
  if (serial_display_timer>=200){
    serial_display_timer = 0;
    display_counter+=1;
    Serial.println(display_counter);
    green = !green;
    digitalWrite(greenLEDpin,green);
  }
  
}