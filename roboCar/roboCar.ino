#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

int Fpin1 = D5; //IN1
int Fpin2 = D6; //IN2
int Bpin1 = D7; //IN3
int Bpin2 = D8; //IN4

const char* ssid = "Next Tech Lab";
const char* password = "ntlapW203";
const char* endpoint = "https://steerbot-6he48xydx-nithishs-projects-8d26ce03.vercel.app/receive";

WiFiClientSecure wifiClient;
HTTPClient http;  // Declare as global to reuse the connection

void setMotorDirection(int accelerator, int brake, int steering) {
    if (accelerator == -1 && brake == -1) {
        digitalWrite(Fpin1, LOW);
        digitalWrite(Fpin2, LOW);
        digitalWrite(Bpin1, LOW);
        digitalWrite(Bpin2, LOW);
    } else if (brake > -1) {
        digitalWrite(Fpin1, LOW);
        digitalWrite(Fpin2, HIGH);  // Front motor

        digitalWrite(Bpin1, HIGH);
        digitalWrite(Bpin2, LOW);  // Back FRONT MOTOR
    } else if (accelerator > -1) {
        if (steering <= -0.2) {
            digitalWrite(Fpin1, LOW);
            digitalWrite(Fpin2, HIGH);  // Front motor

            digitalWrite(Bpin1, LOW);
            digitalWrite(Bpin2, HIGH);  // Back FRONT
        } else if (steering > 0) {
            digitalWrite(Fpin1, HIGH);
            digitalWrite(Fpin2, LOW);  // Front motor

            digitalWrite(Bpin1, HIGH);
            digitalWrite(Bpin2, HIGH);  // Back FRONT
        } else {
            digitalWrite(Fpin1, HIGH);
            digitalWrite(Fpin2, LOW);  // Front motor

            digitalWrite(Bpin1, LOW);
            digitalWrite(Bpin2, HIGH);  // Back FRONT
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
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    wifiClient.setInsecure();
    http.setReuse(true);  // Enable persistent connection (keep-alive)
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        http.begin(wifiClient, endpoint);  // Initialize HTTP connection each loop
        int httpCode = http.GET();  // Make the GET request
        
        if (httpCode > 0) {
            String payload = http.getString();  // Get the response payload
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

        http.end();  // Close the connection after the request to prevent memory leaks
    } else {
        Serial.println("WiFi Disconnected");
    }

    delay(5);  // Reduce the delay to improve responsiveness
}
