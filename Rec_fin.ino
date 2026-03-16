#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ESP32Servo.h>
#include <Preferences.h>

Servo servo1;   // GPIO 3

// Linear Actuator
#define IN1 6
#define IN2 7  

int motorSpeed = 255; // Actuator Speed

Preferences prefs;

int x = 0;

// Save values to flash
void saveValues() {
  prefs.putInt("servo1", x);
}

// ESP-NOW receive callback
void onReceive(const esp_now_recv_info *info,
               const uint8_t *data,
               int len) {

  Serial.println("📩 Data received");

  uint8_t command;
  memcpy(&command, data, sizeof(command));

  Serial.print("➡ Received Command: ");
  Serial.println(command);

  switch (command) {

    case 1:
      if (x < 180) {
        x += 10;
        servo1.write(x);
        saveValues();
        Serial.print("Servo1: ");
        Serial.print(x);
      }
      break;

    case 2:
      if (x > 0) {
        x -= 10;
        servo1.write(x);
        saveValues();
         Serial.print("Servo1: ");
        Serial.print(x);
      }
      break;

    case 3:
      digitalWrite(IN2, LOW);
      digitalWrite(IN1, motorSpeed);
      Serial.println("Hand Grab");
      break;

    case 4:
      digitalWrite(IN2, motorSpeed);
      digitalWrite(IN1, LOW);
      Serial.println("Hand Release");
      break;

    default:
      Serial.println("⚠ Unknown command");
      break;
  }

}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== RECEIVER WITH SERVO STARTED ===");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  servo1.attach(3);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Open NVS namespace
  prefs.begin("servoData", false);

  // Read stored values
  x = prefs.getInt("servo1", 0);  // default 0

  // Move servos to last position
  servo1.write(x);

  Serial.println("✅ Receiver ready, servos restored");
}

void loop() {
}
