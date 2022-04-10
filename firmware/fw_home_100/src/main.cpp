/*
 * Company: Pill Locker Inc.
 * Name of Project: Pill Locker Pocket Unit
 * Version: 1.1
 * Compartible Hardware: REV3.2
 * Date Created: Monday August 9, 2021
 * Last Modified: Monday August 9, 2021
 */
//************************************************************LIBRARIES******************************************************************//
#include "settings.h"

//*******************************************************FUNCTION DECLARATION************************************************************//
// DEVICE MODES LOGIC
void ActivatedMode();
void TestMode();
void DemoMode();
void UtilitiesMode();
void InactiveMode();

// FLASH STORAGE
void initStorage();

// INSTRUCTION FUNCTIONS
void utilitiesModeHandler(std::string instruction);
void activatedModeHandler();
void testModeHandler(std::string instruction);
void demoModeHandler();

void closeInstruction();
void loadInstruction();
void dispenseInstruction();

// INITIAL SETUP
void setPinsForDeepSleep();

// BATTERY
void initBattery();

// DEEP SLEEP
void startSleepTimer();

// DUAL CORE
void core0Task(void *pvParameters);
void core1Task(void *pvParameters);

// MOTORS
void initMotors();
void moveMotor(int motor, int direction, int steps);

// RTC
void initRTC();
void updateTime();
void IRAM_ATTR updateTimeSeconds();
void IRAM_ATTR updateTimeSleep();
void alarmController();
void adjustTime(std::string data);

// BUZZER
void initBuzzer();
void soundBuzzer();

// BUTTONS
void initButtons();
void buttonHandler();

void initScreen();
void updateScreen();
void mainMenuScreenManager();
void activeModeScreenManager();
void testModeScreenManager();
void utilitiesModeScreenManager();
void demoModeScreenManager();
void sleepWarningScreenManager();

// BLE
struct BluetoothInstruction
{
    // Restart device
    bool restart_device = false;

    // Device Modes
    bool active_mode_request = false;
    bool test_mode_request = false;
    bool utilities_mode_request = false;
    bool demo_mode_request = false;

    // Exit demo mode
    bool exit_demo_mode_request = false;

    // Device Display Info request
    bool send_status_message = false;
    bool send_current_settings = false;

    // Pass possible complex instruction to CPU core for processing
    bool complex_instruction_request = false;
    std::string complex_instruction_string;

    // Step count for test mode
    uint16_t test_step_count;
    bool exit_move_motor_mode = false;

    // Update time
    bool update_time = false;

    // Send out Log info
    bool send_log_info = false;

} myBluetoothInstructions;

void initBLE();
void IRAM_ATTR adjustTimeRequest();
void connectionManager();
void sendBLEMessage(String data);
void assignBluetoothFlag(std::string data);
void bluetothInstructionHandlerCore0();
void bluetothInstructionHandlerCore1();

// Interrupts
void IRAM_ATTR batterySaverStart();

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        // This has to be here in order for the screen manager to not display the warnign message on disconnect. FIX!!!!!!!!
        screen_flags.skip_screen = true;
        timerRestart(battery_timer);
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();
        assignBluetoothFlag(rxValue);
    }
};

//***********************************************************INTERRUPTS******************************************************************//
// RTC

void IRAM_ATTR globalTimeKeeperInterrupt()
{
    portENTER_CRITICAL_ISR(&mux);
    update_time = true;
    portEXIT_CRITICAL_ISR(&mux);
}

// POWER
void IRAM_ATTR batterySaverStart()
{
    portENTER_CRITICAL_ISR(&mux);
    bool sleep = false;
    if ((this_device_settings.current_device_mode[0] == 'D') || (this_device_settings.current_device_mode[0] == 'A'))
    {
        sleep = true;
    }
    else if (!deviceConnected)
    {
        sleep = true;
    }

    if (sleep)
    {
        if (serial_debugging_enabled)
        {
            Serial.println("[POW]  Device going to sleep to reduce power consumption");
        }
        pill_locker_sleeping = true;
        setPinsForDeepSleep();
        esp_deep_sleep_start();
    }
    portEXIT_CRITICAL_ISR(&mux);
}

// Does not work
void IRAM_ATTR batteryDisconnected()
{
    portENTER_CRITICAL_ISR(&mux);
    Serial.println("[POW]  rtc int detected");
    portEXIT_CRITICAL_ISR(&mux);
}

//**************************************************************SETUP********************************************************************//
void setup()
{
    long initial_time = micros();

    // SERIAL
    if (serial_debugging_enabled)
    {
        Serial.begin(115200);
        Serial.setDebugOutput(0);
    }

    // Wait for Serial
    if (first_boot)
    {
        if (serial_debugging_enabled)
        {
            while (!Serial)
                ;
            delay(2000);
        }
    }

    // FLASH STORAGE
    initStorage();

    // BATTERY
    if (this_device_settings.current_device_mode[0] == '0')
    {
        battery_timer = timerBegin(1, 80, true);
        timerAttachInterrupt(battery_timer, &batterySaverStart, true);
        timerAlarmWrite(battery_timer, this_device_settings.time_to_sleep_time_a * uS_TO_S_FACTOR, true); // microseconds (1s)
    }
    else if (this_device_settings.current_device_mode[0] == 'D')
    {
        battery_timer = timerBegin(1, 80, true);
        timerAttachInterrupt(battery_timer, &batterySaverStart, true);
        timerAlarmWrite(battery_timer, this_device_settings.time_to_sleep_time_d * uS_TO_S_FACTOR, true); // microseconds (1s)
                                                                                                          // timerStop(battery_timer);
        // timerAlarmWrite(battery_timer, this_device_settings.time_to_sleep_time_d * uS_TO_S_FACTOR, true); //microseconds (1s)
        //  timerStop(battery_timer);
    }

    // MOTORS
    initMotors();

    // RTC
    initRTC();

    // ALARMS
    alarmController();

    // BATTERY
    initBattery();

    // BUZZER
    initBuzzer();

    // -- BUTTONS
    initButtons();

    //  --  SOFTWARE INTERRUPTS:
    // RTC
    real_time_clock_timer = timerBegin(0, 80, true);
    timerAttachInterrupt(real_time_clock_timer, &globalTimeKeeperInterrupt, true);
    timerAlarmWrite(real_time_clock_timer, 1000000, true); // microseconds (1s)

    //  --   HARDWARE INTERRUPTS:
    // POWER
    // pinMode(2, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(2), batteryDisconnected, FALLING);

    // SCREEN
    initScreen();

    // -- BLE
    initBLE();

    // -- DUAL CORE SETUP
    disableCore0WDT();
    // disableCore1WDT();

    // -- DUAL CORE SETUP
    xTaskCreatePinnedToCore(
        core0Task, /* Task function. */
        "Task0",   /* name of task. */
        10000,     /* Stack size of task in Memory */
        NULL,      /* parameter of the task */
        1,         /* priority of the task: 0 to 25, 25 being the highest*/
        &Core0,    /* Task handle to keep track of created task */
        0);        /* pin task to core 0 */

    xTaskCreatePinnedToCore(
        core1Task, /* Task function. */
        "Task1",   /* name of task. */
        10000,     /* Stack size of task */
        NULL,      /* parameter of the task */
        1,         /* priority of the task */
        &Core1,    /* Task handle to keep track of created task */
        1);        /* pin task to core 1 */

    // -- Enable Timer Interrupts
    timerAlarmEnable(real_time_clock_timer);

    if (this_device_settings.current_device_mode[0] == '0')
    {
        timerAlarmEnable(battery_timer);
    }

    Serial.print("[DEB]  Boot time: ");
    Serial.print((micros() - initial_time) / 1000);
    Serial.println(" ms");
}

//**************************************************************CORE 0*******************************************************************//
void core1Task(void *pvParameters)
{
    for (;;)
    {
        // -- BLE Connection Manager
        connectionManager();

        // -- Update TFT
        updateScreen();

        // -- Handle Simple Bluetooth Instructions
        bluetothInstructionHandlerCore1();

        // --  Print Time
        updateTime();

        // -- Brute force buttons if interrupt doesn work
        buttonHandler();

        delay(100);

        // -- print buttons
        // Serial.print("[POW]  POW: ");
        // Serial.print(digitalRead(POW_BUTTON));
        // Serial.print("  ,UP: ");
        // Serial.print(digitalRead(UP_BUTTON));
        // Serial.print("  ,DOWN: ");
        // Serial.print(digitalRead(DOWN_BUTTON));
        // Serial.print("  ,OK: ");
        // Serial.println(digitalRead(OK_BUTTON));

        // Read Alarm Flag
        //  if (rtc.readAlarmInterruptFlag())
        //  {
        //      Serial.println("ALARM!!!!");
        //      rtc.clearAlarmInterruptFlag();
        //  }

        // String var = myNVS.getString("dev_mod");
        // Serial.println(var);

        // for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        // {
        //     uint16_t x = myNVS.getInt("act_intvl_s_" + i);
        //     Serial.println(x);
        // }

        // Force deep sleep
        // pill_locker_sleeping = true;
        // esp_deep_sleep_start();
        yield();
    }
}

//**************************************************************CORE 1*******************************************************************//
void core0Task(void *pvParameters)
{
    for (;;)
    {
        // //Get device mode
        // portENTER_CRITICAL(&mux);
        // char mode = this_device_settings.current_device_mode[0];
        // portEXIT_CRITICAL(&mux);

        // -- Handle Complex Bluetooth Instructions
        bluetothInstructionHandlerCore0();
    }
}

//***************************************************************LOOP********************************************************************//
void loop()
{
    yield();
}

