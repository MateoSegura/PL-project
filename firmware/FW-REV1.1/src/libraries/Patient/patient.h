/*
  Patient.h - Library for managing all patient related
  Created by Mateo Segura, May 20, 2021.
*/
#ifndef patient_h
#define patient_h

#include "Arduino.h"
#include "realTimeClock.h"

#define NUMBER_OF_PRESCRIPTION_ALARMS 24

class Patient
{
public:
  Patient(realTimeClock now);
  void printPrescriptionAlarms();
  bool checkAlarms(realTimeClock now);

private:
  String first_name;
  String last_name;
  uint8_t tray_size_1; //in mm
  uint8_t tray_size_2;
  uint8_t tray_size_3;
  String medication_name_1;
  String medication_name_2;
  String medication_name_3;
  realTimeClock prescription_alarms[NUMBER_OF_PRESCRIPTION_ALARMS];
};

#endif