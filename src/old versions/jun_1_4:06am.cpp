// /*
// * Company
// * Name of Project :
// * Version:
// * Compartible Hardware:
// * Date Created:
// * Last Modified:
// */
// //************************************************************LIBRARIES******************************************************************//
// #include <Arduino.h>
// #include <Adafruit_GFX.h>
// #include <Fonts/FreeMonoBoldOblique12pt7b.h>
// #include <Adafruit_ST7789.h>
// #include <Adafruit_BusIO_Register.h>
// #include <SPI.h>
// #include <DRV8834.h>
// #include <RV-3028-C7.h>
// #include <Wire.h>
// #include "adc_adj.h"
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>
// #include "RTClib.h"
// #include "realTimeClock.h"
// #include "ArduinoNvs.h"
// #include "patient.h"
// #include "driver/rtc_io.h"
// #include "driver/rtc_cntl.h"
// #include "esp32/ulp.h"

// //*********************************************************DEFAULT SETTINGS**************************************************************//
// /*
// * - The settings below are assigned when the unit is first programmed.
// * - If the unit is "factory reset", these settings will be saved again.
// */

// #define DEFAULT_DEMO_MODE_DISPENSING_INTERVAL_S 40
// #define DEFAULT_ACTIVATED_MODE_DISPENSING_INTERVAL_S 240 // = 4 H , Set in seconds
// #define DEFAULT_STEPS_PER_PILL 250
// #define DEFAULT_PILLS_PER_TRAY 12
// #define DEFAULT_PILLS_TO_DISPENSE 1
// #define DEFAULT_MISSED_PILLS 0
// #define DEFAULT_ACTIVATION_H 7 //24 hour calender. Default is 7
// #define NUMBER_OF_TRAYS 3      //Max. is 3

// #define OUT_DIRECTION -1
// #define IN_DIRECTION 1

// //*********************************************************OTHER SETTINGS***************************************************************//

// // The unit will automatically go to sleep after this time period, if there's no activity (button press), and BLE is not connected
// #define uS_TO_S_FACTOR 1000000
// #define TIME_TO_POWER_SAVE_START_S 30

// //*****************************************************USER DEFINED VARIABLES***********************************************************//
// String FIRMWARE_VERSION = "V3.1";
// const bool serial_debugging_enabled = true;
// RTC_DATA_ATTR bool first_boot = true;

// //*****************************************************BLUETOOTH INSTRUCTIONS***********************************************************//
// /*
// * - The booleans below act as flags. Once assigned, CPU cores will perform instruction accordingly
// * - If the unit is "factory reset", these settings will be saved again.
// */

// bool print_time = false;
// bool update_time_at_ble_connect = false;
// bool skip_screen = false; //This will prevent the warning message from showing after disconnect
// bool clear_display = false;
// bool clean_screen_once = true; //only used for warning screen

// //Screen flags
// bool display_sleep_warning_flag = false;

// //************************************************************DEFINTIONS*****************************************************************//
// //PILL LOCKER SETTINGS

// //FLASH STORAGE
// //MOTORS
// #define MOTOR_STEPS 80
// #define MICROSTEPS 1
// #define NORMAL_RPM 1000
// #define LOAD_RPM 1000
// #define STEP 4
// #define DIR 32
// #define MOTORSLEEP1 25
// #define MOTORSLEEP2 26
// #define MOTORSLEEP3 27

// //SCREEN
// #define TFT_CS 12
// #define TFT_RST 19
// #define TFT_MOSI 23
// #define TFT_SCLK 18
// #define TFT_DC 14
// #define LEDK 13

// //BUTTONS
// #define POW_BUTTON 34
// #define UP_BUTTON 35
// #define DOWN_BUTTON 36
// #define OK_BUTTON 39

// //BUZZER
// #define AUDIO 15

// //POWER MANAGEMENT
// #define VBAT_3V3 33
// #define REG_STAT 5
// #define BAT_STAT1 16
// #define BAT_STAT2 17
// #define WAKE_UP 2

// //BLE Service Characteristics
// #define BLENAME "ESP32 BLE Example"
// #define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
// #define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// //*************************************************************OBJECTS*******************************************************************//
// //FLASH STORAGE
// ArduinoNvs myNVS;

// //DUAL CORE
// static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
// SemaphoreHandle_t semaphore;

// static TaskHandle_t Core0;
// static TaskHandle_t Core1;

// //INTERRUPTS
// hw_timer_t *real_time_clock_timer = NULL;
// hw_timer_t *battery_timer = NULL;

// //MOTORS
// DRV8834 motor1(MOTOR_STEPS, DIR, STEP);
// DRV8834 motor2(MOTOR_STEPS, DIR, STEP);
// DRV8834 motor3(MOTOR_STEPS, DIR, STEP);

// //SCREEN
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// GFXcanvas1 canvas(128, 32); // 128x32 pixel canvas

// //RTC
// RTC_DATA_ATTR bool pill_locker_sleeping = false;
// RV3028 rtc;
// DateTime global_time;
// bool update_time; //flag

// //BLE
// BLEServer *pServer = NULL;
// BLECharacteristic *pTxCharacteristic;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;

// //********************************************************PILL LOCKER SETTINGS************************************************************//
// RTC_DATA_ATTR class PillLockerSettings
// {
// public:
//     uint8_t using_default_settings; //0: false, 1: true
//     //Device Constrains
//     uint8_t device_type; // 0: Pocket model, 1: In Home Model
//     String current_device_mode;
//     uint16_t activated_dispense_interval_s[NUMBER_OF_TRAYS];
//     uint16_t demo_dispense_interval_s[NUMBER_OF_TRAYS];
//     uint8_t pills_per_tray[NUMBER_OF_TRAYS];
//     uint8_t motor_steps_per_pill[NUMBER_OF_TRAYS];
//     realTimeClock real_time_clock;

//     //Patient Information
//     String patient_first_name;
//     String patient_last_name;
//     String medication_name[NUMBER_OF_TRAYS];

//     //Information to keep when activated
//     uint8_t pills_remaining[NUMBER_OF_TRAYS];
//     uint8_t missed_dosages[NUMBER_OF_TRAYS];

//     void setDefaultSettings()
//     {
//         //Set default settings flag
//         this->using_default_settings = 1; //1 is true

//         //Set device Type
//         this->device_type = 0; //Default is Pocket Model
//         this->current_device_mode = '0';

//         //Set tray dependent settings
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             this->activated_dispense_interval_s[i] = DEFAULT_ACTIVATED_MODE_DISPENSING_INTERVAL_S;
//             this->demo_dispense_interval_s[i] = DEFAULT_DEMO_MODE_DISPENSING_INTERVAL_S;
//             this->pills_per_tray[i] = DEFAULT_PILLS_PER_TRAY;
//             this->motor_steps_per_pill[i] = DEFAULT_STEPS_PER_PILL;
//             this->medication_name[i] = "Not set";

//             //Activated variables
//             this->pills_remaining[i] = DEFAULT_PILLS_TO_DISPENSE;
//             this->missed_dosages[i] = DEFAULT_MISSED_PILLS;
//         }

//         //Set Patient Information
//         this->patient_first_name = "Not set";
//         this->patient_last_name = "Not set";

//         //Set Wake Up alarms
//         // rtc.updateTime();
//         // rtc.enableAlarmInterrupt((rtc.getMinutes() + 1), rtc.getHours(), rtc.getDate(), false, 4);
//         // rtc.disableAlarmInterrupt();
//     }