//*******************************************************FUNCTION DEFINITION************************************************************//
// INSTRUCTIONS
void utilitiesModeHandler(std::string instruction)
{
    // No instruction received
    int instruction_length = instruction.length();
    if (instruction_length > 0)
    {
        if (serial_debugging_enabled)
        {
            Serial.print("[UTI]  Attempting to decode complex instruction: ");
            Serial.print(instruction.c_str());
            Serial.print(" ,of length ");
            Serial.println(instruction_length);
        }

        // flag for valid instruction
        bool valid_instruction = true;

        // Use fold level 4 to see all instructions below neatly
        switch (instruction[0])
        {
        // Change demo interval:
        case 'D':
            if (instruction[1] == 'I' && instruction_length < 6)
            {
                std::string interval_string;

                // Check if instruction contains only numbers.
                for (int i = 2; i < instruction_length; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        interval_string += instruction[i];
                    }
                }

                // Decode instruction
                if (valid_instruction)
                {
                    // Convert characters to integer
                    uint16_t temp = atoi(interval_string.c_str());

                    // Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
                    portENTER_CRITICAL(&mux);
                    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                    {
                        this_device_settings.demo_dispense_interval_s[i] = temp;
                    }
                    portEXIT_CRITICAL(&mux);

                    if (serial_debugging_enabled)
                    {
                        Serial.println("[UTI]  Valid Instrution");
                        Serial.print("[UTI]  Changing demo interval to: ");
                        Serial.print(temp);
                        Serial.println(" s");
                    }

                    // Send BLE message to indicate change
                    String ble_message = "Demo Interval Changed to: ";
                    ble_message += temp;
                    ble_message += " s";
                    sendBLEMessage(ble_message);
                }
            }
            else
            {
                valid_instruction = false;
            }
            break;

        // Set the number of pills per tray
        case 'P':
            if (instruction[1] == 'T' && instruction_length < 6)
            {
                std::string interval_string;

                // Check if instruction contains only numbers.
                for (int i = 2; i < instruction_length; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        interval_string += instruction[i];
                    }
                }

                // Decode instruction
                if (valid_instruction)
                {
                    // Convert characters to integer
                    uint16_t temp = atoi(interval_string.c_str());

                    // Check if temp is within limits
                    portENTER_CRITICAL(&mux);

                    bool within_limts = true;

                    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                    {
                        if (temp > this_device_settings.pill_slots_per_tray[i])
                        {
                            within_limts = false;
                        }
                    }

                    portEXIT_CRITICAL(&mux);

                    // Change values if within limits
                    if (within_limts)
                    {
                        portENTER_CRITICAL(&mux);
                        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                        {
                            this_device_settings.loaded_pills_per_tray[i] = temp;
                            this_device_settings.dosages_remaining[i] = temp;
                        }
                        portEXIT_CRITICAL(&mux);

                        if (serial_debugging_enabled)
                        {
                            Serial.println("[UTI]  Valid Instrution");
                            Serial.print("[UTI]  Changing pills per tray to: ");
                            Serial.print(temp);
                            Serial.println(" pills");
                        }

                        // Send BLE message to indicate change
                        String ble_message = "Loaded Pills per tray changed to: ";
                        ble_message += temp;
                        ble_message += " pills";
                        sendBLEMessage(ble_message);
                    }
                    else
                    {
                        // Indicate User
                        sendBLEMessage("Value is out of range");
                        sendBLEMessage("Make sure the loaded pills setting is within the following ranges:");

                        if (serial_debugging_enabled)
                        {
                            Serial.println("[UTI]  Value is out of range");
                            Serial.println("[UTI]  Make sure the loaded pills setting is within the following ranges:");
                        }

                        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                        {
                            String temp = "Tray ";
                            temp += i + 1;
                            temp += ": ";
                            temp += this_device_settings.pill_slots_per_tray[i];
                            temp += " pill slots";
                            if (serial_debugging_enabled)
                            {
                                Serial.println("[UTI]  " + temp);
                            }
                            sendBLEMessage(temp);
                        }
                    }
                }
            }
            else
            {
                valid_instruction = false;
            }
            break;

        // Set the steps per motor & motor speed
        case 'M':
            if (instruction[1] == 'S' && instruction_length < 8)
            {
                std::string interval_string;

                // Check if instruction contains only numbers.
                for (int i = 2; i < instruction_length; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        interval_string += instruction[i];
                    }
                }

                // Decode instruction
                if (valid_instruction)
                {
                    // Convert characters to integer
                    uint16_t temp = atoi(interval_string.c_str());

                    // Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
                    portENTER_CRITICAL(&mux);
                    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                    {
                        this_device_settings.motor_steps_per_slot[i] = temp;
                    }
                    portEXIT_CRITICAL(&mux);

                    if (serial_debugging_enabled)
                    {
                        Serial.println("[UTI]  Valid Instrution");
                        Serial.print("[UTI]  Changing steps per pill to: ");
                        Serial.print(temp);
                        Serial.println(" steps");
                    }

                    // Send BLE message to indicate change
                    String ble_message = "Steps per pill changed to: ";
                    ble_message += temp;
                    ble_message += " steps";
                    sendBLEMessage(ble_message);
                }
            }
            // Motor Speed
            else if (instruction[1] == 'O' && instruction[2] == 'S' && instruction_length < 8)
            {
                std::string speed_string;

                // Check if instruction contains only numbers.
                for (int i = 3; i < instruction_length; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        speed_string += instruction[i];
                    }
                }

                // Decode instruction
                if (valid_instruction)
                {
                    // Convert characters to integer
                    uint16_t speed = atoi(speed_string.c_str());

                    // Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
                    portENTER_CRITICAL(&mux);
                    this_device_settings.motor_speed = speed;
                    portEXIT_CRITICAL(&mux);

                    if (serial_debugging_enabled)
                    {
                        Serial.println("[UTI]  Valid Instrution");
                        Serial.print("[UTI]  Changing motor speed to: ");
                        Serial.print(speed);
                        Serial.println(" rev/min");
                    }

                    // Send BLE message to indicate change
                    String ble_message = "Changing motor speed to: ";
                    ble_message += speed;
                    ble_message += " rev/min";
                    sendBLEMessage(ble_message);
                }
                else
                {
                    valid_instruction = false;
                }
            }
            else
            {
                valid_instruction = false;
            }
            break;

        // Set Date/time
        case 'T':
            if (instruction[1] == 'D' && instruction_length == 20)
            {
                // Create temporary strings
                std::string month_string, day_string, year_string, hour_string, minute_string, second_string;

                // -- Check if instruction contains only numbers.
                // Month
                for (int i = 3; i < 5; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        month_string += instruction[i];
                    }
                }

                // Day
                for (int i = 6; i < 8; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        day_string += instruction[i];
                    }
                }

                // Year
                for (int i = 9; i < 11; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        year_string += instruction[i];
                    }
                }

                // Hour
                for (int i = 12; i < 14; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        hour_string += instruction[i];
                    }
                }

                // Minute
                for (int i = 15; i < 17; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        minute_string += instruction[i];
                    }
                }

                // Second
                for (int i = 18; i < 20; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        second_string += instruction[i];
                    }
                }

                // Decode instruction
                if (valid_instruction)
                {
                    // Convert characters to integer
                    uint16_t temp_year = atoi(year_string.c_str());
                    uint8_t temp_month = atoi(month_string.c_str());
                    uint8_t temp_day = atoi(day_string.c_str());

                    uint8_t temp_hour = atoi(hour_string.c_str());
                    uint8_t temp_minute = atoi(minute_string.c_str());
                    uint8_t temp_second = atoi(second_string.c_str());

                    // There's an error with rtc if second is set to 0
                    if (temp_second < 10)
                    {
                        temp_second = 10;
                    }

                    // Check if values are within range
                    bool within_range = false;
                    if (temp_year <= 2021)
                    {
                        if (temp_month <= 12)
                        {
                            if (temp_day <= 31)
                            {
                                if (temp_hour < 24)
                                {
                                    if (temp_minute < 60)
                                    {
                                        if (temp_second < 60)
                                        {
                                            within_range = true;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (within_range)
                    {
                        // Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
                        portENTER_CRITICAL(&mux);
                        myBluetoothInstructions.update_time = true;
                        this_device_settings.newTime = DateTime(temp_year, temp_month, temp_day, temp_hour, temp_minute, temp_second);
                        String print_temp = this_device_settings.newTime.timestamp();
                        portEXIT_CRITICAL(&mux);

                        if (serial_debugging_enabled)
                        {
                            Serial.println("[UTI]  Valid Instrution");
                            Serial.print("[UTI]  Changing device time to: ");
                            Serial.println(print_temp);
                        }

                        // Send BLE message to indicate change
                        String ble_message = "New Device Time: ";
                        ble_message += print_temp;
                        sendBLEMessage(ble_message);
                    }
                    else
                    {
                        if (serial_debugging_enabled)
                        {
                            Serial.println("[UTI]  Unvalid Date time");
                        }

                        // Send BLE message to indicate error
                        sendBLEMessage("Unvalid Date time");
                    }
                }
            }
            else
            {
                valid_instruction = false;
            }
            break;

        // Activated Mode Dispense Interval or RESET
        case 'R':
        {
            int reset = instruction.compare("RESET");
            if (reset == 0)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[UTI]  Unit will be reset, and set to default settings in 5 seconds...");
                }

                // Send BLE message to indicate reset
                sendBLEMessage("Unit will be reset, and set to default settings in 5 seconds...");

                delay(5000);
                first_boot = true;
                ESP.restart();
            }
            else
            {
                if (instruction[1] == 'I' && instruction_length == 7)
                {
                    // Create temporary strings
                    std::string hour_string, minute_string;

                    // -- Check if instruction contains only numbers.
                    // Hour
                    for (int i = 2; i < 4; i++)
                    {
                        if (!isdigit(instruction[i]))
                        {
                            valid_instruction = false;
                            break;
                        }
                        else
                        {
                            hour_string += instruction[i];
                        }
                    }

                    // Minute
                    for (int i = 5; i < 7; i++)
                    {
                        if (!isdigit(instruction[i]))
                        {
                            valid_instruction = false;
                            break;
                        }
                        else
                        {
                            minute_string += instruction[i];
                        }
                    }

                    // Decode instruction
                    if (valid_instruction)
                    {
                        // Convert characters to integer
                        uint8_t temp_hour = atoi(hour_string.c_str());
                        uint8_t temp_minute = atoi(minute_string.c_str());

                        // Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
                        if (temp_hour < 24 && temp_minute < 60)
                        {
                            portENTER_CRITICAL(&mux);
                            for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                            {
                                this_device_settings.activated_dispense_interval_h[i] = temp_hour;
                                this_device_settings.activated_dispense_interval_m[i] = temp_minute;
                            }
                            portEXIT_CRITICAL(&mux);

                            //  For debug purposes
                            String print_temp;
                            print_temp += temp_hour;
                            print_temp += " H ";
                            print_temp += temp_minute;
                            print_temp += " M";

                            if (serial_debugging_enabled)
                            {
                                Serial.println("[UTI]  Valid Instrution");
                                Serial.print("[UTI]  Changing Activated Interval to: ");
                                Serial.println(print_temp);
                            }

                            // Send BLE message to indicate change
                            String ble_message = "Activated Interval set to: ";
                            ble_message += print_temp;
                            sendBLEMessage(ble_message);
                        }
                        else
                        {
                            if (serial_debugging_enabled)
                            {
                                Serial.println("[UTI]  Values are out of range");
                            }

                            // Send BLE message to indicate change
                            sendBLEMessage("Values are out of range");
                        }
                    }
                }
                else
                {
                    valid_instruction = false;
                }
            }
        }
        break;

        // Select board configuration
        case 'V':
            if (instruction.length() == 2)
            {
                if (instruction[1] == 'H')
                {
                    this_device_settings.device_type = 1; // 1 = Home model
                    if (serial_debugging_enabled)
                    {
                        Serial.println("[UTI]  Device type has been set to Home Model");
                    }
                    sendBLEMessage("Device type set to Home Model");
                }
                else if (instruction[1] == 'P')
                {
                    this_device_settings.device_type = 0; // 0 = Pocket model
                    if (serial_debugging_enabled)
                    {
                        Serial.println("[UTI]  Device type has been set to Pocket Model");
                    }
                    sendBLEMessage("Device type set to Pocket Model");
                }
                else
                {
                    valid_instruction = false;
                }
            }
            else
            {
                valid_instruction = false;
            }
            break;

        // SAVE or Slots per tray
        case 'S':
        {
            int save = instruction.compare("SAVE"); // Since function can return negative values
            if (save == 0)
            {

                this_device_settings.saveSettings();
                if (serial_debugging_enabled)
                {
                    Serial.println("[UTI]  Settings Saved");
                }
                sendBLEMessage("Settings saved");
            }
            else
            {
                if (instruction[1] == 'P' && instruction[2] == 'T' && instruction_length < 7)
                {
                    std::string slots_string;

                    // Check if instruction contains only numbers.
                    for (int i = 3; i < instruction_length; i++)
                    {
                        if (!isdigit(instruction[i]))
                        {
                            valid_instruction = false;
                            break;
                        }
                        else
                        {
                            slots_string += instruction[i];
                        }
                    }

                    // Decode instruction
                    if (valid_instruction)
                    {
                        // Convert characters to integer
                        uint16_t temp = atoi(slots_string.c_str());

                        // Set values to device settings (will not be saved until save instruction. if utils. mode is exited, settings will revert to previous)
                        portENTER_CRITICAL(&mux);
                        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                        {
                            this_device_settings.pill_slots_per_tray[i] = temp;
                        }
                        portEXIT_CRITICAL(&mux);

                        if (serial_debugging_enabled)
                        {
                            Serial.println("[UTI]  Valid Instrution");
                            Serial.print("[UTI]  Changing pill slots per tray to: ");
                            Serial.println(temp);
                        }

                        // Send BLE message to indicate change
                        String ble_message = "Pill slots per tray changed to: ";
                        ble_message += temp;
                        sendBLEMessage(ble_message);
                    }
                    else
                    {
                        valid_instruction = false;
                    }
                }
                else
                {
                    valid_instruction = false;
                }
            }
            break;
        }

        // LOAD: Opens all trays
        case 'L':
        {
            int save = instruction.compare("LOAD"); // Since function can return negative values
            if (save == 0)
            {
                this_device_settings.saveSettings();
                if (serial_debugging_enabled)
                {
                    Serial.println("[UTI]  Loading Pill Locker");
                }
                sendBLEMessage("Loading Pill Locker");

                // Unload Pill Locker
                loadInstruction();
            }
            else
            {
                valid_instruction = false;
            }
            break;
        }

        // CLOSE: Closes all trays
        case 'C':
        {
            int save = instruction.compare("CLOSE"); // Since function can return negative values
            if (save == 0)
            {
                this_device_settings.saveSettings();
                if (serial_debugging_enabled)
                {
                    Serial.println("[UTI]  Closing trays of Pill Locker");
                }
                sendBLEMessage("Closing trays of Pill Locker");

                // Unload Pill Locker
                closeInstruction();
            }
            else
            {
                valid_instruction = false;
            }
            break;
        }

        // EXIT
        case 'E':
        {
            int exit = instruction.compare("EXIT"); // Since function can return negative values
            if (exit == 0)
            {

                // Exit to main menu
                portENTER_CRITICAL(&mux);
                this_device_settings.current_device_mode[0] = '0';
                screen_flags.clear_display = true;
                portEXIT_CRITICAL(&mux);

                this_device_settings.saveSettings();
                this_device_settings.restoreSettings();

                // BEGIN MOTORS
                motor1.begin(this_device_settings.motor_speed, MICROSTEPS);
                motor2.begin(this_device_settings.motor_speed, MICROSTEPS);
                motor3.begin(this_device_settings.motor_speed, MICROSTEPS);

                if (serial_debugging_enabled)
                {
                    Serial.println("[UTI]  Settings restored from memory. Returning back to main menu");
                }

                sendBLEMessage("Settings restored from memory. Returning back to main menu");
            }
            else
            {
                valid_instruction = false;
            }
            break;
        }

        default:
            valid_instruction = false;
            break;
        }

        // Send message if instruction was invalid
        if (!valid_instruction)
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[UTI]  Invalid Instruction");
                sendBLEMessage("Invalid Instruction");
            }
        }

        // Unset flag
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.complex_instruction_request = false;
        portEXIT_CRITICAL(&mux);
    }
    else
    {
        delay(10); // Prevent core from panicking
    }
}

