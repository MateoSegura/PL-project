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

// #define uS_TO_S_FACTOR 1000000
// #define TIME_TO_SLEEP_uS_S 1000000  /* Time ESP32 will go to sleep (1 Second) */
// #define TIME_TO_SLEEP_uS_M 60000000 /* Time ESP32 will go to sleep (1 Minute) */
// #define TIME_TO_POWER_SAVE_START 30
// #define NUMBER_OF_DUMMY_ARRAYS 2
// bool sleep_minutes = true; //make true for real product

// #define OUT_DIRECTION -1
// #define IN_DIRECTION 1

// #define STEPS_PER_TRAY 1166
// #define TOTAL_STEPS 1166

// String FIRMWARE_VERSION = "V3.1";

// const bool serial_debugging_enabled = true;
// RTC_DATA_ATTR bool first_boot = true;

// //Set time again after battery loss
// bool set_time = true;

// //4 Main Device types

// //Bluetooth Instructions
// char bluetooth_instruction = '0';
// bool print_time = false;
// bool unload_instruction = false;
// bool load_instruction = false;
// bool dispense_instruction = false;
// bool update_time_at_ble_connect = false;

// //Other Instructions
// bool use_dummy_arrays = false;

// //************************************************************DEFINTIONS*****************************************************************//
// //PILL LOCKER SETTINGS
// #define NUMBER_OF_TRAYS 3

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

// ArduinoNvs myNVS;

// //PILL LOCKER SETTINGS
// class PillLockerSettings
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
//         this->current_device_mode = 'M';

//         //Set tray dependent settings
//         for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//         {
//             this->activated_dispense_interval_s[i] = 2;
//             this->demo_dispense_interval_s[i] = i;
//             this->pills_per_tray[i] = 0;
//             this->motor_steps_per_pill[i] = 0;
//             this->medication_name[i] = "Not set";

//             //Activated variables
//             this->pills_remaining[i] = 0;
//             this->missed_dosages[i] = 0;
//         }

//         //Set Patient Information
//         this->patient_first_name = "Not set";
//         this->patient_last_name = "Not set";
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
// };

// RTC_DATA_ATTR PillLockerSettings this_device_settings;

// //DUAL CORE
// static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

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

// //RTC
// RTC_DATA_ATTR bool pill_locker_sleeping = false;
// RV3028 rtc;
// DateTime global_time;
// bool update_time; //flag

// RTC_DATA_ATTR realTimeClock dummy_time_arrays[NUMBER_OF_DUMMY_ARRAYS];

// //PATIENT DATA
// Patient myPatient = Patient(this_device_settings.real_time_clock);
// bool dispense_prescription = false;

// //BLE
// BLEServer *pServer = NULL;
// BLECharacteristic *pTxCharacteristic;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;

// //*******************************************************FUNCTION DECLARATION************************************************************//

// //SERIAL

// // FLASH STORAGE
// void initStorage();

// //INSTRUCTION FUNCTIONS
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

// //BLE
// void initBLE();
// void assignBluetoothFlag(std::string data);
// void IRAM_ATTR adjustTimeRequest();
// void connectionManager();
// void sendBLEMessage(String data);

// void handleBluetoothInstruction();

// class MyServerCallbacks : public BLEServerCallbacks
// {
//     void onConnect(BLEServer *pServer)
//     {
//         deviceConnected = true;
//     };

//     void onDisconnect(BLEServer *pServer)
//     {
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
// void IRAM_ATTR batterySaver()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     if (!deviceConnected)
//     {
//         Serial.println("[POW]  Device going to sleep to reduce power consumption");
//         pill_locker_sleeping = true;
//         setPinsForDeepSleep();
//         esp_deep_sleep_start();
//     }
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //BUTTONS
// // Resets batterySaverTimer
// void IRAM_ATTR buttonInterrupt()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     Serial.println("[POW]  Battery saver timer reset");
//     battery_timer = 0;
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //Does not work
// void IRAM_ATTR batteryDisconnected()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     Serial.println("[POW]  Battery not detected");
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //**************************************************************SETUP********************************************************************//
// void setup()
// {
//     //SERIAL
//     if (serial_debugging_enabled)
//     {
//         Serial.begin(115200);
//     }

