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

// const bool serial_debugging_enabled = true;
// RTC_DATA_ATTR bool first_boot = true;

// //Set time again after battery loss
// bool set_time = true;

// //Instructions
// bool unload_instruction = false;
// bool load_instruction = false;
// bool dispense_instruction = false;
// bool use_dummy_arrays = false;

// //************************************************************DEFINTIONS*****************************************************************//

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

// //DUAL CORE
// static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// static TaskHandle_t Core0;
// static TaskHandle_t Core1;

// //INTERRUPTS
// hw_timer_t *timer = NULL;

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

// RTC_DATA_ATTR realTimeClock myRealTimeClock;
// RTC_DATA_ATTR realTimeClock dummy_time_arrays[NUMBER_OF_DUMMY_ARRAYS];

// //PATIENT DATA
// Patient myPatient = Patient(myRealTimeClock);

// //BLE
// BLEServer *pServer = NULL;
// BLECharacteristic *pTxCharacteristic;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;

// //*******************************************************FUNCTION DECLARATION************************************************************//

// // FLASH STORAGE
// void initStorage();

// //INSTRUCTION FUNCTIONS
// void unloadInstruction();
// void loadInstruction();
// void dispenseInstruction();

// //INITIAL SETUP
// void initialSetup();

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
// void IRAM_ATTR updateTimeMinutes();
// void alarmController();

// //BUZZER
// void initBuzzer();
// void soundBuzzer();

// //BUTTONS
// void initButtons();

// //SCREEN
// void initScreen();

// //BLE
// void initBLE();
// void handleBluetoothMessage(std::string data);
// void connectionManager();

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
//         handleBluetoothMessage(rxValue);
//     }
// };

// //***********************************************************INTERRUPTS******************************************************************//

// //RTC
// void IRAM_ATTR globalTimeKeeperInterrupt()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     updateTimeSeconds();
//     portEXIT_CRITICAL_ISR(&mux);
// }

// //POWER
// void IRAM_ATTR batterySaver()
// {
//     portENTER_CRITICAL_ISR(&mux);
//     Serial.println("[POW]  Device going to sleep to reduce power consumption");
//     pill_locker_sleeping = true;
//     esp_deep_sleep_start();
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
//     Serial.begin(115200);

//     //Wait for Serial
//     if (first_boot)
//     {
//         while (!Serial)
//             ;
//         delay(100);
//     }

//     //FLASH STORAGE
//     initStorage();

//     //RTC
//     initRTC();

//     //Increment time and check if time to wake up
//     // 2202 microseconds to here
//     if (pill_locker_sleeping)
//     {
//         alarmController();
//     }

//     //BATTERY
//     initBattery();

//     //MOTORS
//     initMotors();

//     //BUZZER
//     initBuzzer();

//     //BUTTONS
//     initButtons();

//     //SCREEN
//     initScreen();

//     //  --  SOFTWARE INTERRUPTS:
//     //RTC
//     timer = timerBegin(0, 80, true);
//     timerAttachInterrupt(timer, &globalTimeKeeperInterrupt, true);
//     timerAlarmWrite(timer, 1000000, true); //microseconds (1s)
//     timerAlarmEnable(timer);               //Initialize Timer

//     //BATTERY
//     timer = timerBegin(1, 80, true);
//     timerAttachInterrupt(timer, &batterySaver, true);
//     timerAlarmWrite(timer, TIME_TO_POWER_SAVE_START * uS_TO_S_FACTOR, true); //microseconds (1s)
//     timerAlarmEnable(timer);                                                 //Initialize Timer

//     //  --   HARDWARE INTERRUPTS:
//     //POWER
//     //attachInterrupt(digitalPinToInterrupt(VBAT_3V3), batteryDisconnected, FALLING);

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

//     //BLE
//     initBLE();
// }

// //**************************************************************CORE 0*******************************************************************//
// void core0Task(void *pvParameters)
// {
//     for (;;)
//     {
//         if (unload_instruction)
//         {
//             unloadInstruction();
//             unload_instruction = false;
//         }

//         if (load_instruction)
//         {
//             loadInstruction();
//             load_instruction = false;
//         }

//         if (dispense_instruction)
//         {
//             dispenseInstruction();
//             dispense_instruction = false;
//         }

//         if (dispense_instruction == false && unload_instruction == false && load_instruction == false)
//         {
//             delay(200);
//         }

//         // --  Print buttons
//         // Serial.print(digitalRead(POW_BUTTON));
//         // Serial.print(digitalRead(UP_BUTTON));
//         // Serial.print(digitalRead(DOWN_BUTTON));
//         // Serial.println(digitalRead(OK_BUTTON));

//         // --  Print Time
//         if (serial_debugging_enabled)
//         {
//             printTime();
//         }

//         delay(100);

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
//     NVS.begin();

//     //Check if it's first time being programmed

//     if (first_boot == true)
//     {
//         NVS.eraseAll();
//     }