void testModeHandler(std::string instruction)
{
    // No instruction received
    int instruction_length = instruction.length();
    if (instruction_length > 0)
    {
        if (serial_debugging_enabled)
        {
            Serial.print("[TES]  Attempting to decode complex instruction: ");
            Serial.print(instruction.c_str());
            Serial.print(" ,of length ");
            Serial.println(instruction_length);
        }

        // flag for valid instruction
        bool valid_instruction = true;
        int motor_number = 0;

        // Use fold level 4 to see all instructions below neatly
        switch (instruction[0])
        {
        // MOVE:n:d:sss     Move n motor in d direction by sss steps
        case 'M':
            if (instruction[1] == 'O' && instruction[2] == 'V' && instruction[3] == 'E' && instruction_length <= 15)
            {

                // Which motor
                std::string temp_steps;
                int8_t direction = IN_DIRECTION; // by default to eliminate warning

                switch (instruction[5])
                {
                case '1':
                    motor_number = 1;
                    break;
                case '2':
                    motor_number = 2;
                    break;
                case '3':
                    motor_number = 3;
                    break;
                default:
                    if (serial_debugging_enabled)
                    {
                        Serial.println("[TES]  Invalid Motor Number");
                    }
                    sendBLEMessage("Invalid Motor Number");
                    valid_instruction = false;
                    break;
                }

                // Which direction
                switch (instruction[7])
                {
                case 'I':
                    direction = IN_DIRECTION;
                    break;
                case 'O':
                    direction = OUT_DIRECTION;
                    break;
                default:
                    if (serial_debugging_enabled)
                    {
                        Serial.println("[TES]  Invalid Direction");
                    }
                    sendBLEMessage("Invalid Motor Number");
                    valid_instruction = false;
                    break;
                }

                // Check if motor steps are only digits
                for (int i = 9; i < instruction_length; i++)
                {
                    if (!isdigit(instruction[i]))
                    {
                        valid_instruction = false;
                        break;
                    }
                    else
                    {
                        temp_steps += instruction[i];
                    }
                }

                // Move motors only if its a valid instruction
                if (valid_instruction)
                {
                    // Set steps to variables
                    uint16_t steps = atoi(temp_steps.c_str());

                    // Create debug string
                    String debug_mesag = "Moving motor ";
                    debug_mesag += motor_number;
                    debug_mesag += " by ";
                    debug_mesag += steps;

                    if (serial_debugging_enabled)
                    {
                        Serial.println("[TES]  " + debug_mesag);
                    }

                    sendBLEMessage(debug_mesag);

                    // move motor accordignly
                    moveMotor(motor_number, direction, steps);

                    // Create debug string
                    debug_mesag = ""; // Flush String
                    debug_mesag = "Finished moving motor ";
                    debug_mesag += motor_number;
                    debug_mesag += " by ";
                    debug_mesag += steps;

                    if (serial_debugging_enabled)
                    {
                        Serial.println("[TES]  " + debug_mesag);
                    }
                    sendBLEMessage(debug_mesag);
                }
            }
            // Move motor using button pad
            else if (instruction[1] == 'M' && instruction_length == 4)
            {
                switch (instruction[3])
                {
                case '1':
                    motor_number = 1;
                    break;
                case '2':
                    motor_number = 2;
                    break;
                case '3':
                    motor_number = 3;
                    break;
                default:
                    if (serial_debugging_enabled)
                    {
                        Serial.println("[TES]  Invalid Motor Number");
                    }
                    sendBLEMessage("Invalid Motor Number");
                    valid_instruction = false;
                    break;
                }

                // GO into motor move mode
                if (valid_instruction)
                {
                    // Set screen flags
                    screen_flags.clear_display = true;
                    screen_flags.display_steps_on_screen = true;

                    // Stay in loop while moving
                    bool exit = false;
                    long last_button_press_time = millis();

                    portENTER_CRITICAL(&mux);
                    buttons_are_free = false;
                    portEXIT_CRITICAL(&mux);

                    // START MOTORS AT A VERY SLOW SPEED SO IT CAN MOVE SLOW
                    int testrpm = 60;
                    motor1.begin(testrpm, MICROSTEPS);
                    motor2.begin(testrpm, MICROSTEPS);
                    motor3.begin(testrpm, MICROSTEPS);

                    String ble_message = "Use monitor & buttons to move motor ";
                    ble_message += (motor_number);
                    sendBLEMessage(ble_message);

                    while (exit == false)
                    {
                        if ((millis() - last_button_press_time) >= (SWITCH_DEBOUNCE_TIME_mS * 3))
                        {
                            if (digitalRead(UP_BUTTON) == 0)
                            {
                                // Update count
                                portENTER_CRITICAL(&mux);
                                myBluetoothInstructions.test_step_count = myBluetoothInstructions.test_step_count + NUMBER_OF_STEPS_MOVED_IN_TEST_MODE;
                                portEXIT_CRITICAL(&mux);

                                // Reset Timer
                                last_button_press_time = millis();

                                // Debug
                                if (serial_debugging_enabled)
                                {
                                    String _debug_mesage = "[TES]  Moving motor ";
                                    _debug_mesage += motor_number;
                                    _debug_mesage += " out by ";
                                    _debug_mesage += NUMBER_OF_STEPS_MOVED_IN_TEST_MODE;
                                    _debug_mesage += "steps";

                                    Serial.println(_debug_mesage);
                                }

                                // Move Motor
                                moveMotor(motor_number, OUT_DIRECTION, NUMBER_OF_STEPS_MOVED_IN_TEST_MODE);
                            }

                            if (digitalRead(DOWN_BUTTON) == 0)
                            {
                                // Reset Timer
                                last_button_press_time = millis();

                                // Update count
                                portENTER_CRITICAL(&mux);
                                myBluetoothInstructions.test_step_count = myBluetoothInstructions.test_step_count - NUMBER_OF_STEPS_MOVED_IN_TEST_MODE;
                                portEXIT_CRITICAL(&mux);

                                // Debug
                                if (serial_debugging_enabled)
                                {
                                    String _debug_mesage = "[TES]  Moving motor ";
                                    _debug_mesage += motor_number;
                                    _debug_mesage += " out by 10 step";

                                    Serial.println(_debug_mesage);
                                }

                                // Move Motor
                                moveMotor(motor_number, IN_DIRECTION, NUMBER_OF_STEPS_MOVED_IN_TEST_MODE);
                            }

                            if (digitalRead(OK_BUTTON) == 0)
                            {
                                exit = true;
                                last_button_press_time = millis();
                            }
                        }
                    }

                    // Update count
                    portENTER_CRITICAL(&mux);
                    myBluetoothInstructions.test_step_count = 0;

                    // Free buttons
                    buttons_are_free = true;

                    // Unset screen flags
                    screen_flags.clear_display = true;
                    screen_flags.display_steps_on_screen = false;
                    portEXIT_CRITICAL(&mux);
                }
            }
            else
            {
                valid_instruction = false;
            }
            break;

        case 'S':
        {
            int sound = instruction.compare("SOUND");

            if (sound == 0)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[TES]  Sounding Buzzer");
                }
                sendBLEMessage("Sounding Buzzer");
                soundBuzzer();
            }
            else
            {
                valid_instruction = false;
            }
        }
        break;

        case 'E':
        {
            int exit = instruction.compare("EXIT");
            if (exit == 0)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[TES]  Exiting test mode");
                }
                sendBLEMessage("Exiting test mode");

                // Exit to main menu
                portENTER_CRITICAL(&mux);
                this_device_settings.current_device_mode = '0';
                screen_flags.clear_display = true;
                portEXIT_CRITICAL(&mux);
            }
            else
            {
                valid_instruction = false;
            }
            break;
        }

        default:
            valid_instruction = false;
            break;
        }
        // Send message if instruction was invalid
        if (!valid_instruction)
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[TES]  Invalid Instruction");
                sendBLEMessage("Invalid Instruction");
            }
        }

        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.complex_instruction_request = false;
        portEXIT_CRITICAL(&mux);
    }
    else
    {
        delay(10);
    }
}

