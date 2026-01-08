#define DEBUG_MODE // เปิดใช้งาน Logger (ถ้าเอาออก โค้ด Logger จะหายไป)
#include "AsyncConsole.h"

void someTask(void *pvParameters) {
  int count = 0;
  while (1) {
    CONSOLE_DEBUG("Working from someTask: %d", count++);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void setup() {
  // 1. Initialize Logger (Plug & Play)
  CONSOLE_INIT(115200);

  CONSOLE_LOG("System starting...");
  CONSOLE_INFO("This is an info message");
  CONSOLE_WARN("This is a warning message");
  CONSOLE_ERROR("This is an error message with values: %d, %.2f", 42, 3.14);

  // 1. ตัวอย่างการใช้ CONSOLE_PRINT (พิมพ์ต่อเนื่องในบรรทัดเดียว)
  CONSOLE_PRINT("System checking: ");
  for (int i = 0; i < 5; i++) {
    CONSOLE_PRINT(".");
    delay(200);
  }
  CONSOLE_LOG(" [READY]"); // CONSOLE_LOG จะขึ้นบรรทัดใหม่ให้เองแบบ Clean

  // 2. ตัวอย่างการใช้ CONSOLE_PROGRESS (แถบความคืบหน้าแบบอยู่ที่เดิม)
  CONSOLE_INFO("Starting Hardware Initialization...");
  for (int i = 0; i <= 100; i += 10) {
    CONSOLE_PROGRESS("Hardware", i);
    delay(300);
  }

  // 2. สร้างงานอื่นเพื่อดูการทำงานข้าม Task
  xTaskCreate(someTask, "work_task", 2048, NULL, 1, NULL);
}

void loop() {
  static int loopCount = 0;
  CONSOLE_INFO("Main loop count: %d", loopCount++);

  // ทดสอบส่ง Log ถี่ๆ เพื่อดูว่า Blocking หรือไม่
  // แม้จะส่งเร็วมาก แต่ Task หลักก็ยังทำงานต่อได้ (Log อาจจะโดน Drop ถ้า Queue เต็ม)
  // for(int i=0; i<10; i++) {
  //    CONSOLE_DEBUG("Flood test %d", i);
  // }

  if (loopCount % 5 == 0) {
    CONSOLE_WARN("Performing a flush before a long sleep simulation...");
    CONSOLE_FLUSH();
    delay(5000); // จำลองการทำงานที่ต้องการให้ Log จบก่อน
  }

  delay(2000);
}
