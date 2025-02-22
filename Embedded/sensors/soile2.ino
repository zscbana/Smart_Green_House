#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <DHT.h>  // Include DHT sensor library

// WiFi credentials
const char* ssid = "Update me in case of proaction";
const char* password = "Update me in case of proaction";

// InfluxDB configuration
const String influxDB_URL = "Update me in case of proaction";
const String influxDB_Token = "Update me in case of proaction";

// Pins
#define soil_moisture_pin 34  // ADC1 pin (GPIO34)
#define ldr_pin 19  // GPIO19 for LDR sensor
#define led_pin 17
#define dry_soil_led 27
#define DHT_PIN 18  // GPIO18 for DHT11 data pin

// Threshold
#define moisture_threshold 3000

// Initialize DHT sensor
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  pinMode(dry_soil_led, OUTPUT);
  dht.begin();  // Start DHT sensor

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

  // Read soil moisture sensor (in millivolts)
  int soil_moisture_value = analogReadMilliVolts(soil_moisture_pin);
  Serial.print("Moisture (mV): ");
  Serial.println(soil_moisture_value);

  // Read LDR sensor (in millivolts)
  int light_value = analogReadMilliVolts(ldr_pin);
  Serial.print("Light Level (mV): ");
  Serial.println(light_value);

  // Read DHT11 sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Print DHT11 readings
  Serial.print("Temperature (°C): ");
  Serial.println(temperature);
  Serial.print("Humidity (%): ");
  Serial.println(humidity);

  // Control LEDs
  if (soil_moisture_value < moisture_threshold) {
    digitalWrite(dry_soil_led, HIGH);
    digitalWrite(led_pin, LOW);
  } else {
    digitalWrite(dry_soil_led, LOW);
    digitalWrite(led_pin, HIGH);
  }

  // Send to InfluxDB
  if (!isnan(temperature) && !isnan(humidity)) {
    sendToInfluxDB(soil_moisture_value, light_value, temperature, humidity);
  } else {
    Serial.println("Invalid temperature/humidity readings");
  }

  delay(2000);
}

void sendToInfluxDB(int moisture_value, int light_value, float temperature, float humidity) {
  HTTPClient http;
  
  // Send all data in the same bucket with different fields
  String payload = "environment_data,location=field1 soil_moisture=" + String(moisture_value) +
                   ",light_level=" + String(light_value) +
                   ",temperature=" + String(temperature) +
                   ",humidity=" + String(humidity) +
                   " " + String(time(nullptr));

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
