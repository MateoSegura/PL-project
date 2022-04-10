//*********************************************************DEFAULT SETTINGS**************************************************************//
#define FIRMWARE_VERSION 10

#define DEFAULT_DEMO_MODE_DISPENSING_INTERVAL_S 10
#define DEFAULT_ACTIVATED_MODE_DISPENSING_INTERVAL_H 4  // = 4 H
#define DEFAULT_ACTIVATED_MODE_DISPENSING_INTERVAL_M 10 // = 10 M

#define DEFAULT_PILL_SLOTS_PER_TRAY 12

#define DEFAULT_POCKET_STEPS_PER_PILL 1135
#define DEFAULT_MOTOR_RPM 2000

#define DEFAULT_PILLS_PER_TRAY 7
#define DEFAULT_PILLS_TO_DISPENSE 4
#define DEFAULT_MISSED_PILLS 0
#define DEFAULT_ACTIVATION_H 7 // 24 hour calender. Default is 7
#define NUMBER_OF_TRAYS 3      // Max. is 3

#define OUT_DIRECTION 1
#define IN_DIRECTION -1

#define DEFAULT_TIME_TO_POWER_SAVE_START_D 8
#define DEFAULT_TIME_TO_POWER_SAVE_START_A 60

#define NUMBER_OF_STEPS_MOVED_IN_TEST_MODE 40

//*********************************************************RUN TIME VARIABLES**************************************************************//
#define uS_TO_S_FACTOR 1000000

#define SWITCH_DEBOUNCE_TIME_mS 200

// PILL LOCKER SETTINGS

// FLASH STORAGE
// MOTORS
#define MOTOR_STEPS 80
#define MICROSTEPS 1
#define LOAD_RPM 1000
#define STEP 27
#define DIR 12
#define MOTORSLEEP1 33
#define MOTORSLEEP2 25
#define MOTORSLEEP3 26

// SCREEN
#define TFT_CS 15
#define TFT_RST 13
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_DC 14
#define LEDK 4

// BUTTONS
#define POW_BUTTON 35
#define UP_BUTTON 34
#define DOWN_BUTTON 39
#define OK_BUTTON 36

// BUZZER
#define AUDIO 19

// POWER MANAGEMENT
#define VBAT_3V3 32
#define REG_STAT 5
#define BAT_STAT1 16
#define BAT_STAT2 17
#define WAKE_UP 2

// BLE Service Characteristics
#define BLENAME "PILL LOCKER UNIT 1"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// The following are used to eliminate the delay in the sound Buzzer function
#define BUZZER_TIME_INTERVAL 100 // ms
#define BUZZER_REPETITIONS 5     // times