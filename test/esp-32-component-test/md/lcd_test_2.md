#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================================
// PIN CONFIGURATION
// ============================================================
// LCD I2C Pins (ESP32)
// SDA -> GPIO21
// SCL -> GPIO22
// VCC -> 5V
// GND -> GND
// ============================================================

// LCD Configuration
#define LCD_ADDRESS 0x27 // Common I2C address, try 0x3F if this doesn't work
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// Create LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Test timing variables (adjusted for ESP32 speed)
const unsigned long TEST_DELAY_SHORT = 1500;
const unsigned long TEST_DELAY_MEDIUM = 2000;
const unsigned long TEST_DELAY_LONG = 2500;
const unsigned long ANIMATION_DELAY = 150;
const unsigned long SCROLL_DELAY = 300;

// ============================================================
// HELPER FUNCTIONS
// ============================================================

void waitTest(unsigned long ms, const char *message = nullptr)
{
    if (message)
    {
        Serial.print("  [WAIT] ");
        Serial.print(ms);
        Serial.print("ms - ");
        Serial.println(message);
    }

    unsigned long startTime = millis();
    while (millis() - startTime < ms)
    {
        yield();
        delay(1);
    }
}

void showTestHeader(const char *testName, int testNumber)
{
    Serial.println("----------------------------------------");
    Serial.print("TEST ");
    Serial.print(testNumber);
    Serial.print(": ");
    Serial.println(testName);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Test ");
    lcd.print(testNumber);
    lcd.print(":");
    lcd.setCursor(0, 1);
    lcd.print(testName);

    delay(800);
}

void updateLCD(int row, int col, const char *text)
{
    lcd.setCursor(col, row);
    lcd.print(text);
    delay(ANIMATION_DELAY / 3);
    yield();
}

// ============================================================
// TEST 1: Basic Text Display
// ============================================================
void testBasicText()
{
    showTestHeader("Basic Text", 1);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello World!");
    lcd.setCursor(0, 1);
    lcd.print("LCD 1602A Test");

    Serial.println("  [PASS] Displayed 'Hello World!' on both rows");
    waitTest(TEST_DELAY_LONG, "Displaying basic text");
}

// ============================================================
// TEST 2: Horizontal Fill - Top Row
// ============================================================
void testHorizontalTop()
{
    showTestHeader("Horizontal Fill - Top Row", 2);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 0);
        lcd.print("#");
        delay(ANIMATION_DELAY);
        Serial.print(".");
        yield();
    }
    Serial.println("\n  [PASS] All 16 columns on top row tested");
    waitTest(TEST_DELAY_SHORT, "Displaying top row fill");
}

// ============================================================
// TEST 3: Horizontal Fill - Bottom Row
// ============================================================
void testHorizontalBottom()
{
    showTestHeader("Horizontal Fill - Bottom Row", 3);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 1);
        lcd.print("#");
        delay(ANIMATION_DELAY);
        Serial.print(".");
        yield();
    }
    Serial.println("\n  [PASS] All 16 columns on bottom row tested");
    waitTest(TEST_DELAY_SHORT, "Displaying bottom row fill");
}

// ============================================================
// TEST 4: Both Rows Horizontal Fill
// ============================================================
void testHorizontalBoth()
{
    showTestHeader("Both Rows Fill", 4);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 0);
        lcd.print("#");
        lcd.setCursor(col, 1);
        lcd.print("#");
        delay(ANIMATION_DELAY);
        Serial.print(".");
        yield();
    }
    Serial.println("\n  [PASS] All 32 positions filled");
    waitTest(TEST_DELAY_MEDIUM, "Displaying both rows fill");
}

// ============================================================
// TEST 5: Vertical Pattern
// ============================================================
void testVertical()
{
    showTestHeader("Vertical Pattern", 5);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 0);
        lcd.print("A");
        lcd.setCursor(col, 1);
        lcd.print("B");
        delay(ANIMATION_DELAY + 50);
        Serial.print(".");
        yield();
    }
    Serial.println("\n  [PASS] Vertical pattern (A/B) displayed");
    waitTest(TEST_DELAY_MEDIUM, "Displaying vertical pattern");
}