//     void verifyMemorySave(String variable, bool operation)
//     {
//         if (operation != true)
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[MEM]  Error storing variable" + variable);
//             }
//         }
//     }

//     void saveSettings()
//     {
//         /*
//             NVS Library only allows key name of type String, max length 15 characters.
//             The following are the key value pairs used to non volatile storage

//             def_settings => using_default_settings
//             dev_typ => device_type
//             dev_mod => current_device_mode
//             act_intvl_s_ => activated_dispense_interval_s
//             demo_intvl_s_ => demo_dispense_interval_s
//             pills_p_tray_ => pills_per_tray
//             steps_per_p_ => motor_steps_per_pill
//             med_name_ => medication_name
//             pills_rem_ => pills_remaining
//             missed_dos_ => missed_dosage
//             p_f_name => patient_first_name
//             p_l_name => patient_last_name

//         */

//         //Set default settings flag
//         verifyMemorySave("def_settings", myNVS.setInt("def_settings", this->using_default_settings));

//         //Set device Type
//         verifyMemorySave("dev_typ", myNVS.setInt("dev_typ", this->device_type));
//         verifyMemorySave("dev_mod", myNVS.setString("dev_mod", this->current_device_mode));

//         //Set tray dependent settings
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             verifyMemorySave(("act_intvl_s_" + i), myNVS.setInt(("act_intvl_s_" + i), this->activated_dispense_interval_s[i]));
//             verifyMemorySave(("demo_intvl_s_" + i), myNVS.setInt(("demo_intvl_s_" + i), this->demo_dispense_interval_s[i]));

//             verifyMemorySave(("pills_p_tray_" + i), myNVS.setInt(("pills_p_tray_" + i), this->pills_per_tray[i]));
//             verifyMemorySave(("steps_per_p_" + i), myNVS.setInt(("steps_per_p_" + i), this->motor_steps_per_pill[i]));
//             verifyMemorySave(("med_name_" + i), myNVS.setString(("med_name_" + i), this->medication_name[i]));

//             //Activated variables
//             verifyMemorySave(("pills_rem_" + i), myNVS.setInt(("pills_rem_" + i), this->pills_remaining[i]));
//             verifyMemorySave(("missed_dos_" + i), myNVS.setInt(("missed_dos_" + i), this->missed_dosages[i]));
//         }

//         //Set Patient Information
//         verifyMemorySave("p_f_name", myNVS.setString("p_f_name", this->patient_first_name));
//         verifyMemorySave("p_l_name", myNVS.setString("p_l_name", this->patient_first_name));
//     }

//     void restoreSettings()
//     {
//         //Get default settings flag
//         this->using_default_settings = myNVS.getInt("def_settings");

//         //Set Device type
//         this->device_type = myNVS.getInt("dev_typ");
//         this->current_device_mode = myNVS.getString("dev_mod");

//         //Get tray dependent settings
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             this->activated_dispense_interval_s[i] = myNVS.getInt(("act_intvl_s_" + i));
//             this->demo_dispense_interval_s[i] = myNVS.getInt(("demo_intvl_s_" + i));

//             this->pills_per_tray[i] = myNVS.getInt(("pills_p_tray_" + i));
//             this->motor_steps_per_pill[i] = myNVS.getInt(("steps_per_p_" + i));
//             this->medication_name[i] = myNVS.getString(("med_name_" + i));

//             //Activated variables
//             this->pills_remaining[i] = myNVS.getInt(("pills_rem_" + i));
//             this->missed_dosages[i] = myNVS.getInt(("missed_dos_" + i));
//         }

//         //Get Patient Information
//         this->patient_first_name = myNVS.getString("p_f_name");
//         this->patient_last_name = myNVS.getString("p_l_name");
//     }
// } this_device_settings;

// //*******************************************************FUNCTION DECLARATION************************************************************//
// //DEVICE MODES LOGIC
// void ActivatedMode();
// void TestMode();
// void DemoMode();
// void UtilitiesMode();
// void InactiveMode();

// //SERIAL

// // FLASH STORAGE
// void initStorage();

// //INSTRUCTION FUNCTIONS
// void utilitiesModeHandler(std::string instruction);
// void activatedModeHandler();
// void testModeHandler(std::string instruction);
// void demoModeHandler();

// void unloadInstruction();
// void loadInstruction();
// void dispenseInstruction();

// //INITIAL SETUP
// void setPinsForDeepSleep();

// //BATTERY
// void initBattery();

// //DEEP SLEEP
// void startSleepTimer();

// //DUAL CORE
// void core0Task(void *pvParameters);
// void core1Task(void *pvParameters);

// //MOTORS
// void initMotors();
// void moveMotor(int motor, int direction, int steps);

// //RTC
// void initRTC();
// void printTime();
// void IRAM_ATTR updateTimeSeconds();
// void IRAM_ATTR updateTimeSleep();
// void alarmController();
// void adjustTime(std::string data);

// //BUZZER
// void initBuzzer();
// void soundBuzzer();

// //BUTTONS
// void initButtons();

// //SCREEN
// void initScreen();
// void updateScreen();
// void mainMenuScreenManager();
// void activeModeScreenManager();
// void testModeScreenManager();
// void utilitiesModeScreenManager();
// void demoModeScreenManager();
// void sleepWarningScreenManager();

// //BLE
// struct BluetoothInstruction
// {
//     //Restart device
//     bool restart_device = false;

//     //Device Modes
//     bool active_mode_request = false;
//     bool test_mode_request = false;
//     bool utilities_mode_request = false;
//     bool demo_mode_request = false;

//     //Exit any device mode
//     bool exit_mode_request = false;

//     //Device Display Info request
//     bool send_status_message = false;
//     bool send_current_settings = false;

//     //Pass possible complex instruction to CPU core for processing
//     bool complex_instruction_request = false;
//     std::string complex_instruction_string;

// } myBluetoothInstructions;

// void initBLE();
// void IRAM_ATTR adjustTimeRequest();
// void connectionManager();
// void sendBLEMessage(String data);
// void assignBluetoothFlag(std::string data);
// void bluetothInstructionHandlerCore0();
// void bluetothInstructionHandlerCore1();

// class MyServerCallbacks : public BLEServerCallbacks
// {
//     void onConnect(BLEServer *pServer)
//     {
//         deviceConnected = true;
//     };

//     void onDisconnect(BLEServer *pServer)
//     {
//         //This has to be here in order for the screen manager to not display the warnign message on disconnect. FIX!!!!!!!!
//         skip_screen = true;
//         timerRestart(battery_timer);
//         deviceConnected = false;
//     }
// };

// class MyCallbacks : public BLECharacteristicCallbacks
// {
//     void onWrite(BLECharacteristic *pCharacteristic)
//     {
//         std::string rxValue = pCharacteristic->getValue();
//         assignBluetoothFlag(rxValue);
//     }
// };

// //***********************************************************INTERRUPTS******************************************************************//

// //RTC
// void IRAM_ATTR globalTimeKeeperInterrupt()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     update_time = true;
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //POWER

// void IRAM_ATTR batterySaverStart()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     if (!deviceConnected)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[POW]  Device going to sleep to reduce power consumption");
//         }
//         pill_locker_sleeping = true;
//         setPinsForDeepSleep();
//         esp_deep_sleep_start();
//     }
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //BUTTONS
// void IRAM_ATTR buttonInterrupt()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     timerRestart(battery_timer);
//     Serial.println("[POW]  Battery saver timer reset");
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //Does not work
// void IRAM_ATTR batteryDisconnected()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     Serial.println("[POW]  rtc int detected");
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //**************************************************************SETUP********************************************************************//
// void setup()
// {
//     long initial_time = micros();

