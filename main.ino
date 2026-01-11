#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <DHT.h>
#include "BluetoothSerial.h"

// ================= CONFIG =================
#define DEVICE_ID   "esp32-001"
#define API_REPORT  "http://your-api.com/iot/gas/report"
#define API_COMMAND "http://your-api.com/iot/gas/command?device_id=esp32-001"

// ================= PIN =================
#define MQ2_PIN     34
#define DHT_PIN     32
#define BUZZER_PIN  25
#define DHT_TYPE    DHT22

// ================= GLOBAL =================
Preferences prefs;
BluetoothSerial SerialBT;
DHT dht(DHT_PIN, DHT_TYPE);

bool wifiConnected = false;
bool setupMode = false;

bool buzzerEnabled = true;
bool buzzerState = false;
int gasThreshold = 3000;

unsigned long lastSensor = 0;
unsigned long lastCommand = 0;
unsigned long lastBeep = 0;

// ================= WIFI =================
void connectWiFi() {
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");

  Serial.println("\nTrying WiFi...");
  Serial.println("SSID: [" + ssid + "]");

  if (ssid == "") {
    Serial.println("No SSID saved → Setup Mode");
    setupMode = true;
    wifiConnected = false;
    return;
  }

  WiFi.mode(WIFI_OFF);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  unsigned long start = millis();
  while (millis() - start < 20000) {
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      setupMode = false;
      Serial.println("\nWiFi CONNECTED");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      return;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi FAILED → Setup Mode");
  wifiConnected = false;
  setupMode = true;
}

// ================= BLUETOOTH =================
void handleBluetooth() {
  if (!setupMode) return;
  if (!SerialBT.available()) return;

  String cmd = SerialBT.readStringUntil('\n');
  cmd.trim();

  if (cmd.startsWith("WIFI:")) {
    int c = cmd.indexOf(',');
    if (c > 5) {
      String ssid = cmd.substring(5, c);
      String pass = cmd.substring(c + 1);

      prefs.putString("ssid", ssid);
      prefs.putString("pass", pass);

      SerialBT.println("WiFi saved, restarting...");
      delay(1000);
      ESP.restart();
    }
  }

  if (cmd == "RESET") {
    prefs.clear();
    SerialBT.println("WiFi cleared, restarting...");
    delay(1000);
    ESP.restart();
  }
}

// ================= SENSOR =================
void readSensor() {
  int gas = analogRead(MQ2_PIN);
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // ===== BUZZER BEEP-BEEP =====
  if (gas > gasThreshold && buzzerEnabled) {
    if (millis() - lastBeep > 500) {
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
      lastBeep = millis();
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
  }

  // ===== API REPORT =====
  if (wifiConnected) {
    HTTPClient http;
    http.begin(API_REPORT);
    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"device_id\":\"" DEVICE_ID "\",";
    payload += "\"gas\":" + String(gas) + ",";
    payload += "\"temperature\":" + String(t) + ",";
    payload += "\"humidity\":" + String(h);
    payload += "}";

    http.POST(payload);
    http.end();
  }

  Serial.printf(
    "Gas:%d T:%.1f H:%.1f Buzzer:%s\n",
    gas, t, h, buzzerEnabled ? "ON" : "OFF"
  );
}

// ================= COMMAND =================
void fetchCommand() {
  if (!wifiConnected) return;

  HTTPClient http;
  http.begin(API_COMMAND);

  if (http.GET() == 200) {
    String res = http.getString();

    if (res.indexOf("\"buzzer\":false") > 0) buzzerEnabled = false;
    if (res.indexOf("\"buzzer\":true") > 0)  buzzerEnabled = true;

    int idx = res.indexOf("\"threshold\":");
    if (idx > 0) {
      gasThreshold = res.substring(idx + 12).toInt();
    }
  }
  http.end();
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // ==== WAJIB UNTUK HINDARI WIFI ERROR ====
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  WiFi.persistent(false);
  delay(2000);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();
  prefs.begin("wifi", false);

  connectWiFi();

  if (setupMode) {
    SerialBT.begin("ESP32-WIFI-SETUP");
    Serial.println("Bluetooth setup mode");
  }
}

// ================= LOOP =================
void loop() {
  handleBluetooth();

  if (millis() - lastSensor > 2000) {
    readSensor();
    lastSensor = millis();
  }

  if (millis() - lastCommand > 5000) {
    fetchCommand();
    lastCommand = millis();
  }
}
