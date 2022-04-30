#include <Arduino.h>

elapsedMillis serial_display_timer;
uint32_t display_counter;

/****************************************************************/
/*                         Pin Defintions                       */
const int8_t greenLEDpin       = 2;
const int8_t redLEDpin         = 5;
const int8_t CSCANPin          = 31;
const int8_t INTCANPin         = 21;
const int8_t CSconfigAPin      = 26;
const int8_t CSconfigBPin      = 27;
const int8_t buttonPin         = 24;
const int8_t encoderAPin       = 28;
const int8_t encoderBPin       = 25;
const int8_t CStouchPin        = 26;
const int8_t IH1Pin            = 35;
const int8_t IH2Pin            = 36;
const int8_t IL1Pin            = 37;
const int8_t IL2Pin            = 38;
const int8_t ignitionCtlPin    = 39;

const uint8_t numPWMs = 6;
const int8_t PWMPins[numPWMs]     = {16,17,22,23,29,30};
uint16_t pwmValue[numPWMs] = {500,1000,1500,2000,2500,3000};
uint16_t pwmFrequency[numPWMs] = {245,245,200,200,100,100};

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
    digitalWrite(IH2Pin,LOW);
    digitalWrite(IL1Pin,LOW);
    digitalWrite(IL2Pin,LOW);
    digitalWrite(ignitionCtlPin,LOW);
    
    uint8_t i;
    for (i = 0; i < numPWMs; i++) pinMode(PWMPins[i], OUTPUT);  
}


void setup() {
  // put your setup code here, to run once:
  setPinModes();
  //Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  bool button_state = digitalRead(buttonPin);
  digitalWrite(greenLEDpin,!button_state);
  if (serial_display_timer>=200){
    serial_display_timer = 0;
    display_counter+=1;
    Serial.printf("%d ",display_counter);
    Serial.printf("button_state: %d\n",button_state);
  }
  
}