//     //Increment time and check if time to wake up
//     // 2202 microseconds to here
//     if (pill_locker_sleeping)
//     {
//         alarmController();
//     }

//     //Wait for Serial
//     if (first_boot)
//     {
//         while (!Serial)
//             ;
//         delay(100);
//     }

//     long initial = micros();
//     //FLASH STORAGE
//     initStorage();
//     Serial.println(micros() - initial);

//     //RTC
//     initRTC();

//     //BATTERY
//     initBattery();

//     //MOTORS
//     initMotors();

//     //BUZZER
//     initBuzzer();

//     //SCREEN
//     initScreen();

//     //  --  SOFTWARE INTERRUPTS:
//     //RTC
//     real_time_clock_timer = timerBegin(0, 80, true);
//     timerAttachInterrupt(real_time_clock_timer, &globalTimeKeeperInterrupt, true);
//     timerAlarmWrite(real_time_clock_timer, 1000000, true); //microseconds (1s)
//     timerAlarmEnable(real_time_clock_timer);               //Initialize Timer

//     //BATTERY
//     battery_timer = timerBegin(1, 80, true);
//     timerAttachInterrupt(battery_timer, &batterySaver, true);
//     timerAlarmWrite(battery_timer, TIME_TO_POWER_SAVE_START * uS_TO_S_FACTOR, true); //microseconds (1s)
//     timerAlarmEnable(battery_timer);                                                 //Initialize Timer

//     //  --   HARDWARE INTERRUPTS:
//     //POWER
//     //attachInterrupt(digitalPinToInterrupt(VBAT_3V3), batteryDisconnected, FALLING);

//     //BLE
//     initBLE();

//     //BUTTONS
//     initButtons();

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
// }

// //**************************************************************CORE 0*******************************************************************//
// void core0Task(void *pvParameters)
// {
//     for (;;)
//     {
//         //Check for Bluetooth Instruction
//         if (bluetooth_instruction)
//         {
//             handleBluetoothInstruction();
//         }
//         // --  Print buttons
//         // Serial.print(digitalRead(POW_BUTTON));
//         // Serial.print(digitalRead(UP_BUTTON));
//         // Serial.print(digitalRead(DOWN_BUTTON));
//         // Serial.println(digitalRead(OK_BUTTON));

//         // --  Print Time
//         bool instruction;
//         portENTER_CRITICAL(&mux);
//         instruction = print_time;
//         portEXIT_CRITICAL(&mux);

//         if (instruction)
//         {
//             if (serial_debugging_enabled)
//             {
//                 printTime();
//                 portENTER_CRITICAL(&mux);
//                 print_time = false;
//                 portEXIT_CRITICAL(&mux);
//             }
//         }

//         if (dispense_prescription)
//         {
//             soundBuzzer();
//             dispense_prescription = false;
//             portENTER_CRITICAL(&mux);
//             dispense_instruction = true;
//             portEXIT_CRITICAL(&mux);
//         }
//         else
//         {
//             delay(100);
//         }

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
//             update_time = false;
//         }

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
//     }
// }

// //**************************************************************CORE 1*******************************************************************//
// void core1Task(void *pvParameters)
// {
//     for (;;)
//     {
//         connectionManager();
//     }
// }

