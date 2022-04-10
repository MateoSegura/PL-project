#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_BusIO_Register.h>
#include <SPI.h>
#include <DRV8834.h>
#include <SparkFun_RV8803.h>
#include <Wire.h>
#include "adc_adj.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "RTClib.h"
#include "libraries/Patient/patient.h"
#include "libraries/Patient/realTimeClock.h"
#include "ArduinoNvs.h"
#include "driver/rtc_io.h"
#include "driver/rtc_cntl.h"
#include "globals.h"

//*************************************************************OBJECTS*******************************************************************//
const bool serial_debugging_enabled = true;

// DEVICE
RTC_DATA_ATTR bool first_boot = true;
RTC_DATA_ATTR bool device_is_dispensing = false;

// FLASH STORAGE
ArduinoNvs myNVS;

// DUAL CORE
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t semaphore;

static TaskHandle_t Core0;
static TaskHandle_t Core1;

// INTERRUPTS
hw_timer_t *real_time_clock_timer = NULL;
hw_timer_t *battery_timer = NULL;
hw_timer_t *watch_dog_reset_timer = NULL;

// MOTORS
DRV8834 motor1(MOTOR_STEPS, DIR, STEP);
DRV8834 motor2(MOTOR_STEPS, DIR, STEP);
DRV8834 motor3(MOTOR_STEPS, DIR, STEP);

// SCREEN
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas1 canvas(128, 32); // 128x32 pixel canvas

// RTC
RTC_DATA_ATTR bool pill_locker_sleeping = false;
RV8803 rtc;
DateTime global_time;
bool update_time; // flag

// BLE
BLEServer *pServer = NULL;
BLECharacteristic *UARTCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// BUTTONS
long switch_activation_time;
bool buttons_are_free = false;

// SCREEN
struct screenFlags
{

    bool skip_screen = false; // This will prevent the warning message from showing after disconnect
    bool clear_display = false;
    bool clean_screen_once = true; // only used for warning screen
    // Screen flags
    bool display_sleep_warning_flag = false;
    bool display_steps_on_screen = false;

} screen_flags;

