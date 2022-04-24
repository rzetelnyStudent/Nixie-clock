#include "displaying.h"

// sets pins, connected to drivers, as outputs. Call it on setup
void Displaying<cfg::Mode::NIXIE>::init() {
  DDRB |= (1 << DRVER_NIXIE_2_D) | (1 << DRVER_NIXIE_2_C) | (1 << DRVER_NIXIE_2_B) | (1 << DRVER_NIXIE_2_A);       // all pins as outputs
  DDRD |= (1 << DRVER_NIXIE_3_C) | (1 << DRVER_NIXIE_3_B) | (1 << DRVER_NIXIE_3_A);
  DDRC |= (1 << DRVER_NIXIE_4_D) | (1 << DRVER_NIXIE_4_C) | (1 << DRVER_NIXIE_4_B) | (1 << DRVER_NIXIE_4_A);
  DDRB |= (1 << MOSFET_1) | (1 << MOSFET_2);      // and mosfets' driver pins as outputs
  DDRD |= (1 << HR_ONES_SWITCH) | (1 << MIN_SWITCH);
}

#ifdef USE_OLED_LIBRARIES
// Call it on setup, inits adafruit class and OLED display
void Displaying<cfg::Mode::OLED>::init() {
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    for (;;); // Don't proceed, loop forever
  }
  _display.display();
  _display.clearDisplay();
  _display.setTextSize(4);
  _display.setTextColor(SSD1306_WHITE);
}


// displays parameter time (in bcd) on OLED. Can display half of display, if the editing mode is on
// param miganie_state: true -> all digits on; false -> half of digits on (blinking in editing mode)
// param set_hours: true -> hours off, minutes on; false -> the other way around
void Displaying<cfg::Mode::OLED>::display(Time time, bool miganie_state, Blank_what blanking) {
  Nixie digit;          // only uses nixie struct, has nothing to do with real nixies :)
  digit = splitTimeTo4(time);
  _display.clearDisplay();
  if (miganie_state) {               // just display everything (no blinking)
    switch (blanking)
    {
      case Blank_what::_23_7:       // forced to display 23:07
        _display.setCursor(0, 0);
        _display.print(2);
        _display.print(3);
        _display.setCursor(50, 0);
        _display.print(":");
        _display.setCursor(68, 0);
        _display.print(0);
        _display.print(7);
        break;
      default:                      // display actual "time"
        _display.setCursor(0, 0);
        _display.print(digit.HR_tens);
        _display.print(digit.HR_ones);
        _display.setCursor(50, 0);
        _display.print(":");
        _display.setCursor(68, 0);
        _display.print(digit.MIN_tens);
        _display.print(digit.MIN_ones);
        break;
    }
  } else {                          // here is done blinking a half of display. If user edits hours, needs to turn off hours (temporarily)
    switch (blanking)
    {
      case Blank_what::HOURS:       // blank hours (only minutes are displayed)
        _display.setCursor(50, 0);
        _display.print(":");
        _display.setCursor(68, 0);
        _display.print(digit.MIN_tens);
        _display.print(digit.MIN_ones);
        break;
      case Blank_what::MINS:     // blank minutes (only hours are displayed)
        _display.setCursor(0, 0);
        _display.print(digit.HR_tens);
        _display.print(digit.HR_ones);
        _display.setCursor(50, 0);
        _display.print(":");
        break;
        // if (blanking == Blank_what::ALL or _23_7), then only clearDisplay() and display() would execute. We want to blank whole display
    }
  }
  _display.display();
}
#endif


// displays parameter time (in bcd) on nixies. Can display half of display, if the editing mode is on
// param miganie_state: true -> all nixies on; false -> half of nixies on (blinking in editing mode)
// param set_hours: true -> hours off, minutes on; false -> the other way around
void Displaying<cfg::Mode::NIXIE>::display(Time time, bool miganie_state, Blank_what blanking) {
  Nixie nixie;
  nixie = splitTimeTo4(time);
  if (miganie_state) {     // just display everything (no blanking)
    if (blanking == Blank_what::_23_7) {
      displayHours(2, 3);
      displayMins(0, 7);
      minutesOn();
      hoursOn();
    } else {     // display real time
      displayHours(nixie.HR_tens, nixie.HR_ones);
      displayMins(nixie.MIN_tens, nixie.MIN_ones);
      minutesOn();
      hoursOn();
    }
  } else {     // here is done blinking a half of display. If user edits hours, needs to turn off hours (temporarily)
    if (blanking == Blank_what::HOURS) {
      displayMins(nixie.MIN_tens, nixie.MIN_ones);
      minutesOn();
      hoursOff();
    } else if (blanking == Blank_what::MINS) {          // If user edits minutes, needs to turn off minutes (temporarily)
      displayHours(nixie.HR_tens, nixie.HR_ones);
      minutesOff();
      hoursOn();
    } else {     // (blanking == Blank_what::ALL || _23_07) => needs to turn off evrything (useful for animation)
      hoursOff();
      minutesOff();
    }
  }
}

