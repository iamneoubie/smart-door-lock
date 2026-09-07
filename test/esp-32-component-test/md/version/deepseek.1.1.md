/*
 * ESP32 Smart Lock System - VERSION 1.13
 * ============================================
 * Features:
 * - Working RFID from v1.2 with clean integration
 * - Clean LCD display from v1.6
 * - Modular, maintainable code structure
 * - Centralized servo state management
 * - All state checks in one place
 * - Cleaned up and simplified all functions
 * - v1.13: "Already" states are completely passive (no buzzer)
 */

#include "BluetoothSerial.h"
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>

// ============================================================
// PIN CONFIGURATION
// ============================================================

// Keypad - 4x4 Matrix
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD - I2C
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// RFID - RC522
#define SS_PIN 5
#define RST_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

// DHT11
#define DHT_PIN 17
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Servo
#define SERVO_PIN 2
Servo myServo;

// Buzzer
#define BUZZER_PIN 16
#define BUZZER_FREQ 1500

// Bluetooth
BluetoothSerial SerialBT;

// ============================================================
// SYSTEM CONSTANTS
// ============================================================

// Security
const char *BLUETOOTH_NAME = "Smart Lock";
const String BACKDOOR_KEY = "54321";

// Servo Positions
const int SERVO_LOCK_POS = 0;
const int SERVO_UNLOCK_POS = 180;
const int SERVO_SPEED_DELAY = 12;
const int SERVO_POSITION_TOLERANCE = 3;

// Timing Constants
const unsigned long DEBOUNCE_DELAY = 80;
const unsigned long LCD_UPDATE_INTERVAL = 200;
const unsigned long SCREEN_TIMEOUT = 30000;
const unsigned long PIN_TIMEOUT = 10000;
const unsigned long AUTO_LOCK_DELAY = 20000;
const unsigned long RFID_COOLDOWN = 300;
const unsigned long DISPLAY_DURATION = 10000;
const unsigned long DHT_READ_INTERVAL = 2000;

// ============================================================
// SYSTEM STATE
// ============================================================

bool isLocked = true;
bool btConnected = false;
String connectedDeviceAddress = "";
unsigned long systemStartTime = 0;
unsigned long lastActivityTime = 0;
unsigned long lastKeyPressTime = 0;
unsigned long lastLCDUpdateTime = 0;
unsigned long rfidCooldown = 0;
unsigned long unlockStartTime = 0;
bool autoLockPending = false;
unsigned long lastDHTReadTime = 0;
bool keypadOperationInProgress = false;
bool rfidInitialized = false;

// LCD Buffer
String lcdRow0 = "";
String lcdRow1 = "";
bool lcdNeedsUpdate = true;

// Keypad State
String pinBuffer = "";
String pendingAction = "";
bool isPinMode = false;
unsigned long pinStartTime = 0;
bool pinTimeoutTriggered = false;

// Display Modes
enum DisplayMode
{
    DISPLAY_MAIN,
    DISPLAY_TEMPERATURE,
    DISPLAY_BLUETOOTH
};
DisplayMode currentDisplayMode = DISPLAY_MAIN;
unsigned long displayStartTime = 0;

// DHT11 data
float lastTemperature = 0;
float lastHumidity = 0;
bool hasDHTData = false;

// ============================================================
// HELPER FUNCTIONS
// ============================================================

String getTimestamp()
{
    unsigned long elapsed = millis() - systemStartTime;
    unsigned long seconds = elapsed / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;
    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return String(timeStr);
}

void logMsg(String msg)
{
    Serial.println("[" + getTimestamp() + "] " + msg);
}

// ============================================================
// LCD FUNCTIONS
// ============================================================

void setLCD(int row, const String &text)
{
    String trimmed = text;
    if (trimmed.length() > 16)
        trimmed = trimmed.substring(0, 16);
    while (trimmed.length() < 16)
        trimmed += " ";

    if (row == 0)
    {
        if (lcdRow0 != trimmed)
        {
            lcdRow0 = trimmed;
            lcdNeedsUpdate = true;
        }
    }
    else
    {
        if (lcdRow1 != trimmed)
        {
            lcdRow1 = trimmed;
            lcdNeedsUpdate = true;
        }
    }
}

