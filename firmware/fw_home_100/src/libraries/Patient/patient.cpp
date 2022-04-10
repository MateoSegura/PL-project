#include "Arduino.h"
#include "patient.h"

Patient::Patient(realTimeClock now)
{
    this->first_name = "Mateo";
    this->last_name = "Segura";
    this->tray_size_1 = 10;
    this->tray_size_2 = 10;
    this->tray_size_3 = 10;
    this->medication_name_1 = "Advil";
    this->medication_name_2 = "M&M's";
    this->medication_name_3 = "Other";

    //create dummy alarms
    for (int i = 0; i < NUMBER_OF_PRESCRIPTION_ALARMS; i++)
    {
        prescription_alarms[i].year = now.year;
        prescription_alarms[i].month = now.month;
        prescription_alarms[i].day = now.day;
        prescription_alarms[i].hour = now.hour;
        prescription_alarms[i].minute = i + 31;
        prescription_alarms[i].second = now.second;
    }
}

void Patient::printPrescriptionAlarms()
{
    for (int i = 0; i < NUMBER_OF_PRESCRIPTION_ALARMS; i++)
    {
        Serial.print("[RTC]  Dummy Alarm set to: ");
        Serial.print(prescription_alarms[i].year);
        Serial.print("/");
        Serial.print(prescription_alarms[i].month);
        Serial.print("/");
        Serial.print(prescription_alarms[i].day);
        Serial.print("  ");
        Serial.print(prescription_alarms[i].hour);
        Serial.print(":");
        Serial.print(prescription_alarms[i].minute);
        Serial.print(":");
        Serial.println(prescription_alarms[i].second);
    }
}

bool Patient::checkAlarms(realTimeClock now)
{
    for (int i = 0; i < NUMBER_OF_PRESCRIPTION_ALARMS; i++)
    {
        if (now.minute == prescription_alarms[i].minute)
        {
            if (now.hour == prescription_alarms[i].hour)
            {
                return true;
            }
        }
    }
    return false;
}