//     uint64_t unit_programmed = NVS.getInt("unit_programmed");
//     if (unit_programmed == 0)
//     {
//         if (serial_debugging_enabled)
//         {
//             delay(1000);
//             Serial.println("PILL LOCKER REV 3.1");
//             Serial.println("Booting...");
//             Serial.println();
//             Serial.println("[RTC]  Flash Memory Erased");
//             NVS.eraseAll();
//             NVS.setInt("unit_programmed", 1);
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
//     Serial.println("Excecuting dispense instruction");
//     tft.setCursor(20, 20); //width, height
//     tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//     tft.setTextSize(2);

//     tft.println("Dispensing");
//     delay(2000);
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

// void handleBluetoothMessage(std::string data)
// {
//     if (data.length() > 0)
//     {
//         int len = data.length();
//         Serial.print("Data: ");

//         for (int i = 0; i < len; i++)
//         {
//             Serial.print(data[i]);
//         }
//         Serial.println();

//         switch (data[0])
//         {
//         case 'U':
//             Serial.println("Instruction 1: Unlaod Pills");
//             portENTER_CRITICAL_ISR(&mux);
//             unload_instruction = true;
//             portEXIT_CRITICAL_ISR(&mux);
//             break;
//         case 'L':
//             Serial.println("Instruction 2: Load Pills");
//             portENTER_CRITICAL_ISR(&mux);
//             load_instruction = true;
//             portEXIT_CRITICAL_ISR(&mux);
//             break;
//         case 'D':
//             Serial.println("Instruction 3: Dispense Pills");
//             portENTER_CRITICAL_ISR(&mux);
//             dispense_instruction = true;
//             portEXIT_CRITICAL_ISR(&mux);
//             break;
//         default:
//             break;
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
//         Serial.println("[BLE]  BLE Service is connected.");
//         oldDeviceConnected = deviceConnected;
//     }
// }

// //INITIAL SETUP
// void initialSetup()
// {
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
//     //Increment boot number and print it every reboot
//     if (sleep_minutes)
//     {
//         esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_uS_M);
//     }
//     else
//     {
//         esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_uS_S);
//     }

//     // -- Print wake up reason
//     if (serial_debugging_enabled)
//     {
//         esp_sleep_wakeup_cause_t wakeup_reason;
//         wakeup_reason = esp_sleep_get_wakeup_cause();

//         switch (wakeup_reason)
//         {
//         case ESP_SLEEP_WAKEUP_EXT0:
//             Serial.println("[RTC]  Wakeup caused by external signal using RTC_IO");
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

//     // Set Compiler Time only when re-programmed
//     if (first_boot)
//     {
//         //create dummy arrays
//         global_time = DateTime(__DATE__, __TIME__);
//         myRealTimeClock.year = global_time.year();
//         myRealTimeClock.month = global_time.month();
//         myRealTimeClock.day = global_time.day();
//         myRealTimeClock.hour = global_time.hour();
//         myRealTimeClock.minute = global_time.minute();
//         myRealTimeClock.second = global_time.second();

//         //Setting dummy arrays
//         if (use_dummy_arrays)
//         {
//             if (sleep_minutes)
//             {
//                 for (int i = 0; i < NUMBER_OF_DUMMY_ARRAYS; i++)
//                 {
//                     dummy_time_arrays[i] = myRealTimeClock;
//                     dummy_time_arrays[i].minute = dummy_time_arrays[i].minute + (i + 2);
//                     if (serial_debugging_enabled)
//                     {
//                         Serial.print("[RTC]  Dummy Alarm set to: ");
//                         Serial.print(dummy_time_arrays[i].year);
//                         Serial.print("/");
//                         Serial.print(dummy_time_arrays[i].month);
//                         Serial.print("/");
//                         Serial.print(dummy_time_arrays[i].day);
//                         Serial.print("  ");
//                         Serial.print(dummy_time_arrays[i].hour);
//                         Serial.print(":");
//                         Serial.print(dummy_time_arrays[i].minute);
//                         Serial.print(":");
//                         Serial.println(dummy_time_arrays[i].second);
//                     }
//                 }
//             }
//             else
//             {
//                 for (int i = 0; i < NUMBER_OF_DUMMY_ARRAYS; i++)
//                 {
//                     dummy_time_arrays[i] = myRealTimeClock;
//                     dummy_time_arrays[i].minute = dummy_time_arrays[i].second + (i + 2);
//                     if (serial_debugging_enabled)
//                     {
//                         Serial.print("[RTC]  Dummy Alarm set to: ");
//                         Serial.print(dummy_time_arrays[i].year);
//                         Serial.print("/");
//                         Serial.print(dummy_time_arrays[i].month);
//                         Serial.print("/");
//                         Serial.print(dummy_time_arrays[i].day);
//                         Serial.print("  ");
//                         Serial.print(dummy_time_arrays[i].hour);
//                         Serial.print(":");
//                         Serial.print(dummy_time_arrays[i].minute);
//                         Serial.print(":");
//                         Serial.println(dummy_time_arrays[i].second);
//                     }
//                 }
//             }
//         }
//         else
//         {
//             myPatient = Patient(myRealTimeClock);
//             myPatient.printPrescriptionAlarms();
//         }

