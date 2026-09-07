#include <Keypad.h>

// LED pin for visual feedback
#define LED_PIN 2 // Built-in LED on most ESP32 boards

// Keypad pin definitions (matching your wiring)
const byte ROWS = 4;
const byte COLS = 4;

// Keymap for 4x4 keypad
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// GPIO pins for rows (R1-R4)
byte rowPins[ROWS] = {13, 12, 14, 27}; // GPIO13, GPIO12, GPIO14, GPIO27

// GPIO pins for columns (C1-C4)
byte colPins[COLS] = {26, 25, 33, 32}; // GPIO26, GPIO25, GPIO33, GPIO32

// Create Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables for key press debouncing and timing
unsigned long lastKeyPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 50; // milliseconds

void setup()
{
    // CRITICAL FIX: Add delay BEFORE Serial begins
    delay(1000); // Give time for Serial monitor to connect

    // Initialize Serial communication
    Serial.begin(115200); // Higher baud rate for ESP32

    // Wait for Serial to be ready (ESP32 specific)
    unsigned long startTime = millis();
    while (!Serial && millis() - startTime < 3000)
    {
        delay(10); // Wait up to 3 seconds for USB connection
    }

    delay(100); // Extra stability delay

    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Print startup information
    Serial.println("\n========================================");
    Serial.println("    ESP32 4x4 Matrix Keypad Test");
    Serial.println("========================================");
    Serial.println("Pin Configuration:");
    Serial.println("  Rows:  GPIO13, GPIO12, GPIO14, GPIO27");
    Serial.println("  Cols:  GPIO26, GPIO25, GPIO33, GPIO32");
    Serial.println("----------------------------------------");
    Serial.println("Press any key on the keypad...");
    Serial.println("========================================\n");

    // Test LED at startup
    startupBlink();
}

void loop()
{
    // Get the key pressed (non-blocking)
    char key = keypad.getKey();

    // If a key is pressed, process it
    if (key)
    {
        handleKeyPress(key);
    }

    // Optional: Add a small delay to prevent CPU hogging
    delay(10);
}

/**
 * Handle a single key press
 */
void handleKeyPress(char key)
{
    unsigned long currentTime = millis();

    // Simple debouncing
    if (currentTime - lastKeyPressTime < DEBOUNCE_DELAY)
    {
        return;
    }
    lastKeyPressTime = currentTime;

    // Print key information with timestamp
    Serial.print("[");
    Serial.print(millis());
    Serial.print(" ms] Key Pressed: ");
    Serial.print(key);

    // Identify key type
    if (key >= '0' && key <= '9')
    {
        Serial.println(" (Numeric)");
    }
    else if (key == '*' || key == '#')
    {
        Serial.println(" (Special)");
    }
    else if (key >= 'A' && key <= 'D')
    {
        Serial.println(" (Function)");
    }
    else
    {
        Serial.println(" (Unknown)");
    }

    // Visual feedback - blink LED
    blinkLED(100);

    // Additional action based on specific keys
    performKeyAction(key);
}

/**
 * Perform specific actions for certain keys
 */
void performKeyAction(char key)
{
    switch (key)
    {
    case 'A':
        Serial.println("  → Action: Lock System");
        // Add your lock code here
        break;
    case 'B':
        Serial.println("  → Action: Unlock System");
        // Add your unlock code here
        break;
    case 'C':
        Serial.println("  → Action: Toggle Feature");
        // Add your toggle code here
        break;
    case 'D':
        Serial.println("  → Action: Reset System");
        resetSystem();
        break;
    case '*':
        Serial.println("  → Action: Clear/Back");
        // Add clear/back code here
        break;
    case '#':
        Serial.println("  → Action: Enter/Confirm");
        // Add enter/confirm code here
        break;
    default:
        // For numeric keys, simulate entering a password
        if (key >= '0' && key <= '9')
        {
            Serial.print("  → Entering digit: ");
            Serial.println(key);
            // Add digit entry code here
        }
        break;
    }
}

/**
 * Check for long key holds (optional feature)
 */
void checkKeyHold()
{
    // This is a placeholder for key hold detection
    // You can implement this using keypad.isPressed() and keypad.getState()
    // Example:
    /*
    if (keypad.isPressed()) {
        static unsigned long pressStartTime = 0;
        static char pressedKey = 0;

        char currentKey = keypad.getKey();
        if (currentKey && pressedKey != currentKey) {
            pressedKey = currentKey;
            pressStartTime = millis();
        }

        if (pressedKey && millis() - pressStartTime > 2000) {
            Serial.print("Long press detected on key: ");
            Serial.println(pressedKey);
            pressStartTime = millis();  // Reset to avoid repeated triggers
        }
    }
    */
}

/**
 * Blink LED for visual feedback
 */
void blinkLED(int duration)
{
    digitalWrite(LED_PIN, HIGH);
    delay(duration);
    digitalWrite(LED_PIN, LOW);
}

/**
 * Startup blink sequence
 */
void startupBlink()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }
}

/**
 * System reset function
 */
void resetSystem()
{
    Serial.println("  → System Reset!");
    // Perform any reset operations here
    // For example, clear buffer, reset variables, etc.
    startupBlink();

    // Optional: ESP32 software reset
    // ESP.restart();  // Uncomment if you want to restart ESP32
}

/**
 * Optional: Function to read entire keypad state
 * Returns a 4x4 matrix of key states
 */
void readKeypadState()
{
    Serial.println("\n--- Keypad State Scan ---");
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            Serial.print("Key [");
            Serial.print(row);
            Serial.print(",");
            Serial.print(col);
            Serial.print("]: ");
            Serial.print(keys[row][col]);
            Serial.print("  ");
        }
        Serial.println();
    }
    Serial.println("--------------------------");
}

/**
 * Print available commands (for serial monitor)
 */
void printHelp()
{
    Serial.println("\n--- Available Commands ---");
    Serial.println("  Keys 0-9: Enter digits");
    Serial.println("  Key A: Lock System");
    Serial.println("  Key B: Unlock System");
    Serial.println("  Key C: Toggle Feature");
    Serial.println("  Key D: Reset System");
    Serial.println("  Key *: Clear/Back");
    Serial.println("  Key #: Enter/Confirm");
    Serial.println("--------------------------\n");
}

// Uncomment this if you want to read keypad state periodically
/*
void periodicKeypadScan() {
    static unsigned long lastScanTime = 0;
    if (millis() - lastScanTime > 5000) {  // Every 5 seconds
        readKeypadState();
        lastScanTime = millis();
    }
}
*/