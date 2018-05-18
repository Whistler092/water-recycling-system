
/* 
 * this is Arduino code to use Dual axis XY Joystick with Nokia 5110 
 * also read the switch.
 * Other Arduino library and videos http://robojax.com/learn/arduino/
 
 * Watch the video for this code to learn it fully.
  * Watch the video here: https://youtu.be/Pk5Wig5EO0s
 * this code is offered "as is" without any warranty.
 * if you are sharing this code, you must keep this copyright note.
 
Nokia 5100 LCD Example Code
   Graphics driver and PCD8544 interface code for SparkFun's
   84x48 Graphic LCD.
   https://www.sparkfun.com/products/10168
   Original Source code
https://github.com/sparkfun/GraphicLCD_Nokia_5110

  This stuff could all be put into a library, but we wanted to
  leave it all in one sketch to keep it as transparent as possible.

  Hardware: (Note most of these pins can be swapped)
    Graphic LCD Pin ---------- Arduino Pin
       1-VCC       ----------------  5V
       2-GND       ----------------  GND
       3-SCE       ----------------  7 16
       4-RST       ----------------  6 5
       5-D/C       ----------------  5 4
       6-DN(MOSI)  ----------------  11 14
       7-SCLK      ----------------  13 12
       8-LED       - 330 Ohm res --  9  13
   The SCLK, DN(MOSI), must remain where they are, but the other 
   pins can be swapped. The LED pin should remain a PWM-capable
   pin. Don't forget to stick a current-limiting resistor in line
   between the LCD's LED pin and Arduino pin 9!
   Modefied by Ahmad S. for Robojax.com
   on Mar 11, 2018 at 20:49 at Ajax, Ontario, Canada
*/
#include <SPI.h>
#include "LCD_Functions.h"

void setup()
{
  Serial.begin(9600);

  lcdBegin(); // This will setup our pins, and initialize the LCD
  //updateDisplay(); // with displayMap untouched, SFE logo
  setContrast(40); // Good values range from 40-60
  delay(2000);

  clearDisplay(BLACK);
  updateDisplay();
}


void loop()
{
 
    // setPixel takes 2 to 3 parameters. The first two params
    // are x and y variables. The third optional variable is
    // a "color" boolean. 1 for black, 0 for white.
    // setPixel() with two variables will set the pixel with
    // the color set to black.
    // clearPixel() will call setPixel with with color set to
    // white.
   // setPixel(random(0, LCD_WIDTH), random(0, LCD_HEIGHT));
    // After drawing something, we must call updateDisplay()
    // to actually make the display draw something new.

    //invertDisplay(); // This will swap all bits in our display

    // setLine(x0, y0, x1, y1, bw) takes five variables. The
    // first four are coordinates for the start and end of the 
    // line. The last variable is the color (1=black, 0=white).
    setLine(1, 4, 70, 4, BLACK);
    updateDisplay();
delay(2000);
    //analogWrite(blPin, i); // blPin is ocnnected to BL LED

  /* setRect Example */
  clearDisplay(WHITE); // Start fresh

  // setCircle takes 5 parameters -- x0, y0, radius, bw, and
  // lineThickness. x0 and y0 are the center coords of the circ.
  // radius is the...radius. bw is the color (0=white, 1=black)
  // lineThickness is the line width of the circle, 1 = smallest
  // thickness moves in towards center.
   setCircle(20, 30, 20, BLACK, 2);
  updateDisplay();
  delay(2000);
clearDisplay(WHITE);

  setStr("Welcome to ", 0, 0, BLACK);
      updateDisplay();
    delay(100);
  setLine(0, 9, 70, 9, BLACK);
    updateDisplay();
    delay(100);
   setStr("Robojax ", 20, 20, BLACK);
  updateDisplay();
  delay(2000);
  clearDisplay(WHITE);

  
  // setRect takes six parameters (x0, y0, x1, y0, fill, bw)
  // x0, y0, x1, and y0 are the two diagonal corner coordinates
  // fill is a boolean, which determines if the rectangle is
  // filled in. bw determines the color 0=white, 1=black.
    setRect(10, 10, 70, 40, 1, BLACK);
    setStr("Robojax ", 15, 20, WHITE);
    updateDisplay();
  delay(2000);

  /* setCircle Example */
  clearDisplay(WHITE);  
 
}