//     //SERIAL
//     if (serial_debugging_enabled)
//     {
//         Serial.begin(115200);
//     }

//     //Wait for Serial
//     if (first_boot)
//     {
//         if (serial_debugging_enabled)
//         {
//             while (!Serial)
//                 ;
//             delay(2000);
//         }
//     }

//     //FLASH STORAGE
//     initStorage();

//     //RTC
//     initRTC();

//     //ALARMS
//     alarmController();

//     //BATTERY
//     initBattery();

//     //MOTORS
//     initMotors();

//     //BUZZER
//     initBuzzer();

//     // -- BUTTONS
//     initButtons();

//     //  --  SOFTWARE INTERRUPTS:
//     //RTC
//     real_time_clock_timer = timerBegin(0, 80, true);
//     timerAttachInterrupt(real_time_clock_timer, &globalTimeKeeperInterrupt, true);
//     timerAlarmWrite(real_time_clock_timer, 1000000, true); //microseconds (1s)

//     //BATTERY
//     battery_timer = timerBegin(1, 80, true);
//     timerAttachInterrupt(battery_timer, &batterySaverStart, true);
//     timerAlarmWrite(battery_timer, TIME_TO_POWER_SAVE_START_S * uS_TO_S_FACTOR, true); //microseconds (1s)

//     //  --   HARDWARE INTERRUPTS:
//     //POWER
//     // pinMode(2, INPUT_PULLUP);
//     // attachInterrupt(digitalPinToInterrupt(2), batteryDisconnected, FALLING);

//     //SCREEN
//     initScreen();

//     // -- BLE
//     initBLE();

//     // -- DUAL CORE SETUP
//     xTaskCreatePinnedToCore(
//         core0Task, /* Task function. */
//         "Task0",   /* name of task. */
//         10000,     /* Stack size of task in Memory */
//         NULL,      /* parameter of the task */
//         1,         /* priority of the task: 0 to 25, 25 being the highest*/
//         &Core0,    /* Task handle to keep track of created task */
//         0);        /* pin task to core 0 */

//     xTaskCreatePinnedToCore(
//         core1Task, /* Task function. */
//         "Task1",   /* name of task. */
//         10000,     /* Stack size of task */
//         NULL,      /* parameter of the task */
//         1,         /* priority of the task */
//         &Core1,    /* Task handle to keep track of created task */
//         1);        /* pin task to core 1 */

//     // -- Enable Timer Interrupts
//     timerAlarmEnable(real_time_clock_timer);
//     timerAlarmEnable(battery_timer);

//     Serial.print("[DEB]  Boot time: ");
//     Serial.print((micros() - initial_time) / 1000);
//     Serial.println(" ms");
// }

// //**************************************************************CORE 0*******************************************************************//
// void core1Task(void *pvParameters)
// {
//     for (;;)
//     {
//         // -- BLE Connection Manager
//         connectionManager();

//         // -- Update TFT
//         updateScreen();

//         // -- Handle Simple Bluetooth Instructions
//         bluetothInstructionHandlerCore0();

//         // --  Print Time
//         if (update_time == true)
//         {

//             if (rtc.updateTime() == false) //Updates the time variables from RTC
//             {
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[RTC]  Could not update time");
//                 }
//             }
//             else
//             {
//                 String currentTime = rtc.stringTimeStamp();
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.print("[RTC]  ");
//                     Serial.println(currentTime);
//                 }
//             }
//             //Unset flag
//             update_time = false;
//         }

//         //Read Alarm Flag
//         // if (rtc.readAlarmInterruptFlag())
//         // {
//         //     Serial.println("ALARM!!!!");
//         //     rtc.clearAlarmInterruptFlag();
//         // }

//         // String var = myNVS.getString("dev_mod");
//         // Serial.println(var);

//         // for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         // {
//         //     uint16_t x = myNVS.getInt("act_intvl_s_" + i);
//         //     Serial.println(x);
//         // }

//         // Force deep sleep
//         // pill_locker_sleeping = true;
//         // esp_deep_sleep_start();
//         yield();
//     }
// }

// //**************************************************************CORE 1*******************************************************************//
// void core0Task(void *pvParameters)
// {
//     for (;;)
//     {
//         //Get device mode
//         portENTER_CRITICAL(&mux);
//         char mode = this_device_settings.current_device_mode[0];
//         portEXIT_CRITICAL(&mux);

//         // -- Handle Complex Bluetooth Instructions
//         bluetothInstructionHandlerCore1();
//     }
// }

// //***************************************************************LOOP********************************************************************//
// void loop()
// {
//     yield();
// }

// //*******************************************************FUNCTION DEFINITION************************************************************//
// //DEVICE MODES LOGIC
// void ActivatedMode() {}

// void TestMode()
// {
// }

// void DemoMode() {}
// void UtilitiesMode() {}
// void InactiveMode() {}

// //FLASH STORAGE
// void initStorage()
// {
//     myNVS.begin();

//     //Check if it's first time boot after being programmed
//     if (first_boot == true)
//     {
//         myNVS.eraseAll();
//         if (myNVS.setInt("boot", 1) != true)
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[MEM]  Error storing variable");
//             }
//         }
//     }

//     // -- Program default settings on start up if not programmed
//     uint64_t unit_programmed = myNVS.getInt("unit_p");
//     if (unit_programmed == 0)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.println("PILL LOCKER REV 3.1");
//             Serial.println("Booting...");
//             Serial.println();
//             Serial.println("[RTC]  Flash Memory Erased");
//             if (myNVS.setInt("unit_p", 1) != true)
//             {
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[MEM]  Error storing variable o");
//                 }
//             };
//             this_device_settings.setDefaultSettings();
//             this_device_settings.saveSettings();
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[MEM]  Default settings saved to memory");
//             }
//         }
//     }

//     // -- Restore settings from memory if device is programmed
//     if (unit_programmed == 1)
//     {
//         this_device_settings.restoreSettings();
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[MEM]  Device settings have been restored");
//         }
//         if (this_device_settings.using_default_settings == 1) //true
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[MEM]  Device using default settings");
//             }
//         }
//         else if (this_device_settings.using_default_settings == 0) //false
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[MEM]  Device using custom settings");
//             }
//         }
//         else //should never get here
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[MEM]  Device settings seem corrupted");
//             }
//         }
//     }
// }

// //INSTRUCTIONS
// void utilitiesModeHandler(std::string instruction)
// {
//     //No instruction received
//     int instruction_length = instruction.length();
//     if (instruction_length > 0)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.print("[UTI]  Attempting to decode complex instruction: ");
//             Serial.println(instruction.c_str());
//         }

//         //flag for valid instruction
//         bool valid_instruction = true;

//         switch (instruction[0])
//         {
//         //Change demo interval
//         case 'D':
//             if (instruction[1] == 'I' && instruction_length < 6)
//             {
//                 std::string interval_string;

//                 //Check if instruction contains only numbers.
//                 for (int i = 2; i < instruction_length; i++)
//                 {
//                     if (!isdigit(instruction[i]))
//                     {
//                         valid_instruction = false;
//                         break;
//                     }
//                     else
//                     {
//                         interval_string += instruction[i];
//                     }
//                 }

//                 //Decode instruction
//                 if (valid_instruction)
//                 {
//                     //Convert characters to integer
//                     uint16_t temp = atoi(interval_string.c_str());

