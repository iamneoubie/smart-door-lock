#line 1 "D:\\Repository\\smart-door-lock\\test\\esp-32-component-test\\md\\servo.md"
#include <ESP32Servo.h>

Servo myServo;

// Servo positions
const int OPEN_POS = 0;    // Open position (0 degrees)
const int CLOSE_POS = 180; // Close position (180 degrees)

void setup()
{
    Serial.begin(115200);
    myServo.attach(2); // Signal pin to GPIO2

    Serial.println("ESP32 Servo Ready!");

    // Start with servo at close position
    myServo.write(CLOSE_POS);
    delay(1000);
}

void loop()
{
    // Open
    Serial.println("Opening...");
    openBuzzer();
    delay(2000);

    // Close
    Serial.println("Closing...");
    closeBuzzer();
    delay(2000);
}

// Function to open the buzzer
void openBuzzer()
{
    myServo.write(OPEN_POS);
    delay(500); // Allow time for servo to reach position
}

// Function to close the buzzer
void closeBuzzer()
{
    myServo.write(CLOSE_POS);
    delay(500); // Allow time for servo to reach position
}