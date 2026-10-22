#line 1 "D:\\Repository\\smart-door-lock\\test\\esp-32-component-test\\md\\network-firebase.md"
/*
 * WiFi Connectivity Test
 * ======================
 * Standalone diagnostic for the Smart Lock hardware.
 * Mirrors the WiFi pattern from the Internet-of-Things project.
 *
 * Prints:
 *   - Every WiFi event (connect, got IP, disconnect, fail)
 *   - Signal strength (RSSI) once connected
 *   - A DNS + HTTPS probe to Firebase once connected
 *   - A periodic heartbeat so you can see drops in real time
 *
 * No other hardware is touched. No LCD, no keypad, no servo.
 * Just plug in, open Serial Monitor @ 115200, read the log.
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>

// ---- Same credentials as your IoT project ----
#define WIFI_SSID       "Router 5G"
#define WIFI_PASSWORD   "admin123"

const char *FIREBASE_URL  = "https://smart-lock-d7f5a-default-rtdb.firebaseio.com";
const char *FIREBASE_AUTH = "tYosT0qMFtbKevswgzyg37MEOyVDOMm8wdohGzWL";

// ---- Tuning ----
const unsigned long HEARTBEAT_INTERVAL_MS = 5000;   // status line every 5s
const unsigned long RECONNECT_RETRY_MS    = 10000;  // retry connect every 10s
const unsigned long CONNECT_TIMEOUT_MS    = 15000;  // single attempt limit

// ---- State ----
bool wifiReady = false;
unsigned long lastHeartbeat = 0;
unsigned long lastRetryAttempt = 0;
unsigned long connectStartedAt = 0;
bool connectInProgress = false;
bool firstFirebaseProbeDone = false;

// ============================================================
// WiFi event log — we get told what happens, not just poll it
// ============================================================
void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("[EVT] STA_START       - station mode started");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[EVT] STA_CONNECTED  - associated with AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("[EVT] STA_GOT_IP     - IP acquired");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("[EVT] STA_DISCONNECTED - lost connection");
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("[EVT] STA_LOST_IP    - IP released");
        break;
    default:
        // Ignore the noisy AP/scan events; we only run STA.
        break;
    }
}

// ============================================================
// Human-readable WiFi status
// ============================================================
const char* wifiStatusStr(wl_status_t s) {
    switch (s) {
    case WL_IDLE_STATUS:     return "IDLE";
    case WL_NO_SSID_AVAIL:   return "NO_SSID_AVAIL (SSID not found)";
    case WL_SCAN_COMPLETED:  return "SCAN_COMPLETED";
    case WL_CONNECTED:       return "CONNECTED";
    case WL_CONNECT_FAILED:  return "CONNECT_FAILED (bad password?)";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED:    return "DISCONNECTED";
    default:                 return "UNKNOWN";
    }
}

// ============================================================
// Non-blocking connect kick-off
// ============================================================
void startWiFiConnect() {
    Serial.printf("[WIFI] Connecting to \"%s\" ...\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);           // helps on some routers
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    connectStartedAt = millis();
    connectInProgress = true;
}

// ============================================================
// One-shot HTTPS probe to Firebase to prove internet works
// ============================================================
void probeFirebase() {
    Serial.println("[HTTP] Probing Firebase REST endpoint ...");
    NetworkClientSecure client;
    client.setInsecure();   // skip cert validation for diagnostic
    HTTPClient http;
    String url = String(FIREBASE_URL) + "/sensors/status.json?auth=" + FIREBASE_AUTH;
    http.begin(client, url);
    http.setTimeout(5000);
    int code = http.GET();
    Serial.printf("[HTTP] GET /sensors/status.json -> HTTP %d\n", code);
    if (code > 0) {
        String body = http.getString();
        body.trim();
        if (body.length() > 80) body = body.substring(0, 80) + "...";
        Serial.println("[HTTP] Body: " + body);
    } else {
        Serial.printf("[HTTP] Error: %s\n", http.errorToString(code).c_str());
    }
    http.end();
    firstFirebaseProbeDone = true;
}

// ============================================================
// setup
// ============================================================
void setup() {
    delay(500);
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("========================================");
    Serial.println(" WiFi Connectivity Test");
    Serial.println("========================================");
    Serial.printf("SSID     : %s\n", WIFI_SSID);
    Serial.printf("Password : %s\n", WIFI_PASSWORD);
    Serial.printf("Firebase : %s\n", FIREBASE_URL);
    Serial.println("----------------------------------------");

    // Register event handler BEFORE begin()
    WiFi.onEvent(onWiFiEvent);

    // Clear any prior persistent config that could confuse the join
    WiFi.persistent(false);
    WiFi.disconnect(true, true);   // disconnect + erase stored credentials
    delay(200);

    // Show what networks are visible so we can confirm the SSID exists
    Serial.println("[SCAN] Scanning for networks ...");
    int n = WiFi.scanNetworks();
    Serial.printf("[SCAN] Found %d network(s):\n", n);
    for (int i = 0; i < n; i++) {
        Serial.printf("   %2d) %-24s  RSSI %4d dBm  ch %2d  %s\n",
                      i + 1,
                      WiFi.SSID(i).c_str(),
                      WiFi.RSSI(i),
                      WiFi.channel(i),
                      (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "SECURED");
    }
    if (n == 0) {
        Serial.println("   (none — router off, out of range, or 5 GHz only)");
    }
    Serial.println("----------------------------------------");

    // Kick off the first connection attempt
    startWiFiConnect();
}

// ============================================================
// loop
// ============================================================
void loop() {
    unsigned long now = millis();

    // ---- Handle connection lifecycle ----
    wl_status_t st = WiFi.status();

    if (st == WL_CONNECTED && !wifiReady) {
        wifiReady = true;
        connectInProgress = false;
        Serial.println("[WIFI] *** CONNECTED ***");
        Serial.printf("[WIFI] IP      : %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WIFI] Gateway : %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("[WIFI] Subnet  : %s\n", WiFi.subnetMask().toString().c_str());
        Serial.printf("[WIFI] DNS     : %s\n", WiFi.dnsIP().toString().c_str());
        Serial.printf("[WIFI] RSSI    : %d dBm\n", WiFi.RSSI());
        Serial.printf("[WIFI] Channel : %d\n", WiFi.channel());
        Serial.printf("[WIFI] BSSID   : %s\n", WiFi.BSSIDstr().c_str());
        Serial.printf("[WIFI] Hostname: %s\n", WiFi.getHostname());
    }

    if (st != WL_CONNECTED && wifiReady) {
        wifiReady = false;
        Serial.println("[WIFI] *** DISCONNECTED ***");
    }

    // If a connect attempt has been outstanding too long, report and retry
    if (connectInProgress && !wifiReady) {
        if (now - connectStartedAt > CONNECT_TIMEOUT_MS) {
            Serial.printf("[WIFI] Attempt timed out after %lus — status: %s\n",
                          (now - connectStartedAt) / 1000,
                          wifiStatusStr(st));
            connectInProgress = false;
        }
    }

    // Retry on a fixed cadence — only when not already trying
    if (!wifiReady && !connectInProgress && (now - lastRetryAttempt >= RECONNECT_RETRY_MS)) {
        lastRetryAttempt = now;
        Serial.println("[WIFI] Retrying connection ...");
        WiFi.disconnect();          // clean slate, but keep stored creds
        delay(50);
        startWiFiConnect();
    }

    // ---- Periodic heartbeat ----
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = now;
        if (wifiReady) {
            Serial.printf("[HB] connected  RSSI %4d dBm  IP %s  heap %u\n",
                          WiFi.RSSI(),
                          WiFi.localIP().toString().c_str(),
                          (unsigned)ESP.getFreeHeap());
        } else {
            Serial.printf("[HB] offline    status: %s  heap %u\n",
                          wifiStatusStr(st),
                          (unsigned)ESP.getFreeHeap());
        }
    }

    // ---- One-shot Firebase probe once we have an IP ----
    if (wifiReady && !firstFirebaseProbeDone) {
        probeFirebase();
    }
}