void updateLCD()
{
    if (!lcdNeedsUpdate)
        return;
    if (millis() - lastLCDUpdateTime < LCD_UPDATE_INTERVAL)
        return;

    lastLCDUpdateTime = millis();
    lcd.setCursor(0, 0);
    lcd.print(lcdRow0);
    lcd.setCursor(0, 1);
    lcd.print(lcdRow1);
    lcdNeedsUpdate = false;
}

void showMainScreen()
{
    currentDisplayMode = DISPLAY_MAIN;
    String status = isLocked ? "LOCKED" : "UNLOCKED";
    setLCD(0, status);
    setLCD(1, "A=LOCK B=UNLOCK");
    updateLCD();
}

void showPinEntry()
{
    setLCD(0, "ENTER PIN");
    String display = pinBuffer;
    while (display.length() < 16)
        display += " ";
    setLCD(1, display);
    updateLCD();
}

void showActionMessage(String msg)
{
    setLCD(0, msg);
    setLCD(1, "");
    updateLCD();
}

void showMessage(String msg1, String msg2 = "")
{
    setLCD(0, msg1);
    setLCD(1, msg2);
    updateLCD();
}

void showTemperature()
{
    currentDisplayMode = DISPLAY_TEMPERATURE;
    displayStartTime = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t))
    {
        lastTemperature = t;
        lastHumidity = h;
        hasDHTData = true;
        char line1[17], line2[17];
        snprintf(line1, sizeof(line1), "TEMP: %2.0f C", t);
        snprintf(line2, sizeof(line2), "HUM:  %2.0f %%", h);
        setLCD(0, String(line1));
        setLCD(1, String(line2));
        logMsg("DHT: T:" + String(t, 1) + "C, H:" + String(h, 1) + "%");
    }
    else
    {
        delay(100);
        h = dht.readHumidity();
        t = dht.readTemperature();
        if (!isnan(h) && !isnan(t))
        {
            lastTemperature = t;
            lastHumidity = h;
            hasDHTData = true;
            char line1[17], line2[17];
            snprintf(line1, sizeof(line1), "TEMP: %2.0f C", t);
            snprintf(line2, sizeof(line2), "HUM:  %2.0f %%", h);
            setLCD(0, String(line1));
            setLCD(1, String(line2));
            logMsg("DHT: T:" + String(t, 1) + "C, H:" + String(h, 1) + "% (retry)");
        }
        else
        {
            setLCD(0, "TEMP: ERROR");
            setLCD(1, "READ FAILED");
            logMsg("DHT: Read error");
        }
    }
    updateLCD();
}

void showBluetoothInfo()
{
    currentDisplayMode = DISPLAY_BLUETOOTH;
    displayStartTime = millis();

    if (btConnected && connectedDeviceAddress.length() > 0)
    {
        setLCD(0, "CONNECTED DEVICE");
        setLCD(1, connectedDeviceAddress);
        logMsg("BT: Displaying - " + connectedDeviceAddress);
    }
    else
    {
        setLCD(0, "CONNECTED DEVICE");
        setLCD(1, "NO CONNECTION");
        logMsg("BT: Displaying - No connection");
    }
    updateLCD();
}

void updateActivity()
{
    lastActivityTime = millis();
    if (!lcd.getBacklight())
    {
        lcd.backlight();
        lcdNeedsUpdate = true;
    }
}

void manageLCD()
{
    if (millis() - lastActivityTime > SCREEN_TIMEOUT)
    {
        if (lcd.getBacklight())
            lcd.noBacklight();
    }
    else
    {
        if (!lcd.getBacklight())
        {
            lcd.backlight();
            lcdNeedsUpdate = true;
        }
    }

    if (currentDisplayMode == DISPLAY_TEMPERATURE || currentDisplayMode == DISPLAY_BLUETOOTH)
    {
        if (millis() - displayStartTime > DISPLAY_DURATION)
        {
            logMsg("DISPLAY: Timeout - returning to main");
            showMainScreen();
        }
    }

    updateLCD();
}

// ============================================================
// BUZZER FUNCTIONS
// ============================================================

void buzzerInit()
{
    ledcAttach(BUZZER_PIN, BUZZER_FREQ, 8);
    delay(50);
}