void Displaying<cfg::Mode::NIXIE>::displayHours(const uint8_t tens, const uint8_t ones) {      // very custom method just to set actual pins
  if ((tens > 2) || (ones > 9)) {        // arguments reasonability check
    PORTB &= ~PORTB_HR_TENS_MSK;     // if arguments are not valid, display 0:
    PORTB &= ~PORTB_HR_ONES_MSK;
  } else {       // if arguments OK, display time
    PORTB &= ~PORTB_HR_TENS_MSK;
    PORTB |= tens << DIGIT_TENS_Pos;
    PORTB &= ~PORTB_HR_ONES_MSK;
    PORTB |= ones;
  }
}

void Displaying<cfg::Mode::NIXIE>::displayMins(const uint8_t tens, const uint8_t ones) {
  if ((tens > 7) || (ones > 9)) {       // arguments reasonability check
    PORTD &= ~PORTD_MIN_TENS_MSK;      // if arguments are not valid, display :00
    PORTC &= ~PORTC_MIN_ONES_MSK;
  } else {      // if arguments OK, display time
    PORTD &= ~PORTD_MIN_TENS_MSK;
    PORTD |= tens << DIGIT_TENS_Pos;
    PORTC &= ~PORTC_MIN_ONES_MSK;
    PORTC |= ones;
  }
}

// Switch transistors are arranged so that: GPIO = HIGH -> Nixie off
void Displaying<cfg::Mode::NIXIE>::hoursOff() {
  PORTD |= (1 << PORTD7);      // turn hours ones switch transistor on
  PORTB &= ~((1 << MOSFET_1) | (1 << MOSFET_2));   // turn hours (1, 2) mosfets off
}

void Displaying<cfg::Mode::NIXIE>::hoursOn() {
  PORTD &= ~(1 << PORTD7);   // turn off hours ones switch transistor, note that hour tens are controlled via mosfets directly
}

void Displaying<cfg::Mode::NIXIE>::minutesOn() {        // turn off minutes switch transistor
  PORTD &= ~(1 << MIN_SWITCH);
}

void Displaying<cfg::Mode::NIXIE>::minutesOff() {         // turn on minutes switch transistor
  PORTD |= (1 << MIN_SWITCH);
}


#ifdef USE_OLED_LIBRARIES
// displayError() : call when RTC chip is not working. When OLED is used, displays "Error".
void Displaying<cfg::Mode::OLED>::displayError() {
  _display.clearDisplay();
  _display.setCursor(0, 0);
  _display.print("Error");
  _display.display();
}
#endif

// With nixies, displays "10:70". That's an "error code".
void Displaying<cfg::Mode::NIXIE>::displayError() {
  displayHours(1, 0);
  displayMins(7, 0);
  minutesOn();
  hoursOn();
}

// splits bcd time to 4 bytes, each containing single digit to be displayed
Nixie Displaying<cfg::Mode::NIXIE>::splitTimeTo4(Time &time) {
  Nixie nixie;
  nixie.HR_tens = (time.hours & DIGIT_HR_TENS_MSK) >> DIGIT_TENS_Pos;
  nixie.HR_ones = (time.hours & DIGIT_HR_ONES_MSK);
  nixie.MIN_tens = (time.minutes & DIGIT_MIN_TENS_MSK) >> DIGIT_TENS_Pos;
  nixie.MIN_ones = (time.minutes & DIGIT_MIN_ONES_MSK);
  return nixie;
}

#ifdef USE_OLED_LIBRARIES
// the same method:
Nixie Displaying<cfg::Mode::OLED>::splitTimeTo4(Time &time) {
  Nixie nixie;
  nixie.HR_tens = (time.hours & DIGIT_HR_TENS_MSK) >> DIGIT_TENS_Pos;
  nixie.HR_ones = (time.hours & DIGIT_HR_ONES_MSK);
  nixie.MIN_tens = (time.minutes & DIGIT_MIN_TENS_MSK) >> DIGIT_TENS_Pos;
  nixie.MIN_ones = (time.minutes & DIGIT_MIN_ONES_MSK);
  return nixie;
}
#endif