void demoModeHandler()
{
    if (device_is_dispensing)
    {
        dispenseInstruction();
        if (this_device_settings.current_device_mode[0] == 'D')
        {
            this_device_settings.setupDemoModeSleep();
        }
        device_is_dispensing = false;
        screen_flags.clear_display = true;
    }
}

void activatedModeHandler()
{
}

void closeInstruction()
{
    // TODO: Add this for home model
    //  Get total steps to come out
    portENTER_CRITICAL(&mux);
    uint16_t total_steps[NUMBER_OF_TRAYS];
    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
    {
        total_steps[i] = this_device_settings.motor_steps_per_slot[i] * this_device_settings.loaded_pills_per_tray[i];
    }
    portEXIT_CRITICAL(&mux);

    // Open trays
    String ble_mesage;
    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
    {
        // Debugging
        if (serial_debugging_enabled)
        {
            String debug_mesage;
            debug_mesage = "[UTI]  Closing Tray ";
            debug_mesage += (i + 1);
            debug_mesage += " by ";
            debug_mesage += total_steps[i];
            debug_mesage += " steps";
            Serial.println(debug_mesage);
        }

        // BLE Message
        ble_mesage = "Closing tray ";
        ble_mesage += (i + 1);
        ble_mesage += " by ";
        ble_mesage += total_steps[i];
        ble_mesage += " steps";
        sendBLEMessage(ble_mesage);
        ble_mesage = ""; // Flush String

        // Move Motor
        moveMotor((i + 1), IN_DIRECTION, total_steps[i]);

        if (serial_debugging_enabled)
        {
            portENTER_CRITICAL(&mux);
            String on_done;
            on_done = "[UTI]  Finished closing tray ";
            on_done += i + 1;
            Serial.println(on_done);
            portEXIT_CRITICAL(&mux);
        }

        // BLE Message
        ble_mesage = "Tray ";
        ble_mesage += (i + 1);
        ble_mesage += " closed";
        sendBLEMessage(ble_mesage);
    }

    if (serial_debugging_enabled)
    {
        Serial.println("[UTI]  Device has finished closing trays");
    }
    sendBLEMessage("Pill Locker has finished closing all trays");
}

void loadInstruction()
{
    // TODO: Add this for home model

    // Get total steps to come out
    portENTER_CRITICAL(&mux);
    uint16_t total_steps[NUMBER_OF_TRAYS];
    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
    {
        total_steps[i] = this_device_settings.motor_steps_per_slot[i] * this_device_settings.loaded_pills_per_tray[i];
    }
    portEXIT_CRITICAL(&mux);

    // Open trays
    String ble_mesage;
    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
    {
        if (serial_debugging_enabled)
        {
            String debug_mesage;

            debug_mesage = "[UTI]  Opening Tray ";
            debug_mesage += (i + 1);
            debug_mesage += " by ";
            debug_mesage += total_steps[i];
            debug_mesage += " steps";
            Serial.println(debug_mesage);
        }

        // BLE Message
        ble_mesage = "Opening tray ";
        ble_mesage += (i + 1);
        ble_mesage += " by ";
        ble_mesage += total_steps[i];
        ble_mesage += " steps";
        sendBLEMessage(ble_mesage);
        ble_mesage = ""; // Flush String

        // Move Motor
        moveMotor((i + 1), OUT_DIRECTION, total_steps[i]);

        if (serial_debugging_enabled)
        {
            portENTER_CRITICAL(&mux);
            String on_done;
            on_done = "[UTI]  Finished opening tray ";
            on_done += i + 1;
            Serial.println(on_done);
            portEXIT_CRITICAL(&mux);
        }

        // BLE Message
        ble_mesage += "Tray ";
        ble_mesage += (i + 1);
        ble_mesage += " opened";
        sendBLEMessage(ble_mesage);
    }

    if (serial_debugging_enabled)
    {
        Serial.println("[UTI]  Device has opened all trays");
    }
    sendBLEMessage("Pill Locker has finished opening all trays");
}

