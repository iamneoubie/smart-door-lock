/*
 * ESP32 Buzzer - GPIO 16
 * Simple buzzer: 1 second ON, 1 second OFF
 * 
 * WHY THIS WORKS:
 * - Uses ledcAttach() which is compatible with ESP32 Arduino Core 3.3.11
 * - Direct pin control works with older ESP32 cores
 * - GPIO 16 handled correctly with this approach
 * 
 * WHY OTHER SYNTAX FAILS:
 * - ledcSetup() + ledcAttachPin() requires newer core (4.x+)
 * - Channel-based PWM (ledcWrite(CHANNEL)) may not map correctly on GPIO 16
 * - GPIO 16 has special hardware conflicts that this method avoids
 */

#include <Arduino.h>

#define BUZZER_PIN 16      // GPIO pin for buzzer
#define BUZZER_FREQ 1500   // Frequency in Hz (audible tone)

void setup()
{
    Serial.begin(115200);
    
    /*
     * ledcAttach(pin, freq, resolution)
     * 
     * WHY THIS SPECIFIC SYNTAX:
     * - Compatible with ESP32 Arduino Core 3.3.11 (your version)
     * - Binds the pin directly to PWM with the specified frequency
     * - Resolution: 8-bit (0-255) is standard for audio
     * 
     * WHY ledcSetup() + ledcAttachPin() FAILS:
     * - These functions were introduced in newer ESP32 cores (4.x+)
     * - Your core (3.3.11) doesn't have them - causes compile errors
     * - "error: 'ledcSetup' was not declared in this scope"
     * 
     * WHY CHANNEL-BASED APPROACH MIGHT FAIL:
     * - ledcWrite(CHANNEL, value) uses channel numbers (0-15)
     * - GPIO 16 may not map correctly to channel 0 on some boards
     * - Direct pin control (ledcWrite(PIN)) is more reliable
     */
    ledcAttach(BUZZER_PIN, BUZZER_FREQ, 8);
    
    Serial.println("Buzzer: 1s ON, 1s OFF");
}

void loop()
{
    /*
     * ledcWriteTone(pin, frequency)
     * WHY USE THIS:
     * - Generates continuous tone at specified frequency
     * - Direct pin control - works with ledcAttach()
     * - Simpler than calculating duty cycle manually
     * 
     * WHY ledcWriteNote() MIGHT FAIL:
     * - Musical note function requires newer core
     * - Not available in ESP32 Core 3.3.11
     */
    ledcWriteTone(BUZZER_PIN, BUZZER_FREQ);
    Serial.println("ON");
    delay(1000);  // 1 second ON
    
    /*
     * ledcWrite(pin, 0)
     * WHY THIS:
     * - Sets PWM duty cycle to 0 (0% = OFF)
     * - Direct pin control works reliably
     * - 8-bit resolution means 0 = fully off
     * 
     * WHY digitalWrite(LOW) MIGHT NOT WORK:
     * - Once PWM is enabled, digitalWrite may not override it
     * - GPIO 16 might stay in PWM mode
     * - ledcWrite() properly controls the PWM signal
     */
    ledcWrite(BUZZER_PIN, 0);
    Serial.println("OFF");
    delay(1000);  // 1 second OFF
}

/*
 * IF GPIO 16 STILL DOESN'T WORK:
 * 
 * WHY GPIO 16 IS PROBLEMATIC:
 * 1. PSRAM conflict (uses GPIO 16 & 17 if enabled)
 * 2. Special hardware pin with limited functions
 * 3. May need additional initialization
 * 
 * SOLUTIONS:
 * 1. Try these pins instead: #define BUZZER_PIN 26
 *    (GPIO 26, 27, 32, 33 work well)
 * 
 * 2. Check PSRAM is disabled:
 *    Tools → PSRAM → "Disabled"
 * 
 * 3. Alternative with delay after setup:
 *    ledcAttach(BUZZER_PIN, BUZZER_FREQ, 8);
 *    delay(50);  // Give hardware time to initialize
 */