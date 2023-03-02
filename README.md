# Nixie-clock
This is my nixie clock project.
For detailed description and a schematic of this project, please visit the project page:
https://makeelectronicsgreatagainblog.wordpress.com/2022/04/24/nixie-clock/

![image](https://user-images.githubusercontent.com/72305802/222466031-7029fcae-4053-4450-b50a-d1d87c701c1e.png)


Compiled in Arduino IDE 1.8.6.

The Code could be compiled as an OLED display clock or as a nixie clock.

To switch between those versions comment/uncomment _#define USE_OLED_LIBRARIES_ in _config.h_ file.

I run the OLED version on an Arduino UNO board and the nixie version on an Arduino Pro Mini 5V 16MHz.

Before uploading the code, I increased the BOD level to 4,3V, by altering fuses in boards.txt file and reburning the bootloader.

![image](https://user-images.githubusercontent.com/72305802/222466239-99055175-f0f6-411a-98f3-c3849f0aa406.png)
![image](https://user-images.githubusercontent.com/72305802/222466327-d5047a9a-d372-48d4-9be1-62f0cea9f714.png)