// //***************************************************************LOOP********************************************************************//
// void loop()
// {
//     yield();
// }
// //*******************************************************FUNCTION DEFINITION************************************************************//

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
//             delay(2000);
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
//             Serial.println("[MEM]  Device Settings have been restored");
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
//     moveMotor(2, OUT_DIRECTION, TOTAL_STEPS);
//     //DISABLE ALL MOTORS
//     digitalWrite(MOTORSLEEP1, HIGH);
//     digitalWrite(MOTORSLEEP2, HIGH);
//     digitalWrite(MOTORSLEEP3, HIGH);
//     unload_instruction = false;
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
//     moveMotor(2, IN_DIRECTION, TOTAL_STEPS);
//     //DISABLE ALL MOTORS
//     digitalWrite(MOTORSLEEP1, HIGH);
//     digitalWrite(MOTORSLEEP2, HIGH);
//     digitalWrite(MOTORSLEEP3, HIGH);
//     load_instruction = false;

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
//     Serial.println("[BLE]  Device is advertising & ready to connect");
// }

// void sendBLEMessage(String data)
// {
//     portENTER_CRITICAL(&mux);
//     String ble_data;
//     ble_data += data;

//     //Send Data
//     pTxCharacteristic->setValue((char *)ble_data.c_str());
//     pTxCharacteristic->notify();
//     portEXIT_CRITICAL(&mux);
// }

// void assignBluetoothFlag(std::string data)
// {
//     if (data.length() > 0)
//     {
//         //Print raw data
//         int len = data.length();
//         if (serial_debugging_enabled)
//         {
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
//                 ESP.restart();
//                 break;
//             //Activate Device
//             case 'A':
//                 if (serial_debugging_enabled)
//                 {

//                     Serial.println("[BLE]  Device Mode: Activate Mode ");
//                 }
//                 this_device_settings.current_device_mode = 'A';
//                 sendBLEMessage("Pill Locker is now in Activated Mode");
//                 break;

//             //Test
//             case 'T':
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[BLE]  Device Modes: Test Mode ");
//                 }
//                 sendBLEMessage("Pill Locker is now in Test Mode");
//                 this_device_settings.current_device_mode = 'T';
//                 break;

//             //Utilities
//             case 'U':
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[BLE]  Device Modes: Utilities Mode ");
//                 }
//                 sendBLEMessage("Pill Locker is now in Utilities Mode");
//                 this_device_settings.current_device_mode = 'U';
//                 break;

//             //Demo
//             case 'D':
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[BLE]  Device Modes: Demo Mode ");
//                 }
//                 sendBLEMessage("Pill Locker is now in in Demo Mode");
//                 this_device_settings.current_device_mode = 'D';
//                 break;

//             //Exit Demo mode
//             case '#':
//                 if (this_device_settings.current_device_mode[0] == 'D')
//                 {
//                     if (serial_debugging_enabled)
//                     {
//                         Serial.println("[BLE]  Exiting Demo Mode ");
//                     }
//                     sendBLEMessage("Exiting demo mode");
//                 }
//                 break;

//             //Send out status message
//             case '$':
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[BLE]  Sending out status message");
//                 }

//                 //Print out header
//                 status_message += "PILL LOCKER CURRENT STATUS";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String

//                 //First Message
//                 portENTER_CRITICAL(&mux);
//                 status_message += "RTC: ";
//                 status_message += this_device_settings.real_time_clock.year;
//                 status_message += '/';
//                 status_message += this_device_settings.real_time_clock.month;
//                 status_message += '/';
//                 status_message += this_device_settings.real_time_clock.day;
//                 status_message += ' ';
//                 status_message += this_device_settings.real_time_clock.hour;
//                 status_message += ':';
//                 status_message += this_device_settings.real_time_clock.minute;
//                 status_message += ':';
//                 status_message += this_device_settings.real_time_clock.second;
//                 portEXIT_CRITICAL(&mux);
//                 sendBLEMessage(status_message);

//                 //Second Message
//                 status_message = "";
//                 status_message += "Remaining pills: ";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String
//                 for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                 {
//                     status_message += ("        Tray ");
//                     status_message += (i + 1);
//                     status_message += ": ";
//                     status_message += this_device_settings.pills_remaining[i];
//                 }
//                 sendBLEMessage(status_message);

