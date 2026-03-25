#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ESP32Servo.h>
#include <Preferences.h>

Servo servo1;

#define IN1 6
#define IN2 7
#define LAstop 2000   // milliseconds

Preferences prefs;

int x = 0;

// =====================
// RTOS QUEUES
// =====================
QueueHandle_t servoQueue;
QueueHandle_t actuatorQueue;

// =====================
// COMMAND STRUCT
// =====================
typedef struct {
  uint8_t cmd;
} Command_t;

// =====================
// SAVE SERVO POSITION
// =====================
void saveValues() {
  prefs.putInt("servo1", x);
}

// =====================
// ESP-NOW CALLBACK (ISR SAFE)
// =====================
void onReceive(const esp_now_recv_info *info,
               const uint8_t *data,
               int len) {

  Command_t command;
  memcpy(&command.cmd, data, sizeof(command.cmd));

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (command.cmd == 1 || command.cmd == 2) {
    xQueueSendFromISR(servoQueue, &command, &xHigherPriorityTaskWoken);
  }
  else if (command.cmd == 3 || command.cmd == 4) {
    xQueueSendFromISR(actuatorQueue, &command, &xHigherPriorityTaskWoken);
  }

  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}

// =====================
// SERVO TASK
// =====================
void servoTask(void *pvParameters) {
  Command_t cmd;

  while (1) {
    if (xQueueReceive(servoQueue, &cmd, portMAX_DELAY)) {

      if (cmd.cmd == 1 && x < 180) {
        x += 5;
      }
      else if (cmd.cmd == 2 && x > 0) {
        x -= 5;
      }

      servo1.write(x);
      saveValues();

      Serial.print("Servo Position: ");
      Serial.println(x);
    }
  }
}

// =====================
// ACTUATOR TASK (millis-based NON-BLOCKING)
// =====================
void actuatorTask(void *pvParameters) {
  Command_t cmd;

  bool isMoving = false;
  unsigned long startTime = 0;

  while (1) {

    // Check for new command (non-blocking)
    if (xQueueReceive(actuatorQueue, &cmd, 0)) {

      if (cmd.cmd == 3) {
        Serial.println("Hand Grab");

        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);

        startTime = millis();
        isMoving = true;
      }

      else if (cmd.cmd == 4) {
        Serial.println("Hand Release");

        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);

        startTime = millis();
        isMoving = true;
      }
    }

    // Stop actuator after time (non-blocking)
    if (isMoving && (millis() - startTime >= LAstop)) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);

      Serial.println("Actuator Stopped");

      isMoving = false;
    }

    // Small delay to yield CPU
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== RTOS RECEIVER STARTED (ESP32-C3) ===");

  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // ESP-NOW init
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  // Servo
  servo1.attach(3);

  // Actuator pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // Preferences (restore servo position)
  prefs.begin("servoData", false);
  x = prefs.getInt("servo1", 0);
  servo1.write(x);

  // Create queues
  servoQueue = xQueueCreate(10, sizeof(Command_t));
  actuatorQueue = xQueueCreate(10, sizeof(Command_t));

  // Create RTOS tasks
  xTaskCreate(servoTask, "ServoTask", 2048, NULL, 1, NULL);
  xTaskCreate(actuatorTask, "ActuatorTask", 2048, NULL, 1, NULL);

  Serial.println("✅ System Ready");
}

// =====================
// LOOP (NOT USED)
// =====================
void loop() {
}