//                     //Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
//                     portENTER_CRITICAL(&mux);
//                     for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                     {
//                         this_device_settings.demo_dispense_interval_s[i] = temp;
//                     }
//                     portEXIT_CRITICAL(&mux);

//                     if (serial_debugging_enabled)
//                     {
//                         Serial.println("[UTI]  Valid Instrution");
//                         Serial.print("[UTI]  Changing demo interval to: ");
//                         Serial.print(temp);
//                         Serial.println(" s");
//                     }

//                     //Send BLE message to indicate change
//                     String ble_message = "Demo Interval Changed to: ";
//                     ble_message += temp;
//                     ble_message += " s";
//                     sendBLEMessage(ble_message);
//                 }
//             }
//             break;

//         //Set the number of pills per tray
//         case 'P':
//             if (instruction[1] == 'T' && instruction_length < 6)
//             {
//                 std::string interval_string;

//                 //Check if instruction contains only numbers.
//                 for (int i = 2; i < instruction_length; i++)
//                 {
//                     if (!isdigit(instruction[i]))
//                     {
//                         valid_instruction = false;
//                         break;
//                     }
//                     else
//                     {
//                         interval_string += instruction[i];
//                     }
//                 }

//                 //Decode instruction
//                 if (valid_instruction)
//                 {
//                     //Convert characters to integer
//                     uint16_t temp = atoi(interval_string.c_str());

//                     //Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
//                     portENTER_CRITICAL(&mux);
//                     for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                     {
//                         this_device_settings.pills_per_tray[i] = temp;
//                     }
//                     portEXIT_CRITICAL(&mux);

//                     if (serial_debugging_enabled)
//                     {
//                         Serial.println("[UTI]  Valid Instrution");
//                         Serial.print("[UTI]  Changing pills per tray to: ");
//                         Serial.print(temp);
//                         Serial.println(" pills");
//                     }

//                     //Send BLE message to indicate change
//                     String ble_message = "Pills per tray changed to: ";
//                     ble_message += temp;
//                     ble_message += " pills";
//                     sendBLEMessage(ble_message);
//                 }
//             }
//             break;

//         //Set the steps per motor
//         case 'M':
//             if (instruction[1] == 'S' && instruction_length < 6)
//             {
//                 std::string interval_string;

//                 //Check if instruction contains only numbers.
//                 for (int i = 2; i < instruction_length; i++)
//                 {
//                     if (!isdigit(instruction[i]))
//                     {
//                         valid_instruction = false;
//                         break;
//                     }
//                     else
//                     {
//                         interval_string += instruction[i];
//                     }
//                 }

//                 //Decode instruction
//                 if (valid_instruction)
//                 {
//                     //Convert characters to integer
//                     uint16_t temp = atoi(interval_string.c_str());

//                     //Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
//                     portENTER_CRITICAL(&mux);
//                     for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                     {
//                         this_device_settings.motor_steps_per_pill[i] = temp;
//                     }
//                     portEXIT_CRITICAL(&mux);

//                     if (serial_debugging_enabled)
//                     {
//                         Serial.println("[UTI]  Valid Instrution");
//                         Serial.print("[UTI]  Changing steps per pill to: ");
//                         Serial.print(temp);
//                         Serial.println(" steps");
//                     }

//                     //Send BLE message to indicate change
//                     String ble_message = "Steps per pill to: ";
//                     ble_message += temp;
//                     ble_message += " steps";
//                     sendBLEMessage(ble_message);
//                 }
//             }
//             break;

//         default:
//             break;
//         }

//         //Send message if instruction was invalid
//         if (!valid_instruction)
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[UTI]  Invalid Instruction");
//                 sendBLEMessage("Invalid Instruction");
//             }
//         }

//         //Unset flag
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.complex_instruction_request = false;
//         portEXIT_CRITICAL(&mux);
//     }
//     else
//     {
//         delay(10); //Prevent core from panicking
//     }
// }

// void activatedModeHandler()
// {
// }

// void testModeHandler(std::string instruction)
// {
// }

// void demoModeHandler()
// {
// }

// void unloadInstruction()
// {

//     Serial.println("Excecuting unload instruction");
//     portENTER_CRITICAL(&mux);

//     tft.setCursor(20, 20); //width, height
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(2);

//     tft.print("Unloading Device");
//     portEXIT_CRITICAL(&mux);
//     delay(2000);
//     tft.setCursor(20, 20);
//     tft.setTextColor(ST77XX_BLACK);
//     tft.print("Unloading Device");

//     digitalWrite(MOTORSLEEP1, LOW);
//     digitalWrite(MOTORSLEEP2, LOW);
//     digitalWrite(MOTORSLEEP3, LOW);
//     //moveMotor(2, OUT_DIRECTION, TOTAL_STEPS);
//     //DISABLE ALL MOTORS
//     digitalWrite(MOTORSLEEP1, HIGH);
//     digitalWrite(MOTORSLEEP2, HIGH);
//     digitalWrite(MOTORSLEEP3, HIGH);
//     //unload_instruction = false;
// }

// void loadInstruction()
// {
//     //ENABLE ALL MOTORS
//     Serial.println("Excecuting load instruction");
//     tft.setCursor(20, 20); //width, height
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(2);

//     tft.print("Loading Device");
//     delay(2000);
//     tft.setCursor(20, 20);
//     tft.setTextColor(ST77XX_BLACK);
//     tft.print("Loading Device");

//     digitalWrite(MOTORSLEEP1, LOW);
//     digitalWrite(MOTORSLEEP2, LOW);
//     digitalWrite(MOTORSLEEP3, LOW);
//     //moveMotor(2, IN_DIRECTION, TOTAL_STEPS);
//     //DISABLE ALL MOTORS
//     digitalWrite(MOTORSLEEP1, HIGH);
//     digitalWrite(MOTORSLEEP2, HIGH);
//     digitalWrite(MOTORSLEEP3, HIGH);
//     //load_instruction = false;

//     // //This was on loop
//     // for (int i = 0; i < 3; i++)
//     // {
//     //     moveMotor(1, OUT_DIRECTION, 2000);
//     //     moveMotor(1, IN_DIRECTION, 2000);
//     //     delay(1000);
//     //     moveMotor(2, OUT_DIRECTION, 2000);
//     //     moveMotor(2, IN_DIRECTION, 2000);
//     //     delay(10);
//     //     moveMotor(3, OUT_DIRECTION, 2000);
//     //     moveMotor(3, IN_DIRECTION, 2000);
//     //     delay(10);
//     // }
// }

// void dispenseInstruction()
// {
//     Serial.println("[MOT]  Excecuting dispense instruction");
//     tft.setCursor(20, 20); //width, height
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(2);

//     tft.println("Dispensing");
//     moveMotor(1, 1, 1000);
//     tft.setCursor(20, 20);
//     tft.setTextColor(ST77XX_BLACK);
//     tft.println("Dispensing");
// }

// //BLE
// void initBLE()
// {
//     //BLE
//     BLEDevice::init(BLENAME);
//     pServer = BLEDevice::createServer();
//     pServer->setCallbacks(new MyServerCallbacks());
//     BLEService *pService = pServer->createService(SERVICE_UUID);

//     pTxCharacteristic = pService->createCharacteristic(
//         CHARACTERISTIC_UUID,
//         BLECharacteristic::PROPERTY_READ |
//             BLECharacteristic::PROPERTY_WRITE |
//             BLECharacteristic::PROPERTY_NOTIFY |
//             BLECharacteristic::PROPERTY_INDICATE);

