#ifndef DISPLAYING_H
#define DISPLAYING_H

#include "Arduino.h"
#include "time.h"
#include "Nixie.h"
#include "config.h"
#include "Blank_what.h"


#ifdef USE_OLED_LIBRARIES
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define _SCREEN_WIDTH 128 // OLED display width, in pixels
#define _SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#endif

template <cfg::Mode mode>
class Displaying {
};

#ifdef USE_OLED_LIBRARIES
template <>
class Displaying<cfg::Mode::OLED>
{
    const uint8_t DIGIT_HR_TENS_MSK = B00110000;
    const uint8_t DIGIT_HR_ONES_MSK = B00001111;
    const uint8_t DIGIT_MIN_TENS_MSK = B01110000;
    const uint8_t DIGIT_MIN_ONES_MSK = B00001111;
    const uint8_t DIGIT_TENS_Pos = 4;
    
    Adafruit_SSD1306 _display;

  public:
    Displaying() {
      Adafruit_SSD1306 _display(_SCREEN_WIDTH, _SCREEN_HEIGHT, &Wire, OLED_RESET);     // initialize Adafruit class
      this->_display = _display;
    }
    void init();
    void display(Time time, bool miganie_state = true, Blank_what blanking = Blank_what::HOURS);
    void displayError();
    Nixie splitTimeTo4(Time &time);
};
#endif


template <>
class Displaying<cfg::Mode::NIXIE>
{
    const uint8_t MOSFET_1 = DDB4; // 12       // Q1 in schematic
    const uint8_t MOSFET_2 = DDB5; // 13       // Q2 in schematic
    const uint8_t HR_ONES_SWITCH = PORTD7;     // Q5 in schematic
    const uint8_t MIN_SWITCH = PORTD1;     // Q6 and Q7 in schematic
    // there are 3 nixie drivers. They drive nixies 2, 3, 4 numbered from left to right in actual display (see below). Each driver has A, B, C, D inputs coded in binary
    //                                                                    HR         MIN
    //                                                nixies:           |$| |$|    |$| |$|
    /*                                                                  /    \     /     \                 */
    //                                                            mosfets  drvr2 drvr3   drvr4

    //                                       v arduino pin
    const uint8_t DRVER_NIXIE_2_A = DDB0; // 8                       hours
    const uint8_t DRVER_NIXIE_2_B = DDB1; // 9
    const uint8_t DRVER_NIXIE_2_C = DDB2; // 10
    const uint8_t DRVER_NIXIE_2_D = DDB3; // 11
    const uint8_t DRVER_NIXIE_3_A = DDD4; // 4                       minutes 0 - 5
    const uint8_t DRVER_NIXIE_3_B = DDD5; // 5
    const uint8_t DRVER_NIXIE_3_C = DDD6; // 6
    const uint8_t DRVER_NIXIE_4_A = DDC0; // 14                      minutes 0 - 9
    const uint8_t DRVER_NIXIE_4_B = DDC1; // 15
    const uint8_t DRVER_NIXIE_4_C = DDC2; // 16
    const uint8_t DRVER_NIXIE_4_D = DDC3; // 17

    const uint8_t DIGIT_HR_TENS_MSK = B00110000;       
    const uint8_t DIGIT_HR_ONES_MSK = B00001111;
    const uint8_t DIGIT_MIN_TENS_MSK = B01110000;
    const uint8_t DIGIT_MIN_ONES_MSK = B00001111;
    const uint8_t DIGIT_TENS_Pos = 4;

    const uint8_t PORTB_HR_TENS_MSK = B00110000;
    const uint8_t PORTB_HR_ONES_MSK = B00001111;
    const uint8_t PORTD_MIN_TENS_MSK = B01110000;       
    const uint8_t PORTC_MIN_ONES_MSK = B00001111;

    void displayHours(uint8_t tens, uint8_t ones);
    void displayMins(uint8_t tens, uint8_t ones);
    void hoursOff();
    void hoursOn();
    void minutesOff();
    void minutesOn();

  public:
    void init();
    void display(Time time, bool miganie_state = true, Blank_what blanking = Blank_what::HOURS);
    void displayError();
    Nixie splitTimeTo4(Time &time);
};

#endif