//         first_boot = false;
//     }
// }

// void printTime()
// {
//     portENTER_CRITICAL(&mux);
//     Serial.print("[RTC]  ");
//     Serial.print(myRealTimeClock.year);
//     Serial.print("/");
//     Serial.print(myRealTimeClock.month);
//     Serial.print("/");
//     Serial.print(myRealTimeClock.day);
//     Serial.print("  ");
//     Serial.print(myRealTimeClock.hour);
//     Serial.print(":");
//     Serial.print(myRealTimeClock.minute);
//     Serial.print(":");
//     Serial.println(myRealTimeClock.second);
//     portEXIT_CRITICAL(&mux);
// }

// void IRAM_ATTR updateTimeSeconds()
// {
//     //Seconds
//     myRealTimeClock.second++;
//     if (myRealTimeClock.second == 60)
//     {
//         myRealTimeClock.second = 0;
//         myRealTimeClock.minute++;
//     }

//     //Minutes
//     if (myRealTimeClock.minute == 60)
//     {
//         myRealTimeClock.minute = 0;
//         myRealTimeClock.hour++;
//     }

//     //Hour
//     if (myRealTimeClock.hour == 24)
//     {

//         myRealTimeClock.hour = 0;
//         myRealTimeClock.day++;
//     }

//     //Day
//     if (myRealTimeClock.day == 30)
//     {
//         myRealTimeClock.day = 0;
//         myRealTimeClock.month++;
//     }
// }

// void IRAM_ATTR updateTimeMinutes()
// {
//     //Minutes
//     myRealTimeClock.minute++;
//     if (myRealTimeClock.minute == 60)
//     {
//         myRealTimeClock.minute = 0;
//         myRealTimeClock.hour++;
//     }

//     //Hour
//     if (myRealTimeClock.hour == 24)
//     {

//         myRealTimeClock.hour = 0;
//         myRealTimeClock.day++;
//     }

//     //Day
//     if (myRealTimeClock.day == 30)
//     {
//         myRealTimeClock.day = 0;
//         myRealTimeClock.month++;
//     }
// }

// void alarmController()
// {
//     //Fucntion takes appx. 175 usec
//     // Update time as necessary
//     if (sleep_minutes)
//     {
//         updateTimeMinutes();
//     }
//     else
//     {
//         updateTimeSeconds();
//     }

//     printTime();

//     //DateTime _now = myRealTimeClock.getDateTimeObject();

//     bool wakeup = false;

//     // Set dummy wake up arrays just for testing
//     if (use_dummy_arrays)
//     {
//         if (sleep_minutes)
//         {
//             for (int i = 0; i < NUMBER_OF_DUMMY_ARRAYS; i++)
//             {
//                 if (myRealTimeClock.minute == dummy_time_arrays[i].minute)
//                 {
//                     if (myRealTimeClock.hour == dummy_time_arrays[i].hour)
//                     {
//                         wakeup = true;
//                     }
//                 }
//             }
//         }
//         else
//         {
//             for (int i = 0; i < NUMBER_OF_DUMMY_ARRAYS; i++)
//             {
//                 if (myRealTimeClock.second == dummy_time_arrays[i].second)
//                 {
//                     if (myRealTimeClock.minute == dummy_time_arrays[i].minute)
//                     {
//                         if (myRealTimeClock.hour == dummy_time_arrays[i].hour)
//                         {
//                             wakeup = true;
//                         }
//                     }
//                 }
//             }
//         }
//     }
//     else
//     {
//         wakeup = myPatient.checkAlarms(myRealTimeClock);
//     }

//     if (wakeup)
//     {
//         Serial.println("[RTC]  Dispense Alarm Triggered");
//         //pill_locker_sleeping = false;
//     }
//     else
//     {
//         Serial.println("[RTC]  No alarm detected. ESP going back to sleep");
//         pill_locker_sleeping = true;
//         esp_deep_sleep_start();
//     }
// }

// //BUTTONS
// void initButtons()
// {
//     //ACTIVE LOW
//     pinMode(POW_BUTTON, INPUT);
//     pinMode(UP_BUTTON, INPUT);
//     pinMode(DOWN_BUTTON, INPUT);
//     pinMode(OK_BUTTON, INPUT);
// }

// //BUZZER
// void initBuzzer()
// {
//     ledcAttachPin(AUDIO, 0);
//     ledcSetup(0, 4000, 8);
// }

// void soundBuzzer()
// {
//     for (int i = 0; i < 3; i++)
//     {
//         for (int i = 0; i < 5; i++)
//         {
//             ledcWrite(0, 128);
//             delay(100);
//             ledcWrite(0, 0);
//             delay(100);
//         }
//         delay(500);
//     }
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