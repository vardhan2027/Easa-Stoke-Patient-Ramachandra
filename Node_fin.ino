#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Receiver MAC
uint8_t receiverMAC[] = {0x58, 0x8C, 0x81, 0xAC, 0x65, 0xDC};

uint8_t commandValue;

void sendCommand(uint8_t value, const char* label) {

  commandValue = value;

  esp_err_t result = esp_now_send(receiverMAC,
                                  &commandValue,
                                  sizeof(commandValue));

  Serial.print("Button ");
  Serial.print(label);
  Serial.print(" → Command ");
  Serial.print(value);

  if (result == ESP_OK) {
    Serial.println("  [SENT]");
  } else {
    Serial.println("  [SEND FAILED]");
  }

  delay(300); // debounce
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== SENDER STARTED ===");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("❌ Failed to add peer");
    return;
  }

  Serial.println("✅ ESP-NOW ready");

  pinMode(2, INPUT_PULLUP); // S1
  pinMode(3, INPUT_PULLUP); // S2
  pinMode(4, INPUT_PULLUP); // S3
  pinMode(5, INPUT_PULLUP); // S4
}

void loop() {
  if (digitalRead(2) == LOW) sendCommand(1, "S1");
  if (digitalRead(3) == LOW) sendCommand(2, "S2");
  if (digitalRead(4) == LOW) sendCommand(3, "S3");
  if (digitalRead(5) == LOW) sendCommand(4, "S4");
}