//                 //Third Message
//                 status_message = "";
//                 status_message += "Missed Dosages: ";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String
//                 for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                 {
//                     status_message += ("        Tray ");
//                     status_message += (i + 1);
//                     status_message += ": ";
//                     status_message += this_device_settings.missed_dosages[i];
//                 }
//                 sendBLEMessage(status_message);

//                 break;

//             // Send out current settings
//             case '?':
//                 if (serial_debugging_enabled)
//                 {
//                     Serial.println("[BLE]  Sending out current settings");
//                 }

//                 //Print out header
//                 status_message += "PILL LOCKER CURRENT SETTINGS";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String

//                 //Firmware Version
//                 sendBLEMessage(("Firmware Version: " + FIRMWARE_VERSION));
//                 status_message = ""; //Flush String

//                 //Demo Dispense Interval
//                 status_message += "Demo Dispense Interval: ";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String
//                 for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                 {
//                     status_message += ("        Tray ");
//                     status_message += (i + 1);
//                     status_message += ": ";
//                     status_message += this_device_settings.demo_dispense_interval_s[i];
//                     status_message += "s";
//                 }
//                 sendBLEMessage(status_message);

//                 //Active Dispense Interval
//                 status_message = "";
//                 status_message += "Activated Dispense Interval: ";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String
//                 for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                 {
//                     status_message += ("        Tray ");
//                     status_message += (i + 1);
//                     status_message += ": ";
//                     status_message += this_device_settings.activated_dispense_interval_s[i];
//                     status_message += "s";
//                 }
//                 sendBLEMessage(status_message);

//                 //Steps per pill
//                 status_message = ""; //Flush string
//                 status_message += "Steps per pill: ";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String
//                 for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                 {
//                     status_message += ("        Tray ");
//                     status_message += (i + 1);
//                     status_message += ": ";
//                     status_message += this_device_settings.motor_steps_per_pill[i];
//                 }
//                 sendBLEMessage(status_message);

//                 //Pill per tray
//                 status_message = ""; //Flush string
//                 status_message += "Pills per tray: ";
//                 sendBLEMessage(status_message);
//                 status_message = ""; //Flush String
//                 for (int i = 0; i < NUMBER_OF_TRAYS; i++)
//                 {
//                     status_message += ("        Tray ");
//                     status_message += (i + 1);
//                     status_message += ": ";
//                     status_message += this_device_settings.pills_per_tray[i];
//                 }
//                 sendBLEMessage(status_message);

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
//     }
// }

// void connectionManager()
// {

//     // disconnecting
//     if (!deviceConnected && oldDeviceConnected)
//     {
//         Serial.println("[BLE]  BLE Service has disconnected.");
//         delay(200);
//         pServer->startAdvertising(); // restart advertising
//         oldDeviceConnected = deviceConnected;
//     }

//     // connecting
//     if (deviceConnected && !oldDeviceConnected)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[BLE]  BLE Service is connected.");
//         }

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

// void handleBluetoothInstruction()
// {
//     /*
//         THE SWITCH STATEMENT MUST REMAIN CONSTATN WITH THE ONE IN adjustTimeRequest(). This is because the function mentioned is
//         an interrupt, and some of the instructions might cause a core to panic.
//     */
//     switch (bluetooth_instruction)
//     {
//         // Unlaod Pills
//     case 'U':
//         portENTER_CRITICAL_ISR(&mux);
//         bluetooth_instruction = '0';
//         portEXIT_CRITICAL_ISR(&mux);
//         break;
//         // Load Pills
//     case 'L':
//         portENTER_CRITICAL_ISR(&mux);
//         bluetooth_instruction = '0';
//         portEXIT_CRITICAL_ISR(&mux);
//         break;
//         // Dispense Pills
//     case 'D':
//         portENTER_CRITICAL_ISR(&mux);
//         bluetooth_instruction = '0';
//         portEXIT_CRITICAL_ISR(&mux);
//         break;
//         // Update time at initial BLE connection
//     default:
//         break;
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