//     pTxCharacteristic->addDescriptor(new BLE2902());
//     pTxCharacteristic->setCallbacks(new MyCallbacks());

//     pService->start();
//     pServer->getAdvertising()->start();
//     if (serial_debugging_enabled)
//     {
//         Serial.println("[BLE]  Device is advertising & ready to connect");
//     }
// }

// void sendBLEMessage(String data)
// {
//     portENTER_CRITICAL(&mux);
//     String ble_data;
//     ble_data += data;

//     //Send Data
//     pTxCharacteristic->setValue((char *)ble_data.c_str());
//     pTxCharacteristic->notify();
//     yield(); //AS
//     portEXIT_CRITICAL(&mux);
// }

// void IRAM_ATTR assignBluetoothFlag(std::string data)
// {
//     if (data.length() > 0)
//     {
//         portENTER_CRITICAL_ISR(&mux);
//         //Print raw data
//         int len = data.length();
//         if (serial_debugging_enabled)
//         {
//             // Serial.print("[BLE]  Receive Interrupt Exceuting in Core: ");
//             // Serial.println(xPortGetCoreID());
//             Serial.print("[BLE]  Data received: ");

//             for (int i = 0; i < len; i++)
//             {
//                 Serial.print(data[i]);
//             }
//             Serial.println();
//         }

//         // --  Separate main 4 instructions
//         if (len == 1) //Only main insctructions should be single character
//         {
//             String status_message;
//             switch (data[0])
//             {
//             //RESET
//             case 'R':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.restart_device = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;
//             //Activate Device
//             case 'A':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.active_mode_request = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;

//             //Test
//             case 'T':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.demo_mode_request = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;

//             //Utilities
//             case 'U':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.utilities_mode_request = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;

//             //Demo
//             case 'D':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.demo_mode_request = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;

//             //Exit Demo mode
//             case '#':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.exit_mode_request = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 // if (this_device_settings.current_device_mode[0] == 'D')
//                 // {
//                 //     if (serial_debugging_enabled)
//                 //     {
//                 //         Serial.println("[BLE]  Exiting Demo Mode ");
//                 //     }
//                 //     sendBLEMessage("Exiting demo mode");
//                 // }
//                 break;

//             //Send out status message
//             case '$':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.send_status_message = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;

//             // Send out current settings
//             case '?':
//                 portENTER_CRITICAL_ISR(&mux);
//                 myBluetoothInstructions.send_current_settings = true;
//                 portEXIT_CRITICAL_ISR(&mux);
//                 break;
//             default:
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[BLE]  Invalid Instruction");
//                 }
//                 sendBLEMessage("Invalid Instruction");
//                 break;
//             }
//         }

//         // -- Utilities Instructions Flags
//         else
//         {
//             portENTER_CRITICAL(&mux);
//             myBluetoothInstructions.complex_instruction_string = data;
//             myBluetoothInstructions.complex_instruction_request = true;
//             char mode = this_device_settings.current_device_mode[0];
//             portEXIT_CRITICAL(&mux);
//         }
//         portEXIT_CRITICAL_ISR(&mux);
//     }
// }

// void bluetothInstructionHandlerCore0()
// {
//     // --  Restart Device 'R'
//     if (myBluetoothInstructions.restart_device)
//     {
//         ESP.restart();
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.restart_device = false;
//         portEXIT_CRITICAL(&mux);
//     }

//     // -- Activate Device 'A'
//     if (myBluetoothInstructions.active_mode_request)
//     {
//         if (serial_debugging_enabled)
//         {

//             Serial.println("[BLE]  Device Mode: Activate Mode ");
//         }
//         this_device_settings.current_device_mode = 'A';
//         sendBLEMessage("Pill Locker is now in Activated Mode");

//         //Clear display
//         clear_display = true;

//         //Unset flag
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.active_mode_request = false;
//         portEXIT_CRITICAL(&mux);
//     }

//     // -- Test Mode set 'T'
//     if (myBluetoothInstructions.test_mode_request)
//     {
//         if (serial_debugging_enabled)
//         {

//             Serial.println("[BLE]  Device Mode: Test Mode ");
//         }
//         this_device_settings.current_device_mode = 'T';
//         sendBLEMessage("Pill Locker is now in Test Mode");

//         //Clear display
//         clear_display = true;

//         //Unset flag
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.test_mode_request = false;
//         portEXIT_CRITICAL(&mux);
//     }

//     // -- Utilities Mode set 'U'
//     if (myBluetoothInstructions.utilities_mode_request)
//     {
//         if (serial_debugging_enabled)
//         {

//             Serial.println("[BLE]  Device Mode: Utilities Mode ");
//         }
//         this_device_settings.current_device_mode = 'U';
//         sendBLEMessage("Pill Locker is now in Utilities Mode");

//         //Clear display
//         clear_display = true;

//         //Unset flag
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.utilities_mode_request = false;
//         portEXIT_CRITICAL(&mux);
//     }

//     // -- Demo Mode set 'D'
//     if (myBluetoothInstructions.demo_mode_request)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[BLE]  Device Mode: Demo Mode ");
//         }
//         this_device_settings.current_device_mode = 'D';
//         sendBLEMessage("Pill Locker is now in Demo Mode");

//         //Clear display
//         clear_display = true;

//         //Unset flag
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.demo_mode_request = false;
//         portEXIT_CRITICAL(&mux);
//     }

//     // -- Exit current device mode '#'
//     if (myBluetoothInstructions.exit_mode_request)
//     {
//         portENTER_CRITICAL(&mux);
//         char mode = this_device_settings.current_device_mode[0];
//         portEXIT_CRITICAL(&mux);

//         switch (mode)
//         {
//         //Active mode
//         case 'A':
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[DEV]  Exiting Active mode");
//             }
//             sendBLEMessage("Pill Locker exiting Active mode");
//             break;
//         //Test mode
//         case 'T':
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[DEV]  Exiting Test mode");
//             }
//             sendBLEMessage("Pill Locker exiting Test mode");
//             break;
//         //Demo mode
//         case 'D':
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[DEV]  Exiting Demo mode");
//             }
//             sendBLEMessage("Pill Locker exiting Demo mode");
//             break;
//         //Utilities mode
//         case 'U':
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[DEV]  Exiting Utilities Mode");
//             }
//             sendBLEMessage("Pill Locker exiting Utilities mode");
//             break;
//         default:
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[DEV]  Device is not in any mode right now");
//             }
//             sendBLEMessage("Device is not in any mode right now, type 'help' for more info");
//             break;
//         }

//         portENTER_CRITICAL(&mux);
//         this_device_settings.current_device_mode[0] = '0'; //Exit to main menu
//         myBluetoothInstructions.exit_mode_request = false; //Unset flag
//         clear_display = true;                              //Clear Display
//         portEXIT_CRITICAL(&mux);
//     }

//     // --  Send out status message '$'
//     if (myBluetoothInstructions.send_status_message)
//     {
//         String status_message;
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[BLE]  Sending out status message");
//         }

//         //Print out header
//         status_message += "PILL LOCKER CURRENT STATUS";
//         sendBLEMessage(status_message);
//         status_message = ""; //Flush String

//         //First Message
//         status_message += "RTC: ";
//         status_message += rtc.stringTimeStamp();
//         sendBLEMessage(status_message);

//         //Second Message
//         status_message = "";
//         status_message += "Remaining pills: ";
//         sendBLEMessage(status_message);
//         status_message = ""; //Flush String
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             status_message += ("        Tray ");
//             status_message += (i + 1);
//             status_message += ": ";
//             status_message += this_device_settings.pills_remaining[i];
//         }
//         sendBLEMessage(status_message);