void buzzerBeep(int duration = 150, int freq = 1500)
{
    ledcWriteTone(BUZZER_PIN, freq);
    delay(duration);
    ledcWrite(BUZZER_PIN, 0);
}

void buzzerSuccess()
{
    buzzerBeep(100, 2000);
    delay(60);
    buzzerBeep(100, 2500);
}

void buzzerError()
{
    for (int i = 0; i < 3; i++)
    {
        buzzerBeep(100, 800);
        delay(60);
    }
}

void buzzerClick()
{
    buzzerBeep(30, 1500);
}

// ============================================================
// DOOR STATUS - READ ONLY
// ============================================================
// IMPORTANT: These functions NEVER call servo.write(), attach(), or moveServo().
// Status/reporting must never cause physical servo movement.
String getDoorStatus()
{
    return isLocked ? "LOCKED" : "UNLOCKED";
}

int getExpectedServoPosition()
{
    return isLocked ? SERVO_LOCK_POS : SERVO_UNLOCK_POS;
}

bool isServoAtExpectedPosition()
{
    int position = myServo.read();
    int expected = getExpectedServoPosition();

    if (position < 0)
        return false;

    return abs(position - expected) <= SERVO_POSITION_TOLERANCE;
}

void reportDoorStatus(const String &source)
{
    String status = getDoorStatus();
    int servoPosition = myServo.read();
    bool positionOK = isServoAtExpectedPosition();

    logMsg(source + ": Status - " + status +
           ", servo position = " + String(servoPosition) +
           ", expected = " + String(getExpectedServoPosition()) +
           ", position check = " + (positionOK ? "OK" : "MISMATCH"));
}

// ============================================================
// SERVO FUNCTIONS - CENTRALIZED STATE MANAGEMENT
// ============================================================

void servoInit()
{
    myServo.attach(SERVO_PIN);
    delay(200);
    myServo.write(SERVO_LOCK_POS);
    delay(500);
    isLocked = true;
    logMsg("Servo initialized at LOCK position");
}

void moveServo(int target)
{
    int current = myServo.read();

    // Never issue a write if the commanded position is already reached.
    if (current < 0)
        current = target;

    if (current == target)
    {
        logMsg("SERVO: Target already reached - no movement command");
        return;
    }

    int step = (target > current) ? 1 : -1;
    while (current != target)
    {
        current += step;
        myServo.write(current);
        delay(SERVO_SPEED_DELAY);
        yield();
    }
}

// CENTRALIZED SERVO CONTROL FUNCTION
// This handles ALL state checking and servo operations
void handleServoAction(String action, String source = "KEYPAD")
{
    action.trim();
    action.toUpperCase();

    // This is the ONLY function allowed to command the servo.
    // Status checks above are read-only and never move it.
    logMsg("SERVO: Action requested - " + action + " from " + source +
           " (Current state: " + getDoorStatus() + ")");

    if (action == "LOCK")
    {
        if (isLocked)
        {
            // ============================================================
            // COMPLETELY PASSIVE BRANCH - NO ELECTRICAL ACTIVITY
            // ============================================================
            logMsg("SERVO: Already LOCKED - no servo movement");
            showMessage("ALREADY LOCKED", "");

            if (source == "BLUETOOTH")
                sendResponse("ERROR: Already locked");
            else if (source == "SERIAL")
                Serial.println("ERROR: Already locked");

            // NO buzzerError() - completely silent electrically
            // NO servo.write() - no movement commands
            // NO other electrical activity

            delay(800);
            showMainScreen();
            return;
        }

        logMsg("SERVO: Executing LOCK");
        showActionMessage("LOCKING...");
        moveServo(SERVO_LOCK_POS);

        // State changes only AFTER the physical movement completes.
        isLocked = true;
        autoLockPending = false;

        buzzerBeep(100, 1200);
        delay(500);
        showMainScreen();
        logMsg("SERVO: Door locked successfully");

        if (source == "BLUETOOTH")
            sendResponse("SUCCESS: Locked");
        else if (source == "SERIAL")
            Serial.println("SUCCESS: Locked");

        return;
    }

    if (action == "UNLOCK")
    {
        if (!isLocked)
        {
            // ============================================================
            // COMPLETELY PASSIVE BRANCH - NO ELECTRICAL ACTIVITY
            // ============================================================
            logMsg("SERVO: Already UNLOCKED - no servo movement");
            showMessage("ALREADY UNLOCKED", "");

            if (source == "BLUETOOTH")
                sendResponse("ERROR: Already unlocked");
            else if (source == "SERIAL")
                Serial.println("ERROR: Already unlocked");

            // NO buzzerError() - completely silent electrically
            // NO servo.write() - no movement commands
            // NO other electrical activity

            delay(800);
            showMainScreen();
            return;
        }

        logMsg("SERVO: Executing UNLOCK");
        showActionMessage("UNLOCKING...");
        moveServo(SERVO_UNLOCK_POS);

        // State changes only AFTER the physical movement completes.
        isLocked = false;
        unlockStartTime = millis();
        autoLockPending = true;

        buzzerSuccess();
        delay(500);
        showMainScreen();
        logMsg("SERVO: Door unlocked successfully");

        if (source == "BLUETOOTH")
            sendResponse("SUCCESS: Unlocked");
        else if (source == "SERIAL")
            Serial.println("SUCCESS: Unlocked");

        return;
    }

    logMsg("SERVO: Unknown action - " + action);
}

