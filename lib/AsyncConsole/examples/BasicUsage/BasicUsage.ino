#include <Arduino.h>

/**
 * AsyncConsole Example: Basic Usage
 *
 * ในตัวอย่างนี้จะแสดงวิธีการใช้งานพื้นฐาน:
 * 1. เปิดโหมด Debug ด้วย DEBUG_MODE
 * 2. เริ่มต้นการทำงานด้วย CONSOLE_INIT
 * 3. การพิมพ์ Log ระดับต่างๆ (DEBUG, INFO, WARN, ERROR)
 * 4. การใช้ CONSOLE_PROGRESS สำหรับแถบความคืบหน้า
 */

#define DEBUG_MODE
#include "AsyncConsole.h"

void setup() {
  // เริ่มการทำงานที่ 115200 bps
  CONSOLE_INIT(115200);

  CONSOLE_LOG("--- Basic Example Starting ---");

  // พิมพ์ข้อความธรรมดา
  CONSOLE_INFO("This is an information message");

  // พิมพ์ข้อความพร้อมค่าตัวแปร (Printf style)
  int sensorValue = 4095;
  float voltage = 3.3;
  CONSOLE_DEBUG("Debug info: value=%d, voltage=%.2fV", sensorValue, voltage);

  CONSOLE_WARN("This is a warning!");
  CONSOLE_ERROR("This is an error message!");

  // ตัวอย่างการใช้ CONSOLE_PRINT (พิมพ์ดิบๆ ต่อเนื่อง)
  CONSOLE_PRINT("Initializing components: ");
  for (int i = 0; i < 3; i++) {
    CONSOLE_PRINT("*");
    delay(300);
  }
  CONSOLE_LOG(" Done!");

  // ตัวอย่างการใช้ CONSOLE_PROGRESS (แบบอยู่ที่เดิม)
  CONSOLE_INFO("Starting upload sequence:");
  for (int i = 0; i <= 100; i += 10) {
    CONSOLE_PROGRESS("Upload", i);
    delay(300);
  }
}

void loop() {
  static int count = 0;
  CONSOLE_INFO("Main loop sequence: %d", count++);
  delay(5000);
}
