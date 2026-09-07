#line 1 "D:\\Repository\\smart-door-lock\\test\\esp-32-component-test\\md\\network-firebase.md"
#include <WiFi.h>
#include <FirebaseESP32.h> // Example library, check which one you're using

// --- Define Wi-Fi Variables ---
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"

// --- Define Firebase Variables (Email/Password example) ---
#define API_KEY "YourApiKey"
#define DATABASE_URL "YourDatabaseURL" // For Realtime Database
#define USER_EMAIL "YourFirebaseUserEmail"
#define USER_PASSWORD "YourFirebaseUserPassword"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Connected!");

  // Configure Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}