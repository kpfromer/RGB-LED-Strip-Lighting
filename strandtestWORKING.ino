#include "LPD8806.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif

// Example to control LPD8806-based RGB LED Modules in a strip

/*****************************************************************************/

// Number of RGB LEDs in strand:
int nLEDs = 32*1;//32per strip

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 13;//22, green
int clockPin = 12;//23
//Door pin is 11
int potPin = A0;
int lastDoorPinState = 0;

String curMode = "off";

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

  Serial.begin(9600);
  pinMode(11, INPUT_PULLUP);//DOOR PIN try anlog pins
  pinMode(potPin, INPUT);
}


void loop() {

  // Send a simple pixel chase in...
  /*
    colorChase(strip.Color(127, 127, 127), 50); // White
    colorChase(strip.Color(127,   0,   0), 50); // Red
    colorChase(strip.Color(127, 127,   0), 50); // Yellow
    colorChase(strip.Color(  0, 127,   0), 50); // Green
    colorChase(strip.Color(  0, 127, 127), 50); // Cyan
    colorChase(strip.Color(  0,   0, 127), 50); // Blue
    colorChase(strip.Color(127,   0, 127), 50); // Violet

    // Send a theater pixel chase in...
    theaterChase(strip.Color(127, 127, 127), 50); // White
    theaterChase(strip.Color(127,   0,   0), 50); // Red
    theaterChase(strip.Color(127, 127,   0), 50); // Yellow
    theaterChase(strip.Color(  0, 127,   0), 50); // Green
    theaterChase(strip.Color(  0, 127, 127), 50); // Cyan
    theaterChase(strip.Color(  0,   0, 127), 50); // Blue
    theaterChase(strip.Color(127,   0, 127), 50); // Violet

    // Fill the entire strip with...
    colorWipe(strip.Color(127,   0,   0), 50);  // Red
    colorWipe(strip.Color(  0, 127,   0), 50);  // Green
    colorWipe(strip.Color(  0,   0, 127), 50);  // Blue
  */
  if (digitalRead(11) != lastDoorPinState) { //If the state of the door has changed
    //int hour = hour();
    if (digitalRead(11) == HIGH) { //Door pin Open
      colorWipe(0, 50);//set lights off
    } else if (digitalRead(11) == LOW /*&& (hour<=6) || hour>=15) Time Not Working*/) { //If door is closed and if the time is less than 7am or greater than 4 pm then turn on lights
      colorWipe(strip.Color(127, 127, 127), 50);
    }
  }
  
//  int sensorValue = analogRead(potPin);
//  Serial.println(sensorValue);
//  if(sensorValue >= 0 && sensorValue < 221){
//    colorWipe(0, 50);
//  }else if(sensorValue > 221 && sensorValue < (221*2)){
//    colorWipe(strip.Color(127, 127, 127), 50);
//  }else if(sensorValue > (221*2) && sensorValue < (221*3)){
//    rainbow(10);
//  }
  
  lastDoorPinState = digitalRead(11); //set the last state for future.
  switch (Serial.parseInt()) {
    case 1://On
      curMode = "on";
      colorWipe(strip.Color(127, 127, 127), 50);
      break;
    case 2://Off
      curMode = "off";
      colorWipe(0, 50);
      break;
    case 3://Speical Color
      curMode = "on";
      {
        int r = 0; int g = 0; int b = 0;
        while (Serial.available() == 0) {}
        r = Serial.parseInt();
        while (Serial.available() == 0) {}
        g = Serial.parseInt();
        while (Serial.available() == 0) {}
        b = Serial.parseInt();
        colorWipe(strip.Color(r, g, b), 50);
      }
      break;
    case 4://Rainbow!
      rainbow(10);
      curMode = "on";
      break;
    case 5:
      Serial.println(curMode);
  }

  /*
    if(Serial.available()){
    Serial.println(Serial.readString().substring(0,2));
    if(Serial.readString().substring(0,2) == "on"){
      colorWipe(strip.Color(127, 127, 127), 50);
    }else if(Serial.readString().substring(0,2) == "of"){
      Serial.println("Off");
      colorWipe(0, 50);
    }
    }*/
  /*
    if(Serial.readString()=="on"){
    Serial.println("On");
    colorWipe(strip.Color(127, 127, 127), 50);
    }else if(Serial.readString()=="off"){
    Serial.println("Off");
    stripOff();
    }*/

  /*}else{
    Serial.println(digitalRead(raspPin));
    if(digitalRead(raspPin) == HIGH){
      colorWipe(strip.Color(127, 127, 127), 50);
      curMode = "on";
    }else if(digitalRead(raspPin) == LOW){
      colorWipe(0, 50);
      curMode = "off";
    }
    }*/
  //rainbow(10);
  //rainbowCycle(0);  // make it go through the cycle fairly fast
  //theaterChaseRainbow(50);
}

void stripOff() {
  colorWipe(strip.Color(0, 0, 0), 50);
}

void colorSet() {
  colorWipe(strip.Color(127, 127, 127), 50);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void rainbow(uint8_t wait) {
  int i, j;
  while (Serial.available() == 0) { //Run Unless serial input!
    for (j = 0; j < 384 && Serial.available() == 0; j++) { // 3 cycles of all 384 colors in the wheel
      for (i = 0; i < strip.numPixels() && Serial.available() == 0; i++) {
        strip.setPixelColor(i, Wheel( (i + j) % 384));
      }
      strip.show();   // write all the pixels out
      delay(wait);
    }
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 384 * 5; j++) {   // 5 cycles of all 384 colors in the wheel
    for (i = 0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Fill the dots progressively along the strip.
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Chase one dot down the full strip.
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for (i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 384; j++) {   // cycle all 384 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 384)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}
/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch (WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break;
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break;
    case 2:
      b = 127 - WheelPos % 128;  //blue down
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break;
  }
  return (strip.Color(r, g, b));
}