void dispenseInstruction()
{
    // Get device mode
    portENTER_CRITICAL(&mux);
    // char mode = this_device_settings.current_device_mode[0];

    // Tray dependent settings
    uint8_t _missed_dosages[NUMBER_OF_TRAYS];
    uint8_t _pill_slots_per_tray[NUMBER_OF_TRAYS];
    uint8_t _pills_remaining[NUMBER_OF_TRAYS];
    uint8_t _pills_loaded[NUMBER_OF_TRAYS];

    // Function variables
    uint8_t _number_of_trays_to_open[NUMBER_OF_TRAYS];

    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
    {
        _missed_dosages[i] = this_device_settings.dosages_missed[i];
        _pill_slots_per_tray[i] = this_device_settings.pill_slots_per_tray[i];
        _pills_remaining[i] = this_device_settings.dosages_remaining[i];
        _pills_loaded[i] = this_device_settings.loaded_pills_per_tray[i];
    }

    portEXIT_CRITICAL(&mux);

    // Demo Mode Dispense
    //  This mode will only dispense one tray until finished
    // Check if there are any pills missed

    for (int i = 0; i < NUMBER_OF_TRAYS; i++)
    {
        _number_of_trays_to_open[i] = (_pills_loaded[i] + 1) - (_pills_remaining[i]);
    }

    // Disable Main Menu Buttons
    portENTER_CRITICAL(&mux);
    buttons_are_free = false;
    portEXIT_CRITICAL(&mux);

    // Button press flag
    long initial_time = millis();
    long time_interval = 10000; // 10 seconds

    if (_number_of_trays_to_open[0] > 0 && _number_of_trays_to_open[0] <= this_device_settings.pill_slots_per_tray[0])
    {
        this_device_settings.device_dispensing_flags.awaiting_button_input = true;
        int alarm_count = 0; // 5 alarm counts or 50 seconds before device shuts off automatically

        soundBuzzer();

        int alarm_length = 2; // x * 10 seconds

        // Wait for user to press button to open tray. Timeout is 60 seconds
        while (this_device_settings.device_dispensing_flags.awaiting_button_input && this_device_settings.current_device_mode[0] == 'D' && alarm_count < alarm_length)
        {

            // Sound Buzzer every 10 seconds
            if ((millis() - initial_time) >= time_interval)
            {
                initial_time = millis();
                // Serial.println("Sounding Buzzer");
                alarm_count++;
                soundBuzzer();
            }

            if ((millis() - switch_activation_time) > SWITCH_DEBOUNCE_TIME_mS)
            {
                if (digitalRead(POW_BUTTON) == 0)
                {
                    this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                    switch_activation_time = millis();
                }

                if (digitalRead(UP_BUTTON) == 0)
                {
                    this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                    switch_activation_time = millis();
                }

                if (digitalRead(DOWN_BUTTON) == 0)
                {
                    this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                    switch_activation_time = millis();
                }

                if (digitalRead(OK_BUTTON) == 0)
                {
                    this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                    switch_activation_time = millis();
                }
            }
        }

        // Check that a change of device mode didnt exit loop above. Device mode can be changed if connected to bluetooth
        if (alarm_count < alarm_length)
        {
            portENTER_CRITICAL(&mux);
            this_device_settings.device_dispensing_flags.dispensing_pill = true;
            screen_flags.clear_display = true;
            portEXIT_CRITICAL(&mux);

            // Calculate necessary variables
            uint16_t motor_steps = _number_of_trays_to_open[0] * this_device_settings.motor_steps_per_slot[0];

            // Log
            this_device_settings.logPillDispensed();

            Serial.println("[DEM]  Dispensing pill");

            Serial.print("[DEM]  Number of pill slots in tray:  ");
            Serial.println(this_device_settings.pill_slots_per_tray[0]);

            Serial.print("[DEM]  Number of loaded pills:  ");
            Serial.println(this_device_settings.loaded_pills_per_tray[0]);

            Serial.print("[DEM]  Number of pills remaining:  ");
            Serial.println(_pills_remaining[0]);

            Serial.print("[DEM]  Number of missed dosages:  ");
            Serial.println(_missed_dosages[0]);

            Serial.print("[DEM]  Number of trays to open:  ");
            Serial.println(_number_of_trays_to_open[0]);

            Serial.print("[DEM]  Moving tray by ");
            Serial.print(motor_steps);
            Serial.println(" steps");

            // TODO: Here's where the magic happens

            //  Open Tray
            moveMotor(1, OUT_DIRECTION, motor_steps);
            delay(2000);

            if (this_device_settings.device_type == '0') // Pocket Unit
            {
                Serial.println("Opening for pocket mode");

                // Set flags for screen
                portENTER_CRITICAL(&mux);
                this_device_settings.device_dispensing_flags.tray_open = true;
                this_device_settings.device_dispensing_flags.awaiting_button_input = true;
                screen_flags.clear_display = true;
                portEXIT_CRITICAL(&mux);

                // Reset Buzzer Count
                alarm_count = 0;
                initial_time = millis();

                Serial.println("awaiting for tray close");

                soundBuzzer();

                while (this_device_settings.device_dispensing_flags.awaiting_button_input && alarm_count < 3)
                {
                    // Sound Buzzer every 10 seconds
                    if ((millis() - initial_time) >= time_interval)
                    {
                        initial_time = millis();
                        // Serial.println("[DEM] Sounding Buzzer");
                        alarm_count++;
                        soundBuzzer();
                    }

                    if ((millis() - switch_activation_time) > SWITCH_DEBOUNCE_TIME_mS)
                    {
                        if (digitalRead(POW_BUTTON) == 0)
                        {
                            this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                            switch_activation_time = millis();
                        }

                        if (digitalRead(UP_BUTTON) == 0)
                        {
                            this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                            switch_activation_time = millis();
                        }

                        if (digitalRead(DOWN_BUTTON) == 0)
                        {
                            this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                            switch_activation_time = millis();
                        }

                        if (digitalRead(OK_BUTTON) == 0)
                        {
                            this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                            switch_activation_time = millis();
                        }
                    }
                }

                // Set flags for screen
                portENTER_CRITICAL(&mux);
                this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                this_device_settings.device_dispensing_flags.dispensing_pill = false;
                screen_flags.clear_display = true;
                portEXIT_CRITICAL(&mux);

                // Close tray
                delay(2000);
                moveMotor(1, IN_DIRECTION, motor_steps);

                portENTER_CRITICAL(&mux);
                this_device_settings.device_dispensing_flags.tray_open = false;

                for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                {
                    this_device_settings.dosages_dispensed[i] = this_device_settings.dosages_dispensed[i] + 1;
                    this_device_settings.dosages_remaining[i] = this_device_settings.dosages_remaining[i] - 1;
                }
                portEXIT_CRITICAL(&mux);
            }
            else // Home Unit model
            {
                // TODO: Find out number of steps per tray in home unit model
                // TODO: Program app to configure PL into home mode
                Serial.println("Opening for home unit mode");

                // Set flags for screen
                portENTER_CRITICAL(&mux);
                this_device_settings.device_dispensing_flags.tray_open = true;
                this_device_settings.device_dispensing_flags.awaiting_button_input = true;
                screen_flags.clear_display = true;
                portEXIT_CRITICAL(&mux);

                delay(5000); // 5 second delay

                // Set flags for screen
                portENTER_CRITICAL(&mux);
                this_device_settings.device_dispensing_flags.tray_open = false;
                this_device_settings.device_dispensing_flags.awaiting_button_input = false;
                this_device_settings.device_dispensing_flags.dispensing_pill = false;

                screen_flags.clear_display = true;

                for (int i = 0; i < NUMBER_OF_TRAYS; i++)
                {
                    this_device_settings.dosages_dispensed[i] = this_device_settings.dosages_dispensed[i] + 1;
                    this_device_settings.dosages_remaining[i] = this_device_settings.dosages_remaining[i] - 1;
                }
                portEXIT_CRITICAL(&mux);
            }

            delay(2000);
            Serial.println("[DEM]  Tray closed");
        }
        else
        {
            Serial.println("[DEM]  Missed Pill");

            portENTER_CRITICAL(&mux);
            for (int i = 0; i < NUMBER_OF_TRAYS; i++)
            {
                this_device_settings.dosages_missed[i] = this_device_settings.dosages_missed[i] + 1;
            }
            portEXIT_CRITICAL(&mux);

            Serial.print("[DEM]  Number of pills remaining:  ");
            Serial.println(this_device_settings.dosages_remaining[0]);

            Serial.print("[DEM]  Number of missed dosages:  ");
            Serial.println(this_device_settings.dosages_missed[0]);

            // Log
            this_device_settings.logPillMissed();
        }

        // Go back to main menu once done
        if (this_device_settings.dosages_missed[0] == 0)
        {
            if (this_device_settings.dosages_remaining[0] == 0)
            {
                if (this_device_settings.current_device_mode[0] == 'D')
                {
                    this_device_settings.exitDemoMode();
                    this_device_settings.current_device_mode[0] = '0';
                }
            }
        }
        else
        {
            if (this_device_settings.dosages_missed[0] + this_device_settings.dosages_dispensed[0] == this_device_settings.loaded_pills_per_tray[0])
            {
                if (this_device_settings.current_device_mode[0] == 'D')
                {
                    this_device_settings.exitDemoMode();
                    this_device_settings.current_device_mode[0] = '0';
                }
            }
        }

        Serial.println(this_device_settings.dosages_missed[0] + this_device_settings.dosages_dispensed[0]);

        this_device_settings.saveSettings();
    }
}

// BLE
void initBLE()
{
    // BLE
    BLEDevice::init(BLENAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);

    UARTCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);

    UARTCharacteristic->addDescriptor(new BLE2902());
    UARTCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    pServer->getAdvertising()->start();

    if (serial_debugging_enabled)
    {
        Serial.println("[BLE]  Device is advertising & ready to connect");
    }
}

void sendBLEMessage(String data)
{
    portENTER_CRITICAL(&mux);
    String ble_data;
    ble_data += data;

    // Send Data
    UARTCharacteristic->setValue((char *)ble_data.c_str());
    UARTCharacteristic->notify();
    yield(); // AS
    portEXIT_CRITICAL(&mux);
}

void connectionManager()
{
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        portENTER_CRITICAL(&mux);
        char mode = this_device_settings.current_device_mode[0];
        portEXIT_CRITICAL(&mux);

        // Force device to go to normal mode
        if (mode != 'A' && mode != '0')
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[BLE]  Device was disconnected without exiting a mode. Setting to default");
            }

            portENTER_CRITICAL(&mux);
            this_device_settings.current_device_mode[0] = '0';
            screen_flags.clear_display = true;
            portEXIT_CRITICAL(&mux);
        }

        // Clear main screen
        if (mode == '0')
        {
            screen_flags.clear_display = true;
        }

        oldDeviceConnected = deviceConnected;

        if (serial_debugging_enabled)
        {
            Serial.println("[BLE]  BLE Service has disconnected.");
        }
        pServer->startAdvertising(); // restart advertising
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        if (serial_debugging_enabled)
        {
            Serial.println("[BLE]  BLE Service is connected.");
        }
        portENTER_CRITICAL(&mux);
        screen_flags.clear_display = true;
        portEXIT_CRITICAL(&mux);
        oldDeviceConnected = deviceConnected;
    }
}

void IRAM_ATTR assignBluetoothFlag(std::string data)
{
    if (data.length() > 0)
    {
        portENTER_CRITICAL_ISR(&mux);
        // Print raw data
        int len = data.length();
        if (serial_debugging_enabled)
        {
            // Serial.print("[BLE]  Receive Interrupt Exceuting in Core: ");
            // Serial.println(xPortGetCoreID());
            Serial.print("[BLE]  Data received: ");

            for (int i = 0; i < len; i++)
            {
                Serial.print(data[i]);
            }
            Serial.println();
        }

        // --  Separate main 4 instructions
        if (len == 1) // Only main insctructions should be single character
        {
            String status_message;
            switch (data[0])
            {
            case 'L':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.send_log_info = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Activate Device
            case 'A':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.active_mode_request = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Test
            case 'T':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.test_mode_request = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Utilities
            case 'U':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.utilities_mode_request = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Demo
            case 'D':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.demo_mode_request = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Exit Demo mode
            case '#':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.exit_demo_mode_request = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Send out status message
            case '$':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.send_status_message = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;

            // Send out current settings
            case '?':
                portENTER_CRITICAL_ISR(&mux);
                myBluetoothInstructions.send_current_settings = true;
                portEXIT_CRITICAL_ISR(&mux);
                break;
            default:
                if (serial_debugging_enabled)
                {
                    Serial.println("[BLE]  Invalid Instruction");
                }
                sendBLEMessage("Invalid Instruction");
                break;
            }
        }

        // -- Utilities Instructions Flags
        else
        {
            portENTER_CRITICAL(&mux);
            myBluetoothInstructions.complex_instruction_string = data;
            myBluetoothInstructions.complex_instruction_request = true;
            // char mode = this_device_settings.current_device_mode[0];
            portEXIT_CRITICAL(&mux);
        }
        portEXIT_CRITICAL_ISR(&mux);
    }
}

