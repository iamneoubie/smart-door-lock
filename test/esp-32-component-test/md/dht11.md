/*
 * DHT11 Ultra Simple Test - ESP32 GPIO17
 */

#include <DHT.h>

#define DHT_PIN 17
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    dht.begin();
    Serial.println("DHT11 Test Started - GPIO17");
}

void loop()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
        Serial.println("Sensor Error!");
    }
    else
    {
        Serial.print("Temperature: ");
        Serial.print(t, 1);
        Serial.print(" C, Humidity: ");
        Serial.print(h, 1);
        Serial.println("%");
    }

    delay(2000);
}