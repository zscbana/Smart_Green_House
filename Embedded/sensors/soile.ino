#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// WiFi credentials
const char* ssid = "-";
const char* password = "-";

// InfluxDB configuration
const String influxDB_URL = "http://192.168.1.2:8086/api/v2/write?org=omar&bucket=esp32&precision=s";
const String influxDB_Token = "MUl4RVDRv79quQivZx_R9C2OH93_q6d0DejjazdaZIGke4yqmBl2sbas0dWGOM6uy2dOX_dVSxdEQQKsJ3bEHA==";

// Pins
#define soil_moisture_pin 34  // ADC1 pin (GPIO34)
#define led_pin 17
#define dry_soil_led 27

// Threshold
#define moisture_threshold 3000

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  pinMode(dry_soil_led, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Configure time (for InfluxDB timestamp)
  configTime(0, 0, "pool.ntp.org");  // Get time from NTP
}

void loop() {
  // Robust WiFi reconnection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconnected!");
    } else {
      Serial.println("\nFailed to reconnect!");
      return;  // Skip loop iteration if offline
    }
  }

  // Read sensor (in millivolts)
  int soil_moisture_value = analogReadMilliVolts(soil_moisture_pin);
  Serial.print("Moisture (mV): ");
  Serial.println(soil_moisture_value);

  // Control LEDs
  if (soil_moisture_value < moisture_threshold) {
    digitalWrite(dry_soil_led, HIGH);
    digitalWrite(led_pin, LOW);
  } else {
    digitalWrite(dry_soil_led, LOW);
    digitalWrite(led_pin, HIGH);
  }

  // Send to InfluxDB
  if (soil_moisture_value > 0) {
    sendToInfluxDB(soil_moisture_value);
  } else {
    Serial.println("Invalid moisture reading");
  }

  delay(5000);
}

void sendToInfluxDB(int moisture_value) {
  HTTPClient http;
  String payload = "soil_moisture,location=field1 value=" + String(moisture_value) + " " + String(time(nullptr));

  http.begin(influxDB_URL);
  http.addHeader("Authorization", "Token " + influxDB_Token);
  http.addHeader("Content-Type", "text/plain");

  int httpCode = http.POST(payload);
  
  if (httpCode == HTTP_CODE_NO_CONTENT || httpCode == HTTP_CODE_OK) {  // 204 or 200
    Serial.println("Data sent to InfluxDB!");
  } else {
    Serial.print("InfluxDB Error: ");
    Serial.println(httpCode);
  }
  http.end();
}