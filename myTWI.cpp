#include "myTWI.h"

const bool ACK = true;
const bool NACK = false;

void TWIInit() {    // sets up TWI

  PORTC |= (1 << PORTC4) | (1 << PORTC5);   // Activate pull-ups
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));   // Prescaler bits to 00
  TWBR = 72;   // 16MHz clock, 100kHz I2C
  /* twi bit rate formula:
    SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
    It is 72 for a 16Mhz board with 100kHz TWI */
  TWCR = (1 << TWEN);  // enable twi module
}

void TWIStart() {      // sends start
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
}

void TWIWrite(uint8_t data) {   // writes 8-bit data to slave
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
}

uint8_t TWIRead(bool ack_or_nack) {   // if ack_or_nack == true => ack is sent
  if (ack_or_nack == NACK) {
    TWCR = (1 << TWINT) | (1 << TWEN);
  } else {                            // if ack_or_nack == false => nack is sent
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  }
  while ((TWCR & (1 << TWINT)) == 0);
  return TWDR;
}

void TWIStop() {
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  while ((TWCR & (1 << TWSTO)));    // www.avrfreaks.net/forum/how-detect-end-sending-stop-condition-over-twi
}

uint8_t TWIGetStatus() {    // returns status of TWI. See datasheet for possible condition codes
  uint8_t status;
  status = TWSR & 0xF8;    //mask status
  return status;
}

void TWIcrashHandler() {
  TWCR = 0;   // releases SDA and SCL lines to high impedance
  TWCR = (1 << TWEN) | (1 << TWEA);   // reinitialize TWI
}
