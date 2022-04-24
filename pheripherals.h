#ifndef PHERIPHERALS_H
#define PHERIPHERALS_H

#include "Arduino.h"
#include <avr/power.h>
#include <avr/sleep.h>

const uint8_t Button_UP = 2;     // PD2
const uint8_t Button_CHG = 3;    // PD3
const uint8_t RELAY = PORTD0;
const uint8_t BTN_UP_MSK = 1 << Button_UP;      // 0b00000100
const uint8_t BTN_CHG_MSK = 1 << Button_CHG;    // 0b00001000

inline void setupTimer2_16mS() {    // if I could, I would setup timer as 20ms interrupt. However, Timer2 max interval = 16ms
  power_timer2_enable();
  TCNT2 = 0;   // reset the counter
  TCCR2A = (TCCR2A & ~(1 << COM2A1) & ~(1 << COM2A0) & ~(1 << COM2B1) & ~(1 << COM2B0) & ~(1 << WGM20)) | (1 << WGM21);   // Configure timer 2 for CTC mode
  TCCR2B = (TCCR2B & ~(1 << WGM22)) | (1 << CS22) | (1 << CS21) | (1 << CS20);   // Start timer at Fcpu/1024
  TIMSK2 = (TIMSK2 & ~(1 << OCIE2B) & ~(1 << TOIE2)) | (1 << OCIE2A);   // Enable CTC interrupt
  OCR2A = 255;   // Set CTC compare value with a prescaler of 1024 (OCRA = 156 => 10MS interval, 255 = 16ms

}

inline void setupWDT8s() {    // WARNING: this function have to be called with interrupts disabled

  WDTCSR = (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDE) | (1 << WDIE) | (1 << WDP3) | (1 << WDP0);    //  8s
}

inline void attachButtonsInts() {         // both trigger on falling edge
  DDRD = DDRD & ~(1 << Button_CHG) & ~(1 << Button_UP);     // INPUT_PULLUP
  PORTD |= (1 << Button_CHG) | (1 << Button_UP);
  EICRA = (EICRA & ~(1 << ISC00) & ~(1 << ISC10)) | (1 << ISC11) | (1 << ISC01);      // falling edge
  EIMSK |= (1 << INT0) | (1 << INT1);             // enable interrupts
}

inline void disableSomePher() {
  UCSR0B = 0;      // disconnect pin 0 and 1 from UART. (arduino's init() does so)
  ADCSRA = 0;
  power_adc_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_spi_disable();
  power_usart0_disable();
}

inline bool digitalReadBtnUP() {
  return bool(PIND & BTN_UP_MSK);     // everything non-zero (i. e. 00000100) casts to true, else (00000000) -> 0
}

inline bool digitalReadBtnCHG() {
  return bool(PIND & BTN_CHG_MSK);
}

inline void RelayON() {
  DDRD |= (1 << RELAY);
  PORTD |= (1 << RELAY);
}

inline void goToSleep() {         // Puts MCU to sleep in Pover-Save, so that both WDT and Timer2 can wake up.
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  sleep_disable();
}

inline void handleWDTreset()      // WARNING: this function have to be called with interrupts disabled
{
  if (MCUSR & (1 << WDRF)) {              // If a reset was caused by the Watchdog Timer...
    MCUSR &= ~(1 << WDRF);                // Clear the WDT reset flag
    WDTCSR |= (1 << WDCE) | (1 << WDE);   // Enable the WD Change Bit
    WDTCSR = 0x00;                        // Disable the WDT
  }
}


#endif
