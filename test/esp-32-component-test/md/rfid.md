#include <SPI.h>
#include <MFRC522.h>

// Pin definitions (match your wiring)
#define SS_PIN 5  // SDA
#define RST_PIN 4 // RST

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup()
{
    Serial.begin(115200);              // Initialize serial
    SPI.begin();                       // Init SPI bus
    mfrc522.PCD_Init();                // Init RC522
    mfrc522.PCD_DumpVersionToSerial(); // Show firmware version (optional)
    Serial.println("RFID reader ready. Place a tag near the antenna.");
}

void loop()
{
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return; // No card, go back to loop (very fast)
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return; // Failed to read, restart
    }

    // Card found – show its UID
    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Print card type (optional)
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print("Card type: ");
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Halt PICC and stop encryption (cleanup)
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Small delay to avoid spamming the same card
    delay(500);
}