//         //Third Message
//         status_message = "";
//         status_message += "Missed Dosages: ";
//         sendBLEMessage(status_message);
//         status_message = ""; //Flush String
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             status_message += ("        Tray ");
//             status_message += (i + 1);
//             status_message += ": ";
//             status_message += this_device_settings.missed_dosages[i];
//         }
//         sendBLEMessage(status_message);

//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.send_status_message = false;
//         portEXIT_CRITICAL(&mux);
//     }

//     // -- Send out current settings '?'
//     if (myBluetoothInstructions.send_current_settings)
//     {
//         String settings_message;
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[BLE]  Sending out current settings");
//         }

//         //Print out header
//         settings_message += "PILL LOCKER CURRENT SETTINGS";
//         sendBLEMessage(settings_message);
//         settings_message = ""; //Flush String

//         //Firmware Version
//         sendBLEMessage(("Firmware Version: " + FIRMWARE_VERSION));
//         settings_message = ""; //Flush String

//         //Demo Dispense Interval
//         settings_message += "Demo Dispense Interval: ";
//         sendBLEMessage(settings_message);
//         settings_message = ""; //Flush String
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             settings_message += ("        Tray ");
//             settings_message += (i + 1);
//             settings_message += ": ";
//             settings_message += this_device_settings.demo_dispense_interval_s[i];
//             settings_message += "s";
//         }
//         sendBLEMessage(settings_message);

//         //Active Dispense Interval
//         settings_message = "";
//         settings_message += "Activated Dispense Interval: ";
//         sendBLEMessage(settings_message);
//         settings_message = ""; //Flush String
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             settings_message += ("        Tray ");
//             settings_message += (i + 1);
//             settings_message += ": ";
//             settings_message += this_device_settings.activated_dispense_interval_s[i];
//             settings_message += "s";
//         }
//         sendBLEMessage(settings_message);

//         //Steps per pill
//         settings_message = ""; //Flush string
//         settings_message += "Steps per pill: ";
//         sendBLEMessage(settings_message);
//         settings_message = ""; //Flush String
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             settings_message += ("        Tray ");
//             settings_message += (i + 1);
//             settings_message += ": ";
//             settings_message += this_device_settings.motor_steps_per_pill[i];
//         }
//         sendBLEMessage(settings_message);

//         //Pill per tray
//         settings_message = ""; //Flush string
//         settings_message += "Pills per tray: ";
//         sendBLEMessage(settings_message);
//         settings_message = ""; //Flush String
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             settings_message += ("        Tray ");
//             settings_message += (i + 1);
//             settings_message += ": ";
//             settings_message += this_device_settings.pills_per_tray[i];
//         }
//         sendBLEMessage(settings_message);

//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.send_current_settings = false;
//         portEXIT_CRITICAL(&mux);
//     }
// }

// void bluetothInstructionHandlerCore1()
// {
//     std::string complex_instruction; //Declare possible instruction var

//     portENTER_CRITICAL(&mux);
//     char mode = this_device_settings.current_device_mode[0];
//     //if this flag is true, it must be unset at the end of each device mode handler!
//     if (myBluetoothInstructions.complex_instruction_request)
//     {
//         complex_instruction = myBluetoothInstructions.complex_instruction_string;
//     } //Check for possible instruction
//     portEXIT_CRITICAL(&mux);

//     switch (mode)
//     {
//     case 'A':
//         ///Activated Mode
//         activatedModeHandler();

//         break;
//     case 'T':
//         ///Test Mode
//         testModeHandler(complex_instruction);
//         portENTER_CRITICAL(&mux);
//         myBluetoothInstructions.complex_instruction_request = false;
//         portEXIT_CRITICAL(&mux);
//         break;
//     case 'U':
//         //Utilities Mode
//         utilitiesModeHandler(complex_instruction);
//         break;
//     case 'D':
//         //Demo Mode
//         demoModeHandler();
//         break;
//     case '0':
//         //Do nothing
//         delay(100);
//         break;
//     default:
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[ERR]  Invalid Device Mode");
//         }
//         break;
//     }
// }

// void connectionManager()
// {
//     // disconnecting
//     if (!deviceConnected && oldDeviceConnected)
//     {
//         portENTER_CRITICAL(&mux);
//         char mode = this_device_settings.current_device_mode[0];
//         portEXIT_CRITICAL(&mux);

//         //Force device to go to normal mode
//         if (mode != 'A' && mode != '0')
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[BLE]  Device was disconnected without exiting a mode. Setting to default");
//             }

//             portENTER_CRITICAL(&mux);
//             this_device_settings.current_device_mode[0] = '0';
//             clear_display = true;
//             portEXIT_CRITICAL(&mux);
//         }

//         //Clear main screen
//         if (mode == '0')
//         {
//             clear_display = true;
//         }

//         if (this_device_settings.current_device_mode[0])
//             oldDeviceConnected = deviceConnected;
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[BLE]  BLE Service has disconnected.");
//         }
//         pServer->startAdvertising(); // restart advertising
//     }

//     // connecting
//     if (deviceConnected && !oldDeviceConnected)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[BLE]  BLE Service is connected.");
//         }
//         portENTER_CRITICAL(&mux);
//         clear_display = true;
//         portEXIT_CRITICAL(&mux);
//         oldDeviceConnected = deviceConnected;
//     }
// }

// void IRAM_ATTR adjustTimeRequest()
// {
//     if (deviceConnected)
//     {
//         portENTER_CRITICAL(&mux);
//         //Create data string: split data with commas
//         String data;
//         data += "TR";

//         //Send Data
//         pTxCharacteristic->setValue((char *)data.c_str());
//         pTxCharacteristic->notify();
//         portEXIT_CRITICAL(&mux);
//     }
// }

// //INITIAL SETUP
// void setPinsForDeepSleep()
// {
//     //SET UP WAKE UP BUTTONS
//     esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, 0); //POWER BUTON

//     rtc_gpio_init((gpio_num_t)MOTORSLEEP1);
//     rtc_gpio_set_direction((gpio_num_t)MOTORSLEEP1, RTC_GPIO_MODE_OUTPUT_ONLY);
//     rtc_gpio_set_level((gpio_num_t)MOTORSLEEP1, 1); //GPIO HIGH

//     rtc_gpio_init((gpio_num_t)MOTORSLEEP2);
//     rtc_gpio_set_direction((gpio_num_t)MOTORSLEEP2, RTC_GPIO_MODE_OUTPUT_ONLY);
//     rtc_gpio_set_level((gpio_num_t)MOTORSLEEP2, 1); //GPIO HIGH

//     rtc_gpio_init((gpio_num_t)MOTORSLEEP3);
//     rtc_gpio_set_direction((gpio_num_t)MOTORSLEEP3, RTC_GPIO_MODE_OUTPUT_ONLY);
//     rtc_gpio_set_level((gpio_num_t)MOTORSLEEP3, 1); //GPIO HIGH

//     esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
// }

// //BATTERY
// void initBattery()
// {
//     pinMode(VBAT_3V3, INPUT_PULLDOWN);
//     pinMode(BAT_STAT1, INPUT_PULLUP);
//     pinMode(BAT_STAT2, INPUT_PULLUP);
// }

// //MOTORS
// void initMotors()
// {
//     //SLEEP (ACTIVE LOW)
//     pinMode(MOTORSLEEP1, OUTPUT);
//     pinMode(MOTORSLEEP2, OUTPUT);
//     pinMode(MOTORSLEEP3, OUTPUT);