void bluetothInstructionHandlerCore1()
{
    portENTER_CRITICAL(&mux);
    char mode = this_device_settings.current_device_mode[0];
    portEXIT_CRITICAL(&mux);

    if (myBluetoothInstructions.send_log_info)
    {
        Serial.println("sending log info");
        for (int i = 1; i < this_device_settings.dosages_missed[0] + 1; i++)
        {
            sendBLEMessage(this_device_settings.retrieveLog(i));
        }

        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.send_log_info = false;
        portEXIT_CRITICAL(&mux);
    }

    // --  Restart Device 'R'
    if (myBluetoothInstructions.restart_device)
    {
        ESP.restart();
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.restart_device = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Activate Device 'A'
    if (myBluetoothInstructions.active_mode_request)
    {
        if (mode == '0')
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Device Mode: Activate Mode ");
            }
            this_device_settings.current_device_mode = 'A';
            sendBLEMessage("Pill Locker is now in Activated Mode");

            // Clear display
            screen_flags.clear_display = true;
        }
        else
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Exit current mode first ");
            }
            sendBLEMessage("Exit current mode first");
        }
        // Unset flag
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.active_mode_request = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Test Mode set 'T'
    if (myBluetoothInstructions.test_mode_request)
    {
        if (mode == '0')
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Device Mode: Test Mode ");
            }
            this_device_settings.current_device_mode = 'T';
            sendBLEMessage("Pill Locker is now in Test Mode");

            // Clear display
            screen_flags.clear_display = true;
        }
        else
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Exit current mode first ");
            }
            sendBLEMessage("Exit current mode first");
        }

        // Unset flag
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.test_mode_request = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Utilities Mode set 'U'
    if (myBluetoothInstructions.utilities_mode_request)
    {
        if (mode == '0')
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Device Mode: Utilities Mode ");
            }
            this_device_settings.current_device_mode = 'U';
            sendBLEMessage("Pill Locker is now in Utilities Mode");

            // Clear display
            screen_flags.clear_display = true;
        }
        else
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Exit current mode first ");
            }
            sendBLEMessage("Exit current mode first");
        }

        // Unset flag
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.utilities_mode_request = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Demo Mode set 'D'
    if (myBluetoothInstructions.demo_mode_request)
    {
        if (mode == '0')
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Device Mode: Demo Mode ");
            }

            // Clear display
            screen_flags.clear_display = true;

            this_device_settings.setupDemoModeSleep();

            portENTER_CRITICAL(&mux);
            this_device_settings.current_device_mode[0] = 'D';
            portEXIT_CRITICAL(&mux);

            this_device_settings.saveSettings();

            sendBLEMessage("Pill Locker is now in Demo Mode");
        }
        else
        {
            if (serial_debugging_enabled)
            {

                Serial.println("[BLE]  Exit current mode first ");
            }
            sendBLEMessage("Exit current mode first");
        }
        // Unset flag
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.demo_mode_request = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Exit demo device mode '#'
    if (myBluetoothInstructions.exit_demo_mode_request)
    {
        if (mode == 'D')
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[DEV]  Exiting demo mode");
            }
            portENTER_CRITICAL(&mux);
            this_device_settings.current_device_mode[0] = '0'; // Exit to main menu
            screen_flags.clear_display = true;                 // Clear Display
            screen_flags.skip_screen = true;
            portEXIT_CRITICAL(&mux);

            // Deactivate sleep mode
            esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);

            this_device_settings.saveSettings();
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Current status & setting saved to memory");
            }
        }
        else
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[DEV]  Device is not in any mode right now");
            }
            sendBLEMessage("Device is not in demo mode right now, type help for more info");
        }

        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.exit_demo_mode_request = false; // Unset flag
        portEXIT_CRITICAL(&mux);
    }

    // --  Send out status message '$'
    if (myBluetoothInstructions.send_status_message)
    {
        String status_message;
        if (serial_debugging_enabled)
        {
            Serial.println("[BLE]  Sending out status message");
        }

        // Print out header
        status_message += "PILL LOCKER CURRENT STATUS";
        sendBLEMessage(status_message);
        status_message = ""; // Flush String

        // First Message
        status_message += "RTC: ";
        status_message += rtc.stringTime();
        sendBLEMessage(status_message);

        // Second Message
        status_message = "";
        status_message += "Remaining pills: ";
        sendBLEMessage(status_message);
        status_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            status_message += ("        Tray ");
            status_message += (i + 1);
            status_message += ": ";
            status_message += this_device_settings.dosages_remaining[i];
        }
        sendBLEMessage(status_message);

        // Third Message
        status_message = "";
        status_message += "Missed Dosages: ";
        sendBLEMessage(status_message);
        status_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            status_message += ("        Tray ");
            status_message += (i + 1);
            status_message += ": ";
            status_message += this_device_settings.dosages_missed[i];
        }
        sendBLEMessage(status_message);

        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.send_status_message = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Send out current settings '?'
    if (myBluetoothInstructions.send_current_settings)
    {
        String settings_message;
        if (serial_debugging_enabled)
        {
            Serial.println("[BLE]  Sending out current settings");
        }

        // Print out header
        settings_message += "PILL LOCKER CURRENT SETTINGS";
        sendBLEMessage(settings_message);
        settings_message = ""; // Flush String

        // Firmware Version
        sendBLEMessage(("Firmware Version: " + FIRMWARE_VERSION));
        settings_message = ""; // Flush String

        // Demo Dispense Interval
        settings_message += "Demo Dispense Interval: ";
        sendBLEMessage(settings_message);
        settings_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            settings_message += ("        Tray ");
            settings_message += (i + 1);
            settings_message += ": ";
            settings_message += this_device_settings.demo_dispense_interval_s[i];
            settings_message += "s";
        }
        sendBLEMessage(settings_message);

        // Active Dispense Interval
        settings_message = "";
        settings_message += "Activated Dispense Interval: ";
        sendBLEMessage(settings_message);
        settings_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            settings_message += ("        Tray ");
            settings_message += (i + 1);
            settings_message += ": ";
            settings_message += this_device_settings.activated_dispense_interval_h[i];
            settings_message += "h ";
            settings_message += this_device_settings.activated_dispense_interval_m[i];
            settings_message += "m";
        }
        sendBLEMessage(settings_message);

        // Motor Speed
        settings_message = "";
        settings_message += "Motor speed: ";
        settings_message += this_device_settings.motor_speed;
        settings_message += " rev/min";
        sendBLEMessage(settings_message);

        // Steps per pill
        settings_message = ""; // Flush string
        settings_message += "Steps per pill: ";
        sendBLEMessage(settings_message);
        settings_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            settings_message += ("        Tray ");
            settings_message += (i + 1);
            settings_message += ": ";
            settings_message += this_device_settings.motor_steps_per_slot[i];
        }
        sendBLEMessage(settings_message);

        // Slots per tray
        settings_message = ""; // Flush string
        settings_message += "Pill slots per tray: ";
        sendBLEMessage(settings_message);
        settings_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            settings_message += ("        Tray ");
            settings_message += (i + 1);
            settings_message += ": ";
            settings_message += this_device_settings.pill_slots_per_tray[i];
        }
        sendBLEMessage(settings_message);

        // Pill per tray
        settings_message = ""; // Flush string
        settings_message += "Loaded pills per tray: ";
        sendBLEMessage(settings_message);
        settings_message = ""; // Flush String
        for (int i = 0; i < NUMBER_OF_TRAYS; i++)
        {
            settings_message += ("        Tray ");
            settings_message += (i + 1);
            settings_message += ": ";
            settings_message += this_device_settings.loaded_pills_per_tray[i];
        }
        sendBLEMessage(settings_message);

        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.send_current_settings = false;
        portEXIT_CRITICAL(&mux);
    }

    // -- Exit modes
}

void bluetothInstructionHandlerCore0()
{
    std::string complex_instruction; // Declare possible instruction var

    portENTER_CRITICAL(&mux);
    char mode = this_device_settings.current_device_mode[0];
    // if this flag is true, it must be unset at the end of each device mode handler!
    if (myBluetoothInstructions.complex_instruction_request)
    {
        complex_instruction = myBluetoothInstructions.complex_instruction_string;
    } // Check for possible instruction
    portEXIT_CRITICAL(&mux);

    switch (mode)
    {
    case 'A':
        /// Activated Mode
        activatedModeHandler();

        break;
    case 'T':
        /// Test Mode
        testModeHandler(complex_instruction);
        break;
    case 'U':
        // Utilities Mode
        utilitiesModeHandler(complex_instruction);
        break;
    case 'D':
        // Demo Mode
        demoModeHandler();
        break;
    case '0':
        portENTER_CRITICAL(&mux);
        myBluetoothInstructions.complex_instruction_request = false;
        myBluetoothInstructions.complex_instruction_string = ""; // Flush String if not in Util or Test Mode
        portEXIT_CRITICAL(&mux);
        // Do nothing
        delay(100);
        break;
    default:
        if (serial_debugging_enabled)
        {
            Serial.println("[ERR]  Invalid Device Mode");
        }
        break;
    }
}

void IRAM_ATTR adjustTimeRequest()
{
    if (deviceConnected)
    {
        portENTER_CRITICAL(&mux);
        // Create data string: split data with commas
        String data;
        data += "TR";

        // Send Data
        UARTCharacteristic->setValue((char *)data.c_str());
        UARTCharacteristic->notify();
        portEXIT_CRITICAL(&mux);
    }
}

