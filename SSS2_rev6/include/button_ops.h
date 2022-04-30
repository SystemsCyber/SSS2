
#include <Arduino.h>
#include <SSS2defines.h>
#include <OneButton.h>


OneButton button(buttonPin, true);

bool ignitionCtrlState;

void myClickFunction() {}//Do nothing at this time.
void myDoubleClickFunction() {}//Do nothing at this time.
void longPress() {digitalWrite(greenLEDpin,HIGH);} 
void longPressStop() {digitalWrite(greenLEDpin,LOW);} 

void longPressStart() {
    Serial.println("Long Press Start");
//   ignitionCtlState = !ignitionCtlState;
//   commandPrefix = "50";
//   if (ignitionCtlState) commandString = "1";
//   else commandString = "0";
//   fastSetSetting();
}

void start_button(){
  button.attachClick(myClickFunction);
  button.attachDoubleClick(myDoubleClickFunction);
  button.attachLongPressStart(longPressStart);
  button.attachLongPressStop(longPressStop);
  button.attachDuringLongPress(longPress);
  button.setPressTicks(2000);
  button.setClickTicks(250);
}