//     //DISABLE ALL MOTORS
//     digitalWrite(MOTORSLEEP1, HIGH);
//     digitalWrite(MOTORSLEEP2, HIGH);
//     digitalWrite(MOTORSLEEP3, HIGH);

//     //BEGIN MOTORS
//     motor1.begin(LOAD_RPM, MICROSTEPS);
//     motor2.begin(LOAD_RPM, MICROSTEPS);
//     motor3.begin(LOAD_RPM, MICROSTEPS);
// }

// void moveMotor(int motor, int direction, int steps)
// {
//     switch (motor)
//     {
//     case 1:
//         digitalWrite(MOTORSLEEP1, LOW);
//         if (direction == -1)
//         {
//             motor1.move(-steps);
//         }
//         else if (direction == 1)
//         {
//             motor1.move(steps);
//         }
//         digitalWrite(MOTORSLEEP1, HIGH);
//         break;
//     case 2:
//         digitalWrite(MOTORSLEEP2, LOW);
//         if (direction == -1)
//         {
//             motor2.move(-steps);
//         }
//         else if (direction == 1)
//         {
//             motor2.move(steps);
//         }
//         digitalWrite(MOTORSLEEP2, HIGH);
//         break;
//     case 3:
//         digitalWrite(MOTORSLEEP3, LOW);
//         if (direction == -1)
//         {
//             motor3.move(-steps);
//         }
//         else if (direction == 1)
//         {
//             motor3.move(steps);
//         }
//         digitalWrite(MOTORSLEEP3, HIGH);
//         break;
//     default:
//         break;
//     }
// }

// //SCREEN
// void initScreen()
// {
//     //ENABLE SCREEN
//     pinMode(TFT_RST, OUTPUT);
//     digitalWrite(TFT_RST, HIGH);

//     //INIT SCREEN
//     tft.init(136, 241); // Init ST7789 240x135
//     tft.setSPISpeed(40000000);
//     tft.setRotation(1);
//     tft.fillScreen(ST77XX_BLACK);

//     //Update screen just to get
//     //updateScreen();

//     //Display Initial Time
//     if (deviceConnected)
//     {
//         tft.setCursor(10, 10);
//         tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
//         tft.setTextSize(2);
//         tft.print("BLE CON");
//     }
//     else
//     {
//         tft.setCursor(10, 10);
//         tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
//         tft.setTextSize(2);
//         tft.print("BLE DIS");
//     }
//     tft.setCursor(140, 10);
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(2);
//     String header;
//     int hours, minutes, seconds;
//     hours = rtc.getHours();

//     //Create nice time string to print
//     if (hours < 10)
//     {
//         header += " ";
//     }
//     header += hours;
//     header += ":";

//     minutes = rtc.getMinutes();
//     if (minutes < 10)
//     {
//         header += " ";
//     }
//     header += minutes;
//     header += ":";

//     seconds = rtc.getSeconds();
//     if (seconds < 10)
//     {
//         header += " ";
//     }
//     header += seconds;

//     //print header
//     tft.print(header);

//     tft.drawLine(0, 30, 241, 30, ST77XX_WHITE);

//     //TURN ON BACK LIGHT
//     pinMode(LEDK, OUTPUT);
//     digitalWrite(LEDK, LOW);
// }

// void updateScreen()
// {
//     if (deviceConnected)
//     {
//         tft.setCursor(10, 10);
//         tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
//         tft.setTextSize(2);
//         tft.print("BLE CON");
//     }
//     else
//     {
//         tft.setCursor(10, 10);
//         tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
//         tft.setTextSize(2);
//         tft.print("BLE DIS");
//     }
//     tft.setCursor(140, 10);
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(2);
//     String header;
//     int hours, minutes, seconds;
//     hours = rtc.getHours();

//     //Create nice time string to print
//     if (hours < 10)
//     {
//         header += " ";
//     }
//     header += hours;
//     header += ":";

//     minutes = rtc.getMinutes();
//     if (minutes < 10)
//     {
//         header += " ";
//     }
//     header += minutes;
//     header += ":";

//     seconds = rtc.getSeconds();
//     if (seconds < 10)
//     {
//         header += " ";
//     }
//     header += seconds;

//     //print header
//     tft.print(header);

//     tft.drawLine(0, 30, 241, 30, ST77XX_WHITE);

//     //Clear if necessary
//     if (clear_display)
//     {
//         tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);
//         clear_display = false;
//     }

//     portENTER_CRITICAL(&mux);
//     char mode = this_device_settings.current_device_mode[0];
//     portEXIT_CRITICAL(&mux);

//     switch (mode)
//     {
//     case 'A':
//         ///Activated Mode
//         activeModeScreenManager();
//         break;
//     case 'T':
//         ///Test Mode
//         testModeScreenManager();
//         break;
//     case 'U':
//         //Utilities Mode
//         utilitiesModeScreenManager();
//         break;
//     case 'D':
//         //Demo Mode
//         demoModeScreenManager();
//         break;
//     case '0':
//         mainMenuScreenManager();
//         break;
//     default:
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[ERR]  Invalid Device Mode");
//         }
//         break;
//     }
//     //Display sleep warning
//     sleepWarningScreenManager();
// }

// void activeModeScreenManager()
// {
// }

// void mainMenuScreenManager()
// {
//     if (!display_sleep_warning_flag)
//     {
//         if (deviceConnected)
//         {
//             tft.setCursor(0, 40);
//             tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//             tft.setTextSize(2);
//             //with size text 2, 20 characters fit per line
//             tft.print("   Select a mode:   ");
//             tft.setCursor(0, 60);
//             tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
//             tft.setTextSize(2);
//             tft.print("     [A]ctivate          [D]emo              [T]est              [U]tilities");
//         }
//         else
//         {
//             tft.setCursor(0, 50);
//             tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//             tft.setTextSize(2);
//             //with size text 2, 20 characters fit per line
//             tft.print("   Welcome to the       Pill Locker.");
//             tft.setCursor(0, 90);
//             tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
//             tft.setTextSize(2);
//             tft.print("Connect to Bluetooth   to get started   ");
//         }
//     }
// }

// void testModeScreenManager()
// {
//     if (!display_sleep_warning_flag)
//     {
//         tft.setCursor(0, 40);
//         tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
//         tft.setTextSize(2);
//         //with size text 2, 20 characters fit per line
//         tft.print("  Device is now in       Test Mode     ");
//         tft.setCursor(0, 80);
//         tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
//         tft.setTextSize(2);
//         tft.print("     Use App to       change settings   ");
//     }
// }

// void utilitiesModeScreenManager()
// {
//     if (deviceConnected)
//     {
//         if (!display_sleep_warning_flag)
//         {
//             tft.setCursor(0, 40);
//             tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
//             tft.setTextSize(2);
//             //with size text 2, 20 characters fit per line
//             tft.print("  Device is now in     Utilities Mode   ");
//             tft.setCursor(0, 80);
//             tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
//             tft.setTextSize(2);
//             tft.print("     Use App to       change settings   ");
//         }
//     }
// }

// void demoModeScreenManager()
// {
// }

