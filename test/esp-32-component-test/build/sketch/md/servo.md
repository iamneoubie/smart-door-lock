#line 1 "D:\\Repository\\smart-door-lock\\test\\esp-32-component-test\\md\\servo.md"
#include <ESP32Servo.h>

Servo myServo;

// ============================================================
// SERVO PIN
// ============================================================
#define SERVO_PIN 2 // Signal pin to GPIO2

// ============================================================
// SERVO POSITIONS
// Matches Smart Lock v1.17 convention:
//   0°   = LOCKED / CLOSED
//   180° = UNLOCKED / OPEN
// ============================================================
const int LOCK_POS = 120; // Locked (closed) position
const int UNLOCK_POS = 0; // Unlocked (open) position

// ============================================================
// SETUP
// ============================================================
void setup()
{
    Serial.begin(115200);
    myServo.attach(SERVO_PIN);

    Serial.println("ESP32 Servo Ready!");

    // Start with servo at LOCKED position
    myServo.write(LOCK_POS);
    delay(1000);
}

// ============================================================
// LOOP
// ============================================================
void loop()
{
    // Unlock / Open
    Serial.println("Opening (UNLOCK)...");
    openDoor();
    delay(2000);

    // Lock / Close
    Serial.println("Closing (LOCK)...");
    closeDoor();
    delay(2000);
}

// ============================================================
// SERVO FUNCTIONS
// ============================================================

// Move servo to UNLOCKED (open) position
void openDoor()
{
    myServo.write(UNLOCK_POS);
    delay(500); // Allow time for servo to reach position
}

// Move servo to LOCKED (closed) position
void closeDoor()
{
    myServo.write(LOCK_POS);
    delay(500); // Allow time for servo to reach position
}