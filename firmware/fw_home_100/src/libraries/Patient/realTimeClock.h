/*
  Patient.h - Library for managing all patient related
  Created by Mateo Segura, May 20, 2021.
*/
#ifndef realTimeClock_h
#define realTimeClock_h

#include "Arduino.h"
#include "RTClib.h"

class realTimeClock
{
public:
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  DateTime getDateTimeObject()
  {
    DateTime now = DateTime(this->year, this->month, this->day, this->hour, this->minute, this->second);
    return now;
  }
};

#endif