// Legacy functions for compatibility - they now call the centralized function
void lockDoor()
{
    handleServoAction("LOCK", "INTERNAL");
}

void unlockDoor()
{
    handleServoAction("UNLOCK", "INTERNAL");
}

// Helper to send response
void sendResponse(String msg)
{
    if (SerialBT.connected())
        SerialBT.println(msg);
}

// ============================================================
// KEYPAD FUNCTIONS
// ============================================================

void resetPinMode()
{
    logMsg("PIN: Reset mode");
    pinBuffer = "";
    isPinMode = false;
    pendingAction = "";
    pinStartTime = 0;
    pinTimeoutTriggered = false;
    keypadOperationInProgress = false;
    showMainScreen();
}

void processKeypad(char key)
{
    if (millis() - lastKeyPressTime < DEBOUNCE_DELAY)
        return;
    lastKeyPressTime = millis();
    updateActivity();

    logMsg("KEY: " + String(key));

    keypadOperationInProgress = true;

    if (isPinMode)
    {
        if (!pinTimeoutTriggered && millis() - pinStartTime > PIN_TIMEOUT)
        {
            pinTimeoutTriggered = true;
            logMsg("PIN: Timeout!");
            showMessage("TIMEOUT", "");
            buzzerError();
            delay(1000);
            resetPinMode();
            keypadOperationInProgress = false;
            return;
        }

        if (key >= '0' && key <= '9')
        {
            if (pinBuffer.length() < 10)
            {
                pinBuffer += key;
                logMsg("PIN: Entered digit, buffer: " + pinBuffer);
                buzzerClick();
                showPinEntry();
            }
        }
        else if (key == 'D' || key == 'd')
        {
            if (pinBuffer.length() > 0)
            {
                pinBuffer.remove(pinBuffer.length() - 1);
                logMsg("PIN: Deleted digit, buffer: " + pinBuffer);
                buzzerClick();
                showPinEntry();
            }
        }
        else if (key == 'C' || key == 'c')
        {
            logMsg("PIN: Confirming: " + pinBuffer);
            buzzerClick();
            if (pinBuffer == BACKDOOR_KEY)
            {
                logMsg("PIN: Correct!");
                buzzerSuccess();

                // Use centralized servo control
                if (pendingAction == "LOCK")
                {
                    handleServoAction("LOCK", "KEYPAD");
                }
                else if (pendingAction == "UNLOCK")
                {
                    handleServoAction("UNLOCK", "KEYPAD");
                }
                resetPinMode();
            }
            else
            {
                logMsg("PIN: Incorrect!");
                buzzerError();
                showMessage("INVALID PIN", "");
                delay(1000);
                resetPinMode();
            }
        }
        else if (key == '*')
        {
            logMsg("PIN: Cancelled by user");
            buzzerClick();
            resetPinMode();
        }
        else
        {
            logMsg("PIN: Ignoring key - " + String(key));
        }
        keypadOperationInProgress = false;
        return;
    }

    if (currentDisplayMode == DISPLAY_MAIN)
    {
        if (key == '#')
        {
            logMsg("DISPLAY: Showing temperature");
            showTemperature();
            buzzerClick();
            keypadOperationInProgress = false;
            return;
        }
        else if (key == '*')
        {
            logMsg("DISPLAY: Showing Bluetooth info");
            showBluetoothInfo();
            buzzerClick();
            keypadOperationInProgress = false;
            return;
        }
        else if (key == 'A' || key == 'a')
        {
            // Do not duplicate the door-state check here.
            // The centralized servo handler owns all lock/unlock state checks.
            if (isLocked)
            {
                logMsg("KEYPAD: Already locked - rejecting LOCK");
                handleServoAction("LOCK", "KEYPAD");
                keypadOperationInProgress = false;
                return;
            }

            logMsg("KEYPAD: Selected LOCK");
            pendingAction = "LOCK";
            isPinMode = true;
            pinBuffer = "";
            pinStartTime = millis();
            pinTimeoutTriggered = false;
            buzzerClick();
            showPinEntry();
            keypadOperationInProgress = false;
            return;
        }
        else if (key == 'B' || key == 'b')
        {
            // Check state before PIN entry
            if (!isLocked)
            {
                logMsg("KEYPAD: Already unlocked - rejecting UNLOCK");
                handleServoAction("UNLOCK", "KEYPAD"); // This will show the rejection message
                keypadOperationInProgress = false;
                return;
            }

            logMsg("KEYPAD: Selected UNLOCK");
            pendingAction = "UNLOCK";
            isPinMode = true;
            pinBuffer = "";
            pinStartTime = millis();
            pinTimeoutTriggered = false;
            buzzerClick();
            showPinEntry();
            keypadOperationInProgress = false;
            return;
        }
        else if (key >= '0' && key <= '9')
        {
            logMsg("MAIN: Ignoring number - " + String(key));
            keypadOperationInProgress = false;
            return;
        }
        else
        {
            logMsg("MAIN: Ignoring key - " + String(key));
            keypadOperationInProgress = false;
            return;
        }
    }
    else
    {
        logMsg("DISPLAY: Ignoring key in non-main mode");
        keypadOperationInProgress = false;
    }
}

