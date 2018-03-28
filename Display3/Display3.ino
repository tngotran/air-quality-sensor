///*
//  LiquidCrystal Library - display() and noDisplay()
//
// Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
// library works with all LCD displays that are compatible with the
// Hitachi HD44780 driver. There are many of them out there, and you
// can usually tell them by the 16-pin interface.
//
// This sketch prints "Hello World!" to the LCD and uses the
// display() and noDisplay() functions to turn on and off
// the display.
//
// The circuit:
// * LCD RS pin to digital pin 12
// * LCD Enable pin to digital pin 11
// * LCD D4 pin to digital pin 5
// * LCD D5 pin to digital pin 4
// * LCD D6 pin to digital pin 3
// * LCD D7 pin to digital pin 2
// * LCD R/W pin to ground
// * 10K resistor:
// * ends to +5V and ground
// * wiper to LCD VO pin (pin 3)
//
// Library originally added 18 Apr 2008
// by David A. Mellis
// library modified 5 Jul 2009
// by Limor Fried (http://www.ladyada.net)
// example added 9 Jul 2009
// by Tom Igoe
// modified 22 Nov 2010
// by Tom Igoe
//
// This example code is in the public domain.
//
// http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay
//
// */
//
//// include the library code:
//#include <LiquidCrystal.h>
//
//// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//
//void setup() {
//  // set up the LCD's number of columns and rows:
//  lcd.begin(16, 2);
//  // Print a message to the LCD.
//  lcd.print("hello, world!");
//}
//
//void loop() {
//  // Turn off the display:
//  lcd.noDisplay();
//  delay(500);
//  // Turn on the display:
//  lcd.display();
//  delay(500);
//}
/*
 A set of custom made large numbers for a 16x2 LCD using the
 LiquidCrystal librabry. Works with displays compatible with the
 Hitachi HD44780 driver.
 The Cuicuit:
 LCD RS pin to D12
 LCD Enable pin to D11
 LCD D4 pin to D5
 LCD D5 pin to D4
 LCD D6 pin to D3
 LCD D7 pin to D2
 LCD Vee tied to a pot to control brightness
 LCD Vss and R/W tied to ground
 LCD Vcc to +5V
 Ronivaldo Sampaio (ronivaldo@gmail.com) for 3 lines
 2013/05/15 -> change for character with only 3 lines
 Original Made by Michael Pilcher
 2/9/2010
 character created using:
 http://mikeyancey.com/hamcalc/lcd_characters.php
 */
