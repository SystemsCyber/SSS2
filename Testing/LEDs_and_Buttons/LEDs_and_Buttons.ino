#include <Encoder.h>

const int8_t greenLEDpin       = 2;
const int8_t redLEDpin         = 5;
const int8_t buttonPin         = 24;
const int8_t encoderAPin       = 28;
const int8_t encoderBPin       = 25;
Encoder knob(encoderAPin, encoderBPin);

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(encoderAPin, INPUT_PULLUP);
  pinMode(encoderBPin, INPUT_PULLUP);
  
  pinMode(greenLEDpin, OUTPUT);
  pinMode(redLEDpin, OUTPUT);

  digitalWrite(greenLEDpin,HIGH);
  digitalWrite(redLEDpin,HIGH);
  delay(1000);
  digitalWrite(greenLEDpin,HIGH);
  digitalWrite(redLEDpin,HIGH);
    while(!Serial);

}
int temp=1;
void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Button: ");
  Serial.print(digitalRead(buttonPin));
    Serial.print(" encoderAPin: ");
Serial.print(digitalRead(encoderAPin));
    Serial.print(" encoderBPin: ");
Serial.println(digitalRead(encoderBPin));

Serial.println(knob.read());
  if (digitalRead(buttonPin)){
    digitalWrite(greenLEDpin,digitalRead(encoderAPin)); 
    digitalWrite(redLEDpin,digitalRead(encoderBPin)); 
  }
  else
  {
    digitalWrite(greenLEDpin,HIGH); 
    digitalWrite(redLEDpin,HIGH);   
    }
    delay(50);
}