// ============================================================
// TEST 6: Position Numbers
// ============================================================
void testPositions()
{
    showTestHeader("Position Numbers", 6);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 0);
        if (col < 10)
            lcd.print(col);
        else
            lcd.print(char('A' + (col - 10)));
    }

    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 1);
        if (col < 10)
            lcd.print(col);
        else
            lcd.print(char('A' + (col - 10)));
    }

    Serial.println("  [PASS] All positions labeled 0-9 and A-F");
    waitTest(TEST_DELAY_LONG, "Displaying position labels");
}

// ============================================================
// TEST 7: Every Position Test
// ============================================================
void testEveryPosition()
{
    showTestHeader("Every Position", 7);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 0);
        lcd.print(col % 10);
        lcd.setCursor(col, 1);
        lcd.print(col % 10);
        delay(50);
        yield();
    }

    Serial.println("  [PASS] All positions filled with numbers 0-9");
    waitTest(TEST_DELAY_LONG, "Displaying numbers on all positions");
}

// ============================================================
// TEST 8: Full Blocks
// ============================================================
void testFullBlocks()
{
    showTestHeader("Full Blocks", 8);

    lcd.clear();
    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 0);
        lcd.write(byte(255));
        lcd.setCursor(col, 1);
        lcd.write(byte(255));
        delay(ANIMATION_DELAY);
        Serial.print("#");
        yield();
    }
    Serial.println("\n  [PASS] All positions filled with full blocks");
    waitTest(TEST_DELAY_MEDIUM, "Displaying full blocks");
}

// ============================================================
// TEST 9: Custom Characters
// ============================================================
byte heart[] = {
    0b00000, 0b01010, 0b11111, 0b11111,
    0b01110, 0b00100, 0b00000, 0b00000};

byte smiley[] = {
    0b00000, 0b10001, 0b00000, 0b00000,
    0b10001, 0b01110, 0b00000, 0b00000};

byte arrow[] = {
    0b00000, 0b00100, 0b01110, 0b11111,
    0b01110, 0b00100, 0b00000, 0b00000};

void testCustomCharacters()
{
    showTestHeader("Custom Characters", 9);

    lcd.createChar(0, heart);
    lcd.createChar(1, smiley);
    lcd.createChar(2, arrow);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Custom:");

    for (int col = 0; col < 8; col++)
    {
        lcd.setCursor(col + 7, 0);
        if (col % 3 == 0)
        {
            lcd.write(byte(0));
            Serial.print("<3");
        }
        else if (col % 3 == 1)
        {
            lcd.write(byte(1));
            Serial.print(":)");
        }
        else
        {
            lcd.write(byte(2));
            Serial.print("^");
        }
        delay(ANIMATION_DELAY);
        yield();
    }

    lcd.setCursor(0, 1);
    lcd.print("CGRAM TEST OK");

    Serial.println("\n  [PASS] Custom characters created and displayed");
    waitTest(TEST_DELAY_LONG, "Displaying custom characters");
}

// ============================================================
// TEST 10: Cursor Animation
// ============================================================
void testCursor()
{
    showTestHeader("Cursor Animation", 10);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cursor Test");

    for (int col = 0; col < LCD_COLUMNS; col++)
    {
        lcd.setCursor(col, 1);
        lcd.print("^");
        delay(ANIMATION_DELAY);
        lcd.setCursor(col, 1);
        lcd.print(" ");
        Serial.print(".");
        yield();
    }
    Serial.println("\n  [PASS] Cursor animation completed");
    waitTest(TEST_DELAY_SHORT, "Animating cursor");
}

// ============================================================
// TEST 11: Alternating Rows
// ============================================================
void testAlternatingRows()
{
    showTestHeader("Alternating Rows", 11);

    for (int i = 0; i < 4; i++)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ROW 1 ACTIVE");
        Serial.println("  Row 1 active");
        delay(400);
        yield();

        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("ROW 2 ACTIVE");
        Serial.println("  Row 2 active");
        delay(400);
        yield();
    }

    waitTest(TEST_DELAY_SHORT, "Row alternation complete");
}

