#include "my_Pcf8563.h"

// void getTime(Time &time): "time" is in bcd. "time" is modified by reference.
// Returns success (true) or failure (false) of operation.
Read_status My_Pcf8563::getTime(Time &time) {
  // it tries to get time
  if (TWI_SUCCESS == getTimeRoutine(time)) {   // when transaction is successful, returns TWI_SUCCESS
    return Read_status::SUCCESS;
  } else {
    TWIcrashHandler();    // reset TWI bus
    time.hours = 0;      // To clear compiler warning and to not rely on default constructor
    time.minutes = 0;
    return Read_status::ERROR;
  }
}

// getTimeRoutine(Time &time): private method. Called by getTime(Time &time). Consist of I2C routines needed to get time.
bool My_Pcf8563::getTimeRoutine(Time &time) {
  TWIInit();
  TWIStart();
  if (TWIGetStatus() != TWI_START) {
    return TWI_ERROR;
  }
  TWIWrite(RTC_WRITE);
  if (TWIGetStatus() != MT_SLA_ACK) {
    return TWI_ERROR;
  }
  TWIWrite(MIN_ADDR);     // set pointer to minutes
  if (TWIGetStatus() != MT_DATA_ACK) {
    return TWI_ERROR;
  }
  TWIStart();            // restart
  if (TWIGetStatus() != TWI_RESTART) {
    return TWI_ERROR;
  }
  TWIWrite(RTC_READ);    // change address to read from slave
  if (TWIGetStatus() != MR_SLA_ACK) {
    return TWI_ERROR;
  }
  /* The datasheet recommends reading time in one go, from seconds to years. Because I am interested only in hours and minutes,
     I don't read calendar stuff. */
  time.minutes = TWIRead(ACK) & ~MINUTES_MSK;     // mask unnecessary bits RTC has in minutes register
  if (TWIGetStatus() != MR_DATA_ACK) {
    return TWI_ERROR;
  }
  time.hours = TWIRead(NACK) & ~HOURS_MSK;   // mask unnecessary bits RTC has in hours register
  if (TWIGetStatus() != MR_DATA_NACK) {
    return TWI_ERROR;
  }
  TWIStop();
  return TWI_SUCCESS;       // if all i2c commands went OK.
}


/* void setTime(): arguments are in bcd! Method does not return any errors. If any occurs twi is reset and method returns.
   Datasheet recommends to set time and calendar registers in one go. I am setting only seconds to 0, minutes and hours.
   Hope it doesn't affect RTC's performance. I'm not using calendar features. */
void My_Pcf8563::setTime(uint8_t hours, uint8_t minutes) const {
  TWIInit();
  TWIStart();
  if (TWIGetStatus() != TWI_START) {
    TWIcrashHandler();     // reset TWI
    return;
  }
  TWIWrite(RTC_WRITE);     // send SLA+W
  if (TWIGetStatus() != MT_SLA_ACK) {
    TWIcrashHandler();
    return;
  }
  TWIWrite(SEC_ADDR);      // set pointer
  if (TWIGetStatus() != MT_DATA_ACK) {
    TWIcrashHandler();
    return;
  }
  TWIWrite(0);    // zero seconds just for convenience.
  if (TWIGetStatus() != MT_DATA_ACK) {
    TWIcrashHandler();
    return;
  }
  TWIWrite(minutes);
  if (TWIGetStatus() != MT_DATA_ACK) {
    TWIcrashHandler();
    return;
  }
  TWIWrite(hours);
  if (TWIGetStatus() != MT_DATA_ACK) {
    TWIcrashHandler();
    return;
  }
  TWIStop();
}
