# Nixie-clock
This is my nixie clock project.
For detailed description and a schematic of this project, please visit:

Compiled in Arduino IDE 1.8.6
Code could be compiled as an OLED display clock or as a nixie clock.
To switch between those versions comment/uncomment _#define USE_OLED_LIBRARIES_ in _config.h_ file.
I run the OLED version on an Arduino UNO board and the nixie version on an Arduino Pro Mini 5V 16MHz.
Before uploading the code, I increased the BOD level to 4,3V, by altering fuses in boards.txt file and reburning the bootloader.