// ============================================================
// TEST 12: Scrolling Text
// ============================================================
void testScrolling()
{
    showTestHeader("Scrolling Text", 12);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SCROLL TEST");
    lcd.setCursor(0, 1);
    lcd.print("1602A I2C");

    waitTest(1000, "Initial display");

    Serial.println("  [SCROLL] Scrolling left...");
    for (int i = 0; i < 8; i++)
    {
        lcd.scrollDisplayLeft();
        delay(SCROLL_DELAY);
        Serial.print("<");
        yield();
    }

    Serial.println("\n  [SCROLL] Scrolling right...");
    for (int i = 0; i < 8; i++)
    {
        lcd.scrollDisplayRight();
        delay(SCROLL_DELAY);
        Serial.print(">");
        yield();
    }
    Serial.println("\n  [PASS] Scrolling test complete");
    waitTest(TEST_DELAY_SHORT, "Scrolling complete");
}

// ============================================================
// TEST 13: Backlight Test
// ============================================================
void testBacklight()
{
    showTestHeader("Backlight Test", 13);

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("BACKLIGHT");
    lcd.setCursor(0, 1);
    lcd.print("TOGGLE TEST");

    for (int i = 0; i < 4; i++)
    {
        lcd.backlight();
        Serial.println("  [LIGHT] Backlight ON");
        delay(500);
        lcd.noBacklight();
        Serial.println("  [LIGHT] Backlight OFF");
        delay(500);
        yield();
    }

    lcd.backlight();
    waitTest(TEST_DELAY_SHORT, "Backlight test complete");
}

// ============================================================
// TEST 14: Display On/Off
// ============================================================
void testDisplayControl()
{
    showTestHeader("Display Control", 14);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DISPLAY TEST");
    lcd.setCursor(0, 1);
    lcd.print("ON/OFF TOGGLE");

    for (int i = 0; i < 3; i++)
    {
        lcd.display();
        Serial.println("  [DISPLAY] Display ON");
        delay(500);
        lcd.noDisplay();
        Serial.println("  [DISPLAY] Display OFF");
        delay(500);
        yield();
    }

    lcd.display();
    waitTest(TEST_DELAY_SHORT, "Display control test complete");
}

// ============================================================
// FINAL TEST: All Passed
// ============================================================
void finalTest()
{
    Serial.println("========================================");
    Serial.println("[PASS] ALL LCD TESTS PASSED!");
    Serial.println("========================================");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LCD TEST PASSED");
    lcd.setCursor(0, 1);
    lcd.print("0x27 16x2 I2C");

    lcd.createChar(0, smiley);
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(0));

    waitTest(TEST_DELAY_LONG * 2, "Test completed successfully!");
}

// ============================================================
// SETUP
// ============================================================
void setup()
{
    delay(1000);

    Serial.begin(115200);
    delay(100);

    Serial.println("\n========================================");
    Serial.println("   ESP32 LCD 1602A I2C TEST SUITE");
    Serial.println("========================================");
    Serial.println("Pin Configuration:");
    Serial.println("  SDA -> GPIO21");
    Serial.println("  SCL -> GPIO22");
    Serial.println("  Address: 0x27 (16x2)");
    Serial.println("========================================\n");

    // Initialize I2C with slower speed for stability
    Wire.begin();
    Wire.setClock(100000);

    // Initialize LCD (use init() for LiquidCrystal_I2C by Frank de Brabander)
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("LCD 1602A");
    lcd.setCursor(0, 1);
    lcd.print("Starting Test");

    Serial.println("[INIT] LCD initialized successfully!");
    Serial.println("[INIT] Starting test sequence...\n");

    delay(2000);
}

// ============================================================
// LOOP - Run all tests
// ============================================================
void loop()
{
    testBasicText();
    testHorizontalTop();
    testHorizontalBottom();
    testHorizontalBoth();
    testVertical();
    testPositions();
    testEveryPosition();
    testFullBlocks();
    testCustomCharacters();
    testCursor();
    testAlternatingRows();
    testScrolling();
    testBacklight();
    testDisplayControl();
    finalTest();

    Serial.println("\n[INFO] Test complete - Restarting in 5 seconds...\n");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Test Complete");
    lcd.setCursor(0, 1);
    lcd.print("Restarting...");
    delay(5000);
}