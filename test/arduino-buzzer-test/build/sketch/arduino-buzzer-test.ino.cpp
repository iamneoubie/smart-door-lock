#include <Arduino.h>
#line 1 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
// ============================================================
//              ARDUINO BUZZER SOUND TEST
// ============================================================

const int BUZZER_PIN = 8; // Change buzzer pin here

// Adjustable frequencies
const int LOW_FREQ = 300;
const int MID_FREQ = 1000;
const int HIGH_FREQ = 2000;

// ============================================================
// SETUP
// ============================================================

#line 16 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void setup();
#line 38 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void loop();
#line 113 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void playTone(int frequency, int duration);
#line 131 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void pauseBetweenSounds();
#line 141 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound1_singleLow();
#line 153 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound2_singleMid();
#line 165 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound3_singleHigh();
#line 177 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound4_doubleBeep();
#line 191 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound5_tripleBeep();
#line 207 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound6_rapidBeep();
#line 223 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound7_slowBeep();
#line 239 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound8_risingTone();
#line 261 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound9_fallingTone();
#line 283 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound10_risingFalling();
#line 307 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound11_siren();
#line 335 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound12_alarm();
#line 355 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound13_fastAlarm();
#line 374 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound14_heartbeat();
#line 394 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound15_morseSOS();
#line 429 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound16_threeTone();
#line 447 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound17_frequencySweepUp();
#line 466 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound18_frequencySweepDown();
#line 485 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound19_policeSiren();
#line 502 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void sound20_success();
#line 16 "D:\\Repository\\smart-door-lock\\test\\arduino-buzzer-test\\arduino-buzzer-test.ino"
void setup()
{
    Serial.begin(9600);

    pinMode(BUZZER_PIN, OUTPUT);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("        BUZZER SOUND TEST");
    Serial.println("==========================================");

    Serial.print("Buzzer Pin: ");
    Serial.println(BUZZER_PIN);

    Serial.println("Starting 20 sound tests...");
    Serial.println();
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop()
{

    sound1_singleLow();
    pauseBetweenSounds();

    sound2_singleMid();
    pauseBetweenSounds();

    sound3_singleHigh();
    pauseBetweenSounds();

    sound4_doubleBeep();
    pauseBetweenSounds();

    sound5_tripleBeep();
    pauseBetweenSounds();

    sound6_rapidBeep();
    pauseBetweenSounds();

    sound7_slowBeep();
    pauseBetweenSounds();

    sound8_risingTone();
    pauseBetweenSounds();

    sound9_fallingTone();
    pauseBetweenSounds();

    sound10_risingFalling();
    pauseBetweenSounds();

    sound11_siren();
    pauseBetweenSounds();

    sound12_alarm();
    pauseBetweenSounds();

    sound13_fastAlarm();
    pauseBetweenSounds();

    sound14_heartbeat();
    pauseBetweenSounds();

    sound15_morseSOS();
    pauseBetweenSounds();

    sound16_threeTone();
    pauseBetweenSounds();

    sound17_frequencySweepUp();
    pauseBetweenSounds();

    sound18_frequencySweepDown();
    pauseBetweenSounds();

    sound19_policeSiren();
    pauseBetweenSounds();

    sound20_success();
    pauseBetweenSounds();

    Serial.println();
    Serial.println("========== ALL TESTS COMPLETE ==========");
    Serial.println("Restarting...");
    Serial.println();

    delay(2000);
}

// ============================================================
// BASIC BUZZER FUNCTION
// ============================================================

void playTone(int frequency, int duration)
{

    Serial.print("  ");
    Serial.print(frequency);
    Serial.print(" Hz / ");
    Serial.print(duration);
    Serial.println(" ms");

    tone(BUZZER_PIN, frequency);
    delay(duration);
    noTone(BUZZER_PIN);
}

// ============================================================
// PAUSE
// ============================================================

void pauseBetweenSounds()
{
    noTone(BUZZER_PIN);
    delay(800);
}

// ============================================================
// SOUND 1 - LOW TONE
// ============================================================

void sound1_singleLow()
{

    Serial.println("[1] LOW TONE");

    playTone(300, 800);
}

// ============================================================
// SOUND 2 - MID TONE
// ============================================================

void sound2_singleMid()
{

    Serial.println("[2] MID TONE");

    playTone(1000, 800);
}

// ============================================================
// SOUND 3 - HIGH TONE
// ============================================================

void sound3_singleHigh()
{

    Serial.println("[3] HIGH TONE");

    playTone(2000, 800);
}

// ============================================================
// SOUND 4 - DOUBLE BEEP
// ============================================================

void sound4_doubleBeep()
{

    Serial.println("[4] DOUBLE BEEP");

    playTone(1500, 150);
    delay(150);
    playTone(1500, 150);
}

// ============================================================
// SOUND 5 - TRIPLE BEEP
// ============================================================

void sound5_tripleBeep()
{

    Serial.println("[5] TRIPLE BEEP");

    for (int i = 0; i < 3; i++)
    {
        playTone(1500, 120);
        delay(120);
    }
}

// ============================================================
// SOUND 6 - RAPID BEEP
// ============================================================

void sound6_rapidBeep()
{

    Serial.println("[6] RAPID BEEP");

    for (int i = 0; i < 10; i++)
    {
        playTone(2000, 70);
        delay(70);
    }
}

// ============================================================
// SOUND 7 - SLOW BEEP
// ============================================================

void sound7_slowBeep()
{

    Serial.println("[7] SLOW BEEP");

    for (int i = 0; i < 4; i++)
    {
        playTone(800, 200);
        delay(400);
    }
}

// ============================================================
// SOUND 8 - RISING TONE
// ============================================================

void sound8_risingTone()
{

    Serial.println("[8] RISING TONE");

    for (int frequency = 300; frequency <= 2500; frequency += 100)
    {

        Serial.print("  Frequency: ");
        Serial.println(frequency);

        tone(BUZZER_PIN, frequency);
        delay(30);
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 9 - FALLING TONE
// ============================================================

void sound9_fallingTone()
{

    Serial.println("[9] FALLING TONE");

    for (int frequency = 2500; frequency >= 300; frequency -= 100)
    {

        Serial.print("  Frequency: ");
        Serial.println(frequency);

        tone(BUZZER_PIN, frequency);
        delay(30);
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 10 - RISING + FALLING
// ============================================================

void sound10_risingFalling()
{

    Serial.println("[10] RISING + FALLING");

    for (int frequency = 300; frequency <= 2500; frequency += 100)
    {
        tone(BUZZER_PIN, frequency);
        delay(25);
    }

    for (int frequency = 2500; frequency >= 300; frequency -= 100)
    {
        tone(BUZZER_PIN, frequency);
        delay(25);
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 11 - SIREN
// ============================================================

void sound11_siren()
{

    Serial.println("[11] SIREN");

    for (int i = 0; i < 4; i++)
    {

        for (int frequency = 600; frequency <= 1800; frequency += 30)
        {
            tone(BUZZER_PIN, frequency);
            delay(10);
        }

        for (int frequency = 1800; frequency >= 600; frequency -= 30)
        {
            tone(BUZZER_PIN, frequency);
            delay(10);
        }
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 12 - ALARM
// ============================================================

void sound12_alarm()
{

    Serial.println("[12] ALARM");

    for (int i = 0; i < 8; i++)
    {

        playTone(2200, 150);
        delay(100);

        playTone(700, 150);
        delay(100);
    }
}

// ============================================================
// SOUND 13 - FAST ALARM
// ============================================================

void sound13_fastAlarm()
{

    Serial.println("[13] FAST ALARM");

    for (int i = 0; i < 15; i++)
    {

        tone(BUZZER_PIN, (i % 2 == 0) ? 2500 : 800);
        delay(80);
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 14 - HEARTBEAT
// ============================================================

void sound14_heartbeat()
{

    Serial.println("[14] HEARTBEAT");

    for (int i = 0; i < 5; i++)
    {

        playTone(900, 100);
        delay(100);

        playTone(700, 100);
        delay(350);
    }
}

// ============================================================
// SOUND 15 - MORSE SOS
// ============================================================

void sound15_morseSOS()
{

    Serial.println("[15] MORSE CODE - SOS");

    // S = ...
    for (int i = 0; i < 3; i++)
    {
        playTone(1200, 120);
        delay(100);
    }

    delay(200);

    // O = ---
    for (int i = 0; i < 3; i++)
    {
        playTone(1200, 350);
        delay(100);
    }

    delay(200);

    // S = ...
    for (int i = 0; i < 3; i++)
    {
        playTone(1200, 120);
        delay(100);
    }
}

// ============================================================
// SOUND 16 - THREE TONE
// ============================================================

void sound16_threeTone()
{

    Serial.println("[16] THREE TONE");

    playTone(500, 300);
    delay(100);

    playTone(1000, 300);
    delay(100);

    playTone(2000, 300);
}

// ============================================================
// SOUND 17 - FREQUENCY SWEEP UP
// ============================================================

void sound17_frequencySweepUp()
{

    Serial.println("[17] FREQUENCY SWEEP UP");

    for (int frequency = 100; frequency <= 5000; frequency += 50)
    {

        tone(BUZZER_PIN, frequency);
        delay(10);
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 18 - FREQUENCY SWEEP DOWN
// ============================================================

void sound18_frequencySweepDown()
{

    Serial.println("[18] FREQUENCY SWEEP DOWN");

    for (int frequency = 5000; frequency >= 100; frequency -= 50)
    {

        tone(BUZZER_PIN, frequency);
        delay(10);
    }

    noTone(BUZZER_PIN);
}

// ============================================================
// SOUND 19 - POLICE SIREN
// ============================================================

void sound19_policeSiren()
{

    Serial.println("[19] POLICE SIREN");

    for (int i = 0; i < 6; i++)
    {

        playTone(700, 250);
        playTone(1800, 250);
    }
}

// ============================================================
// SOUND 20 - SUCCESS
// ============================================================

void sound20_success()
{

    Serial.println("[20] SUCCESS");

    playTone(800, 120);
    delay(80);

    playTone(1200, 120);
    delay(80);

    playTone(1800, 250);
}