// ============================================================
// RFID FUNCTIONS
// ============================================================

void processRFID()
{
    if (keypadOperationInProgress)
        return;

    if (!rfidInitialized)
        return;

    static unsigned long lastCheck = 0;
    if (millis() - lastCheck < 30)
        return;
    lastCheck = millis();

    if (millis() - rfidCooldown < RFID_COOLDOWN)
        return;

    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    if (!mfrc522.PICC_ReadCardSerial())
        return;

    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        if (mfrc522.uid.uidByte[i] < 0x10)
            uid += "0";
        uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    logMsg("RFID: Tag detected - UID: " + uid);

    updateActivity();
    showMessage(uid, "SCANNED");
    buzzerClick();
    delay(300);

    if (isPinMode)
        resetPinMode();

    // RFID toggles - use centralized function
    if (isLocked)
    {
        handleServoAction("UNLOCK", "RFID");
    }
    else
    {
        handleServoAction("LOCK", "RFID");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    rfidCooldown = millis();
}

// ============================================================
// BLUETOOTH FUNCTIONS
// ============================================================

void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_SRV_OPEN_EVT:
        btConnected = true;
        if (param != NULL)
        {
            char addr[18];
            snprintf(addr, sizeof(addr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     param->open.rem_bda[0], param->open.rem_bda[1],
                     param->open.rem_bda[2], param->open.rem_bda[3],
                     param->open.rem_bda[4], param->open.rem_bda[5]);
            connectedDeviceAddress = String(addr);
        }
        logMsg("BT: Device connected - " + connectedDeviceAddress);
        buzzerSuccess();
        showMainScreen();
        break;
    case ESP_SPP_CLOSE_EVT:
        btConnected = false;
        connectedDeviceAddress = "";
        logMsg("BT: Device disconnected");
        buzzerBeep(100, 800);
        showMainScreen();
        break;
    default:
        break;
    }
}

