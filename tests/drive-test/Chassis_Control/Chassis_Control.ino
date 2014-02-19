#include <Servo.h>
#include <map>
#include <string>
#include <Wire.h>

// Insert own filepath here
#include "C:\Users\Taylor\Documents\Arduino\libraries\ChassisData.h"

//Packet size of ChassisData object in bytes
#ifndef NECESSARY_PACKET_SIZE
#define NECESSARY_PACKET_SIZE  sizeof(ChassisData)
#endif

#ifndef TIMEOUT
#define TIMEOUT 1000
#endif

const int TALON_LEFT = 8, TALON_RIGHT = 9, MIN_FREQUENCY = 1001, NEUTRAL_FREQUENCY = 1500, MAX_FREQUENCY = 2041;
unsigned long timeLastPacket;

Servo leftMotor, rightMotor;

ChassisData data;
bool dataValid;

bool readBytes(void* dest, size_t len)
{
  char* charDest = (char*)dest;
  for(int i = 0; i < len; i++)
  {
    if(Wire.available())
      *(charDest++) = (char)Wire.read();
    else
      return false;
  }
  return true;
}

void setup()
{
  // Attach the motors and set their starting speed to neutral/zero speed
  leftMotor.attach(TALON_LEFT);
  rightMotor.attach(TALON_RIGHT);
  leftMotor.writeMicroseconds(NEUTRAL_FREQUENCY);
  rightMotor.writeMicroseconds(NEUTRAL_FREQUENCY);
  
  Wire.begin(4);
  
  Serial.begin(115200);
  timeLastPacket = 0;
  dataValid = false;
  
  Serial.println("Initialization complete.");
}

void loop()
{
  static bool timedOut = false;
  static unsigned char fullByte = 255;
  int leftSpeed, rightSpeed;
  
  while(Wire.available() >= NECESSARY_PACKET_SIZE)
  {
    dataValid = readBytes(&data, NECESSARY_PACKET_SIZE);
    Serial.print("DATA RECIEVED: ");
    Serial.println(dataValid);
  }
  
  if(millis() - timeLastPacket < TIMEOUT && dataValid)
  {
    // Set speeds of motors and print
    leftMotor.writeMicroseconds(leftSpeed);
    rightMotor.writeMicroseconds(rightSpeed);
  }
  else
  {
    // Timeout handling (stop motors)
    leftMotor.writeMicroseconds(NEUTRAL_FREQUENCY);
    rightMotor.writeMicroseconds(NEUTRAL_FREQUENCY);
    if(!timedOut) {
      timedOut = true;
      Serial.println("TIMEOUT REACHED");
    }
  }
}
