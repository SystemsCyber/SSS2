#include <Arduino.h>
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> Can0;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

uint32_t BAUDRATE0;
uint32_t BAUDRATE1;

static CAN_message_t txmsg;

void canSniff(const CAN_message_t &msg) {
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}

void start_can(){
    Can0.begin();
    Can0.setBaudRate(BAUDRATE0);
    Can0.setMaxMB(16);
    Can0.enableFIFO();
    Can0.enableFIFOInterrupt();
    Can0.onReceive(canSniff);
    Can0.mailboxStatus();
    
    Can1.begin();
    Can1.setBaudRate(BAUDRATE1);
    Can1.setMaxMB(16);
    Can1.enableFIFO();
    Can1.enableFIFOInterrupt();
    Can1.onReceive(canSniff);
    Can1.mailboxStatus();
}