//********************************************************PILL LOCKER SETTINGS************************************************************//
RTC_DATA_ATTR class PillLockerSettings
{
public:
    //****************************** Patient Information ******************************//
    String patient_first_name;
    String patient_last_name;

    uint8_t activated_dispense_interval_h[NUMBER_OF_TRAYS];
    uint8_t activated_dispense_interval_m[NUMBER_OF_TRAYS];
    uint16_t demo_dispense_interval_s[NUMBER_OF_TRAYS];

    DateTime initial_prescription_time[NUMBER_OF_TRAYS];

    String prescription_name[NUMBER_OF_TRAYS];

    uint8_t dosages_remaining[NUMBER_OF_TRAYS];
    uint8_t dosages_missed[NUMBER_OF_TRAYS];
    uint8_t dosages_dispensed[NUMBER_OF_TRAYS];

    uint8_t device_type; // 0: Pocket model, 1: In Home Model

    //****************************** Device Information ******************************//
    uint16_t pill_slots_per_tray[NUMBER_OF_TRAYS];
    uint16_t loaded_pills_per_tray[NUMBER_OF_TRAYS];

    uint16_t motor_steps_per_slot[NUMBER_OF_TRAYS];
    uint16_t motor_speed;

    //****************************** Run Time Variables ******************************//
    uint8_t using_default_settings;
    String current_device_mode;

    // Dispensing flags
    struct dispensing_flags
    {
    public:
        bool awaiting_button_input = false;
        bool dispensing_pill = false;
        bool tray_open = false;
    } device_dispensing_flags;

    // UpdateTime
    DateTime newTime;

    // GlobalTime
    uint8_t hours, minutes, seconds;

    // -- Sleep timer
    uint16_t time_to_sleep_time_d;
    uint16_t time_to_sleep_time_a;

    // Demo mode must be done using hardware timers
    void setupDemoModeSleep()
    {
        // Set up sleep timer
        timerAlarmWrite(battery_timer, this->time_to_sleep_time_d * uS_TO_S_FACTOR, true); // microseconds (1s)
        timerRestart(battery_timer);
        timerAlarmEnable(battery_timer);

        if (serial_debugging_enabled)
        {
            Serial.println("[DEM]  Demo mode wake up timer was set");
        }

        // Set wake up timer
        esp_sleep_enable_timer_wakeup(this->demo_dispense_interval_s[1] * uS_TO_S_FACTOR);

        // Set up display
        portENTER_CRITICAL(&mux);
        // screen_flags.clear_display = true;
        screen_flags.skip_screen = true;

        portEXIT_CRITICAL(&mux);
    }

    void exitDemoMode()
    {
        portENTER_CRITICAL(&mux);
        // clear_display = true;
        screen_flags.skip_screen = true;
        portEXIT_CRITICAL(&mux);

        timerAlarmWrite(battery_timer, this->time_to_sleep_time_a * uS_TO_S_FACTOR, true); // microseconds (1s)
        timerRestart(battery_timer);
        timerAlarmEnable(battery_timer);
    }

    void setupActivatedMode()
    {
        // Set Wake Up alarms
        //  rtc.updateTime();
        //  rtc.enableAlarmInterrupt((rtc.getMinutes() + 1), rtc.getHours(), rtc.getDate(), false, 4);
        //  rtc.disableAlarmInterrupt();
    }

    void setDefaultSettings()
    {
        // Set default settings flag
        this->using_default_settings = 1; // 1 is true

        // Set device Type
        // TODO: Change device type in this line
        this->device_type = 0; // Default is Pocket Model
        this->current_device_mode = '0';

        // Motor RPMS
        this->motor_speed = DEFAULT_MOTOR_RPM;

        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            // Dispense Intervals
            this->activated_dispense_interval_h[i] = DEFAULT_ACTIVATED_MODE_DISPENSING_INTERVAL_H;
            this->activated_dispense_interval_m[i] = DEFAULT_ACTIVATED_MODE_DISPENSING_INTERVAL_M;
            this->demo_dispense_interval_s[i] = DEFAULT_DEMO_MODE_DISPENSING_INTERVAL_S;

            // Tray settings
            this->pill_slots_per_tray[i] = DEFAULT_PILL_SLOTS_PER_TRAY;
            this->loaded_pills_per_tray[i] = DEFAULT_PILLS_TO_DISPENSE;
            this->motor_steps_per_slot[i] = DEFAULT_POCKET_STEPS_PER_PILL;

            // Activated variables
            this->dosages_remaining[i] = DEFAULT_PILLS_TO_DISPENSE;
            this->dosages_missed[i] = DEFAULT_MISSED_PILLS;
            this->dosages_dispensed[i] = 0;

            this->prescription_name[i] = "Not set";
        }

        // Set Patient Information
        this->patient_first_name = "Not set";
        this->patient_last_name = "Not set";

        // Time to sleep start
        this->time_to_sleep_time_d = DEFAULT_TIME_TO_POWER_SAVE_START_D;
        this->time_to_sleep_time_a = DEFAULT_TIME_TO_POWER_SAVE_START_A;

        // Set Wake Up alarms
        //  rtc.updateTime();
        //  rtc.enableAlarmInterrupt((rtc.getMinutes() + 1), rtc.getHours(), rtc.getDate(), false, 4);
        //  rtc.disableAlarmInterrupt();
    }

    void verifyMemorySave(String variable, bool operation)
    {
        if (operation != true)
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Error storing variable" + variable);
            }
        }
    }

    void saveSettings()
    {
        /*
            NVS Library only allows key name of type String, max length 15 characters.
            The following are the key value pairs used to non volatile storage

            def_settings => using_default_settings
            dev_typ => device_type
            dev_mod => current_device_mode
            act_intvl_h_ => activated_dispense_interval_h
            act_intvl_m_ => activated_dispense_interval_m
            demo_intvl_s_ => demo_dispense_interval_s
            pills_p_tray_ => pills_per_tray
            steps_per_p_ => motor_steps_per_pill
            med_name_ => medication_name
            pills_rem_ => pills_remaining
            missed_dos_ => missed_dosage
            p_f_name => patient_first_name
            p_l_name => patient_last_name

        */

        // Set default settings flag
        verifyMemorySave("def_settings", myNVS.setInt("def_settings", this->using_default_settings));

        // Set device Type
        verifyMemorySave("dev_typ", myNVS.setInt("dev_typ", this->device_type));
        verifyMemorySave("dev_mod", myNVS.setString("dev_mod", this->current_device_mode));

        // MOTOR RPMS
        verifyMemorySave("mot_rpm", myNVS.setInt("mot_rpm", this->motor_speed));

        // Set tray dependent settings
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {

            verifyMemorySave(("act_intvl_h_" + i), myNVS.setInt(("act_intvl_h_" + i), this->activated_dispense_interval_h[i]));
            verifyMemorySave(("act_intvl_m_" + i), myNVS.setInt(("act_intvl_m_" + i), this->activated_dispense_interval_m[i]));
            verifyMemorySave(("demo_intvl_s_" + i), myNVS.setInt(("demo_intvl_s_" + i), this->demo_dispense_interval_s[i]));

            verifyMemorySave(("slots_p_tray_" + i), myNVS.setInt(("slots_p_tray_" + i), this->pill_slots_per_tray[i]));
            verifyMemorySave(("pills_p_tray_" + i), myNVS.setInt(("pills_p_tray_" + i), this->loaded_pills_per_tray[i]));
            verifyMemorySave(("steps_per_p_" + i), myNVS.setInt(("steps_per_p_" + i), this->motor_steps_per_slot[i]));

            // Activated variables
            verifyMemorySave(("pills_rem_" + i), myNVS.setInt(("pills_rem_" + i), this->dosages_remaining[i]));
            verifyMemorySave(("missed_dos_" + i), myNVS.setInt(("missed_dos_" + i), this->dosages_missed[i]));
            verifyMemorySave(("pills_dis_" + i), myNVS.setInt(("pills_dis_" + i), this->dosages_dispensed[i]));

            verifyMemorySave(("med_name_" + i), myNVS.setString(("med_name_" + i), this->prescription_name[i]));
        }

        // Set Patient Information
        verifyMemorySave("p_f_name", myNVS.setString("p_f_name", this->patient_first_name));
        verifyMemorySave("p_l_name", myNVS.setString("p_l_name", this->patient_first_name));

        // Time to sleep start
        verifyMemorySave("t_to_sleep_d", myNVS.setInt("t_to_sleep_d", this->time_to_sleep_time_d));
        verifyMemorySave("t_to_sleep_a", myNVS.setInt("t_to_sleep_a", this->time_to_sleep_time_a));
    }

    void restoreSettings()
    {
        // Get default settings flag
        this->using_default_settings = myNVS.getInt("def_settings");

        // Set Device type
        this->device_type = myNVS.getInt("dev_typ");
        this->current_device_mode = myNVS.getString("dev_mod");

        // MOTOR RPMS
        this->motor_speed = myNVS.getInt("mot_rpm");

        // Get tray dependent settings
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            this->activated_dispense_interval_h[i] = myNVS.getInt(("act_intvl_h_" + i));
            this->activated_dispense_interval_m[i] = myNVS.getInt(("act_intvl_m_" + i));
            this->demo_dispense_interval_s[i] = myNVS.getInt(("demo_intvl_s_" + i));

            this->pill_slots_per_tray[i] = myNVS.getInt(("slots_p_tray_" + i));
            this->loaded_pills_per_tray[i] = myNVS.getInt(("pills_p_tray_" + i));
            this->motor_steps_per_slot[i] = myNVS.getInt(("steps_per_p_" + i));

            // Activated variables
            this->dosages_remaining[i] = myNVS.getInt(("pills_rem_" + i));
            this->dosages_missed[i] = myNVS.getInt(("missed_dos_" + i));
            this->dosages_dispensed[i] = myNVS.getInt(("pills_dis_" + i));

            this->prescription_name[i] = myNVS.getString(("med_name_" + i));
        }

        // Get Patient Information
        this->patient_first_name = myNVS.getString("p_f_name");
        this->patient_last_name = myNVS.getString("p_l_name");

        // Time to sleep start
        this->time_to_sleep_time_d = myNVS.getInt("t_to_sleep_d");
        this->time_to_sleep_time_a = myNVS.getInt("t_to_sleep_a");
    }

    // Logger
    void logPillDispensed()
    {
        String log_message, log_name;

        log_message += this->hours;
        log_message += ":";
        log_message += this->minutes;
        log_message += ":";
        log_message += this->seconds;

        log_name += "log_";
        log_name += log_message;

        verifyMemorySave(log_name, myNVS.setString(log_name, log_name));
        Serial.print("[SET]\tLogging Pill Dispensed: ");
        Serial.print(log_name);
        Serial.print("\t");
        Serial.println(log_message);
    }

    void logPillMissed()
    {
        String log_message, log_name;

        log_message += this->hours;
        log_message += ":";
        log_message += this->minutes;
        log_message += ":";
        log_message += this->seconds;

        log_name += "m_log_";
        log_name += this->dosages_missed[0];

        verifyMemorySave(log_name, myNVS.setString(log_name, log_message));
        Serial.print("[SET]\tLogging Pill Missed: ");
        Serial.print(log_name);
        Serial.print("\t");
        Serial.println(log_message);
    }

    String retrieveLog(uint8_t pill_number)
    {
        String temp1, temp2;
        temp1 += "m_log_";
        temp1 += pill_number;
        temp2 += myNVS.getString(temp1);

        temp1 += " - ";
        temp1 += temp2;

        Serial.println(temp1);

        return temp1;
    }

} this_device_settings;