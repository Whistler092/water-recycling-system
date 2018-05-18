#include "ESP8266_Nokia5110.h"

#define PIN_SCE   5  //D1
#define PIN_RESET 4  //D2
#define PIN_DC    0  //D3
#define PIN_SDIN  2  //D4
#define PIN_SCLK  14 //D5

// LCD Gnd .... GND
// LCD Vcc .... 3.3v

ESP8266_Nokia5110 lcd = ESP8266_Nokia5110(PIN_SCLK,PIN_SDIN,PIN_DC,PIN_SCE,PIN_RESET);


void setup() {
  lcd.begin();
  lcd.clear();
  lcd.setContrast(0x20);
  lcd.setCursor(0,1);
  lcd.print("Te amo <3");
  lcd.setCursor(1,2);
  lcd.print("Amor dm vida");
  
  lcd.setCursor(3,4);
  lcd.print("Att");
  lcd.setCursor(4,5);
  lcd.print("Tu esposo!");

}

void loop() {

}
