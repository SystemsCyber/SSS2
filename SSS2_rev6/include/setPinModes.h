#include <Arduino.h>
#include <SSS2defines.h>

void setPinModes(){
    pinMode(greenLEDpin,     OUTPUT);
    pinMode(redLEDpin,       OUTPUT);
    pinMode(CSCANPin,        OUTPUT);
    pinMode(CSconfigAPin,    OUTPUT);
    pinMode(CSconfigBPin,    OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(IH1Pin,          OUTPUT);
    pinMode(IH2Pin,          OUTPUT);
    pinMode(IL1Pin,          OUTPUT);
    pinMode(IL2Pin,          OUTPUT);
    pinMode(ignitionCtlPin,  OUTPUT);
    pinMode(INTCANPin,        INPUT);
    digitalWrite(CSconfigAPin, HIGH);
    digitalWrite(CSconfigBPin, HIGH);
    digitalWrite(redLEDpin,    HIGH);
    digitalWrite(greenLEDpin,  LOW);
    digitalWrite(IH2Pin,       LOW);
    digitalWrite(IL1Pin,       LOW);
    digitalWrite(IL2Pin,       LOW);
    digitalWrite(ignitionCtlPin,LOW);
    
    const int8_t PWMPins[numPWMs]     = {16,17,22,23,29,30};
    for (uint8_t i = 0; i < numPWMs; i++) pinMode(PWMPins[i], OUTPUT);
    
}
