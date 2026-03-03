#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ESP32Servo.h>
#include <Preferences.h>

Servo servo1;   // GPIO 3
Servo servo2;   // GPIO 2

Preferences prefs;

int x = 0, y = 0;

// Save values to flash
void saveValues() {
  prefs.putInt("servo1", x);
  prefs.putInt("servo2", y);
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
        x += 20;
        servo1.write(x);
        saveValues();
      }
      break;

    case 2:
      if (x > 0) {
        x -= 20;
        servo1.write(x);
        saveValues();
      }
      break;

    case 3:
      if (y < 180) {
        y += 20;
        servo2.write(y);
        saveValues();
      }
      break;

    case 4:
      if (y > 0) {
        y -= 20;
        servo2.write(y);
        saveValues();
      }
      break;

    default:
      Serial.println("⚠ Unknown command");
      break;
  }

  Serial.print("Servo1: ");
  Serial.print(x);
  Serial.print(" | Servo2: ");
  Serial.println(y);
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
  servo2.attach(2);

  // Open NVS namespace
  prefs.begin("servoData", false);

  // Read stored values
  x = prefs.getInt("servo1", 0);  // default 0
  y = prefs.getInt("servo2", 0);

  // Move servos to last position
  servo1.write(x);
  servo2.write(y);

  Serial.println("✅ Receiver ready, servos restored");
}

void loop() {
}