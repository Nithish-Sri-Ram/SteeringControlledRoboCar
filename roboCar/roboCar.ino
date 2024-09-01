#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

int Fpin1 = D5; // IN1
int Fpin2 = D6; // IN2
int Bpin1 = D7; // IN3
int Bpin2 = D8; // IN4

const char* ssid = "Next Tech Lab";
const char* password = "ntlapW203";
const char* endpoint = "https://steerbot-6he48xydx-nithishs-projects-8d26ce03.vercel.app/receive";
const char* send_sensor_endpoint = "https://steerbot-66p9sjh6r-nithishs-projects-8d26ce03.vercel.app/send_sensor";


WiFiClientSecure wifiClient;
HTTPClient http;
unsigned long previousMillis = 0;
const long interval = 50;  // Polling interval to avoid overwhelming the server

void setMotorDirection(int accelerator, int brake, int steering) {
    if (accelerator == -1 && brake == -1) {
        digitalWrite(Fpin1, LOW);
        digitalWrite(Fpin2, LOW);
        digitalWrite(Bpin1, LOW);
        digitalWrite(Bpin2, LOW);
    } else if (brake > -1) {
        digitalWrite(Fpin1, LOW);
        digitalWrite(Fpin2, HIGH);
        digitalWrite(Bpin1, HIGH);
        digitalWrite(Bpin2, LOW);
    } else if (accelerator > -1) {
        if (steering < 0) {
            digitalWrite(Fpin1, LOW);
            digitalWrite(Fpin2, HIGH);
            digitalWrite(Bpin1, LOW);
            digitalWrite(Bpin2, HIGH);
        } else if (steering == 0.97265625) {
            digitalWrite(Fpin1, HIGH);
            digitalWrite(Fpin2, LOW);
            digitalWrite(Bpin1, HIGH);
            digitalWrite(Bpin2, HIGH);
        } else {
            digitalWrite(Fpin1, HIGH);
            digitalWrite(Fpin2, LOW);
            digitalWrite(Bpin1, LOW);
            digitalWrite(Bpin2, HIGH);
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(Fpin1, OUTPUT);
    pinMode(Fpin2, OUTPUT);
    pinMode(Bpin1, OUTPUT);
    pinMode(Bpin2, OUTPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);  // Reduced delay for faster retries
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    wifiClient.setInsecure();
    http.setReuse(true);
        if (WiFi.status() == WL_CONNECTED) {
        float sensorValue = analogRead(A0);  // Replace with actual sensor reading
        Serial.printf("Sensor value read: %f\n", sensorValue);  // Debugging line

        http.begin(wifiClient, send_sensor_endpoint);  // Initialize HTTP connection for POST request

        StaticJsonDocument<200> jsonDoc;
        jsonDoc["sensor"] = sensorValue;
        String jsonData;
        serializeJson(jsonDoc, jsonData);

        Serial.printf("Sending JSON data: %s\n", jsonData.c_str());  // Debugging line

        int httpCode = http.POST(jsonData);  // Send the POST request

        if (httpCode > 0) {
            String response = http.getString();
            Serial.println("Sensor data sent successfully: " + response);
        } else {
            Serial.printf("Failed to send sensor data: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();  // Close the connection after the request
    } else {
        Serial.println("Failed to connect to WiFi, skipping sensor data send.");
    }
    http.begin(wifiClient, endpoint);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (WiFi.status() == WL_CONNECTED) {
            int httpCode = http.GET();
            
            if (httpCode > 0) {
                String payload = http.getString();
                StaticJsonDocument<200> doc;
                DeserializationError error = deserializeJson(doc, payload);

                if (!error) {
                    int accelerator = doc["received_axes"]["accelerator"];
                    int brake = doc["received_axes"]["brake"];
                    int steering = doc["received_axes"]["steering"];
                    setMotorDirection(accelerator, brake, steering);
                } else {
                    Serial.println("Failed to parse JSON");
                }
            } else {
                Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
            }
        } else {
            Serial.println("WiFi Disconnected, attempting to reconnect...");
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(100);  // Retry connection quickly
            }
            Serial.println("Reconnected to WiFi");
        }
    }
}