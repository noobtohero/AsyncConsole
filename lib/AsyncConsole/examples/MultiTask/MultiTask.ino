#include <Arduino.h>

/**
 * AsyncConsole Example: Multi-Tasking
 *
 * ในตัวอย่างนี้จะแสดงว่า Logger สามารถบอกชื่อ Task ที่ส่ง Log มาได้อัตโนมัติ
 * ซึ่งมีประโยชน์มากเวลา Debug งานที่ซ้อนกันหลาย Task แบบ Non-blocking
 */

#define DEBUG_MODE
#include "AsyncConsole.h"

void sensorTask(void *pvParameters) {
  while (1) {
    int val = random(0, 100);
    CONSOLE_PRINT("[SENS] "); // พิมพ์หัวข้อแบบไม่มี Header
    CONSOLE_DEBUG("Reading sensor data: %d", val);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void networkTask(void *pvParameters) {
  while (1) {
    CONSOLE_INFO("Connecting to server...");
    vTaskDelay(pdMS_TO_TICKS(3000));
    CONSOLE_INFO("Data sent successfully");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup() {
  CONSOLE_INIT(115200);
  CONSOLE_INFO("--- Multi-Task Example Starting ---");

  // สร้าง Task ต่างๆ
  xTaskCreate(sensorTask, "sensor_task", 2048, NULL, 1, NULL);
  xTaskCreate(networkTask, "wifi_task", 2048, NULL, 1, NULL);
}

void loop() {
  // งานใน loop หลักก็ทำงานไป Logger จะไม่มาหน่วง loop นี้
  CONSOLE_INFO("Loop is running smoothly");
  delay(10000);
}