// void sleepWarningScreenManager()
// {
//     if (!deviceConnected)
//     {
//         long time = timerRead(battery_timer);
//         long time_10 = (TIME_TO_POWER_SAVE_START_S - 10) * uS_TO_S_FACTOR;
//         if (time >= time_10)
//         {
//             //This will prevent the warning message from showing after disconnect
//             if (!skip_screen && display_sleep_warning_flag == false)
//             {
//                 if (clean_screen_once)
//                 {
//                     clean_screen_once = false;
//                     tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);
//                 }
//                 //Only needed for main menu
//                 display_sleep_warning_flag = true;
//                 tft.setCursor(0, 40);
//                 tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
//                 tft.setTextSize(2);
//                 //with size text 2, 20 characters fit per line
//                 tft.print("  Device will enter  sleep mode soon to     save battery    ");
//                 tft.setCursor(0, 90);
//                 tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//                 tft.setTextSize(2);
//                 tft.print("  Press any button       to cancel     ");
//             }
//             else if (skip_screen)
//             {
//                 skip_screen = false;
//             }
//         }

//         else
//         {
//             if (display_sleep_warning_flag == true)
//             {
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[DIS]  Erasing Warning Message");
//                 }
//                 tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);

//                 //Unset flags
//                 display_sleep_warning_flag = false;
//                 clean_screen_once = true;
//             }
//         }
//     }
//     //This will clean up the display after being connected to BLE
//     else
//     {
//         if (display_sleep_warning_flag == true)
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[DIS]  Erasing Warning Message");
//             }
//             tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);

//             //Unset flags
//             display_sleep_warning_flag = false;
//             clean_screen_once = true;
//         }
//     }
// }

// //RTC
// void initRTC()
// {
//     Wire.begin();
//     Wire.setClock(50000);

//     int init_count = 0;
//     while (init_count < 3)
//     {
//         if (rtc.begin(Wire, false, true, false) == false)
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[RTC]  Problem Initializing RTC");
//             }
//             init_count++;
//         }
//         else
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[RTC]  Initialized Correctly");
//             }
//             init_count = 4;
//         }
//     }

//     if (first_boot)
//     {
//         rtc.setToCompilerTime();
//         rtc.set12Hour();
//         if (rtc.updateTime() == false) //Updates the time variables from RTC
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[RTC]  Could not update time");
//             }
//         }
//         else
//         {
//             String currentTime = rtc.stringTimeStamp();
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[RTC]  Setting to compiler Time");
//                 Serial.print("[RTC]  ");
//                 Serial.println(currentTime);
//             }
//         }

//         //Create Demo Mode arrays

//         //Create Actiated Mode arrays

//         first_boot = false;
//     }
//     else
//     {
//         if (rtc.updateTime() == false) //Updates the time variables from RTC
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.println("[RTC]  Could not update time");
//             }
//         }
//         else
//         {
//             String currentTime = rtc.stringTimeStamp();
//             if (serial_debugging_enabled)
//             {
//                 Serial.print("[RTC]  ");
//                 Serial.println(currentTime);
//             }
//         }
//     }

//     //Backup Switchover Mode
//     // Serial.print("Config EEPROM 0x37 before: ");
//     // Serial.println(rtc.readConfigEEPROM_RAMmirror(0x37));

//     //rtc.setBackupSwitchoverMode(0); //Switchover disabled
//     //rtc.setBackupSwitchoverMode(1); //Direct Switching Mode
//     //rtc.setBackupSwitchoverMode(2); //Standby Mode
//     //rtc.setBackupSwitchoverMode(3); //Level Switching Mode (default)

//     // Serial.print("Config EEPROM 0x37 after: ");
//     // Serial.println(rtc.readConfigEEPROM_RAMmirror(0x37));
// }

// void alarmController()
// {
//     if (pill_locker_sleeping)
//     {
//         // Update time as necessary
//         esp_sleep_wakeup_cause_t wakeup_reason;
//         wakeup_reason = esp_sleep_get_wakeup_cause();

//         // -- Print wake up reason
//         if (serial_debugging_enabled)
//         {
//             switch (wakeup_reason)
//             {
//             case ESP_SLEEP_WAKEUP_EXT0:
//                 Serial.println("[RTC]  Wakeup caused by external signal using Power Button");
//                 break;
//             case ESP_SLEEP_WAKEUP_EXT1:
//                 Serial.println("[RTC]  Wakeup caused by external signal using RTC_CNTL");
//                 break;
//             case ESP_SLEEP_WAKEUP_TIMER:
//                 Serial.println("[RTC]  Wakeup caused by timer");
//                 break;
//             case ESP_SLEEP_WAKEUP_TOUCHPAD:
//                 Serial.println("[RTC]  Wakeup caused by touchpad");
//                 break;
//             case ESP_SLEEP_WAKEUP_ULP:
//                 Serial.println("[RTC]  Wakeup caused by ULP program");
//                 break;
//             default:
//                 Serial.printf("[RTC]  Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
//                 break;
//             }
//         }

//         bool wakeup;
//         //bool wakeup = myPatient.checkAlarms(this_device_settings.real_time_clock);

//         if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
//         {
//             if (wakeup)
//             {
//                 Serial.println("[RTC]  Dispense Alarm Triggered");
//                 //pill_locker_sleeping = false;
//             }
//             else
//             {
//                 Serial.println("[RTC]  No alarm detected. ESP going back to sleep");
//                 pill_locker_sleeping = true;
//                 setPinsForDeepSleep();
//                 esp_deep_sleep_start();
//             }
//         }
//     }
// }

// void adjustTime(std::string data)
// {
//     portENTER_CRITICAL(&mux);

//     portEXIT_CRITICAL(&mux);
// }

// //BUTTONS
// void initButtons()
// {
//     //ACTIVE LOW
//     pinMode(POW_BUTTON, INPUT);
//     pinMode(UP_BUTTON, INPUT);
//     pinMode(DOWN_BUTTON, INPUT);
//     pinMode(OK_BUTTON, INPUT);

//     //RESET BATTERY TIMER IF PRESSED
//     attachInterrupt(UP_BUTTON, buttonInterrupt, FALLING);
//     attachInterrupt(DOWN_BUTTON, buttonInterrupt, FALLING);
//     attachInterrupt(OK_BUTTON, buttonInterrupt, FALLING);
// }

// //BUZZER
// void initBuzzer()
// {
//     ledcAttachPin(AUDIO, 0);
//     ledcSetup(0, 4000, 8);
// }

// void soundBuzzer()
// {
//     // for (int i = 0; i < 5; i++)
//     // {
//     //     ledcWrite(0, 128);
//     //     delay(100);
//     //     ledcWrite(0, 0);
//     //     delay(100);
//     // }
// }

// //FAKE RTC
// void startSleepTimer()
// {
//     // //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
//     // //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//     // digitalWrite(LEDK, LOW);
//     // digitalWrite(TFT_RST, LOW);
//     // esp_deep_sleep_start();
// }

// void print_wakeup_reason()
// {
//     esp_sleep_wakeup_cause_t wakeup_reason;

//     wakeup_reason = esp_sleep_get_wakeup_cause();

//     switch (wakeup_reason)
//     {
//     case ESP_SLEEP_WAKEUP_EXT0:
//         Serial.println("Wakeup caused by external signal using RTC_IO");
//         break;
//     case ESP_SLEEP_WAKEUP_EXT1:
//         Serial.println("Wakeup caused by external signal using RTC_CNTL");
//         break;
//     case ESP_SLEEP_WAKEUP_TIMER:
//         Serial.println("Wakeup caused by timer");
//         break;
//     case ESP_SLEEP_WAKEUP_TOUCHPAD:
//         Serial.println("Wakeup caused by touchpad");
//         break;
//     case ESP_SLEEP_WAKEUP_ULP:
//         Serial.println("Wakeup caused by ULP program");
//         break;
//     default:
//         Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
//         break;
//     }
// }