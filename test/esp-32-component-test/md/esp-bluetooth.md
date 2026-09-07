#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const char *Bluetooth_Broadcast = "Smart Lock";
const String Backdoor_Key = "54321";

bool connected = false;
String connectedDevice = "";
unsigned long startTime = 0;

String getTimestamp()
{
    unsigned long elapsed = millis() - startTime;
    unsigned long seconds = elapsed / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return String(timeStr);
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_SRV_OPEN_EVT:
        connected = true;
        if (param != NULL)
        {
            char addr[18];
            snprintf(addr, sizeof(addr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     param->open.rem_bda[0], param->open.rem_bda[1],
                     param->open.rem_bda[2], param->open.rem_bda[3],
                     param->open.rem_bda[4], param->open.rem_bda[5]);
            connectedDevice = String(addr);
        }
        else
        {
            connectedDevice = "Unknown Device";
        }

        Serial.println();
        Serial.println("[Connected " + getTimestamp() + "] " + connectedDevice);
        Serial.println("[Status " + getTimestamp() + "] Connection established");
        Serial.println("[Status " + getTimestamp() + "] Ready for commands");
        break;

    case ESP_SPP_CLOSE_EVT:
        connected = false;
        Serial.println();
        Serial.println("[Disconnected " + getTimestamp() + "] " + connectedDevice);
        Serial.println("[Status " + getTimestamp() + "] Connection lost");
        Serial.println("[Status " + getTimestamp() + "] Waiting for connection...");
        connectedDevice = "";
        break;

    default:
        break;
    }
}

void processCommand(String command)
{
    command.trim();
    
    Serial.println("[Received " + getTimestamp() + "] " + command);
    
    if (command.startsWith(Backdoor_Key))
    {
        String action = command.substring(Backdoor_Key.length());
        action.trim();
        
        Serial.println("[Key " + getTimestamp() + "] Verification passed");
        
        if (action == "_BT_LOCK")
        {
            Serial.println("[Success " + getTimestamp() + "] Locking Door");
            SerialBT.println("SUCCESS: Door Locked");
        }
        else if (action == "_BT_UNLOCK")
        {
            Serial.println("[Success " + getTimestamp() + "] Unlocking Door");
            SerialBT.println("SUCCESS: Door Unlocked");
        }
        else
        {
            Serial.println("[Failed " + getTimestamp() + "] Invalid command: " + action);
            Serial.println("[Hint " + getTimestamp() + "] Use " + Backdoor_Key + "_BT_LOCK or " + Backdoor_Key + "_BT_UNLOCK");
            SerialBT.println("ERROR: Invalid command format");
        }
    }
    else
    {
        Serial.println("[Failed " + getTimestamp() + "] Access Denied");
        Serial.println("[Security " + getTimestamp() + "] Invalid key attempted");
        SerialBT.println("ERROR: Access Denied");
    }
}

void setup()
{
    Serial.begin(115200);
    startTime = millis();

    Serial.println();
    Serial.println("[System " + getTimestamp() + "] ESP32 Smart Lock Initializing");
    Serial.println("[System " + getTimestamp() + "] Backdoor Key: " + Backdoor_Key);
    Serial.println("[System " + getTimestamp() + "] Starting Bluetooth...");

    SerialBT.register_callback(callback);

    if (!SerialBT.begin(Bluetooth_Broadcast))
    {
        Serial.println("[Error " + getTimestamp() + "] Bluetooth failed to start");
        Serial.println("[Error " + getTimestamp() + "] System halted");
        return;
    }

    Serial.println("[System " + getTimestamp() + "] Bluetooth started");
    Serial.println("[System " + getTimestamp() + "] Device Name: " + String(Bluetooth_Broadcast));
    Serial.println("[System " + getTimestamp() + "] Waiting for connection...");
}

void loop()
{
    if (SerialBT.available())
    {
        String received = SerialBT.readStringUntil('\n');
        received.trim();
        processCommand(received);
    }

    if (Serial.available())
    {
        String message = Serial.readStringUntil('\n');
        message.trim();

        if (message.length() > 0)
        {
            if (connected)
            {
                SerialBT.println(message);
                Serial.println("[Sent " + getTimestamp() + "] " + message);
            }
            else
            {
                Serial.println("[Error " + getTimestamp() + "] Not connected - cannot send");
            }
        }
    }

    delay(10);
}