//     //TURN ON BACK LIGHT
//     pinMode(LEDK, OUTPUT);
//     digitalWrite(LEDK, LOW);

//     //INIT SCREEN

//     tft.init(136, 241); // Init ST7789 240x135
//     tft.setSPISpeed(40000000);
//     tft.setRotation(1);
//     tft.fillScreen(ST77XX_BLACK);
// }

// //RTC
// void initRTC()
// {
//     // //Increment boot number and print it every reboot
//     // if (sleep_minutes)
//     // {
//     //     esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_uS_M);
//     // }
//     // else
//     // {
//     //     esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_uS_S);
//     // }

//     // // Set Compiler Time only when re-programmed
//     // if (first_boot)
//     // {
//     //     //create dummy arrays
//     //     global_time = DateTime(__DATE__, __TIME__);
//     //     this_device_settings.real_time_clock.year = global_time.year();
//     //     this_device_settings.real_time_clock.month = global_time.month();
//     //     this_device_settings.real_time_clock.day = global_time.day();
//     //     this_device_settings.real_time_clock.hour = global_time.hour();
//     //     this_device_settings.real_time_clock.minute = global_time.minute();
//     //     this_device_settings.real_time_clock.second = global_time.second();

//     //     //Setting dummy arrays
//     //     myPatient = Patient(this_device_settings.real_time_clock);
//     //     myPatient.printPrescriptionAlarms();

//     //     first_boot = false;
//     // }

//     Wire.begin();
//     if (rtc.begin() == false)
//     {
//         if (serial_debugging_enabled)
//         {
//             Serial.println("[RTC]  Problem Initializing RTC");
//         }
//         while (1)
//             ;
//     }
//     else
//     {
//         Serial.println("[RTC]  Initialized Correctly");
//         rtc.setToCompilerTime();
//     }

//     if (first_boot)
//     {
//         if (rtc.updateTime() == false) //Updates the time variables from RTC
//         {
//             if (serial_debugging_enabled)
//             {
//                 Serial.print("[RTC]  Could not update time");
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

//         // //create dummy arrays
//         // global_time = DateTime(__DATE__, __TIME__);
//         // this_device_settings.real_time_clock.year = global_time.year();
//         // this_device_settings.real_time_clock.month = global_time.month();
//         // this_device_settings.real_time_clock.day = global_time.day();
//         // this_device_settings.real_time_clock.hour = global_time.hour();
//         // this_device_settings.real_time_clock.minute = global_time.minute();
//         // this_device_settings.real_time_clock.second = global_time.second();

//         // //Setting dummy arrays
//         // myPatient = Patient(this_device_settings.real_time_clock);
//         // myPatient.printPrescriptionAlarms();

//         // first_boot = false;
//     }

//     //Backup Switchover Mode
//     // Serial.print("Config EEPROM 0x37 before: ");
//     // Serial.println(rtc.readConfigEEPROM_RAMmirror(0x37));

//     rtc.setBackupSwitchoverMode(0); //Switchover disabled
//     //rtc.setBackupSwitchoverMode(1); //Direct Switching Mode
//     //rtc.setBackupSwitchoverMode(2); //Standby Mode
//     //rtc.setBackupSwitchoverMode(3); //Level Switching Mode (default)

//     // Serial.print("Config EEPROM 0x37 after: ");
//     // Serial.println(rtc.readConfigEEPROM_RAMmirror(0x37));
// }

// void printTime()
// {
//     portENTER_CRITICAL(&mux);
//     Serial.print("[RTC]  ");
//     Serial.print(this_device_settings.real_time_clock.year);
//     Serial.print("/");
//     Serial.print(this_device_settings.real_time_clock.month);
//     Serial.print("/");
//     Serial.print(this_device_settings.real_time_clock.day);
//     Serial.print("  ");
//     Serial.print(this_device_settings.real_time_clock.hour);
//     Serial.print(":");
//     Serial.print(this_device_settings.real_time_clock.minute);
//     Serial.print(":");
//     Serial.println(this_device_settings.real_time_clock.second);
//     portEXIT_CRITICAL(&mux);
// }