// include the library
#include <LiquidCrystal.h>
// #include "Wire.h"
#include "I2C.h"
#define iaqaddress 0x5A
uint16_t predict;
uint8_t statu;
int32_t resistance;
uint16_t tvoc;
// initialize the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int ini = 0, sec = 0,  min = 0,  hour = 0;
// the 8 arrays that form each segment of the custom numbers
unsigned long previousMillis = 0;
const long interval = 1000;           // interval at which to blink (milliseconds)
uint8_t waitTime = 0;
byte bar1[8] =
{
        B00000,
        B00000,
        B00000,
        B00001,
        B00011,
        B00111,
        B01111,
        B11111
};
byte bar2[8] =
{
        B00000,
        B00000,
        B00000,
        B10000,
        B11000,
        B11100,
        B11110,
        B11111
};
byte bar3[8] =
{
        B11111,
        B11110,
        B11100,
        B11000,
        B10000,
        B00000,
        B00000,
        B00000
};
byte bar4[8] =
{
        B11111,
        B01111,
        B00111,
        B00011,
        B00001,
        B00000,
        B00000,
        B00000
};
byte bar5[8] =
{
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B00000,
        B00000,
        B00000
};
byte bar6[8] =
{
        B00000,
        B00000,
        B00000,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111
};
byte bar7[8] =
{
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111
};
byte bar8[8] =
{
        B00000,
        B00100,
        B01110,
        B01110,
        B11111,
        B01110,
        B00100,
        B00000
};
void setup()
{
  // assignes each segment a write number
  lcd.createChar(1,bar1);
  lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);
  lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);
  lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);
  lcd.createChar(8,bar8);
  // sets the LCD's rows and colums:
  lcd.begin(20, 4);
  //Setup for IAQ
  Serial.begin(9600);
  I2c.begin();
  pinMode(7, OUTPUT);//LED for notify something
  readIAQ();
  //set up clock
  print_un_dot(00);
  update_min(00);
  update_hour(00);
}
void printRPM(int rpm) {
  int m , c, d, u, number;
  number = rpm;
  if (number > 999) {
    m = (number - (number % 1000)) / 1000;
    number = number % 1000;
  } else {
    m = 0;
  }
  if (number > 99) {
    c = (number - (number % 100)) / 100;
    number = number % 100;
  } else {
    c = 0;
  }
  if (number > 9) {
    d = (number - (number % 10)) / 10;
    number = number % 10;
  } else {
    d = 0;
  }
  u = number;
  // lcd.setCursor(0, 0);
  // lcd.print("RPM: ");
  printNumber(m, 4);
  printNumber(c, 7);
  printNumber(d, 11);
  printNumber(u, 14);
}
void loop()
{
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= interval){
      previousMillis = currentMillis;
      sec = sec + 1;
      if(waitTime!=0){
        uint8_t pos = 17;
        if(waitTime>99)
          pos = 17;
        else if (waitTime>9) pos = 18;
        else pos = 19;
        lcd.setCursor(pos, 2);  lcd.print(waitTime--);
      }
      if(sec > 59){
        min = min + 1;
        update_min(min);
        if(min > 59){
          hour = hour + 1;
          if(hour>12) hour = 1;
          update_hour(hour);
          min = 0;
        }
        //if(min % 5 == 0 && flag == true) readIAQ();

        sec = 0;lcd.setCursor(14, 4);lcd.write(32);
      }
      if(sec % 2 == 0)
        print_dot(sec);
      else
        print_un_dot(sec);
      //for debug only
      if(waitTime==0 && sec%10==0)  readIAQ();
    }
}
void readIAQ(){
  readAllBytes();
  checkStatus();
}
void update_hour(int number){
  int d;
  if (number > 9) {
    d = (number - (number % 10)) / 10;
    number = number % 10;
  }else
    d = 0;
  printNumber(d, 0);
  printNumber(number, 3);
}
void update_min(int number){
    int d;
    if (number > 9) {
      d = (number - (number % 10)) / 10;
      number = number % 10;
    }else
      d = 0;
    printNumber(d, 7);
    printNumber(number, 10);
}
void print_dot(int sec){
  lcd.setCursor(6, 1);
  lcd.write(8);
  // lcd.print((char)42);
  lcd.setCursor(6, 2);
  // lcd.print((char)42);
   lcd.write(8);
  lcd.setCursor(13, 4);
  lcd.print(sec);
}
void print_un_dot(int sec){
  lcd.setCursor(6, 1);
  lcd.write(32);
  lcd.setCursor(6, 2);
  lcd.write(32);
  lcd.setCursor(13, 4);
  lcd.print(sec);
}
void readAllBytes()
{
  I2c.read(iaqaddress,9);
  predict = I2c.receive()<< 8 | I2c.receive();
  statu = I2c.receive();
  I2c.receive();I2c.receive();I2c.receive();I2c.receive();
  tvoc = (I2c.receive()<<8 | I2c.receive());
}
void checkStatus()
{
  clearScr();
  if(statu == 0x10)
  {
    //Serial.println("Warming up...");
//    printIAQLCD("warming",12,"up",12);
    lcd.setCursor(13, 0);  lcd.print("warming");
    lcd.setCursor(15, 1);  lcd.print("up...");
    waitTime = 3*60;
  }
  else if(statu == 0x00)
  {
    waitTime = 0;
    //Serial.print("CO2:");    Serial.print(predict);
    //Serial.print(", TVoC:");    Serial.println(tvoc);
    lcd.setCursor(16, 0);  lcd.print("CO2:");
    lcd.setCursor(15, 2);  lcd.print("TVoC:");
    if(predict>1000 || tvoc>300)digitalWrite(7, HIGH);
    else digitalWrite(7, LOW);
    if(predict>999){
      lcd.setCursor(16, 1);  lcd.print(predict); }
    else{
      lcd.setCursor(17, 1);  lcd.print(predict); }


      lcd.setCursor(17, 3);  lcd.print(tvoc);

  }
  else if(statu == 0x01)
  {
    //Serial.println("Busy");

    lcd.setCursor(14, 0);  lcd.print("Sensor");
    lcd.setCursor(15, 1);  lcd.print("busy");
  }
  else if(statu == 0x80)
  {
    //Serial.println("Error");
    lcd.setCursor(14, 0);  lcd.print("Sensor");
    lcd.setCursor(14, 1);  lcd.print("error");
  }
  else{
    //Serial.println("No Status, check module");
    lcd.setCursor(16, 0);  lcd.print("plz");
    lcd.setCursor(13, 1);  lcd.print("connect");
    lcd.setCursor(14, 2);  lcd.print("sensor");
  }
}
//void printIAQLCD(String st1,int ps1,String st2,int ps2)
//{
//
//  lcd.setCursor(ps1, 1);  lcd.print(st1);
//  lcd.setCursor(ps2, 3);  lcd.print(st2);
//
//}
void clearScr(){
   lcd.setCursor(13, 0);  lcd.print("       ");
   lcd.setCursor(13, 1);  lcd.print("       ");
   lcd.setCursor(14, 2);  lcd.print("      ");
   lcd.setCursor(15, 3);  lcd.print("     ");
 }
void custom0(int col)
{ // uses segments to build the number 0
  lcd.setCursor(col, 0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(7);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void custom1(int col)
{
  lcd.setCursor(col,0);
  lcd.write(32);
  lcd.write(1);
  lcd.write(6);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col,3);
  lcd.write(32);
  lcd.write(32);
  lcd.write(5);
}
void custom2(int col)
{
  lcd.setCursor(col,0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(1);
  lcd.write(6);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(7);
  lcd.write(32);
  lcd.write(32);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void custom3(int col)
{
  lcd.setCursor(col,0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(6);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void custom4(int col)
{
  lcd.setCursor(col,0);
  lcd.write(6);
  lcd.write(32);
  lcd.write(6);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(32);
  lcd.write(32);
  lcd.write(5);
}
void custom5(int col)
{
  lcd.setCursor(col,0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 2);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void custom6(int col)
{
  lcd.setCursor(col,0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 2);
  lcd.write(7);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void custom7(int col)
{
  lcd.setCursor(col,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(32);
  lcd.write(32);
  lcd.write(5);
}
void custom8(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(7);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void custom9(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(7);
  lcd.setCursor(col, 2);
  lcd.write(32);
  lcd.write(32);
  lcd.write(7);
  lcd.setCursor(col, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(3);
}
void printNumber(int value, int col) {
  if (value == 0) {
    custom0(col);
  } if (value == 1) {
    custom1(col);
  } if (value == 2) {
    custom2(col);
  } if (value == 3) {
    custom3(col);
  } if (value == 4) {
    custom4(col);
  } if (value == 5) {
    custom5(col);
  } if (value == 6) {
    custom6(col);
  } if (value == 7) {
    custom7(col);
  } if (value == 8) {
    custom8(col);
  } if (value == 9) {
    custom9(col);
  }
}
