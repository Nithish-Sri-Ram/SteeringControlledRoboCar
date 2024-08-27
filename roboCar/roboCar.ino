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
#include <ArduinoJson.h>           // Include the Arduino JSON library for parsing

// Define motor control pins
int Fpin1 = D5; // IN1
int Fpin2 = D6; // IN2
int Bpin1 = D7; // IN3
int Bpin2 = D8; // IN4

// WiFi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// HTTPS endpoint URL
const char* endpoint = "https://example.com/data";

// Function to set motor direction based on command
void setMotorDirection(String command) {
  if (command == "0010") {
    // Stay Put and Right
    digitalWrite(Fpin1, LOW);
    digitalWrite(Fpin2, LOW);
    digitalWrite(Bpin1, HIGH);
    digitalWrite(Bpin2, LOW);
  } else if (command == "0101") {
    // Back
    digitalWrite(Fpin1, LOW);
    digitalWrite(Fpin2, HIGH);
    digitalWrite(Bpin1, LOW);
    digitalWrite(Bpin2, HIGH);
  } else if (command == "1001") {
    // Awesome Left
    digitalWrite(Fpin1, HIGH);
    digitalWrite(Fpin2, LOW);
    digitalWrite(Bpin1, LOW);
    digitalWrite(Bpin2, HIGH);
  } else if (command == "1010") {
    // FRONT
    digitalWrite(Fpin1, HIGH);
    digitalWrite(Fpin2, LOW);
    digitalWrite(Bpin1, HIGH);
    digitalWrite(Bpin2, LOW);
  } else if (command == "1011") {
    // Left
    digitalWrite(Fpin1, HIGH);
    digitalWrite(Fpin2, LOW);
    digitalWrite(Bpin1, LOW);
    digitalWrite(Bpin2, HIGH);
  } else if (command == "1110") {
    // Stay Put Right
    digitalWrite(Fpin1, LOW);
    digitalWrite(Fpin2, LOW);
    digitalWrite(Bpin1, HIGH);
    digitalWrite(Bpin2, LOW);
  } else {
//stop
    digitalWrite(Fpin1, LOW);
    digitalWrite(Fpin2, LOW);
    digitalWrite(Bpin1, LOW);
    digitalWrite(Bpin2, LOW);
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
}

void loop() {
  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(endpoint);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);  // Print the received JSON (fust for debugging)

      // Parse the JSON payload
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        String command = doc["command"];

        setMotorDirection(command);
      } else {
        Serial.println("Failed to parse JSON");
      }
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(500);
}
