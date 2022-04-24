#ifndef MY_TWI_H
#define MY_TWI_H

#include <Arduino.h>

#define TWI_ERROR false
#define TWI_SUCCESS true
#define TWI_START 0x08       // TWI condition codes
#define TWI_RESTART 0x10
#define MT_SLA_ACK 0x18
#define MT_DATA_ACK 0x28
#define MR_SLA_ACK 0x40
#define MR_DATA_ACK 0x50
#define MR_DATA_NACK 0x58

void TWIInit();
void TWIStart();
void TWIWrite(uint8_t data);
uint8_t TWIRead(bool ack_or_nack);
void TWIStop();
uint8_t TWIGetStatus();
void TWIcrashHandler();

#endif
