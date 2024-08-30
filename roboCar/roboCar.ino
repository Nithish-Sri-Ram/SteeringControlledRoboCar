// int Fpin1=D5; //IN1
// int Fpin2=D6; //IN2
// int Bpin1=D7; //IN3
// int Bpin2=D8;  //IN4

// //FYI

// // 0010 - Stay Put and Right
// // 0101 - Back
// // 0110 - Reverse left (No)
// // 1000 - Left
// // 1001 - Awsome Left
// // 1010 - FRONT
// // 1011 - Left
// // 1110 - Stay Put Right

// void setup() {
//   pinMode(Bpin1,OUTPUT);
//   pinMode(Bpin2,OUTPUT);
//   pinMode(Fpin1,OUTPUT);
//   pinMode(Fpin2,OUTPUT);
// }

// void loop() {

//   //stop
//   digitalWrite(Fpin1,LOW);
//   digitalWrite(Fpin2,LOW);
//   digitalWrite(Bpin1,LOW);
//   digitalWrite(Bpin2,LOW);

//   //front
//   digitalWrite(Fpin1,HIGH);
//   digitalWrite(Fpin2,LOW); 
//   digitalWrite(Bpin1,HIGH);
//   digitalWrite(Bpin2,LOW); 

//   delay(1000);

//   //back
//   digitalWrite(Fpin1,LOW);
//   digitalWrite(Fpin2,HIGH);
//   digitalWrite(Bpin1,LOW);
//   digitalWrite(Bpin2,HIGH); 

//   delay(1000);

//   //Right
//   digitalWrite(Fpin1,LOW);
//   digitalWrite(Fpin2,LOW);
//   digitalWrite(Bpin1,HIGH);
//   digitalWrite(Bpin2,LOW); 

//   delay(1000);

//   // Left
//   digitalWrite(Fpin1,HIGH);
//   digitalWrite(Fpin2,LOW);
//   digitalWrite(Bpin1,LOW);
//   digitalWrite(Bpin2,HIGH); 

//   delay(1000);

// }
#include <ESP8266WiFi.h>           // Include the ESP8266 WiFi library
#include <ESP8266HTTPClient.h>     // Include the HTTP client library
#include <WiFiClientSecure.h>      // Include the WiFiClientSecure library for HTTPS
#include <ArduinoJson.h>           // Include the Arduino JSON library for parsing

// Define motor control pins
int Fpin1 = D5; // IN1
int Fpin2 = D6; // IN2
int Bpin1 = D7; // IN3
int Bpin2 = D8; // IN4

/// WiFi credentials
const char* ssid = "srmap-byod";
const char* identity = "AP22110010377";  // Replace with your username
const char* password = "11-Nov-03";

// WPA2 Enterprise Settings
const char* identity = username;  // Usually the same as username
const char* user_password = password;

// HTTPS endpoint URL
const char* endpoint = "https://steeringcontrolledrobocar.onrender.com/endpoint";

// WiFiClientSecure object for HTTPS
WiFiClientSecure wifiClient;

// Function to set motor direction based on accelerator, brake, and steering values
void setMotorDirection(int accelerator, int brake, int steering) {
    if (accelerator == -1 && brake == -1) {
        // Stop
        digitalWrite(Fpin1, LOW);
        digitalWrite(Fpin2, LOW);
        digitalWrite(Bpin1, LOW);
        digitalWrite(Bpin2, LOW);
    } else if (brake > -1) {
        // Move backward, ignore steering
        digitalWrite(Fpin1, LOW);
        digitalWrite(Fpin2, HIGH);
        digitalWrite(Bpin1, LOW);
        digitalWrite(Bpin2, HIGH);
    } else if (accelerator > -1) {
        // Move forward, consider steering
        if (steering == -1) {
            // Turn left while moving forward
            digitalWrite(Fpin1, HIGH);
            digitalWrite(Fpin2, LOW);
            digitalWrite(Bpin1, LOW);
            digitalWrite(Bpin2, HIGH);
        } else if (steering == 1) {
            // Turn right while moving forward
            digitalWrite(Fpin1, LOW);
            digitalWrite(Fpin2, LOW);
            digitalWrite(Bpin1, HIGH);
            digitalWrite(Bpin2, LOW);
        } else {
            // Move straight forward
            digitalWrite(Fpin1, HIGH);
            digitalWrite(Fpin2, LOW);
            digitalWrite(Bpin1, HIGH);
            digitalWrite(Bpin2, LOW);
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Set motor pins as outputs
    pinMode(Fpin1, OUTPUT);
    pinMode(Fpin2, OUTPUT);
    pinMode(Bpin1, OUTPUT);
    pinMode(Bpin2, OUTPUT);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Optionally, disable SSL certificate verification
    wifiClient.setInsecure();  // Use only if you do not want to verify SSL certificate
}

void loop() {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Use the new API with WiFiClientSecure
        http.begin(wifiClient, endpoint);
        int httpCode = http.GET();

        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println(payload);  // Print the received JSON (just for debugging)

            // Parse the JSON payload
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
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }

    delay(100);
}