// INITIAL SETUP
void setPinsForDeepSleep()
{
    // SET UP POWER DOMAIN
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    // SET UP WAKE UP BUTTONS
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0); // POWER BUTON

    // MOTORS
    rtc_gpio_init((gpio_num_t)MOTORSLEEP1);
    rtc_gpio_set_direction((gpio_num_t)MOTORSLEEP1, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level((gpio_num_t)MOTORSLEEP1, 0); // GPIO LOW

    rtc_gpio_init((gpio_num_t)MOTORSLEEP2);
    rtc_gpio_set_direction((gpio_num_t)MOTORSLEEP2, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level((gpio_num_t)MOTORSLEEP2, 0); // GPIO LOW

    rtc_gpio_init((gpio_num_t)MOTORSLEEP3);
    rtc_gpio_set_direction((gpio_num_t)MOTORSLEEP3, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level((gpio_num_t)MOTORSLEEP3, 0); // GPIO LOW

    // SCREEN
    pinMode(TFT_RST, LOW);

    // BUZZER
    screen_flags.display_sleep_warning_flag = false;
}

// BATTERY
void initBattery()
{
    pinMode(VBAT_3V3, INPUT_PULLDOWN);
    pinMode(BAT_STAT1, INPUT_PULLUP);
    pinMode(BAT_STAT2, INPUT_PULLUP);
}

// MOTORS
void initMotors()
{
    // SLEEP (ACTIVE LOW)
    pinMode(MOTORSLEEP1, OUTPUT);
    pinMode(MOTORSLEEP2, OUTPUT);
    pinMode(MOTORSLEEP3, OUTPUT);

    // DISABLE ALL MOTORS
    digitalWrite(MOTORSLEEP1, LOW);
    digitalWrite(MOTORSLEEP2, LOW);
    digitalWrite(MOTORSLEEP3, LOW);

    // BEGIN MOTORS
    motor1.begin(this_device_settings.motor_speed, MICROSTEPS);
    motor2.begin(this_device_settings.motor_speed, MICROSTEPS);
    motor3.begin(this_device_settings.motor_speed, MICROSTEPS);
}

void moveMotor(int motor, int direction, int steps)
{
    switch (motor)
    {
    case 1:
        digitalWrite(MOTORSLEEP1, HIGH);
        if (direction == -1)
        {
            motor1.move(-steps);
        }
        else if (direction == 1)
        {
            motor1.move(steps);
        }
        digitalWrite(MOTORSLEEP1, LOW);
        break;
    case 2:
        digitalWrite(MOTORSLEEP2, HIGH);
        if (direction == -1)
        {
            motor2.move(-steps);
        }
        else if (direction == 1)
        {
            motor2.move(steps);
        }
        digitalWrite(MOTORSLEEP2, LOW);
        break;
    case 3:
        digitalWrite(MOTORSLEEP3, HIGH);
        if (direction == -1)
        {
            motor3.move(-steps);
        }
        else if (direction == 1)
        {
            motor3.move(steps);
        }
        digitalWrite(MOTORSLEEP3, LOW);
        break;
    default:
        break;
    }
}

// SCREEN
void initScreen()
{
    // ENABLE SCREEN
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, HIGH);

    // INIT SCREEN
    tft.init(136, 241); // Init ST7789 240x135
    tft.setSPISpeed(40000000);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    // Update screen just to get
    // updateScreen();

    // Display Initial Time
    if (deviceConnected)
    {
        tft.setCursor(10, 10);
        tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.print("BLE CON");
    }
    else
    {
        tft.setCursor(10, 10);
        tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.print("BLE DIS");
    }
    tft.setCursor(140, 10);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(2);
    String header;
    int hours, minutes, seconds;
    hours = rtc.getHours();

    // Create nice time string to print
    if (hours < 10)
    {
        header += " ";
    }
    header += hours;
    header += ":";

    minutes = rtc.getMinutes();
    if (minutes < 10)
    {
        header += " ";
    }
    header += minutes;
    header += ":";

    seconds = rtc.getSeconds();
    if (seconds < 10)
    {
        header += " ";
    }
    header += seconds;

    // print header
    tft.print(header);

    tft.drawLine(0, 30, 241, 30, ST77XX_WHITE);

    // TURN ON BACK LIGHT
    pinMode(LEDK, OUTPUT);
    digitalWrite(LEDK, LOW);
}

void updateScreen()
{
    if (deviceConnected)
    {
        tft.setCursor(10, 10);
        tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.print("BLE CON");
    }
    else
    {
        tft.setCursor(10, 10);
        tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.print("BLE DIS");
    }
    tft.setCursor(140, 10);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(2);

    // Create nice time string to print & check if its within limits
    String header;
    int hours, minutes, seconds;
    bool within_limits = true;

    // Hours
    hours = rtc.getHours();
    if (hours >= 0 && hours < 24)
    {
        if (hours < 10)
        {
            header += " ";
        }
        header += hours;
        header += ":";
    }
    else
    {
        within_limits = false;
    }

    // Minutes
    minutes = rtc.getMinutes();
    if (minutes >= 0 && minutes < 60)
    {
        if (minutes < 10)
        {
            header += " ";
        }
        header += minutes;
        header += ":";
    }
    else
    {
        within_limits = false;
    }

    // Seconds
    seconds = rtc.getSeconds();
    if (seconds >= 0 && seconds < 60)
    {
        if (seconds < 10)
        {
            header += " ";
        }
        header += seconds;
    }
    else
    {
        within_limits = false;
    }

    // Print if its within limits
    if (within_limits)
    {
        // print header
        tft.print(header);
    }

    // Update device settings time for logging purposes
    portENTER_CRITICAL(&mux);
    this_device_settings.hours = hours;
    this_device_settings.minutes = minutes;
    this_device_settings.seconds = seconds;
    portEXIT_CRITICAL(&mux);

    tft.drawLine(0, 30, 241, 30, ST77XX_WHITE);

    // Clear if necessary
    if (screen_flags.clear_display)
    {
        tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);
        screen_flags.clear_display = false;
    }

    portENTER_CRITICAL(&mux);
    char mode = this_device_settings.current_device_mode[0];
    portEXIT_CRITICAL(&mux);

    switch (mode)
    {
    case 'A':
        /// Activated Mode
        activeModeScreenManager();
        break;
    case 'T':
        /// Test Mode
        testModeScreenManager();
        break;
    case 'U':
        // Utilities Mode
        utilitiesModeScreenManager();
        break;
    case 'D':
        // Demo Mode
        demoModeScreenManager();
        break;
    case '0':
        mainMenuScreenManager();
        break;
    default:
        if (serial_debugging_enabled)
        {
            Serial.println("[ERR]  Invalid Device Mode");
        }
        break;
    }
    // Display sleep warning
    sleepWarningScreenManager();
}

void activeModeScreenManager()
{
}

void mainMenuScreenManager()
{
    if (!screen_flags.display_sleep_warning_flag)
    {
        if (deviceConnected)
        {
            tft.setCursor(0, 40);
            tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            tft.setTextSize(2);
            // with size text 2, 20 characters fit per line
            tft.print("   Select a mode:   ");
            tft.setCursor(0, 60);
            tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
            tft.setTextSize(2);
            tft.print("     [A]ctivate          [D]emo              [T]est              [U]tilities");
        }
        else
        {
            tft.setCursor(0, 50);
            tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            tft.setTextSize(2);
            // with size text 2, 20 characters fit per line
            tft.print("   Welcome to the       Pill Locker.");
            tft.setCursor(0, 90);
            tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
            tft.setTextSize(2);
            tft.print("Connect to Bluetooth   to get started   ");
        }
    }
}

void testModeScreenManager()
{
    if (!screen_flags.display_sleep_warning_flag)
    {
        if (!screen_flags.display_steps_on_screen)
        {
            tft.setCursor(0, 40);
            tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            tft.setTextSize(2);
            // with size text 2, 20 characters fit per line
            tft.print("  Device is now in       Test Mode     ");
            tft.setCursor(0, 80);
            tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
            tft.setTextSize(2);
            tft.print("     Use App to       change settings   ");
        }
        else
        {
            tft.setCursor(0, 40);
            tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            tft.setTextSize(3);
            // with size text 2, 20 characters fit per line
            tft.print("  # of steps");

            // Get step count
            portENTER_CRITICAL(&mux);
            int16_t _step_count = myBluetoothInstructions.test_step_count;
            portEXIT_CRITICAL(&mux);

            int base_pixel = 100;

            // Print step count
            if (_step_count <= (-1000) && _step_count >= (-10000))
            {
                tft.setCursor(base_pixel - 36, 80);
            }
            else if (_step_count <= (-100) && _step_count >= (-1000))
            {
                tft.setCursor(base_pixel - 18, 80);
                tft.fillRect(base_pixel - 36, 80, 18, 24, ST77XX_BLACK);
            }
            else if (_step_count < 0 && _step_count > (-100))
            {
                tft.setCursor(base_pixel, 80);
                tft.fillRect(base_pixel - 36, 80, 36, 24, ST77XX_BLACK);
            }

            else if (_step_count == 0)
            {
                tft.setCursor(base_pixel + (18 * 2), 80);
                tft.fillRect(base_pixel - 36, 80, 72, 24, ST77XX_BLACK);
            }

            else if (_step_count > 0 && _step_count < 10)
            {
                tft.setCursor(base_pixel + (18 * 2), 80);
                tft.fillRect(base_pixel - 36, 80, 54, 24, ST77XX_BLACK);
            }
            else if (_step_count >= 10 && _step_count < 100)
            {
                tft.setCursor(base_pixel + (18), 80);
                tft.fillRect(base_pixel - 18, 80, 36, 24, ST77XX_BLACK);
            }
            else if (_step_count >= 100 && _step_count < 1000)
            {
                tft.setCursor(base_pixel, 80);
                tft.fillRect(base_pixel - 18, 80, 18, 24, ST77XX_BLACK);
            }
            else if (_step_count >= 1000 && _step_count < 10000)
            {
                tft.setCursor(base_pixel - 18, 80);
                // tft.fillRect(base_pixel - 18, 80, 36, 24, ST77XX_BLACK);
            }

            tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            tft.setTextSize(3); // 18 PIX WIDE

            tft.print(_step_count);
        }
    }
}

void utilitiesModeScreenManager()
{
    if (deviceConnected)
    {
        if (!screen_flags.display_sleep_warning_flag)
        {
            tft.setCursor(0, 40);
            tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            tft.setTextSize(2);
            // with size text 2, 20 characters fit per line
            tft.print("  Device is now in     Utilities Mode   ");
            tft.setCursor(0, 80);
            tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
            tft.setTextSize(2);
            tft.print("     Use App to       change settings   ");
        }
    }
}

void demoModeScreenManager()
{
    if (!screen_flags.display_sleep_warning_flag)
    {
        if (!device_is_dispensing)
        {
            tft.setCursor(0, 40);
            tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            tft.setTextSize(2);
            // with size text 2, 20 characters fit per line
            tft.print("  Device is now in       Demo Mode     ");
        }
        else
        {
            portENTER_CRITICAL(&mux);
            bool awaiting_button_input = this_device_settings.device_dispensing_flags.awaiting_button_input;
            bool dispensing_pill = this_device_settings.device_dispensing_flags.dispensing_pill;
            bool tray_open = this_device_settings.device_dispensing_flags.tray_open;
            portEXIT_CRITICAL(&mux);

            // this_device_settings.device_dispensing_flags.tray_open = true;
            // this_device_settings.device_dispensing_flags.awaiting_button_input = true;
            // screen_flags.clear_display = true;

            this_device_settings.device_dispensing_flags.awaiting_button_input = false;
            this_device_settings.device_dispensing_flags.dispensing_pill = false;
            screen_flags.clear_display = true;

            this_device_settings.device_dispensing_flags.tray_open = false;

            if (awaiting_button_input && !tray_open)
            {
                tft.setCursor(0, 60);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(2);
                // with size text 2, 20 characters fit per line
                tft.print("  Press any button    to dispense your      medication");
            }
            else if (!tray_open && dispensing_pill)
            {
                tft.setCursor(0, 60);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(2);
                // with size text 2, 20 characters fit per line
                tft.print("  Dispensing your        medication");
            }
            else if (tray_open && awaiting_button_input && dispensing_pill)
            {
                tft.setCursor(0, 60);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(2);
                // with size text 2, 20 characters fit per line
                if (this_device_settings.device_type == '0')
                    tft.print("  Press any button     close the tray   ");
                else
                    tft.print("   Please pick up     your medication   ");
            }
            else if (tray_open && !dispensing_pill)
            {
                tft.setCursor(0, 60);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(2);
                // with size text 2, 20 characters fit per line
                tft.print("    Closing tray    ");
            }
            else if (!tray_open && !dispensing_pill)
            {
                tft.setCursor(0, 60);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(2);
                // with size text 2, 20 characters fit per line
                tft.print("    settings here    ");
            }
        }
    }
}

