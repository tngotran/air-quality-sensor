#include "Wire.h"

#define iaqaddress 0x5A

uint16_t predict;
uint8_t statu;
int32_t resistance;
uint16_t tvoc;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Serial.println("finish setup");
  pinMode(7, OUTPUT);

}
void waitMin(int x){
  int i = 0,ii=0;
  while(ii<10){
    delay(1000);
    i++;
    if(i>60){
      ii++;
      i=0;
      Serial.print("min ");Serial.println(ii);
    }
    if(ii>x){
      return;
    }
  }

}
void loop()
{

  readAllBytes();
  checkStatus();

  //Serial.print("CO2:");
  //Serial.print(predict);
  //Serial.print(", Status:");
  //Serial.print(statu, HEX);
  //Serial.print(", Resistance:");
  //Serial.print(resistance);
  //Serial.print(", TVoC:");
  //Serial.println(tvoc);

  delay(2000);

}
void readAllBytes()
{
  Serial.print("come here 1");
  Wire.beginTransmission(iaqaddress);

  byte sample = Wire.endTransmission(iaqaddress);
  Serial.print("come here 2");
  if(sample != 0)
  {
    Serial.print("slave does not response");
    Serial.print(sample);
  }else{
    Wire.requestFrom(iaqaddress, 9);
  }


  predict = (Wire.read()<< 8 | Wire.read());
  statu = Wire.read();
  resistance = (Wire.read()& 0x00)| (Wire.read()<<16)| (Wire.read()<<8| Wire.read());
  tvoc = (Wire.read()<<8 | Wire.read());
}
void checkStatus()
{
  if(statu == 0x10)
  {
    Serial.println("Warming up...");
    waitMin(3);
  }
  else if(statu == 0x00)
  {
    //Serial.println("Ready");
    digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
    Serial.print("CO2:");
    Serial.print(predict);
    Serial.print(", TVoC:");
    Serial.println(tvoc);
  }
  else if(statu == 0x01)
  {
    Serial.println("Busy");
  }
  else if(statu == 0x80)
  {
    Serial.println("Error");
  }
  else
  Serial.println("No Status, check module");
}
//
//#include <Wire.h>
//
//#define MPU6050 0x5A              //Device address
//#define CO 0xB5         //Accelerometer configuration address
//
//
//void setup()
//{
//  Serial.begin(9600);
//  Wire.begin();
////  Wire.setClock(100000);
//}
//
//void loop() {
//    angle_calc();
//    delay(1000);
//}
////calculate robot tilt angle
//void angle_calc()
//{
//   Serial.println("Come here1");
////   Wire.beginTransmission(MPU6050);
////
////   Wire.write(CO);                  // starting with register 0x3B (ACCEL_XOUT_H)
////   Wire.endTransmission();
//   Serial.println( "comhere 2");
//   Wire.requestFrom(MPU6050, 2,false);  // request a total of 4 registers (each registers only keep 8 bits, the value is 16 bits so we use an "OR" operator to merge them all)
//    Serial.println("Come here 3");
////    delay(5);
////    Serial.print(Wire.available());
////    while(Wire.available()){
//      int AcX = Wire.read() * 2^8 + Wire.read();
//        Serial.println(AcX);
//        //Serial.println(Wire.read());
////    }
//
//
//
//
//}