// void IRAM_ATTR updateTimeSeconds()
// {
//     //Seconds
//     this_device_settings.real_time_clock.second++;
//     if (this_device_settings.real_time_clock.second == 60)
//     {
//         this_device_settings.real_time_clock.second = 0;
//         this_device_settings.real_time_clock.minute++;
//     }

//     //Minutes
//     if (this_device_settings.real_time_clock.minute == 60)
//     {
//         this_device_settings.real_time_clock.minute = 0;
//         this_device_settings.real_time_clock.hour++;
//     }

//     //Hour
//     if (this_device_settings.real_time_clock.hour == 24)
//     {

//         this_device_settings.real_time_clock.hour = 0;
//         this_device_settings.real_time_clock.day++;
//     }

//     //Day
//     if (this_device_settings.real_time_clock.day == 30)
//     {
//         this_device_settings.real_time_clock.day = 0;
//         this_device_settings.real_time_clock.month++;
//     }
//     portENTER_CRITICAL(&mux);
//     print_time = true;
//     portEXIT_CRITICAL(&mux);
// }

// void IRAM_ATTR updateTimeSleep()
// {
//     //Minutes
//     this_device_settings.real_time_clock.minute++;
//     if (this_device_settings.real_time_clock.minute == 60)
//     {
//         this_device_settings.real_time_clock.minute = 0;
//         this_device_settings.real_time_clock.hour++;
//     }

//     //Hour
//     if (this_device_settings.real_time_clock.hour == 24)
//     {

//         this_device_settings.real_time_clock.hour = 0;
//         this_device_settings.real_time_clock.day++;
//     }

//     //Day
//     if (this_device_settings.real_time_clock.day == 30)
//     {
//         this_device_settings.real_time_clock.day = 0;
//         this_device_settings.real_time_clock.month++;
//     }
//     portENTER_CRITICAL(&mux);
//     print_time = true;
//     portEXIT_CRITICAL(&mux);
// }

// void alarmController()
// {
//     // Update time as necessary
//     Serial.print("[RTC] Deep Sleep Counter:");

//     updateTimeSleep();

//     esp_sleep_wakeup_cause_t wakeup_reason;
//     wakeup_reason = esp_sleep_get_wakeup_cause();

//     // -- Print wake up reason
//     if (serial_debugging_enabled)
//     {
//         switch (wakeup_reason)
//         {
//         case ESP_SLEEP_WAKEUP_EXT0:
//             Serial.println("[RTC]  Wakeup caused by external signal using Power Button");
//             break;
//         case ESP_SLEEP_WAKEUP_EXT1:
//             Serial.println("[RTC]  Wakeup caused by external signal using RTC_CNTL");
//             break;
//         case ESP_SLEEP_WAKEUP_TIMER:
//             Serial.println("[RTC]  Wakeup caused by timer");
//             break;
//         case ESP_SLEEP_WAKEUP_TOUCHPAD:
//             Serial.println("[RTC]  Wakeup caused by touchpad");
//             break;
//         case ESP_SLEEP_WAKEUP_ULP:
//             Serial.println("[RTC]  Wakeup caused by ULP program");
//             break;
//         default:
//             Serial.printf("[RTC]  Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
//             break;
//         }
//     }

//     bool wakeup = myPatient.checkAlarms(this_device_settings.real_time_clock);

//     if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
//     {
//         if (wakeup)
//         {
//             Serial.println("[RTC]  Dispense Alarm Triggered");
//             dispense_prescription = true;
//             //pill_locker_sleeping = false;
//         }
//         else
//         {
//             Serial.println("[RTC]  No alarm detected. ESP going back to sleep");
//             pill_locker_sleeping = true;
//             setPinsForDeepSleep();
//             esp_deep_sleep_start();
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