void sleepWarningScreenManager()
{
    // Get device mode
    portENTER_CRITICAL(&mux);
    char mode = this_device_settings.current_device_mode[0];
    portEXIT_CRITICAL(&mux);

    // Create timers
    long time = timerRead(battery_timer);
    long time_to_warning;

    // Device mode dependent screen
    bool do_display_warning = false;

    if (mode == 'D')
    {
        if (!device_is_dispensing)
        {
            time_to_warning = (this_device_settings.time_to_sleep_time_d - 5) * uS_TO_S_FACTOR;
            do_display_warning = true;
        }
    }
    else
    {
        time_to_warning = (this_device_settings.time_to_sleep_time_a - 10) * uS_TO_S_FACTOR;
        if (!deviceConnected)
        {
            do_display_warning = true;
        }
    }

    // Display screen accordingly
    if (do_display_warning)
    {
        if (time >= time_to_warning)
        {
            // This will prevent the warning message from showing after disconnect
            if (!screen_flags.skip_screen && screen_flags.display_sleep_warning_flag == false)
            {
                if (screen_flags.clean_screen_once)
                {
                    screen_flags.clean_screen_once = false;
                    tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);
                }
                // Only needed for main menu
                screen_flags.display_sleep_warning_flag = true;
                tft.setCursor(0, 40);
                tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
                tft.setTextSize(2);
                // with size text 2, 20 characters fit per line
                tft.print("  Device will enter  sleep mode soon to     save battery    ");
                tft.setCursor(0, 90);
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
                tft.setTextSize(2);
                tft.print("  Press any button       to cancel     ");
            }
            else if (screen_flags.skip_screen)
            {
                screen_flags.skip_screen = false;
            }
        }

        else
        {
            if (screen_flags.display_sleep_warning_flag == true)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[DIS]  Erasing Warning Message");
                }
                tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);

                // Unset flags
                screen_flags.display_sleep_warning_flag = false;
                screen_flags.clean_screen_once = true;
            }
        }
    }
    // This will clean up the display after being connected to BLE
    else
    {
        if (screen_flags.display_sleep_warning_flag == true)
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[DIS]  Erasing Warning Message");
            }
            tft.fillRect(0, 31, 241, 106, ST77XX_BLACK);

            // Unset flags
            screen_flags.display_sleep_warning_flag = false;
            screen_flags.clean_screen_once = true;
        }
    }
}

// FLASH STORAGE
void initStorage()
{
    myNVS.begin();

    // Check if it's first time boot after being programmed
    if (first_boot == true)
    {
        myNVS.eraseAll();
        if (myNVS.setInt("boot", 1) != true)
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Error storing variable");
            }
        }
    }

    // -- Program default settings on start up if not programmed
    uint64_t unit_programmed = myNVS.getInt("unit_p");
    if (unit_programmed == 0)
    {
        if (serial_debugging_enabled)
        {
            Serial.println("PILL LOCKER REV 3.1");
            Serial.println("Booting...");
            Serial.println();
            Serial.println("[RTC]  Flash Memory Erased");
            if (myNVS.setInt("unit_p", 1) != true)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[MEM]  Error storing variable o");
                }
            };
            this_device_settings.setDefaultSettings();
            this_device_settings.saveSettings();
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Default settings saved to memory");
            }
        }
    }

    // -- Restore settings from memory if device is programmed
    if (unit_programmed == 1)
    {
        this_device_settings.restoreSettings();
        if (serial_debugging_enabled)
        {
            Serial.println("[MEM]  Device settings have been restored");
        }
        if (this_device_settings.using_default_settings == 1) // true
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Device using default settings");
            }
        }
        else if (this_device_settings.using_default_settings == 0) // false
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Device using custom settings");
            }
        }
        else // should never get here
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[MEM]  Device settings seem corrupted");
            }
        }
    }
}

// RTC
void initRTC()
{
    Wire.begin();
    Wire.setClock(50000);

    int init_count = 0;
    while (init_count < 3)
    {
        if (rtc.begin(Wire) == false)
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[RTC]  Problem Initializing RTC");
            }
            init_count++;
        }
        else
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[RTC]  Initialized Correctly");
            }
            init_count = 4;
        }
    }

    if (first_boot)
    {
        rtc.setToCompilerTime();
        rtc.set12Hour();
        if (rtc.updateTime() == false) // Updates the time variables from RTC
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[RTC]  Could not update time");
            }
        }
        else
        {
            String currentTime = rtc.stringTime();
            if (serial_debugging_enabled)
            {
                Serial.println("[RTC]  Setting to compiler Time");
                Serial.print("[RTC]  ");
                Serial.println(currentTime);
            }
        }

        // Create Demo Mode arrays

        // Create Actiated Mode arrays

        first_boot = false;
    }
    else
    {
        if (rtc.updateTime() == false) // Updates the time variables from RTC
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[RTC]  Could not update time");
            }
        }
        else
        {
            String currentTime = rtc.stringTime();
            if (serial_debugging_enabled)
            {
                Serial.print("[RTC]  ");
                Serial.println(currentTime);
            }
        }
    }

    // Backup Switchover Mode
    //  Serial.print("Config EEPROM 0x37 before: ");
    //  Serial.println(rtc.readConfigEEPROM_RAMmirror(0x37));

    // rtc.setBackupSwitchoverMode(0); //Switchover disabled
    // rtc.setBackupSwitchoverMode(1); //Direct Switching Mode
    // rtc.setBackupSwitchoverMode(2); //Standby Mode
    // rtc.setBackupSwitchoverMode(3); //Level Switching Mode (default)

    // Serial.print("Config EEPROM 0x37 after: ");
    // Serial.println(rtc.readConfigEEPROM_RAMmirror(0x37));
}

void updateTime()
{
    if (update_time == true)
    {
        portENTER_CRITICAL(&mux);
        bool temp = myBluetoothInstructions.update_time;
        DateTime temp_time = this_device_settings.newTime;
        portEXIT_CRITICAL(&mux);

        // Update time if necessary
        if (temp)
        {
            rtc.set24Hour();
            rtc.setYear(temp_time.year());
            rtc.setMonth(temp_time.month());
            rtc.setDate(temp_time.day());
            rtc.setHours(temp_time.hour());
            rtc.setMinutes(temp_time.minute());
            rtc.setSeconds(temp_time.second());

            Serial.println("[RTC]  Time has been updated");
            portENTER_CRITICAL(&mux);
            myBluetoothInstructions.update_time = false;
            portEXIT_CRITICAL(&mux);

            rtc.set12Hour();
        }
        if (rtc.updateTime() == false) // Updates the time variables from RTC
        {
            if (serial_debugging_enabled)
            {
                Serial.println("[RTC]  Could not update time");
            }
        }
        else
        {
            String currentTime = rtc.stringTime();
            if (serial_debugging_enabled)
            {
                Serial.print("[RTC]  ");
                Serial.println(currentTime);
            }
        }
        // Unset flag
        update_time = false;
    }
}

void alarmController()
{
    if (pill_locker_sleeping)
    {
        // Update time as necessary
        esp_sleep_wakeup_cause_t wakeup_reason;
        wakeup_reason = esp_sleep_get_wakeup_cause();

        // -- Print wake up reason
        if (serial_debugging_enabled)
        {
            switch (wakeup_reason)
            {
            case ESP_SLEEP_WAKEUP_EXT0:
                Serial.println("[RTC]  Wakeup caused by external signal using Power Button");
                break;
            case ESP_SLEEP_WAKEUP_EXT1:
                Serial.println("[RTC]  Wakeup caused by external signal using RTC_CNTL");
                break;
            case ESP_SLEEP_WAKEUP_TIMER:
                Serial.println("[RTC]  Wakeup caused by timer");
                break;
            case ESP_SLEEP_WAKEUP_TOUCHPAD:
                Serial.println("[RTC]  Wakeup caused by touchpad");
                break;
            case ESP_SLEEP_WAKEUP_ULP:
                Serial.println("[RTC]  Wakeup caused by ULP program");
                break;
            default:
                Serial.printf("[RTC]  Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
                break;
            }
        }

        // Demo Mode wake up
        if (this_device_settings.current_device_mode[0] == 'D')
        {
            if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[DEM]  Booting in demo mode");
                    screen_flags.clear_display = true;
                    device_is_dispensing = true;
                }
            }
            else if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)
            {
                Serial.println("got here");
                this_device_settings.setupDemoModeSleep();
                device_is_dispensing = false;
            }
            else
            {
                Serial.println("nop buenp");
            }
        }

        // Activate mode wake up
        else if (this_device_settings.current_device_mode[0] == 'A')
        {
            if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
            {
                if (serial_debugging_enabled)
                {
                    Serial.println("[DEM]  Booting in Active mode");
                    screen_flags.clear_display = true;
                    device_is_dispensing = true;
                }
            }
            else
            {
                // this_device_settings.setupDemoModeSleep();
            }
        }
    }
}

void adjustTime(std::string data)
{
    portENTER_CRITICAL(&mux);

    portEXIT_CRITICAL(&mux);
}

// BUTTONS
void initButtons()
{
    // ACTIVE LOW
    pinMode(POW_BUTTON, INPUT);
    pinMode(UP_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);
    pinMode(OK_BUTTON, INPUT);
}

void buttonHandler()
{
    portENTER_CRITICAL(&mux);
    bool _buttons_are_free = buttons_are_free;
    portEXIT_CRITICAL(&mux);
    bool timer_was_reset = false;

    if (_buttons_are_free)
    {
        if ((millis() - switch_activation_time) > SWITCH_DEBOUNCE_TIME_mS)
        {
            if (digitalRead(POW_BUTTON) == 0)
            {
                timer_was_reset = true;
                switch_activation_time = millis();
            }

            if (digitalRead(UP_BUTTON) == 0)
            {
                timer_was_reset = true;
                switch_activation_time = millis();
            }

            if (digitalRead(DOWN_BUTTON) == 0)
            {
                timer_was_reset = true;
                switch_activation_time = millis();
            }

            if (digitalRead(OK_BUTTON) == 0)
            {

                timer_was_reset = true;
                switch_activation_time = millis();
            }
        }

        if (timer_was_reset)
        {

            if (serial_debugging_enabled)
            {
                timerRestart(battery_timer);
                Serial.println("[POW]  Battery timer reset");
            }
        }
    }
}

// BUZZER
void initBuzzer()
{
    ledcAttachPin(AUDIO, 0);
    ledcSetup(0, 4000, 8);
}

void soundBuzzer()
{
    for (int i = 0; i < 5; i++)
    {
        ledcWrite(0, 200);
        delay(100);
        ledcWrite(0, 0);
        delay(100);
    }
}

// FAKE RTC
void startSleepTimer()
{
    // //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // digitalWrite(LEDK, LOW);
    // digitalWrite(TFT_RST, LOW);
    // esp_deep_sleep_start();
}

void print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        Serial.println("Wakeup caused by ULP program");
        break;
    default:
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
}