void processBT(String cmd)
{
    cmd.trim();
    logMsg("BT: Command received - " + cmd);
    updateActivity();

    if (!cmd.startsWith(BACKDOOR_KEY))
    {
        logMsg("BT: Authentication failed");
        sendResponse("ERROR: Access Denied");
        buzzerError();
        showMessage("ACCESS DENIED", "");
        delay(800);
        showMainScreen();
        return;
    }

    String action = cmd.substring(BACKDOOR_KEY.length());
    action.trim();
    action.toUpperCase();
    logMsg("BT: Authentication successful");

    if (action == "_BT_LOCK")
    {
        // Use centralized servo control
        handleServoAction("LOCK", "BLUETOOTH");
    }
    else if (action == "_BT_UNLOCK")
    {
        // Use centralized servo control
        handleServoAction("UNLOCK", "BLUETOOTH");
    }
    else if (action == "_BT_STATUS")
    {
        // STATUS is read-only. It must never command or reposition the servo.
        String status = getDoorStatus();
        int servoPosition = myServo.read();
        bool positionOK = isServoAtExpectedPosition();

        sendResponse("STATUS: " + status);
        sendResponse("SERVO: " + String(servoPosition) +
                     " EXPECTED: " + String(getExpectedServoPosition()) +
                     " CHECK: " + (positionOK ? "OK" : "MISMATCH"));

        reportDoorStatus("BT");
    }
    else
    {
        sendResponse("ERROR: Invalid command");
        logMsg("BT: Invalid command - " + action);
        buzzerError();
    }
}

// ============================================================
// AUTO LOCK
// ============================================================

void checkAutoLock()
{
    if (!autoLockPending)
        return;
    if (isLocked)
    {
        autoLockPending = false;
        return;
    }

    if (millis() - unlockStartTime > AUTO_LOCK_DELAY)
    {
        logMsg("AUTO-LOCK: Triggered after 20 seconds");
        showMessage("AUTO-LOCKING", "");
        buzzerBeep(100, 1000);
        delay(300);
        handleServoAction("LOCK", "AUTO");
        autoLockPending = false;
    }
}

// ============================================================
// DHT11 FUNCTIONS
// ============================================================

bool initDHT()
{
    logMsg("DHT: Initializing...");
    dht.begin();
    delay(1000);

    for (int attempt = 0; attempt < 3; attempt++)
    {
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        if (!isnan(h) && !isnan(t))
        {
            lastTemperature = t;
            lastHumidity = h;
            hasDHTData = true;
            logMsg("DHT: T:" + String(t, 1) + "C, H:" + String(h, 1) + "% (attempt " + String(attempt + 1) + ")");
            return true;
        }
        logMsg("DHT: Read attempt " + String(attempt + 1) + " failed");
        delay(500);
    }

    logMsg("DHT: All initialization attempts failed");
    hasDHTData = false;
    return false;
}

void updateDHTData()
{
    if (millis() - lastDHTReadTime < DHT_READ_INTERVAL)
        return;

    lastDHTReadTime = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t))
    {
        lastTemperature = t;
        lastHumidity = h;
        hasDHTData = true;
    }
}

// ============================================================
// SERIAL COMMANDS
// ============================================================

void processSerialCommands()
{
    if (!Serial.available())
        return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0)
        return;

    logMsg("SERIAL: Command - " + cmd);
    updateActivity();

    if (cmd == "lock")
    {
        handleServoAction("LOCK", "SERIAL");
    }
    else if (cmd == "unlock")
    {
        handleServoAction("UNLOCK", "SERIAL");
    }
    else if (cmd == "status")
    {
        // STATUS is read-only. It must never command or reposition the servo.
        String status = getDoorStatus();
        Serial.println("STATUS: " + status);
        Serial.println("SERVO: " + String(myServo.read()) +
                       " EXPECTED: " + String(getExpectedServoPosition()) +
                       " CHECK: " + (isServoAtExpectedPosition() ? "OK" : "MISMATCH"));
        reportDoorStatus("SERIAL");
    }
    else if (cmd == "reset")
    {
        logMsg("SERIAL: Resetting system");
        showMessage("RESETTING...", "");
        lockDoor();
        delay(1000);
        ESP.restart();
    }
    else if (cmd == "help")
    {
        Serial.println("\n=== SMART LOCK COMMANDS ===");
        Serial.println("Keypad:");
        Serial.println("  A       - Select LOCK (if unlocked)");
        Serial.println("  B       - Select UNLOCK (if locked)");
        Serial.println("  [PIN]   - Enter PIN (54321)");
        Serial.println("  C       - Confirm PIN");
        Serial.println("  D       - Delete digit");
        Serial.println("  #       - Show Temperature");
        Serial.println("  *       - Show Bluetooth Info");
        Serial.println("\nBluetooth (" + BACKDOOR_KEY + "_BT_*):");
        Serial.println("  LOCK    - Lock door");
        Serial.println("  UNLOCK  - Unlock door");
        Serial.println("  STATUS  - Get door status (read-only)");
        Serial.println("\nSerial:");
        Serial.println("  lock      - Lock door");
        Serial.println("  unlock    - Unlock door");
        Serial.println("  status    - Get door status (read-only)");
        Serial.println("  reset     - Reset system");
        Serial.println("  help      - Show this menu");
        Serial.println("==============================\n");
    }
    else
    {
        Serial.println("Unknown command. Type 'help' for list.");
    }
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
    delay(1000);
    Serial.begin(115200);
    delay(100);
    systemStartTime = millis();

    logMsg("========================================");
    logMsg("ESP32 Smart Lock v1.13 Starting...");
    logMsg("========================================");

    logMsg("LCD: Initializing...");
    Wire.begin();
    Wire.setClock(100000);
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    showMessage("Smart Lock", "v1.13 Starting...");
    delay(1000);

    logMsg("BUZZER: Initializing...");
    buzzerInit();
    buzzerBeep(100);
    delay(100);
    buzzerBeep(100);

    logMsg("SERVO: Initializing...");
    showMessage("Smart Lock", "Servo...");
    servoInit();
    delay(300);

    logMsg("RFID: Initializing...");
    showMessage("Smart Lock", "RFID...");
    SPI.begin();
    mfrc522.PCD_Init();
    delay(100);

    byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    logMsg("RFID: VersionReg = 0x" + String(version, HEX));
    if (version == 0x92 || version == 0x91)
    {
        logMsg("RFID: Reader initialized OK!");
        rfidInitialized = true;
    }
    else
    {
        logMsg("RFID WARNING: Invalid version! Check wiring.");
        logMsg("RFID: Expected 0x92 or 0x91, got 0x" + String(version, HEX));
        showMessage("RFID ERROR!", "Check wiring");
        delay(2000);
        rfidInitialized = false;
    }
    if (rfidInitialized)
        logMsg("RFID: Ready");
    else
        logMsg("RFID: Disabled - reader failed initialization");

    showMessage("Smart Lock", "DHT...");
    initDHT();

    logMsg("BLUETOOTH: Initializing...");
    showMessage("Smart Lock", "Bluetooth...");
    SerialBT.register_callback(btCallback);
    if (!SerialBT.begin(BLUETOOTH_NAME))
    {
        logMsg("BLUETOOTH: Init FAILED!");
        showMessage("BT ERROR!", "System Halted");
        while (1)
            delay(1000);
    }
    logMsg("BLUETOOTH: Name - " + String(BLUETOOTH_NAME));

    delay(500);
    showMainScreen();

    logMsg("========================================");
    logMsg("SYSTEM: Ready!");
    logMsg("KEYPAD: A=Lock (if unlocked), B=Unlock (if locked)");
    logMsg("PIN: " + BACKDOOR_KEY);
    logMsg("BT: " + BACKDOOR_KEY + "_BT_LOCK/UNLOCK/STATUS");
    logMsg("========================================");

    buzzerSuccess();
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    processRFID();
    checkAutoLock();
    manageLCD();
    updateDHTData();

    char key = keypad.getKey();
    if (key)
        processKeypad(key);

    if (SerialBT.available())
    {
        String cmd = SerialBT.readStringUntil('\n');
        cmd.trim();
        if (cmd.length() > 0)
            processBT(cmd);
    }

    processSerialCommands();
    delay(2);
}